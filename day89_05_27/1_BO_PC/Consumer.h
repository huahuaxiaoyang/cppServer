#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include "Thread.h"
#include "TaskQueue.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

class Consumer
{
public:
    Consumer(TaskQueue &taskQue)
    : _taskQue(taskQue)
    {

    }

    ~Consumer()
    {

    }

    void process()
    {
        int cnt = 20;
        while(cnt--)
        {
            int tmp = _taskQue.pop();
            cout << ">>Consumer consume tmp = " << tmp << endl;
            sleep(1);
        }
    }

private:
    TaskQueue &_taskQue;

};

#endif
