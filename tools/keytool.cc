#include "keytool.h"
#include "privsend.h"
#include "utils.h"

using namespace std;
class CKeyTool;

void NewKey(int argc, char const * argv[])
{
    CKeyTool newkey;
    if(newkey.NewKey()) {
        Printf("Private Key: %s\nPublic Key: %s\nAddress : %s\n",
                newkey.GetPrivKey().c_str(),
                newkey.GetPubKey().c_str(),
                newkey.GetAddress().c_str());
        return;
    }
    printf("Create New Key failed!\n");
    return;
}

void ShowkeyHelp()
{
    cout << "Command \"keyshow\" example :" << endl << endl
        << "keyshow stringPrivkey" << endl
		<< "keyshow \"L1kF5amYLK6JZuuWyHTEk7dArcTu5nucFrGC9bUxmHezd8fdY183\"" << endl;
}
void Showkey(int argc, char const * argv[])
{
    if(argc < 3) {
        ShowkeyHelp();
        return;
    }
    string strpriv = argv[2];
    CKeyTool key;
    if(key.InitKey(strpriv)) {
        printf("Private Key: %s\nPublic Key: %s\nAddress : %s\n",
                key.GetPrivKey().c_str(),
                key.GetPubKey().c_str(),
                key.GetAddress().c_str());
        return;
    }
    printf("String(%s) is not a valid private key!\n", strpriv.c_str());
    return;
}

class CKeyTool
{
private:
    CKey key_;
    CPubKey pubkey_;
    string address_;
    bool isInit_;
public:
    CKeyTool(CKey secret) :
    key_(secret),
    pubkey_(secret.GetPubKey()),
    address_(CBitcoinAddress(pubkey_.GetID()).ToString()),
    isInit_(true)
    {}

    CKeyTool() : isInit_(false) {}

    bool NewKey()
    {
        isInit_ = false;
        key_.MakeNewKey(true);
        pubkey_ = key_.GetPubKey();
        if(secret.VerifyPubKey(pubkey)) {
            address_ = CBitcoinAddress(pubkey_.GetID()).ToString();
            isInit_ = true;
        }
        return isInit_;
    }

    bool InitKey(string strPrivkey)
    {
        if(!privSendSigner.GetKeysFromSecret(strPrivkey, key_, pubkey_))
            return false;
        address_ = CBitcoinAddress(pubkey_.GetID()).ToString();
        isInit_ = true;
        return true;
    }

    string GetPrivKey() { return isInit_ ? CBitcoinSecret(key_).ToString() : ""; }
    string GetPubKey() { return isInit_ ? HexStr(pubkey_) : ""; }
    string GetAddress() { return isInit_ ? address_ : ""; }

    bool SignCompact(string strMsg, vector<unsigned char>& vchSigRet)
    {
        return isInit_ ? privSendSigner.SignMessage(strMsg, vchSigRet, key_) : false;
    }

    bool VerifyCompact(const vector<unsigned char>& vchSig, string strMsg, string& strErrRet)
    {
        if(isInit_)
            return privSendSigner.VerifyMessage(pubkey_, vchSig, strMsg, strErrRet);
        else {
            strErrRet = "No valid key to use!\n";
            return false;
        }
    }
};