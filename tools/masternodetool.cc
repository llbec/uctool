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
    //string content = string(argv[2]);
    string head(argv[2], argv[2]+45);
    string version(argv[2]+45, argv[2]+49);
    string timestamp(argv[2]+49, argv[2]+57);
    string type(argv[2]+57, argv[2]+61);
    string txidlen(argv[2]+61, argv[2]+69);
    string txid(argv[2]+69, argv[2]+133);
    string voutid(argv[2]+133, argv[2]+137);
    /*char head[45];
    memcpy(head, argv[2], 45);
    char version[4];
    memcpy(version, argv[2]+45, 4);
    char timestamp[8];
    memcpy(timestamp, argv[2]+49, 8);
    char type[4];
    memcpy(type, argv[2]+57, 4);
    char txidlen[8];
    memcpy(txidlen, argv[2]+61, 8);
    char txid[64];
    memcpy(txid, argv[2]+69, 64);
    char voutid[4];
    memcpy(txid, argv[2]+133, 4);*/
    cout << "Message Head: " << ParseHex2String(head) << endl
        << "Message version: " << ParseHex2String(version) << endl
        << "Message timestamp: " << ParseHex2String(timestamp) << endl
        << "Message type: " << ParseHex2String(type) << endl
        << "Message txid length: " << ParseHex2String(txidlen) << endl
        << "Message txid: " << ParseHex2String(txid) << endl
        << "Message voutid: " << ParseHex2String(voutid) << endl;
}