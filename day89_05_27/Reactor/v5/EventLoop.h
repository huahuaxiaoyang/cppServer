#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "MutexLock.h"
#include <vector>
#include <map>
#include <memory>
#include <functional>

using std::vector;
using std::map;
using std::shared_ptr;
using std::function;

class Acceptor;
class TcpConnection;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;

class EventLoop
{
    using Functors = function<void()>;
public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();
    //事件循环的主函数
    void loop();
    void unloop();

    void waitEpollFd();
    //处理新的连接请求
    void handleNewConnection();
    //消息的发送
    void handleMessage(int fd);

    //创建epfd文件描述符
    int createEpollFd();
    //将文件描述符放在红黑树上进行监听或者从红黑树上摘除
    void addEpollReadFd(int fd);
    void delEpollReadFd(int fd);

    void setConnectionCallback(TcpConnectionCallback &&cb);
    void setMessageCallback(TcpConnectionCallback &&cb);
    void setCloseCallback(TcpConnectionCallback &&cb);

    void handleRead();
    void wakeup();
    int createEventFd();
    void doPengdingFunctors();
    void runInLoop(Functors &&cb);

private:
    int _epfd;//epoll_create创建文件描述符
    int _evtfd;//eventfd系统调用返回的文件描述符
    bool _isLooping;//标识是否在运行
    Acceptor &_acceptor;//连接器的引用，需要调用其中的accept函数
    vector<struct epoll_event> _evtList;//存储满足条件的文件描述符的结构体
    map<int, TcpConnectionPtr> _conns;//存储文件描述符与TcpConnection的键值对

    TcpConnectionCallback _onConnectionCb;//连接建立
    TcpConnectionCallback _onMessageCb;//消息到达
    TcpConnectionCallback _onCloseCb;//连接断开

    vector<Functors> _pendings;//存放的是线程池交给EventLoop发送给客户端的任务
    MutexLock _mutex;//互斥访问vector


};

#endif
