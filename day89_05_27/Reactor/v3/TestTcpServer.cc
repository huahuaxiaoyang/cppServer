#include "TcpServer.h"
#include "TcpConnection.h"
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
    TcpServer server("127.0.0.1", 8888);
    server.setAllCallback(std::move(onConnection)
                          , std::move(onMessage)
                          , std::move(onClose));
    server.start();

}

int main(int argc, char **argv)
{
    test();
    return 0;
}

