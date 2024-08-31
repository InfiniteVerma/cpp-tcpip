#include "timer.h"

#include <sched.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>

#include "iostream"

void printTime(std::time_t time) { std::cout << std::put_time(std::localtime(&time), "%F %T"); }

ScheduledTask::ScheduledTask(double delta) {
    this->last_checked_time = time(nullptr);
    this->delta = delta;
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

void ScheduledTask::executeCallback() { std::cout << "ANANT Timer elapsed, executing callback!!\n\n"; }

Timer* Timer::myTimerInstance = nullptr;

Timer::Timer() {}

Timer::~Timer() {}

Timer* Timer::getInstance() {
    if (!myTimerInstance) {
        myTimerInstance = new Timer();
    }

    return myTimerInstance;
}

void Timer::addTask(ScheduledTask* task) { scheduledTasks.push_back(task); }

void Timer::listTasks() { cout << "Listing timer tasks. Count: " << scheduledTasks.size() << "\n"; }

void Timer::runTimeouts() {
    for (ScheduledTask* task : scheduledTasks) {
        if (task->hasElapsed()) {
            task->executeCallback();
        }
    }
}
