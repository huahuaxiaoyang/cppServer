#include"MyTask.h"


MyTask::MyTask(const string &msg, const TcpConnectionPtr &con)
: _msg(msg)
, _con(con)
{
    
}

void MyTask::process()
{

    //完成任务
    for(int i=0;i<_msg.size();++i)
    {
        _msg[i]=toupper(_msg[i]);
    }
    _con->sendInLoop(_msg);
}