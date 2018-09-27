#include "utils.h"
#include "util.h"
#include "base58.h"


#include "msgtool.h"
#include "keytool.h"
#include "dbtool.h"

using namespace std;

typedef void (*FP_CMD)(int, char const **);
map <string, FP_CMD> mapCommand;

void ShowCmds(int argc, char const *argv[])
{
    cout << "********* commands *********" << endl;
    for(map<string, FP_CMD>::iterator iter = mapCommand.begin(); iter != mapCommand.end(); iter++)
        cout << iter->first << endl;
    cout << "********* commands *********" << endl;
}

void RegisterCmd()
{
    mapCommand.insert(make_pair("asklicense", &AskLicense));
    mapCommand.insert(make_pair("askkeyversion", &AskKeyVersion));
    mapCommand.insert(make_pair("keynew", &NewKey));
	mapCommand.insert(make_pair("keymasternode", &GenKey));
    mapCommand.insert(make_pair("keyshow", &Showkey));
    mapCommand.insert(make_pair("keydecode", &Decodekey));
    mapCommand.insert(make_pair("keyencode", &Encodekey));
    mapCommand.insert(make_pair("keymatch", &Matchkey));
    mapCommand.insert(make_pair("msgserialize", &MsgSerialize));
    mapCommand.insert(make_pair("msgdeserialize", &MsgDeserialize));
#ifdef MYSQL_ENABLE
    mapCommand.insert(make_pair("dbcheck", &DBCheckNode));
    mapCommand.insert(make_pair("dbcheckall", &DBCheckAll));
#endif // MYSQL_ENABLE
}

static boost::scoped_ptr<ECCVerifyHandle> global_VerifyHandle;

void InitChain()
{
    if(GetBoolArg("-testnet", false))
	{
        SelectParams(CBaseChainParams::TESTNET);
		printf("Info: select TEST net!\n");
	}
    else
	{
        SelectParams(CBaseChainParams::MAIN);
		printf("Info: select MAIN net!\n");
	}

    ECC_Start();
	global_VerifyHandle.reset(new ECCVerifyHandle());
}

int main(int argc, char const *argv[])
{
    /*init*/
    SetFilePath("ulordtool.conf");
	LoadConfigFile(mapArgs, mapMultiArgs);
    InitChain();
    InitLog(argv);

    RegisterCmd();

    if(argc < 2)
	{
		ShowCmds(argc, argv);
		return -1;
	}

    string strcmd = string(argv[1]);

    map<string, FP_CMD>::iterator iterCMD = mapCommand.find(strcmd);

	if(iterCMD != mapCommand.end())
		iterCMD->second(argc, argv);
	else
		cout << "Error: unknown method " << strcmd << endl;

    return 0;
}
