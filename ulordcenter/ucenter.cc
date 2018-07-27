#ifdef MYSQL_ENABLE
#include "ucenter.h"
#include "utils.h"

#include "privsend.h"

#include <string>
#include <boost/log/trivial.hpp>  
#include <iostream>
#include <boost/lexical_cast.hpp>  
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace boost;

UlordServer::UlordServer(EventLoop * loop, int idleSeconds, const InetAddress & listenAddr, const CKey priv, const MysqlConnectInfo & ptrDBInfo) :
idleSeconds_(idleSeconds),
server_(loop, listenAddr, "UlordServer"),
codec_(boost::bind(&UlordServer::onStringMessage, this, _1, _2, _3)),
ucenterKey_(priv),
db_(ptrDBInfo)
{
    server_.setConnectionCallback(boost::bind(&UlordServer::onConnection, this, _1));
    server_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    loop->runEvery(1.0, boost::bind(&UlordServer::onTimer, this));
    tablename_ = GetArg("-dbtable","udevforums_major_node_bind");
    //dumpConnectionList();
}

void UlordServer::onConnection(const TcpConnectionPtr & conn)
{
    LOG(INFO) << conn->localAddress().toIpPort() << " -> "
                << conn->peerAddress().toIpPort() << " is "
                << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected()) { 
        Node node;
        node.lastReceiveTime = Timestamp::now();
        connectionList_.push_back(conn);
        node.position = --connectionList_.end();
        conn->setContext(node);
    } else { 
        assert(!conn->getContext().empty());
        const Node& node = boost::any_cast<const Node&>(conn->getContext());
        connectionList_.erase(node.position);
    }
    //dumpConnectionList();
}

void UlordServer::onStringMessage(const TcpConnectionPtr & tcpcli, const std::string & message, Timestamp time)
{
    LOG(INFO) << "receive msg  "<< message.size(); 
    
    //ParseQuest
    std::vector<CMstNodeData> vecnode;
    mstnodequest  mstquest;
    std::istringstream is(message);  
    boost::archive::binary_iarchive ia(is);  
    ia >> mstquest;//从一个保存序列化数据的string里面反序列化，从而得到原来的对象。
    if(mstquest._questtype == MST_QUEST_ONE) {
        CMstNodeData msnode;
        if(!SelectMNData(mstquest._txid, mstquest._voutid, msnode))
            LOG(INFO) << "No valid info for masternode <" << mstquest._txid << ":" << mstquest._voutid << ">";
        else
            vecnode.push_back(msnode);
        if(vecnode.size()==0) {
            CMstNodeData node(0,mstquest._txid, mstquest._voutid);
            node._status = 0;
            node._validflag = 0;
            vecnode.push_back(node);
        }
    } if(mstquest._questtype == MST_QUEST_ALL) {
        LOG(WARNING) << "Rcv MST_QUEST_ALL msg and return.";
        return;
    }

    std::string strinfo;
    mstnoderes  mstres(mstquest._msgversion);
    mstres._num= vecnode.size();
    strinfo = Strings::Format("Send msg: %d masternodes\n", mstres._num);
    std::ostringstream os;
    boost::archive::binary_oarchive oa(os);
    oa<<mstres;
    for(auto & node : vecnode)
    {
        strinfo += Strings::Format("\t<%s:%d> %s - %ld\n", node._txid.c_str(), node._voutid, node._license.c_str(), node._licperiod);
        oa << node;
    }
    LOG(INFO) << strinfo;
    std::string content = os.str();
    muduo::StringPiece sendmessage(content);
    codec_.send(tcpcli, sendmessage);
    return;
}

void UlordServer::onTimer()
{
    //dumpConnectionList();
    Timestamp now = Timestamp::now();
    for (WeakConnectionList::iterator it = connectionList_.begin(); it != connectionList_.end(); )
    {
        TcpConnectionPtr conn = it->lock();
        if (conn) {
            Node * n = boost::any_cast<Node>(conn->getMutableContext());
            double age = timeDifference(now, n->lastReceiveTime);
            if (age > idleSeconds_) {
                if (conn->connected()) {
                    conn->shutdown();
                    LOG(INFO) << "shutting down " << conn->name();
                    conn->forceCloseWithDelay(3.5);  // > round trip of the whole Internet.
                }
            } else if (age < 0) {
                LOG(WARNING) << "Time jump" << conn->name();
                n->lastReceiveTime = now;
            } else {
                break;
            }
            ++it;
        } else {
            LOG(WARNING) << "Expired conn";
            it = connectionList_.erase(it);
        }
    }
}

void UlordServer::dumpConnectionList() const
{
    for (WeakConnectionList::const_iterator it = connectionList_.begin(); it != connectionList_.end(); ++it)
    {
        TcpConnectionPtr conn = it->lock();
        if (conn) {
            const Node& n = boost::any_cast<const Node&>(conn->getContext());
            LOG(INFO) << "conn " << get_pointer(conn) << "\ttime " << n.lastReceiveTime.toString();
        } else {
            LOG(INFO) << "conn:expired";
        }
    }
}

bool UlordServer::IsDBOnline()
{
    for (size_t nPings = 0; nPings < 3; nPings++) {
        if (db_.ping())
            return true;
    }
    LOG(INFO) << "ulord center db is offline!";
    return false;
}

bool UlordServer::SelectMNData(std::string txid, unsigned int voutid, CMstNodeData & mn)
{
    bool result = false;
    MySQLResult res;
    std::string sql = Strings::Format("SELECT trade_txid, trade_vout_no, special_code, status, validdate, node_period FROM %s WHERE trade_txid = '%s' AND trade_vout_no = %d",
                                    tablename_.c_str(), txid.c_str(), voutid);

    if(!IsDBOnline())
        return false;
    db_.query(sql, res);
    if(res.numRows() == 0) {
        LOG(INFO) << "get none info for masternode <" << txid << ":" << voutid << ">";
        return false;
    }
    if(res.numRows() > 1)
        LOG(WARNING) << "get" << res.numRows() << " infos for masternode <" << txid << ":" << voutid << ">";
    
    char **row = res.nextRow();
    CMstNodeData mstnode;
    while(row != nullptr) {
        if(row[0] == NULL || row[1] == NULL || row[2] == NULL)
            continue;
        mstnode._txid       = row[0];
        mstnode._voutid     = atoi(row[1]);
        mstnode._privkey    = row[2];
        mstnode._status     = atoi(row[3]);
        mstnode._licperiod  = atoi(row[4]);
        if(row[5] != NULL)
            mstnode._nodeperiod = atoi(row[5]);
        else
            mstnode._nodeperiod = 0;

        CKey mnpriv;
        if(!privSendSigner.GetKeysFromSecret(mstnode._privkey, mnpriv, mstnode._pubkey)) {
            LOG(INFO) << "DBWatcher::SelectMNData:masternode <" <<mstnode._txid << ":" <<mstnode._voutid << "> private key string " << mstnode._privkey << " is invalid!";
            continue;
        }
        mn = mstnode;
        result = true;
        if(mn._status == 1) {
            mn._validflag = 1;
            break;
        }
        row = res.nextRow();
    }
    return result;
}
#endif // MYSQL_ENABLE
