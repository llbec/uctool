#ifdef MYSQL_ENABLE
#include "ulordcenter.h"

using namespace std;

CUCenter::CUCenter(EventLoop* loop) :
idleSeconds_(GetArg("-idleseconds", 60)),
server_(loop, InetAddress(static_cast<uint16_t>(GetArg("-tcpport", 5009))), "UCenterServer"),
codec_(boost::bind(&UlordServer::onStringMessage, this, _1, _2, _3)),
licversion_(GetArg("-licversion",1)),
db_()
{
    server_.setConnectionCallback(boost::bind(&UlordServer::onConnection, this, _1));
    server_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    loop->runEvery(1.0, boost::bind(&UlordServer::onTimer, this));
}

bool CUCenter::InitUCenterKey()
{
    char cKeyVersion[20];
    int keyVersion = 1;
    std::string strKey;
    
    while(true)
    {
        memset(cKeyVersion, 0, sizeof(cKeyVersion));
        sprintf(cKeyVersion, "-privkey%d", keyVersion);
        strKey = GetArg(std::string(cKeyVersion), "");
        if(strKey.empty()) {
            break;
        }

        try {
            CKeyExtension privkey(1, strKey);
            mapUCenterkey_.insert(pair_int_key_t(keyVersion, privkey.GetKey()));
            
            LOG(INFO) << "Load ucenter key <" << keyVersion << ":" << strKey << ">";
            keyVersion++;
        } catch(int) {
            printf("Invalid ulord center private key in the configuration! %s\n", strKey.c_str());
            return false;
        }
    }
    if(mapUCenterkey_.size() == 0) {
        printf("You must specify a Ulord Center privkey in the configuration.! example privkey1=123qwe\n");
        return false;
    }

    /*init masternode list*/
    mapMNodeList_.clear();
    CUlordDb::map_col_val_t mapSelect;
    mapSelect.insert(make_pair("status", 1));
    vector<CMNode> vecRet;
    if(db_.SelectMNode(mapSelect, vecRet)) {
        for(auto mn:vecRet) mapMNodeList_.insert(make_pair(CMNCoin(mn._txid, mn._voutid), mn));
    }
    return true;
}

void CUCenter::onConnection(const TcpConnectionPtr & conn)
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

void CUCenter::onStringMessage(const TcpConnectionPtr & tcpcli, const std::string & message, Timestamp time)
{
    uint32_t expectlen = GetArg("-requestlen",137);
    LOG(INFO) << "receive msg  "<< message.size() << " ,expect length is " << expectlen;
    if(message.size() != expectlen)
        return;
    
    //ParseQuest
    std::ostringstream os;
    boost::archive::binary_oarchive oa(os);
    std::string strinfo;
    std::vector<CMstNodeData> vecnode;
    mstnodequest  mstquest;
    try {
        std::istringstream is(message);  
        boost::archive::binary_iarchive ia(is);  
        ia >> mstquest;//从一个保存序列化数据的string里面反序列化，从而得到原来的对象。
    }
    catch (const std::exception& ex) {
        LOG(ERROR) << "receive message (" << HexStr(message.c_str(), (message.c_str()+expectlen)) << ") serialize exception:" << ex.what();
        return;
    }
    if(mstquest._questtype == MST_QUEST_ONE) {
        CMstNodeData msnode;
        if(!SelectMNData(mstquest._txid, mstquest._voutid, msnode))
            LOG(INFO) << "No valid info for masternode <" << mstquest._txid << ":" << mstquest._voutid << ">";
        else
            vecnode.push_back(msnode);
        if(vecnode.size()==0) {
            CMstNodeData node(0,mstquest._txid, mstquest._voutid);
            node._status = 0;
            vecnode.push_back(node);
        }
        mstnoderes  mstres(mstquest._msgversion);
        mstres._num = vecnode.size();
        mstres._nodetype = MST_QUEST_ONE;
        strinfo = Strings::Format("Send msg: %d masternodes\n", mstres._num);
        
        oa<<mstres;
        for(auto & node : vecnode)
        {
            strinfo += Strings::Format("\t<%s:%d-%s> %s - %ld\n", node._txid.c_str(), node._voutid, HexStr(node._pubkey).c_str(), node._licence.c_str(), node._licperiod);
            oa << node;
        }
    } else if (mstquest._questtype == MST_QUEST_KEY) {
        mstnoderes  mstres(mstquest._msgversion);
        mstres._num= mapUCenterkey_.size();
        mstres._nodetype = MST_QUEST_KEY;
        strinfo = Strings::Format("Send msg: %d ucenter keys\n", mstres._num);

        oa<<mstres;
        for(map_int_key_cit it = mapUCenterkey_.begin(); it != mapUCenterkey_.end(); it++)
        {
            CcenterKeyData keyPair(it->first, HexStr(it->second.GetPubKey()));
            strinfo += Strings::Format("\t<%d:%s>\n", it->first, HexStr(it->second.GetPubKey()).c_str());
            oa << keyPair;
        }
    } else {
        LOG(INFO) << "Unknown Msg";
        return;
    }
    LOG(INFO) << strinfo;
    std::string content = os.str();
    muduo::StringPiece sendmessage(content);
    codec_.send(tcpcli, sendmessage);
    return;
}

void CUCenter::onTimer()
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

void CUCenter::dumpConnectionList() const
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

bool CUCenter::SelectMNData(std::string txid, unsigned int voutid, CMstNodeData & mn)
{
    LOCK(cs_);
    CMNCoin mnCoin(txid, voutid);
    int64_t tnow = GetTime();
    if(mapMNodeList_.count(mnCoin) != 0) {
        if(mapMNodeList_[mnCoin]._licperiods >= tnow + db_._needUpdatePeriod) {
            mn = *(mapMNodeList_[mnCoin].GetData());
            return true;
        }
    }
    /*update mn list from db*/
    vector<string> vecFilter;
    vector<CMNode> vecRet;
    vecFilter.push_back("status=1");
    vecFilter.push_back(Strings::Format("validdate<%ld", tnow + db_._needUpdatePeriod));
    if(db_.SelectMNode(vecFilter, vecRet)) {
        for(auto mn:vecRet)
            [](CMNCoin& tx, CMNode& mn){ mapMNodeList_.count(tx) != 0 ? mapMNodeList_[tx] = mn : mapMNodeList_.insert(make_pair(tx, mn)); }(CMNCoin(mn._txid, mn._voutid), mn);
    }

    if(mapMNodeList_.count(mnCoin) != 0) {
        mn = *(mapMNodeList_[mnCoin].GetData());
        return true;
    }
    return false;
}
#endif // MYSQL_ENABLE