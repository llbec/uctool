#include "common\utils.h"

int main(int argc, char const *argv[])
{
    SetFilePath("tool.conf");
	LoadConfigFile(mapConfigs, mapMultiConfigs);

    return 0;
}
