#include "msgtool.h"
#include "utils.h"

#include "net.h"
#include "masternodeman.h"

#include <string>
#include <boost/log/trivial.hpp>  
#include <boost/lexical_cast.hpp>  
#include <boost/algorithm/string.hpp>

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

        //rcv
        const int mstnd_iReqBufLen = 600;
        const int mstnd_iReqMsgHeadLen = 4;
        const int mstnd_iReqMsgTimeout = 10;
        char cbuf[mstnd_iReqBufLen];
		memset(cbuf,0,sizeof(cbuf));
		int nBytes = 0;
        int64_t nTimeLast = GetTime();
		while(nBytes <= 0)
		{
            nBytes = recv(hSocket, cbuf, sizeof(cbuf), 0);
            if((GetTime() - nTimeLast) >= mstnd_iReqMsgTimeout) {
                CloseSocket(hSocket);
                printf("Error: wait for ack message timeout\n");
                return;
            }
        }
        if(nBytes > mstnd_iReqBufLen) {
            CloseSocket(hSocket);
            printf("Error: msg have too much bytes %d, need increase rcv buf size\n", nBytes);
            return;
        }

        int msglen = 0;
        memcpy(&msglen, cbuf, mstnd_iReqMsgHeadLen);
        msglen = HNSwapl(msglen);

        if(msglen != nBytes - mstnd_iReqMsgHeadLen) {
            CloseSocket(hSocket);
            printf("Error: receive a error msg length is %d, recv bytes is %d\n", msglen, nBytes);
            return;
        }

        std::string str(cbuf + mstnd_iReqMsgHeadLen, msglen);

        mstnoderes  mstres;
        std::istringstream strstream(str);
        boost::archive::binary_iarchive ia(strstream);
        ia >> mstres;

        if(mstres._num == 1) {  			  
            CMstNodeData mstnode;
            ia >> mstnode;
            if(mstnode._txid != mn.vin.prevout.hash.GetHex() || mstnode._voutid != mn.vin.prevout.n) {
                CloseSocket(hSocket);
                printf("ERROR:receive a invalid msg mn<%s:%d>\n", mstnode._txid.c_str(), mstnode._voutid);
                return;
            }

            printf("Info:Receive MasterNode<%s:%d> certificate %s time = %ld\n", mstnode._txid.c_str(), mstnode._voutid, mstnode._licence.c_str(), mstnode._licperiod);

            /*if(!mstnode.VerifyLicense()) {
                printf("ERROR: verify license failed\n");
                return;
            }*///without pubkey & privkey 
        }
	}
    CloseSocket(hSocket);
    return;
}
