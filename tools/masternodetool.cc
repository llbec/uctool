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