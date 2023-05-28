#ifndef __MYTASK__H__
#define __MYTASK__H__

#include "TcpServer.h"
#include "TcpConnection.h"
//#include "ThreadPool.h"
#include <iostream>
//#include <unistd.h>


class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con);

    void process();

private:
    string _msg;
    TcpConnectionPtr _con;
};


#endif