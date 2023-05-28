#ifndef __THREAD_H__
#define __THREAD_H__

#include "NonCopyable.h"
#include <pthread.h>
#include <functional>

using std::function;

class Thread
: NonCopyable
{
    using ThreadCallback = function<void()>;
public:
    Thread(ThreadCallback &&cb);
     ~Thread();

    //线程运行函数
    void start();
    //线程退出函数
    void join();

private:
    //线程入口函数
    static void *threadFunc(void *arg);
    //执行的任务，抽象的任务，留个派生类使用
    //virtual void run() = 0;

private:
    pthread_t _thid;//线程id
    bool _isRunning;//线程是否运行的标志
    ThreadCallback _cb;//存任务的consumer


};

#endif
