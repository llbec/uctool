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

void MNBoostUnSerialize(int argc, char const * argv[])
{
    if(argc < 3) {
        cout << "Command \"mnbstunseri\" example :" << endl << endl
            << "mnbstunseri HexString" << endl << endl;
        return;
    }
    //string content = string(argv[2]);
    char head[45];
    memccpy(head, argv[2], 45);
    char version[4];
    memccpy(version, argv[2]+45, 4);
    char timestamp[8];
    memccpy(timestamp, argv[2]+49, 8);
    char type[4];
    memccpy(type, argv[2]+57, 4);
    char txidlen[8];
    memccpy(txidlen, argv[2]+61, 8);
    char txid[64];
    memccpy(txid, argv[2]+69, 64);
    char voutid[4];
    memccpy(txid, argv[2]+133, 4);
    cout << "Message Head: " << ParseHex(head) << endl
        << "Message version: " << ParseHex(version) << endl
        << "Message timestamp: " << ParseHex(timestamp) << endl
        << "Message type: " << ParseHex(type) << endl
        << "Message txid length: " << ParseHex(txidlen) << endl
        << "Message txid: " << ParseHex(txid) << endl
        << "Message voutid: " << ParseHex(voutid) << endl
}