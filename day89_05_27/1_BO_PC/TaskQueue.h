#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include "MutexLock.h"
#include "Condition.h"
#include <queue>

using std::queue;

class TaskQueue
{
public:
    TaskQueue(size_t queSize);
    ~TaskQueue();

    //任务队列空与满
    bool empty() const;
    bool full() const;
    //插入数据与取出数据
    void push(const int &value);
    int pop();

private:
    size_t _queSize;//队列大小
    queue<int> _que;//真正存放数据的容器
    MutexLock _mutex;
    Condition _notEmpty;
    Condition _notFull;
};

#endif
