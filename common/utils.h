#ifndef UTCENTER_UTILS_H
#define UTCENTER_UTILS_H

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

extern std::map<std::string, std::string> mapConfigs;
extern std::map<std::string, std::vector<std::string> > mapMultiConfigs;

void SetFilePath(const std::string & filename);
void LoadConfigFile(std::map<std::string, std::string>& mapSettingsRet,
                    std::map<std::string, std::vector<std::string> >& mapMultiSettingsRet);

#endif // UTCENTER_UTILS_H