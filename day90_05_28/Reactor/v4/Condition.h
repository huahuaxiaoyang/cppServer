#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "NonCopyable.h"
/* #include "MutexLock.h" */
#include <pthread.h>

class MutexLock;//类的前向声明

class Condition
: NonCopyable
{
public:
    Condition(MutexLock &mutex);
    ~Condition();

    void wait();
    void notify();
    void notifyAll();

private:
    pthread_cond_t _cond;
    MutexLock &_mutex;

};

#endif
