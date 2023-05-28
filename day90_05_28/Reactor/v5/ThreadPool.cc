#include "ThreadPool.h"
#include "Thread.h"
#include <unistd.h>

ThreadPool::ThreadPool(size_t threadNum, size_t queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _taskQue(_queSize)
, _isExit(false)
{

}

ThreadPool::~ThreadPool()
{

}

//线程池的启动与停止
void ThreadPool::start()
{
    //1、将子线程创建出来，并且存放在vector
    for(size_t idx = 0; idx < _threadNum; ++idx)
    {
        unique_ptr<Thread> up(new Thread(std::bind(&ThreadPool::threadFunc, this)));
        _threads.push_back(std::move(up));
    }

    //2、遍历vector，然后将子线程全部运行起来
    for(auto &th : _threads)
    {
        th->start();
    }
}

void ThreadPool::stop()
{
    //如果任务队列中还有任务，那么工作线程是不能退出来的
    while(!_taskQue.empty())
    {
        sleep(1);
    }

    //1、遍历vector，然后将子线程全部停止运行
    _isExit = true;

    //将所有睡眠在_notEmpty上的线程唤醒
    _taskQue.wakeup();
    for(auto &th : _threads)
    {
        th->join();
    }

}

//添加任务与获取任务
void ThreadPool::addTask(Task &&task)
{
    if(task)
    {
        _taskQue.push(std::move(task));
    }
}

Task ThreadPool::getTask()
{
    return _taskQue.pop();
}

//线程池交给子线程做的任务
void ThreadPool::threadFunc()
{
    //如果子线程拿到任务，也就是执行getTask函数之后，然后去执行
    //任务，也就是调用process函数，如果process执行的比较快，那么
    //有可能_isExit还没有来得及设置为true，又进到while循环，再去
    //拿任务的时候，会调用getTask函数，而此时有可能已经没有任务
    //了，那么就会在此处调用pop函数，就会睡眠；那么如果process
    //函数执行的比较慢的话，那么_isExit就足够的时间将其设置为
    //true，那么线程在执行process之后，在就进不来while循环
    //就不会出现线程睡眠的问题
    while(!_isExit)
    {
        //1、线程池中的每个子线程必须先要获取到任务
        Task taskcb = getTask();
        if(taskcb)
        {
            taskcb();//执行任务
        }
    }
}
