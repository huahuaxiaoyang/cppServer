#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "TcpServer.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
#include "MyTask.h"

using std::cout;
using std::endl;

/* class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {
    }

    void process()
    {

        //完成任务
        for(int i=0;i<_msg.size();++i)
        {
            _msg[i]=toupper(_msg[i]);
        }
        _con->sendInLoop(_msg);
    }

private:
    string _msg;
    TcpConnectionPtr _con;
}; */

class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize
               , const string &ip,
               unsigned short port)
    : _pool(threadNum, queSize)
    , _server(ip, port)
    {

    }

    ~EchoServer()
    {

    }

    void start()
    {
        _pool.start();//子线程会在TaskQueue中取Task，因为现在TaskQueue是空的，所以会调用pop阻塞

        //将Tcp通信过程中的三个事件注册给了TcpServer，然后交给EventLoop，最后交给TcpConnection
        using namespace std::placeholders;
        _server.setAllCallback(std::bind(&EchoServer::onConnection, this, _1)
                               , std::bind(&EchoServer::onMessage, this, _1)
                               , std::bind(&EchoServer::onClose, this, _1));
        //让服务器调用ready函数，使得服务器处于监听状态
        //并且让Reactor/EventLoop进行循环监听(使用epoll_wait进行监听)
        //监听的文件描述符包括三种：
        //1、listenfd（有没有新的连接请求过来）
        //2、evtfd,监听线程池与EventLoop通信的文件描述符,只要被监听，让EventLoop/Reactor
        //将线程池处理之后的msg发送给客户端
        //3、connfd，也就是Acceptor类调用accept函数的正确返回结果，如果有可读事件，
        //就表明客户端发送了数据给了服务器，服务器就可以与客户端进行通信
        _server.start();
    }

    void stop()
    {
        _pool.stop();
        _server.stop();
    }

    //连接建立
    void onConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!!!" << endl;
    }
    
    void onMessage(const TcpConnectionPtr &con)
    {
        string msg = con->receive();
        //处理msg
        cout << ">>recv from client msg = " << msg << endl;
        //需要将msg进行处理之后，再将其发送给客户端
        //就需要做业务逻辑的处理，也就是处理msg
        
        MyTask task(msg, con);
        
        //在执行了push操作，将task放在任务队列里面
        _pool.addTask(std::bind(&MyTask::process, task));
    }
    
    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!!!" << endl;
    }
private:
    ThreadPool _pool;
    TcpServer _server;

};

#endif
