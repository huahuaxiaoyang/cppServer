#include "TaskQueue.h"

TaskQueue::TaskQueue(size_t queSize)
: _queSize(queSize)
, _que()
, _mutex()
, _notEmpty(_mutex)
, _notFull(_mutex)
{

}

TaskQueue::~TaskQueue()
{

}

//任务队列空与满
bool TaskQueue::empty() const
{
    return _que.size() == 0;
}

bool TaskQueue::full() const
{
    return _queSize == _que.size();
}

//插入数据与取出数据
void TaskQueue::push(const int &value)
{
    //上锁
    /* _mutex.lock(); */
    //为了防止死锁，可以使用RAII的思想
    MutexLockGuard autoLock(_mutex);

    //考虑是否为满的情况
    while(full())
    {
        _notFull.wait();//睡眠
    }

    //生产任务，并放在数据结构queue中
    _que.push(value);

    /* if() */
    /* { */
    /*     return; */
    /* } */

    //当生产了数据之后，就需要唤醒在_notEmpty条件
    //变量上的线程
    _notEmpty.notify();

    //上锁
    /* _mutex.unlock(); */
}

int TaskQueue::pop()
{
    //上锁
    /* _mutex.lock(); */
    MutexLockGuard autoLock(_mutex);

    //是否为空
    while(empty())//虚假唤醒
    {
        _notEmpty.wait();//睡眠
    }

    //获取元素，并且将第一个元素删除掉
    int tmp = _que.front();
    _que.pop();

    //当消费了一个数据之后，任务队列中至少有
    //一个空位置，所以需要唤醒_notFull的线程
    _notFull.notify();
    
    //上锁
    /* _mutex.unlock(); */

    return tmp;
}
