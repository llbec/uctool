#include "utils.h"
#include "MySQLConnection.h"
#include "util.h"
#include "masternode.h"

using namespace std;

int main(int argc, char const *argv[])
{
    SetFilePath("licensetool.conf");
	LoadConfigFile(mapConfigs, mapMultiConfigs);

    //ScanTable(GetArg("-sqluser", "root"), GetArg("-sqlpwd", "123456"), GetArg("-sqlthread", 1));

    return 0;
}

