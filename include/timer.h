#ifndef __RFC793_TIMER__
#define __RFC793_TIMER__

#include <chrono>
#include <ctime>
#include <vector>

#include "common.h"
#include "socket.h"

using namespace std;

typedef bool (*FUNCTION)(void*);

class ScheduledTask {
   public:
    bool hasElapsed();
    ScheduledTask(double delta, FUNCTION func, void* callbackData);
    bool executeCallback();

   private:
    time_t last_checked_time;
    double delta;
    FUNCTION callback;
    void* callbackData;
};

class Timer {
   public:
    void addTimer(UINT32 seq, ScheduledTask* task);
    void delTimer(UINT32 seq);
    void delTimers(const UINT32 ack);
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
