#ifndef UTCENTER_UTILS_H
#define UTCENTER_UTILS_H

#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <glog/logging.h>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

//extern std::map<std::string, std::string> _mapConfigs;
//extern std::map<std::string, std::vector<std::string> > _mapMultiConfigs;

class Strings {
public:
  static std::string Format(const char * fmt, ...);
  static void Append(std::string & dest, const char * fmt, ...);
};

std::string GetLogPath();
void SetFilePath(const std::string & filename);
void LoadConfigFile(std::map<std::string, std::string>& mapSettingsRet,
                    std::map<std::string, std::vector<std::string> >& mapMultiSettingsRet);
void InitLog(char const *argv[]);

#endif // UTCENTER_UTILS_H
