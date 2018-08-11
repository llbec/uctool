#include "keytool.h"
#include "privsend.h"
#include "utils.h"

using namespace std;

class CKeyTool
{
private:
    const CKey key_;
    const CPubKey pubkey_;
    const string address_;
public:
    CKeyTool(CKey secret) :
    key_(secret),
    pubkey_(secret.GetPubKey()),
    address_(CBitcoinAddress(pubkey_.GetID()).ToString())
    {}
};