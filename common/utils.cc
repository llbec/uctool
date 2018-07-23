#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

#include "utils.h"

using namespace std;

boost::filesystem::path g_pathConfigFile;
std::map<std::string, std::string> mapConfigs;
std::map<std::string, std::vector<std::string> > mapMultiConfigs;

boost::filesystem::path GetUtCenterDir()
{
    namespace fs = boost::filesystem;
    // Windows < Vista: C:\Documents and Settings\Username\Application Data\UTCenters
    // Windows >= Vista: C:\Users\Username\AppData\Roaming\UTCenters
    // Mac: ~/Library/Application Support/UTCenters
    // Unix: ~/.utcenters
#ifdef WIN32
    // Windows
    return GetSpecialFolderPath(CSIDL_APPDATA) / "UTCenters";
#else
    fs::path pathRet;
    char* pszHome = getenv("HOME");
    if (pszHome == NULL || strlen(pszHome) == 0)
        pathRet = fs::path("/");
    else
        pathRet = fs::path(pszHome);
#ifdef MAC_OSX
    // Mac
    return pathRet / "Library/Application Support/UTCenters";
#else
    // Unix
    return pathRet / ".utcenters";
#endif
#endif
}

void SetFilePath(const std::string & filename)
{
    boost::filesystem::path pathFile (GetUtCenterDir() / filename);
    g_pathConfigFile = pathFile.string();

    cout << "Info: Using config file " << g_pathConfigFile.string() << endl;
}

void LoadConfigFile(map<string, string>& mapSettingsRet,
                    map<string, vector<string> >& mapMultiSettingsRet)
{
    boost::filesystem::ifstream streamConfig(g_pathConfigFile);
    if (!streamConfig.good()){
        // Create empty ulord.conf if it does not excist
        FILE* configFile = fopen(g_pathConfigFile.string().c_str(), "a");
        if (configFile != NULL)
            fclose(configFile);
        return; // Nothing to read, so just return
    }

    set<string> setOptions;
    setOptions.insert("*");

    for (boost::program_options::detail::config_file_iterator it(streamConfig, setOptions), end; it != end; ++it)
    {
        // Don't overwrite existing settings so command line settings override ulord.conf
        string strKey = string("-") + it->string_key;
        string strValue = it->value[0];
        InterpretNegativeSetting(strKey, strValue);
        if (mapSettingsRet.count(strKey) == 0)
            mapSettingsRet[strKey] = strValue;
        mapMultiSettingsRet[strKey].push_back(strValue);
    }
}