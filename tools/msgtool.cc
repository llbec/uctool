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

bool MSTRequest(mstnodequest & tAsk, std::string & strResult);

void GetMNLicenseHelp()
{
    cout << "Command \"asklicense\" example :" << endl << endl
        << "asklicense txid voutid ..." << endl
		<< "asklicense \"2122660463ab2c041a8b8ab406aa314e76f2b4bf88dec75ce7b17af0c8bc2887\" \"1\""<< endl;
}

void AskLicense(int argc, char const * argv[])
{
    if(argc < 4) {
        GetMNLicenseHelp();
        return;
    }

    string strtx = argv[2];
    uint32_t nindex = atoi(argv[3]);

    mstnodequest mstquest(111,MST_QUEST_ONE);
    mstquest._timeStamps = GetTime();
	mstquest._txid = strtx;
	mstquest._voutid = nindex;

    mstnoderes  mstres;
    string strRev;

    if(MSTRequest(mstquest, strRev)) {
        std::istringstream strstream(strRev);
        boost::archive::binary_iarchive ia(strstream);
        ia >> mstres;

        if(mstres._num == 1 && mstres._nodetype == MST_QUEST_ONE) {  			  
            CMstNodeData mstnode;
            ia >> mstnode;
            if(mstnode._txid != strtx || mstnode._voutid != nindex) {
                printf("ERROR:receive a invalid msg mn<%s:%d>\n", mstnode._txid.c_str(), mstnode._voutid);
                return;
            }
            printf("Info:Receive MasterNode<%s:%d> certificate %s time = %ld\n", mstnode._txid.c_str(), mstnode._voutid, mstnode._licence.c_str(), mstnode._licperiod);
        } else {
            printf("Error: receive a invalid msg! num=%d, type=%d\n", mstres._num, mstres._nodetype);
        }
    }
    return;
}

void AskKeyVersion(int argc, char const * argv[])
{
    mstnodequest mstquest(111,MST_QUEST_KEY);
    mstquest._timeStamps = GetTime();
	mstquest._txid = uint256().GetHex();
	mstquest._voutid = 0;

    mstnoderes  mstres;
    string strRev;

    if(MSTRequest(mstquest, strRev)) {
        std::istringstream strstream(strRev);
        boost::archive::binary_iarchive ia(strstream);
        ia >> mstres;

        if(mstres._nodetype == MST_QUEST_KEY) {
            string output = Strings::Format("Info: Receive %d pairs Key&Version:\n",mstres._num);
            for(int i = 0; i < mstres._num; i++)
            {
                CcenterKeyData keynode;
                ia >> keynode;
                Strings::Append(output, "\t<%d:%s>\n", keynode._keyversion, keynode._key.c_str());
            }
            printf("%s", output.c_str());
        } else {
            printf("Error: receive a invalid msg! type=%d\n", mstres._nodetype);
        }
    }
    return;
}

bool MSTRequest(mstnodequest & tAsk, std::string & strResult)
{
    const int mstnd_iReqBufLen = 600;
    const int mstnd_iReqMsgHeadLen = 4;
    const int mstnd_iReqMsgTimeout = 10;
    bool proxyConnectionFailed = false;

    string strService = GetArg("-ucenterserver", "");
    if(strService.empty()) {
        printf("Configure server ip and port in ulordtool.conf frist! Example:ucenterserver=127.0.0.1:5009\n");
        return false;
    }
    CService tService = CService(strService);
    SOCKET tConnect;

    char cbuf[mstnd_iReqBufLen];
    memset(cbuf,0,sizeof(cbuf));

    int buflength = tAsk.GetMsgBuf(cbuf);

    if(ConnectSocket(tService, tConnect, DEFAULT_CONNECT_TIMEOUT, &proxyConnectionFailed)) {
        if (!IsSelectableSocket(tConnect)) {
            printf("Cannot create connection: non-selectable socket created (fd >= FD_SETSIZE ?)\n");
            CloseSocket(tConnect);
            return false;
        }

        int nBytes = send(tConnect, cbuf, buflength, 0);
	    if(nBytes != buflength) {
            printf("CMasternodeCenter::RequestLicense: send msg %d, expect %d", nBytes, buflength);
            CloseSocket(tConnect);
            return false;
        }

        memset(cbuf,0,sizeof(cbuf));
		nBytes = 0;
        int64_t nTimeLast = GetTime();
		while(nBytes <= 0)
		{
            nBytes = recv(tConnect, cbuf, sizeof(cbuf), 0);
            if((GetTime() - nTimeLast) >= mstnd_iReqMsgTimeout) {
                CloseSocket(tConnect);
                printf("Error: wait for ack message timeout\n");
                return false;
            }
        }
        if(nBytes > mstnd_iReqBufLen) {
            CloseSocket(tConnect);
            printf("Error: msg have too much bytes %d, need increase rcv buf size\n", nBytes);
            return false;
        }

        int msglen = 0;
        memcpy(&msglen, cbuf, mstnd_iReqMsgHeadLen);
        msglen = HNSwapl(msglen);

        if(msglen != nBytes - mstnd_iReqMsgHeadLen) {
            CloseSocket(tConnect);
            printf("Error: receive a error msg length is %d, recv bytes is %d\n", msglen, nBytes);
            return false;
        }

        std::string str(cbuf + mstnd_iReqMsgHeadLen, msglen);
        strResult = str;
        CloseSocket(tConnect);
        return true;
    }
    printf("Error: can't connect to %s\n", strService.c_str());
    CloseSocket(tConnect);
    return false;
}

void HelpSerialize()
{
    cout << "Command \"serialize\" example :" << endl << endl
        << "serialize msg-type data ..." << endl
		<< "serialize questone \"2122660463ab2c041a8b8ab406aa314e76f2b4bf88dec75ce7b17af0c8bc2887\" \"1\""<< endl;
}
void MsgSerialize(int argc, char const * argv[])
{
    if(argc < 4) {
        HelpSerialize();
        return;
    }
    string type = argv[2];
    if(type == "questone") {
        if(argc < 5) {
            HelpSerialize();
            return;
        }
        mstnodequest mstquest(111,MST_QUEST_ONE);
        argc >= 6 ? mstquest._timeStamps = atoi(argv[5]) : mstquest._timeStamps = 0;
        mstquest._txid = argv[3];
        mstquest._voutid = atoi(argv[4]);

        std::ostringstream os;
        boost::archive::binary_oarchive oa(os);
        oa << mstquest;
        string strReq = os.str();
        cout << "Serialize result : " << strReq << endl;
    }
    return;
}
