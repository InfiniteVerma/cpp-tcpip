#ifndef __RFC793_TIMER__
#define __RFC793_TIMER__

#include <chrono>
#include <ctime>
#include <vector>

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
    void addTask(ScheduledTask* task);
    void listTasks();
    void runTimeouts();

    static Timer* getInstance();

   private:
    Timer();
    ~Timer();
    vector<ScheduledTask*> scheduledTasks;

    static Timer* myTimerInstance;
};

#endif
