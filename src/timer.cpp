#include "timer.h"

#include <sched.h>

#include <cassert>
#include <ctime>
#include <iomanip>

#include "iostream"

void printTime(std::time_t time) { std::cout << std::put_time(std::localtime(&time), "%F %T"); }

ScheduledTask::ScheduledTask(double delta, FUNCTION callback, void* callbackData) {
    this->last_checked_time = time(nullptr);
    this->delta = delta;
    this->callback = callback;
    this->callbackData = callbackData;
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

bool ScheduledTask::executeCallback() { return this->callback(callbackData); }

Timer* Timer::myTimerInstance = nullptr;

Timer::Timer() {}

Timer::~Timer() {}

Timer* Timer::getInstance() {
    if (!myTimerInstance) {
        myTimerInstance = new Timer();
    }

    return myTimerInstance;
}

void Timer::addTimer(UINT32 seqNumber, ScheduledTask* task) {
    if (scheduledTasks.find(seqNumber) != scheduledTasks.end()) {
        assert(0);
    }
    scheduledTasks.insert({seqNumber, task});
}

void Timer::delTimers(const UINT32 ack_num) {
    LOG(__FUNCTION__, " \n\n\n\n\n");
    LOG(__FUNCTION__, " called for ack: ", ack_num);

    for(auto task : scheduledTasks) {
        if(task.first < ack_num) {
            LOG(__FUNCTION__, " erasing task with seq num: " + to_string(task.first));
            scheduledTasks.erase(task.first);
        }
    }

    LOG(__FUNCTION__, " scheduled task size now: " + to_string(scheduledTasks.size()));
}

void Timer::delTimer(UINT32 seqNumber) {
    LOG(__FUNCTION__, " called for seq: ", seqNumber);
    if (scheduledTasks.find(seqNumber) != scheduledTasks.end()) {
        LOG(__FUNCTION__, " found seqNumber: ", seqNumber);
        scheduledTasks.erase(seqNumber);
    } else {
        LOG(__FUNCTION__, " ERROR didn't find matching timer, available timers:", scheduledTasks.size());
        for (auto data : scheduledTasks) {
            LOG(__FUNCTION__, " seq num: ", data.first);
        }
    }
}

void Timer::listTasks() { cout << "Listing timer tasks. Count: " << scheduledTasks.size() << "\n"; }

void Timer::runTimeouts() {
    LOG(__FUNCTION__, " timers: ", scheduledTasks.size());

    for(auto it = scheduledTasks.begin(); it != scheduledTasks.end(); ) {
        if(it->second->hasElapsed()) {
            LOG(__FUNCTION__, "seq number: ", it->first, " has elapsed. Calling timeout");
            bool ret = it->second->executeCallback();
            if(ret) {
                LOG(__FUNCTION__, "callback returned true, deleting from timer list");
                it = scheduledTasks.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }

    LOG(__FUNCTION__, " DONE timers: ", scheduledTasks.size());
}
