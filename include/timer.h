#ifndef __RFC793_TIMER__
#define __RFC793_TIMER__

#include <chrono>
#include <ctime>
#include <vector>

#include "common.h"

using namespace std;

typedef void (*FUNCTION)(void);

class ScheduledTask {
   public:
    bool hasElapsed();
    ScheduledTask(double delta, FUNCTION func);
    void executeCallback();

   private:
    time_t last_checked_time;
    double delta;
    FUNCTION callback;
};

class Timer {
   public:
    void addTimer(UINT32 seq, ScheduledTask* task);
    void delTimer(UINT32 seq);
    void listTasks();
    void runTimeouts();

    static Timer* getInstance();

   private:
    Timer();
    ~Timer();
    map<UINT32, ScheduledTask*> scheduledTasks;

    static Timer* myTimerInstance;
};

#endif
