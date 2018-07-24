#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

#include "utils.h"

using namespace std;

boost::filesystem::path _pathConfigFile;
//std::map<std::string, std::string> _mapConfigs;
//std::map<std::string, std::vector<std::string> > _mapMultiConfigs;

boost::filesystem::path GetUtCenterDir()
{
    namespace fs = boost::filesystem;
    // Windows < Vista: C:\Documents and Settings\Username\Application Data\utCenters
    // Windows >= Vista: C:\Users\Username\AppData\Roaming\utCenters
    // Mac: ~/Library/Application Support/utCenters
    // Unix: ~/.utcenters
#ifdef WIN32
    // Windows
    return GetSpecialFolderPath(CSIDL_APPDATA) / "utCenters";
#else
    fs::path pathRet;
    char* pszHome = getenv("HOME");
    if (pszHome == NULL || strlen(pszHome) == 0)
        pathRet = fs::path("/");
    else
        pathRet = fs::path(pszHome);
#ifdef MAC_OSX
    // Mac
    return pathRet / "Library/Application Support/utCenters";
#else
    // Unix
    return pathRet / ".utcenters";
#endif
#endif
}

/** Interpret string as boolean, for argument parsing */
static bool ToolsInterpretBool(const std::string& strValue)
{
    if (strValue.empty())
        return true;
    return (atoi(strValue.c_str()) != 0);
}

/** Turn -noX into -X=0 */
static void ToolsInterpretNegativeSetting(std::string& strKey, std::string& strValue)
{
    if (strKey.length()>3 && strKey[0]=='-' && strKey[1]=='n' && strKey[2]=='o')
    {
        strKey = "-" + strKey.substr(3);
        strValue = ToolsInterpretBool(strValue) ? "0" : "1";
    }
}

void SetFilePath(const std::string & filename)
{
    boost::filesystem::create_directories(GetUtCenterDir());
    boost::filesystem::path pathFile (GetUtCenterDir() / filename);
    _pathConfigFile = pathFile.string();

    cout << "Info: Using config file " << _pathConfigFile.string() << endl;
}

std::string GetLogPath()
{
	boost::filesystem::path logpath = GetUtCenterDir();
	logpath /= "logs";
	boost::filesystem::create_directories(logpath);
	return logpath.string();
}

void LoadConfigFile(map<string, string>& mapSettingsRet,
                    map<string, vector<string> >& mapMultiSettingsRet)
{
    boost::filesystem::ifstream streamConfig(_pathConfigFile);
    if (!streamConfig.good()){
        // Create empty ulord.conf if it does not excist
		boost::filesystem::create_directories(_pathConfigFile);
        FILE* configFile = fopen(_pathConfigFile.string().c_str(), "a");
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
        ToolsInterpretNegativeSetting(strKey, strValue);
        if (mapSettingsRet.count(strKey) == 0)
            mapSettingsRet[strKey] = strValue;
        mapMultiSettingsRet[strKey].push_back(strValue);
    }
}

string Strings::Format(const char * fmt, ...) {
    char tmp[512];
    string dest;
    va_list al;
    va_start(al, fmt);
    int len = vsnprintf(tmp, 512, fmt, al);
    va_end(al);
    if (len>511) {
        char * destbuff = new char[len+1];
        va_start(al, fmt);
        len = vsnprintf(destbuff, len+1, fmt, al);
        va_end(al);
        dest.append(destbuff, len);
        delete[] destbuff;
    } else {
        dest.append(tmp, len);
    }
    return dest;
}

void Strings::Append(string & dest, const char * fmt, ...) {
    char tmp[512];
    va_list al;
    va_start(al, fmt);
    int len = vsnprintf(tmp, 512, fmt, al);
    va_end(al);
    if (len>511) {
        char * destbuff = new char[len+1];
        va_start(al, fmt);
        len = vsnprintf(destbuff, len+1, fmt, al);
        va_end(al);
        dest.append(destbuff, len);
        delete[] destbuff;
    } else {
        dest.append(tmp, len);
    }
}

void InitLog(char const *argv[])
{
    // Initialize Google's logging library.	
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = GetLogPath();
    // Log messages at a level >= this flag are automatically sent to
    // stderr in addition to log files.
    FLAGS_stderrthreshold = 3;    // 3: FATAL
    FLAGS_max_log_size    = 100;  // max log file size 100 MB
    FLAGS_logbuflevel     = -1;   // don't buffer logs
    FLAGS_stop_logging_if_full_disk = true;
    return;
}