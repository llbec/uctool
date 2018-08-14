#include "ulord.h"
#include "privsend.h"

using namespace std;
const unsigned char CODE_WORD = 170;

bool CKeyExtension::SetKey(CKey key)
{
    if(!key.IsValid())
        return false;
    
    SetData(Params().Base58Prefix(CChainParams::SECRET_KEY), key.begin(), key.size());
    if(key.IsCompressed())
        vchData.push_back(1);
    pubkey_ = key.GetPubKey();
    address_ = CBitcoinAddress(pubkey_.GetID()).ToString();
    return true;
}

std::string CKeyExtension::Encode()
{
    std::vector<unsigned char> vch = vchVersion;
    vch.insert(vch.end(), vchData.begin(), vchData.end());
    for(auto & v : vch)
        v = v ^ CODE_WORD;
    return EncodeBase58Check(vch);
}

CKey CKeyExtension::GetKey()
{
    CKey ret;
    if(vchData.size() < 32)
        throw -1;
    ret.Set(vchData.begin(), vchData.begin() + 32, vchData.size() > 32 && vchData[32] == 1);
    return ret;
}

bool CKeyExtension::IsValid()
{
    bool fExpectedFormat = vchData.size() == 32 || (vchData.size() == 33 && vchData[32] == 1);
    bool fCorrectVersion = vchVersion == Params().Base58Prefix(CChainParams::SECRET_KEY);
    return fExpectedFormat && fCorrectVersion;
}

CKeyExtension::CKeyExtension(CKey key)
{
    if(!SetKey(key))
        throw -1;
}

CKeyExtension::CKeyExtension(bool bCompress)
{
    CKey newkey;
    newkey.MakeNewKey(bCompress);
    if(!SetKey(newkey))
        throw -1;
    if(!newkey.VerifyPubKey(pubkey_)) {
        throw -1;
    }
}

CKeyExtension::CKeyExtension(std::string strPrivkey)
{
    if(!SetString(strPrivkey) || !IsValid())
        throw -1;
    
    pubkey_ = GetKey().GetPubKey();
    address_ = CBitcoinAddress(pubkey_.GetID()).ToString();

}

CKeyExtension::CKeyExtension(unsigned int nVersionBytes, std::string strPrivkey)
{
    std::vector<unsigned char> vchTemp;
    bool rc58 = DecodeBase58Check(strPrivkey, vchTemp);
    if ((!rc58) || (vchTemp.size() < nVersionBytes)) {
        vchData.clear();
        vchVersion.clear();
        throw -1;
    }

    for(auto & v : vchTemp)
        v = v ^ CODE_WORD;

    vchVersion.assign(vchTemp.begin(), vchTemp.begin() + nVersionBytes);
    vchData.resize(vchTemp.size() - nVersionBytes);
    if (!vchData.empty())
        memcpy(&vchData[0], &vchTemp[nVersionBytes], vchData.size());
    memory_cleanse(&vchTemp[0], vchTemp.size());

    pubkey_ = GetKey().GetPubKey();
    address_ = CBitcoinAddress(pubkey_.GetID()).ToString();
}

bool CKeyExtension::SignCompact(std::string strMsg, std::vector<unsigned char>& vchSigRet)
{
    return privSendSigner.SignMessage(strMsg, vchSigRet, GetKey());
}

bool CKeyExtension::VerifyCompact(const std::vector<unsigned char>& vchSig, std::string strMsg, std::string & strErrRet)
{
    return privSendSigner.VerifyMessage(pubkey_, vchSig, strMsg, strErrRet);
}

bool CKeyExtension::Match(std::string strPub)
{
    CPubKey pubkey(ParseHex(strPub));
    return GetKey().VerifyPubKey(pubkey);
}
