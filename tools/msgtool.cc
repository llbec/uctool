#include "msgtool.h"
#include "utils.h"

#include "net.h"
#include "masternodeman.h"

#include <string>
#include <boost/log/trivial.hpp>  
#include <boost/lexical_cast.hpp>  
#include <boost/algorithm/string.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

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
    const int mstnd_iReqBufLen = 600;
    const int mstnd_iReqMsgHeadLen = 4;
    const int mstnd_iReqMsgTimeout = 10;

    string strService = GetArg("-ucenterserver", "");
    if(strService.empty()) {
        printf("Configure server ip and port in ulordtool.conf frist! Example:ucenterserver=127.0.0.1:5009\n");
        return;
    }

    char cbuf[mstnd_iReqBufLen];
    memset(cbuf,0,sizeof(cbuf));

    string strtx = argv[2];
    uint32_t nindex = atoi(argv[3]);

    mstnodequest mstquest(111,MST_QUEST_ONE);
    mstquest._timeStamps = GetTime();
	mstquest._txid = strtx;
	mstquest._voutid = nindex;
    int buflength = mstquest.GetMsgBuf(cbuf);

    bool proxyConnectionFailed = false;
	SOCKET hSocket;
    CService tService = CService(strService);
    if(ConnectSocket(tService, hSocket, DEFAULT_CONNECT_TIMEOUT, &proxyConnectionFailed)) {
        if (!IsSelectableSocket(hSocket)) {
            printf("Cannot create connection: non-selectable socket created (fd >= FD_SETSIZE ?)\n");
            CloseSocket(hSocket);
            return;
        }

        int nBytes = send(hSocket, cbuf, buflength, 0);
	    if(nBytes != buflength) {
            printf("CMasternodeCenter::RequestLicense: send msg %d, expect %d", nBytes, buflength);
            CloseSocket(hSocket);
            return ;
        }

        //rcv
		memset(cbuf,0,sizeof(cbuf));
		nBytes = 0;
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
            if(mstnode._txid != strtx || mstnode._voutid != nindex) {
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
