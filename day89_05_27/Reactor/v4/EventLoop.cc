#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>
#include <utility>

using std::cerr;
using std::endl;
using std::cout;
using std::make_pair;

EventLoop::EventLoop(Acceptor &acceptor)
: _epfd(createEpollFd())
, _evtfd(createEventFd())
, _isLooping(false)
, _acceptor(acceptor)
, _evtList(1024)
, _mutex()
{
    //监听listenfd
    addEpollReadFd(_acceptor.fd());
    //监听_evtfd
    addEpollReadFd(_evtfd);
}

EventLoop::~EventLoop()
{
    close(_epfd);
    close(_evtfd);
}

//事件循环的主函数
void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping)
    {
        waitEpollFd();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::waitEpollFd()
{
    int nready;
    do
    {
        nready = epoll_wait(_epfd, &*_evtList.begin(), _evtList.size(), 3000);
    }while(-1 == nready && errno == EINTR);

    if(-1 == nready)
    {
        std::cerr << "-1 == nready " << endl;
        return;
    }
    else if(0 == nready)
    {
        cout << ">>epoll_timeout" << endl; 
    }
    else
    {
        //进行扩容
        if(nready == (int)_evtList.size())
        {
            _evtList.resize(2 * nready);
        }

        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = _evtList[idx].data.fd;
            if(fd == _acceptor.fd())//是新的连接请求
            {
                if(_evtList[idx].events & EPOLLIN)
                {
                    //处理新的连接请求
                    handleNewConnection();
                }
            }
            else if(fd == _evtfd)
            {
                if(_evtList[idx].events & EPOLLIN)
                {
                    handleRead();//阻塞等待,被唤醒
                    doPengdingFunctors();
                }

            }
            else//老的连接上有数据发过来
            {
                if(_evtList[idx].events & EPOLLIN)
                {
                    //处理消息的发送
                    handleMessage(fd);
                }

            }
        }
    }
}

//处理新的连接请求
void EventLoop::handleNewConnection()
{
    //如果connfd有正常返回结果的话，那么就表明三次握手建立成功
    int connfd = _acceptor.accept();
    if(connfd < 0)
    {
        perror("accept");
        return;
    }

    //将connfd放在红黑树上进行监听
    addEpollReadFd(connfd);

    //就可以创建TcoConnection对象
    /* TcpConnection con(connfd); */
    TcpConnectionPtr con(new TcpConnection(connfd, this));
    
    //注册Tcp通信过程中的三个半事件
    con->setConnectionCallback(_onConnectionCb);//连接建立
    con->setMessageCallback(_onMessageCb);//消息达到
    con->setCloseCallback(_onCloseCb);//连接断开

    //存储文件描述符connfd与TcpConnection的键值对
    _conns.insert(make_pair(connfd, con));

    con->handleConnectionCallback();//处理连接建立
}

//消息的发送
void EventLoop::handleMessage(int fd)
{
    auto it = _conns.find(fd);
    
    if(it != _conns.end())
    {
        bool flag = it->second->isClosed();
        if(flag)
        {
            //连接断开
            it->second->handleCloseCallback();//处理连接断开的事件
            delEpollReadFd(fd);//将文件描述符从红黑树上摘除
            _conns.erase(it);//还要将文件描述符与连接的键值对从map中删除
        }
        else
        {
            //进行数据的收发
            it->second->handleMessageCallback();//消息的达到
        }
    }
    else
    {
        cout << "该连接不存在" << endl;
        return;
    }
}

//创建epfd文件描述符
int EventLoop::createEpollFd()
{
    int fd = epoll_create1(0);
    if(fd < 0)
    {
        perror("epoll_create1");
        return fd;
    }
    return fd;

}
//将文件描述符放在红黑树上进行监听或者从红黑树上摘除
void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;

    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl add");
        return;
    }
}
void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;

    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl del");
        return;
    }

}

void EventLoop::setConnectionCallback(TcpConnectionCallback &&cb)
{
    _onConnectionCb = std::move(cb);
}

void EventLoop::setMessageCallback(TcpConnectionCallback &&cb)
{
    _onMessageCb = std::move(cb);
}

void EventLoop::setCloseCallback(TcpConnectionCallback &&cb)
{
    _onCloseCb = std::move(cb);
}
    
void EventLoop::handleRead()
{
    uint64_t one = 1;
    int ret = read(_evtfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read");
        return;
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    int ret = write(_evtfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("write");
        return;
    }
}

int EventLoop::createEventFd()
{
    int fd = eventfd(0, 0);
    if(fd < 0)
    {
        perror("eventfd");
        return fd;
    }
    return fd;
}

void EventLoop::doPengdingFunctors()
{
    //将_pendings中的cb交换出来，交给tmp
    //交换的过程中需要加锁
    vector<Functors> tmp;
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendings);
    }

    //遍历新的tmp，执行所有的cb
    for(auto &cb : tmp)
    {
        cb();//执行任务
    }
}

void EventLoop::runInLoop(Functors &&cb)
{
    {
        MutexLockGuard autoLock(_mutex);
        _pendings.push_back(std::move(cb));
    }

    //让线程池唤醒EventLoop，让其发送数据
    wakeup();
}

