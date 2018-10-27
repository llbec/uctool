#ifndef ULORDCENTER_H
#define ULORDCENTER_H

#ifdef MYSQL_ENABLE
#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <boost/bind.hpp>

#include "sync.h"
#include "ulordDB.h"

#include "codec.h"

using namespace muduo;
using namespace muduo::net;

class CUCenter : boost::noncopyable
{
public:
    typedef boost::weak_ptr<TcpConnection> WeakTcpConnectionPtr;
    typedef std::list<WeakTcpConnectionPtr> WeakConnectionList;
    typedef std::map <int, CKey> map_int_key_t;
    typedef std::pair <int, CKey> pair_int_key_t;
    typedef typename map_int_key_t::iterator map_int_key_it;
    typedef typename map_int_key_t::const_iterator map_int_key_cit;
private:
    struct Node : public muduo::copyable
    { 
        Timestamp lastReceiveTime;
        WeakConnectionList::iterator position;
    };
    typedef std::set<TcpConnectionPtr> ConnectionList;
    int idleSeconds_;
    TcpServer server_;
    LengthHeaderCodec codec_;
    ConnectionList connections_;
    WeakConnectionList connectionList_;
    const int licversion_;
    map_int_key_t mapUCenterkey_;
    CUlordDb db_;
    mutable CCriticalSection cs_;
    std::map <CMNCoin, CMNode> mapMNodeList_;
public:
    CUCenter(EventLoop* loop);
    void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }
    void start() { server_.start(); }
    bool InitUCenterKey();
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr & tcpcli, Buffer * buf, Timestamp time){}
    void onStringMessage(const TcpConnectionPtr & tcpcli, const std::string & message, Timestamp time);
    void onTimer();
    void dumpConnectionList() const;
    bool SelectMNData(std::string txid, unsigned int voutid, CMstNodeData & mn);
    int HandlerMsg(const TcpConnectionPtr & tcpcli, const std::string & message);
    bool UnSerializeBoost(const std::string msg, mstnodequest& mq);
};
#endif // MYSQL_ENABLE

#endif // ULORDCENTER_H