#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#include "NonCopyable.h"
#include <pthread.h>

class MutexLock
: NonCopyable
{
public:
    //y/d + 目标行号 + G
    MutexLock();
    ~MutexLock();

    //上锁
    void lock();
    void tryLock();
    //解锁
    void unlock();

    pthread_mutex_t *getMutexLockPtr()
    {
        return &_mutex;
    }

private:
    pthread_mutex_t _mutex;

};

class MutexLockGuard
{
public:
    MutexLockGuard(MutexLock &mutex)
    : _mutex(mutex)
    {
        _mutex.lock();
    }

    ~MutexLockGuard()
    {
        _mutex.unlock();
    }
private:
    MutexLock &_mutex;

};

#endif
