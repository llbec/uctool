#ifndef UTCENTER_MSGTOOL_H
#define UTCENTER_MSGTOOL_H

void AskLicense(int argc, char const * argv[]);
void AskKeyVersion(int argc, char const * argv[]);
bool MSTRequest(const mstnodequest & tAsk, std::string & strResult);
#endif // UTCENTER_MSGTOOL_H
