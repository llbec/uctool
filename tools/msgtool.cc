#include "msgtool.h"
#include "utils.h"

#include "net.h"
#include "masternodeman.h"

using namespace std;

void GetMNLicenseHelp()
{
    cout << "Command \"getmnlicense\" example :" << endl << endl
        << "getmnlicense txid voutid ..." << endl
		<< "getmnlicense \"2122660463ab2c041a8b8ab406aa314e76f2b4bf88dec75ce7b17af0c8bc2887\" \"1\""<< endl;
}
void GetMNLicense(int argc, char const * argv[])
{
    if(argc < 4) {
        GetMNLicenseHelp();
        return;
    }
    string strtx = argv[2];
    uint32_t nindex = atoi(argv[3]);
    uint256 txhash;
    txhash.SetHex(strtx);

    mstnodequest mstquest(111,MST_QUEST_ONE);

    CMasternode mn;
    COutPoint mncoin(txhash, nindex);
    mn.vin.prevout = mncoin;

    bool proxyConnectionFailed = false;
	SOCKET hSocket;
    CService tService = CService(GetArg("-ucenterserver", "10.175.0.211:5009"));
    if(ConnectSocket(tService, hSocket, DEFAULT_CONNECT_TIMEOUT, &proxyConnectionFailed)) {
        if (!IsSelectableSocket(hSocket)) {
            printf("Cannot create connection: non-selectable socket created (fd >= FD_SETSIZE ?)\n");
            CloseSocket(hSocket);
            return;
        }

        if(!SendRequestNsg(hSocket, mn, mstquest)) {
            CloseSocket(hSocket);
            printf("send RequestMsgType error\n");
            return;
        }
    }
}
