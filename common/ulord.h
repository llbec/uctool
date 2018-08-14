#ifndef UTCENTER_ULORD_H
#define UTCENTER_ULORD_H
#include "base58.h"

class CKeyTool
{
private:
    CKey key_;
    CPubKey pubkey_;
    std::string address_;
public:
    CKeyTool(CKey secret);
    CKeyTool(bool bCompress);
    CKeyTool(std::string strPrivkey);
	CKeyTool(bool flag, std::string strPrivkey);

    std::string GetKeyString() { return CBitcoinSecret(key_).ToString(); }
    CKey GetKey() { return key_; }
    std::string GetPubKeyString() { return HexStr(pubkey_); }
    CPubKey GetPubKey() { return pubkey_; }
    std::string GetAddress() { return address_; }

    bool SignCompact(std::string strMsg, std::vector<unsigned char>& vchSigRet);
    bool VerifyCompact(const std::vector<unsigned char>& vchSig, std::string strMsg, std::string & strErrRet);
    bool Match(std::string strPub);
    std::string Encode();
};

#endif // UTCENTER_ULORD_H
