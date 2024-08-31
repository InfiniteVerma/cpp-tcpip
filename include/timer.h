#ifndef __RFC793_TIMER__
#define __RFC793_TIMER__

#include <chrono>
#include <ctime>
#include <vector>

using namespace std;

class ScheduledTask {
   public:
    bool hasElapsed();
    ScheduledTask(chrono::system_clock::time_point start_point, chrono::milliseconds delta);
    void executeCallback();

   private:
    chrono::system_clock::time_point start_point;
    chrono::milliseconds delta;
};

class Timer {
   public:
    void addTask(ScheduledTask task);
    void listTasks();

    static Timer* getInstance();

   private:
    Timer();
    ~Timer();
    vector<ScheduledTask> scheduledTasks;

    static Timer* myTimerInstance;
};

#endif
