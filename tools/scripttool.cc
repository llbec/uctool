#include "scripttool.h"
#include "utils.h"

using namespace std;

void GetPubScript(int argc, char const * argv[])
{
    if(argc < 3) {
        cout << "Command \"scriptnew\" example :" << endl
            << "scriptnew address" << endl
            << "scriptnew UXVAK4VXpMA7Lyy3DCnEGLWurTjfbT6ie7" << endl;
        return;
    }
    string addrstr = argv[2];
    CBitcoinAddress address(addrstr);
    if (!address.IsValid()) {
        cout << "Invalid address!" << endl;
        return;
    }
    CScript scriptPubKey = GetScriptForDestination(address.Get());

    cout << "Hex: " << HexStr(scriptPubKey.begin(), scriptPubKey.end()) << endl
        << "asm: " << ScriptToAsmStr(scriptPubKey) << endl
        << [scriptPubKey]{
            txnouttype type;
            vector<CTxDestination> addresses;
            int nRequired;
            if (!ExtractDestinations(scriptPubKey, type, addresses, nRequired)) {
                return Strings.Format("type: %s\n", GetTxnOutputType(type));
            }
            string ret = Strings.Format("reqSigs: %d\ntype: %s\naddresses: ", nRequired, GetTxnOutputType(type));
            for_each(vector<CTxDestination>::iterator itor = addresses.begin(); itor!=addressess.end(); ++itor) {
                Strings.Append(ret, "%s ", CBitcoinAddress(itor).ToString().c_str())
            }
            Strings.Append(ret, "\n")
            return ret;
        }();
}