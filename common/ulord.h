#ifndef UTCENTER_ULORD_H
#define UTCENTER_ULORD_H
#include "base58.h"

class CKeyTool
{
private:
    CKey key_;
    CPubKey pubkey_;
    string address_;
public:
    CKeyTool(CKey secret);
    CKeyTool(bool bCompress);
    CKeyTool(string strPrivkey);

    string GetPrivKey() { return CBitcoinSecret(key_).ToString(); }
    CKey GetPrivKey() { return key_; }
    string GetPubKey() { return HexStr(pubkey_); }
    CPubKey GetPubKey() { return pubkey_; }
    string GetAddress() { return address_; }

    bool SignCompact(string strMsg, vector<unsigned char>& vchSigRet);
    bool VerifyCompact(const vector<unsigned char>& vchSig, string strMsg, string& strErrRet);
};

#endif // UTCENTER_ULORD_H