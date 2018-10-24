#ifdef MYSQL_ENABLE

#include "utils.h"
#include "licensewatcher.h"

#include "util.h"
#include "privsend.h"

using namespace std;
static boost::scoped_ptr<ECCVerifyHandle> global_VerifyHandle;

void InitChain()
{
    if(GetBoolArg("-testnet", false)) {
        SelectParams(CBaseChainParams::TESTNET);
		printf("Info: select TEST net!\n");
	} else {
        SelectParams(CBaseChainParams::MAIN);
		printf("Info: select MAIN net!\n");
        string err;
        if(!mnodecenter.InitCenter(err)) {
            printf("InitChain:%s\n", err.c_str());
        }
	}

    ECC_Start();
	global_VerifyHandle.reset(new ECCVerifyHandle());
}

int main(int argc, char const *argv[])
{
    /*init*/
    SetFilePath("ulordcenter.conf");
	LoadConfigFile(mapArgs, mapMultiArgs);
    InitChain();
    InitLog(argv);

    try {
        CLicenseWatcher watcher;

        if(argc > 1) {
            if("test" == string(argv[1])) {
                vector<CMNode> vecnode;
                watcher.SelectNeedUpdateMNData(vecnode);
                watcher.UpdateDB(vecnode);
                return 0;
            } else if("clear" == string(argv[1])) {
                watcher.ClearDB();
                return 0;
            }
        } else watcher.Run();
    } catch (int) {
        printf("Error: Constructor failed!\n");
        return -1;
    }

    return 0;
}

#endif // MYSQL_ENABLE
