#include "scripttool.h"
#include "utils.h"
#include "base58.h"
#include "core_io.h"

using namespace std;

void GetPubScript(int argc, char const * argv[])
{
    if(argc < 3) {
        cout << "Command \"scriptpub\" example :" << endl
            << "scriptpub address" << endl
            << "scriptpub UXVAK4VXpMA7Lyy3DCnEGLWurTjfbT6ie7" << endl;
        return;
    }
    string addrstr = argv[2];
    CBitcoinAddress address(addrstr);
    if (!address.IsValid()) {
        cout << "Invalid address!" << endl;
        return;
    }
    CScript scriptPubKey = GetScriptForDestination(address.Get());

    cout << "Hex:       " << HexStr(scriptPubKey.begin(), scriptPubKey.end()) << endl
        << "asm:       " << ScriptToAsmStr(scriptPubKey) << endl
        << [scriptPubKey]{
            txnouttype type;
            vector<CTxDestination> addresses;
            int nRequired;
            if (!ExtractDestinations(scriptPubKey, type, addresses, nRequired)) {
                return Strings::Format("type:      %s\n", GetTxnOutputType(type));
            }
            string ret = Strings::Format("reqSigs:   %d\ntype:      %s\naddresses: ", nRequired, GetTxnOutputType(type));
            for(auto val : addresses) {
                Strings::Append(ret, "%s ", CBitcoinAddress(val).ToString().c_str());
            }
            Strings::Append(ret, "\n");
            return ret;
        }();
}

void ShowBase58Prefix(int argc, char const * argv[])
{
    for(uint i = 0; i < CChainParams::MAX_BASE58_TYPES; i++)
    {
        std::vector<unsigned char> vch = Params().Base58Prefix(i);
        cout << "type " << i << ":" << endl
        for( auto ch : vch)
        {
            cout << ch << endl
        }
    }
}
