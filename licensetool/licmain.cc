#include "utils.h"

int main(int argc, char const *argv[])
{
    SetFilePath("licensetool.conf");
	LoadConfigFile(mapConfigs, mapMultiConfigs);

    return 0;
}
