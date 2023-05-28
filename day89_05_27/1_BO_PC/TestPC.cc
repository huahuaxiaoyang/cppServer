#include "TaskQueue.h"
#include "Producer.h"
#include "Consumer.h"
#include"Thread.h"
#include <iostream>
#include <memory>
#include <unistd.h>

using std::cout;
using std::endl;
using std::unique_ptr;
using std::bind;

void test()
{
    TaskQueue taskQue(10);

    Producer pro(taskQue);
    Consumer con(taskQue);

    Thread thP(bind(&Producer::process,&pro));
    Thread thC(bind(&Consumer::process,&con));

    thP.start();
    thC.start();

    thP.join();
    thC.join();

}
int main(int argc, char **argv)
{
    // TaskQueue taskQue(10);
    // unique_ptr<Thread> pro(new Producer(taskQue));
    // unique_ptr<Thread> con(new Consumer(taskQue));

    // pro->start();
    // con->start();

    // pro->join();
    // con->join();

    test();

    /* MutexLock mutex; */
    /* MutexLock mutex2 = mutex;//error */
    return 0;
}

