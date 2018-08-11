#include "ulord.h"
#include "privsend.h"

using namespace std;

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