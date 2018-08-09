#ifndef UTCENTER_UCENTER_H
#define UTCENTER_UCENTER_H
#ifdef MYSQL_ENABLE
#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <boost/bind.hpp>

#include "MySQLConnection.h"
#include "masternodeman.h"

#include "codec.h"

using namespace muduo;
using namespace muduo::net;

class UlordServer : boost::noncopyable
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
    std::string tablename_;
    const int licversion_;
    MySQLConnection db_;
    map_int_key_t mapUCenterkey_;
public:
    UlordServer(EventLoop* loop, int idleSeconds, const InetAddress& listenAddr, const MysqlConnectInfo & ptrDBInfo);
    void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }
    void start() { server_.start(); }
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr & tcpcli, Buffer * buf, Timestamp time){}
    void onStringMessage(const TcpConnectionPtr & tcpcli, const std::string & message, Timestamp time);
    void onTimer();
    void dumpConnectionList() const;
    bool IsDBOnline();
    bool SelectMNData(std::string txid, unsigned int voutid, CMstNodeData & mn);
    bool InitUCenterKey();
};

#endif // MYSQL_ENABLE
#endif // UTCENTER_UCENTER_H
