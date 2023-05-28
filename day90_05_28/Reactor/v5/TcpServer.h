#ifndef __TCPSERVER_H__
#define  __TCPSERVER_H__

#include "Acceptor.h"
#include "EventLoop.h"


class TcpServer
{
public:
    TcpServer(const string &ip, unsigned short port)
    : _acceptor(ip, port)
    , _loop(_acceptor)
    {

    }

    ~TcpServer()
    {

    }

    //启动服务器与关闭服务器
    void start()
    {
        _acceptor.ready();
        _loop.loop();
    }

    void stop()
    {
        _loop.unloop();
    }

    //三个回调函数的注册
    void setAllCallback(TcpConnectionCallback &&onConnection
                        , TcpConnectionCallback &&onMessage
                        , TcpConnectionCallback &&onClose)
    {
        _loop.setConnectionCallback(std::move(onConnection));
        _loop.setMessageCallback(std::move(onMessage));
        _loop.setCloseCallback(std::move(onClose));
    }
private:
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif
