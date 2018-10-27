#include "ulord.h"
#include "ulordDB.h"


void MNHash(int argc, char const * argv[])
{
    if(argc < 7) {
        cout << "Command \"mnhash\" example :" << endl << endl
            << "mnhash txid voutid pubkey licensePeriod licenseVersion" << endl << endl;
        return;
    }

    CMNode mn;
    mn._txid = argv[2];
    mn._voutid = atoi(argv[3]);
    mn._pubkey = CPubKey(ParseHex(argv[4]));
    mn._licperiod = atoi64(argv[5]);
    mn._licversion = atoi(argv[6]);

    cout << mn.GetLicenseWord().ToString() << endl <<  mn.GetLicenseWord().GetHex() << endl << endl;
}

std::string ParseHex2String(const char* psz)
{
    std::vector<unsigned char> vch = ParseHex(psz);
    std::string res;
    res.insert(res.begin(), vch.begin(), vch.end());
    return res;
}
std::string ParseHex2String(const std::string str)
{
    return ParseHex2String(str.c_str());
}

void MNBoostUnSerialize(int argc, char const * argv[])
{
    if(argc < 3) {
        cout << "Command \"mnbstunseri\" example :" << endl << endl
            << "mnbstunseri HexString" << endl << endl;
        return;
    }
    string hex = string(argv[2]);
    string head = hex.substr(0,90);
    string version = hex.substr(90,8);
    string timestamp = hex.substr(98,16);
    string type = hex.substr(114,8);
    string txidlen = hex.substr(122,16);
    string txid = hex.substr(138,128);
    string voutid = hex.substr(266,8);

    int nversion;
    ParseInt32(version, &nversion);
    int64_t nstamp;
    ParseInt64(version, &nstamp);
    int ntype;
    ParseInt32(version, &ntype);
    int64_t nlen;
    ParseInt64(version, &nlen);
    int nvout;
    ParseInt32(version, &nvout);
    
    cout << "Message Head: " << ParseHex2String(head) << endl
        << "Message version: " << nversion << endl
        << "Message timestamp: " << nstamp << endl
        << "Message type: " << ntype << endl
        << "Message txid length: " << nlen << endl
        << "Message txid: " << ParseHex2String(txid) << endl
        << "Message voutid: " << nvout << endl;
}