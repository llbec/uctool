#include "pubkey.h"
#include "utils.h"

bool SignMNLicense(std::string strPriv, std::string strtx, unsigned int nid, int64_t nperiod)
{
    if(!mapArgs.count("-privkey")) {
        LOG(FATAL) << "did not configure ut center private key!";
        return false;
    }
}