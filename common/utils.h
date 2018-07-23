#ifndef UTCENTER_UTILS_H
#define UTCENTER_UTILS_H

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

extern std::map<std::string, std::string> mapConfigs;
extern std::map<std::string, std::vector<std::string> > mapMultiConfigs;

class Strings {
public:
  static string Format(const char * fmt, ...);
  static void Append(string & dest, const char * fmt, ...);
};

void SetFilePath(const std::string & filename);
void LoadConfigFile(std::map<std::string, std::string>& mapSettingsRet,
                    std::map<std::string, std::vector<std::string> >& mapMultiSettingsRet);

#endif // UTCENTER_UTILS_H