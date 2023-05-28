#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"


class TcpConnection
{
public:
    explicit TcpConnection(int fd);
    ~TcpConnection();
    void send(const string &msg);
    string receive();
    //为了方便调试的函数
    string toString();

private:
    //获取本端地址与对端地址
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

private:
    SocketIO _sockIO;

    //为了调试而加入的函数
    Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;
};

#endif
