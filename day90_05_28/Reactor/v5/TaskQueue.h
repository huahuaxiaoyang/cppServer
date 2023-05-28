#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include "MutexLock.h"
#include "Condition.h"
#include <queue>
#include <functional>

using std::queue;
using std::function;

using ElemType = function<void()>;

class TaskQueue
{
public:
    TaskQueue(size_t queSize);
    ~TaskQueue();

    //任务队列空与满
    bool empty() const;
    bool full() const;
    //插入数据与取出数据
    void push(ElemType &&task);
    ElemType pop();

    //唤醒所有等待在_notEmpty条件变量上的线程
    void wakeup();

private:
    size_t _queSize;//队列大小
    queue<ElemType> _que;//真正存放数据的容器
    MutexLock _mutex;
    Condition _notEmpty;
    Condition _notFull;

    bool _flag;//标志位
};

#endif
