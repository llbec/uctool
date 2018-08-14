#include "ulord.h"
#include "privsend.h"

using namespace std;
const unsigned char CODE_WORD = 170;

CKeyTool::CKeyTool(CKey secret) :
key_(secret),
pubkey_(secret.GetPubKey()),
address_(CBitcoinAddress(pubkey_.GetID()).ToString())
{}

CKeyTool::CKeyTool(bool bCompress)
{
    key_.MakeNewKey(bCompress);
    pubkey_ = key_.GetPubKey();
    if(!key_.VerifyPubKey(pubkey_)) {
        throw -1;
    }
    address_ = CBitcoinAddress(pubkey_.GetID()).ToString();
}

CKeyTool::CKeyTool(std::string strPrivkey)
{
    if(!privSendSigner.GetKeysFromSecret(strPrivkey, key_, pubkey_))
        throw -1;
    address_ = CBitcoinAddress(pubkey_.GetID()).ToString();
}

CKeyTool::CKeyTool(bool flag, std::string strPrivkey)
{
    CBitcoinSecret vchSecret;

    if(!vchSecret.SetString(strPrivkey))
        throw -1;
    CKey codekey = vchSecret.GetKey();
    unsigned char vch[32];
    const unsigned char * pKey = codekey.begin();
    memcpy(vch, pkey, 32);
    /*decode here*/
    for(int i = 0; i < 32; i++)
    {
        vch[i] = vch[i] ^ CODE_WORD;
    }
    
    key_.set(vch[0], vch[31], codekey.IsCompressed());
    if(!key_.IsValid()) {
        throw -1;
    }
    // init others
    pubkey_ = key_.GetPubKey();
    address_ = CBitcoinAddress(pubkey_.GetID()).ToString();
}

bool CKeyTool::SignCompact(std::string strMsg, std::vector<unsigned char>& vchSigRet)
{
    return privSendSigner.SignMessage(strMsg, vchSigRet, key_);
}

bool CKeyTool::VerifyCompact(const std::vector<unsigned char>& vchSig, std::string strMsg, std::string & strErrRet)
{
    return privSendSigner.VerifyMessage(pubkey_, vchSig, strMsg, strErrRet);
}

bool CKeyTool::Match(std::string strPub)
{
    CPubKey pubkey(ParseHex(strPub));
    return key_.VerifyPubKey(pubkey);
}

std::string CKeyTool::Encode()
{
    unsigned char vch[32];
    const unsigned char * pKey = key_.begin();
    memcpy(vch, pkey, 32);
    /*encode here*/
    for(int i = 0; i < 32; i++)
    {
        vch[i] = vch[i] ^ CODE_WORD;
    }

    CKey codekey;
    codekey.set(vch[0], vch[31], key_.IsCompressed());
    if(!codekey.IsValid()) {
        throw -1;
    }
    return CBitcoinSecret(key_).ToString();
}