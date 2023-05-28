#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

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

    //数据发回给客户端
    con->send(msg);
}

void onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has closed!!!" << endl;
}

void test()
{
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready();//此时处于监听状态

    EventLoop loop(acceptor);
    loop.setConnectionCallback(std::move(onConnection));
    /* loop.setConnectionCallback(std::bind(onConnection, std::placeholders::_1)); */
    loop.setMessageCallback(std::move(onMessage));
    loop.setCloseCallback(std::move(onClose));
    loop.loop();


}

int main(int argc, char **argv)
{
    test();
    return 0;
}

