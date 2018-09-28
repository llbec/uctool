#ifdef MYSQL_ENABLE
#include "utils.h"
#include "ulordcenter.h"

#include "util.h"
#include "privsend.h"

using namespace std;
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
    SetFilePath("ulordcenter.conf");
	LoadConfigFile(mapArgs, mapMultiArgs);
    InitChain();
    InitLog(argv);

    EventLoop loop;
    
    CUCenter utcenter(&loop);
    if(!utcenter.InitUCenterKey())
        return -1;

    int nThread = GetArg("-thread", 2);
    if (nThread > 1)
        utcenter.setThreadNum(nThread);
    utcenter.start();
    loop.loop();
    
    return 0;
}

#endif // MYSQL_ENABLE
