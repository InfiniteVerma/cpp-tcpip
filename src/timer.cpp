#include "timer.h"

#include <sched.h>

#include <ctime>
#include <iomanip>

#include "iostream"

void printTime(std::time_t time) { std::cout << std::put_time(std::localtime(&time), "%F %T"); }

ScheduledTask::ScheduledTask(double delta, FUNCTION callback) {
    this->last_checked_time = time(nullptr);
    this->delta = delta;
    this->callback = callback;
}

bool ScheduledTask::hasElapsed() {
    // cout << __FUNCTION__ << " BEGIN\n";
    std::time_t current_time = time(nullptr);

    double elapsed_seconds = difftime(current_time, last_checked_time);

    bool isElapsed = elapsed_seconds >= delta;

    if (isElapsed) {
        // Update last checked time
        last_checked_time = current_time;
        // std::cout << "ELAPSED!!\n";
        // std::cout << "diff: " << elapsed_seconds << " seconds\n";
        // std::cout << "delta: " << delta << " seconds\n";
        // std::cout << "currentTime: ";
        // printTime(current_time);
        // std::cout << " - lastCheckedTime: ";
        // printTime(last_checked_time);
    }

    return isElapsed;
}

void ScheduledTask::executeCallback() { this->callback(); }

Timer* Timer::myTimerInstance = nullptr;

Timer::Timer() {}

Timer::~Timer() {}

Timer* Timer::getInstance() {
    if (!myTimerInstance) {
        myTimerInstance = new Timer();
    }

    return myTimerInstance;
}

void Timer::addTimer(UINT32 seqNumber, ScheduledTask* task) { scheduledTasks.insert({seqNumber, task}); }

void Timer::delTimer(UINT32 seqNumber) {
    LOG(__FUNCTION__, " called for seq: ", seqNumber);
    if (scheduledTasks.find(seqNumber) != scheduledTasks.end()) {
        LOG(__FUNCTION__, " found seqNumber: ", seqNumber);
        scheduledTasks.erase(seqNumber);
    } else {
        LOG(__FUNCTION__, " ERROR didn't find matching timer, available timers:");
        for (auto data : scheduledTasks) {
            LOG(__FUNCTION__, " seq num: ", data.first);
        }
    }
}

void Timer::listTasks() { cout << "Listing timer tasks. Count: " << scheduledTasks.size() << "\n"; }

void Timer::runTimeouts() {
    LOG(__FUNCTION__, " timers: ", scheduledTasks.size());
    for (auto data : scheduledTasks) {
        if (data.second->hasElapsed()) {
            LOG(__FUNCTION__, "seq number: ", data.first, " has elapsed. Calling timeout");
            data.second->executeCallback();
        }
    }
}
