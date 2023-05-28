#include "Acceptor.h"
#include "TcpConnection.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

void test()
{
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready();//此时处于监听状态

    //三次握手就已经建立，可以创建一条TCP连接
    TcpConnection con(acceptor.accept());

    //加调试
    cout << con.toString() << " has connected" << endl;

    while(1)
    {
        cout << ">>recv msg from client: " << con.receive() << endl;
        con.send("hello baby\n");
    }
}

int main(int argc, char **argv)
{
    test();
    return 0;
}

