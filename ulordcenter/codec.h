#ifndef MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
#define MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H

#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Endian.h>
#include <muduo/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <vector>

class LengthHeaderCodec : boost::noncopyable
{
 public:
  typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
                                const std::string& message,
                                muduo::Timestamp)> StringMessageCallback;

  explicit LengthHeaderCodec(const StringMessageCallback& cb)
    : messageCallback_(cb)
  {
  }

  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp receiveTime)
  {
    while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4
    {
      // FIXME: use Buffer::peekInt32()
      const void* data = buf->peek();
      int32_t be321 = *static_cast<const int32_t*>(data); // SIGBUS
      int32_t be32 =  buf->peekInt32(); // SIGBUS
      const int32_t len =  muduo::net::sockets::networkToHost32(be321);
      assert(len==be32);    

      if (len > 65536 || len < 0)
      {
        LOG_ERROR << "Invalid length " << len;
        conn->shutdown();  // FIXME: disable reading
        break;
      }
      else if (buf->readableBytes() >= len + kHeaderLen)
      {
        buf->retrieve(kHeaderLen);
        //std::vector<unsigned char>  message(buf->peek(),buf->peek()+len);
        std::string message(buf->peek(), len);
        messageCallback_(conn, message, receiveTime);
        buf->retrieve(len);
      }
      else
      {
        break;
      }
    }
  }

  // FIXME: TcpConnectionPtr
  void send(const  muduo::net::TcpConnectionPtr &  conn,
            const muduo::StringPiece& message)
  {
    muduo::net::Buffer buf;
    buf.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
    buf.prepend(&be32, sizeof be32);
    conn->send(&buf);
  }

 private:
  StringMessageCallback messageCallback_;
  const static size_t kHeaderLen = sizeof(int32_t);
};

#endif  // MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
