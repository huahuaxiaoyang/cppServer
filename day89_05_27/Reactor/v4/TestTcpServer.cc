#include "TcpServer.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

ThreadPool *gPool = nullptr;

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {

    }
    void process()
    {
        //处理msg
        /* _msg;//就是需要进行处理的msg */
        //进行业务逻辑的处理
        //.....
        //....
        //...
        //将数据的发送交给EventLoop
        //数据在线程池中处理好之后，不能直接使用send函数
        //将其发送给Reactor（EventLoop），发送数据的应该
        //是EventLoop对应的线程
        //Q:什么时候让线程池将数据发送给EventLoop？
        //A：线程池将任务处理好之后，就可以进行发送了
        //Q:如何发送呢？
        //A:可以在线程池中使用连接TcpConnection将数据发送
        //给EventLoop
        //Q:涉及到线程池与EventLoop之间的通信问题？
        //A:进程或者线程之间的通信可以使用eventfd的方法
        _con->sendInLoop(_msg);
        //数据需要发送给EventLoop，而具有发送数据能力只有
        //TcpConnection，所以需要TcpConnection将数据发送
        //给EventLoop/Reactor
        //所以TcpConnection必须要知道EventLoop存在
    }

private:
    string _msg;
    TcpConnectionPtr _con;
};

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
    /* gPool->addTask(std::bind(&MyTask::process, &task));//&task */
    gPool->addTask(std::bind(&MyTask::process, task));//&task
}

void onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has closed!!!" << endl;
}

void test()
{
    ThreadPool pool(4, 10);
    pool.start();//四个子线程会执行pop操作，会在此阻塞
    gPool = &pool;

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

