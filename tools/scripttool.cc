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
    printVch = [](string name, const vector<unsigned char> &vch) {
        cout << "type " << name << ":" << endl;
        for( auto ch : vch)
        {
            cout << ch << endl;
        }
    }
    printVch("PUBKEY_ADDRESS", Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS));
    printVch("SCRIPT_ADDRESS", Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS));
    printVch("SECRET_KEY", Params().Base58Prefix(CChainParams::SECRET_KEY));
    printVch("EXT_PUBLIC_KEY", Params().Base58Prefix(CChainParams::EXT_PUBLIC_KEY));
    printVch("EXT_SECRET_KEY", Params().Base58Prefix(CChainParams::EXT_SECRET_KEY));
    printVchs("EXT_COIN_TYPE", Params().Base58Prefix(CChainParams::EXT_COIN_TYPE));
}
