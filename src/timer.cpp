// clang-format off
#include "timer.h"
#include <sched.h>
#include <chrono>
#include "iostream"
// clang-format on

ScheduledTask::ScheduledTask(chrono::system_clock::time_point start_point, chrono::milliseconds delta) {
    this->start_point = start_point;
    this->delta = delta;
}

bool ScheduledTask::hasElapsed() {
    chrono::system_clock::time_point currentTime = chrono::system_clock::now();

    bool isElapsed = (currentTime - start_point) > delta;

    if (isElapsed) start_point = currentTime;

    return isElapsed;
}

void ScheduledTask::executeCallback() { std::cout << "Timer elapsed, executing callback!\n\n"; }

Timer* Timer::myTimerInstance = nullptr;

Timer::Timer() {}

Timer::~Timer() {}

Timer* Timer::getInstance() {
    if (!myTimerInstance) {
        myTimerInstance = new Timer();
    }

    return myTimerInstance;
}

void Timer::addTask(ScheduledTask task) { scheduledTasks.push_back(task); }

void Timer::listTasks() { cout << "Listing timer tasks. Count: " << scheduledTasks.size() << "\n"; }
