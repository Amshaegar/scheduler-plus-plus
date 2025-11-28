// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <memory>
#include <queue>

#include <gtest/gtest.h>

namespace scheduler{

class ITimeProvider {
public:
    ITimeProvider(){}
    virtual ~ITimeProvider(){};

    virtual std::chrono::time_point<std::chrono::high_resolution_clock> currentTime() = 0;
};

class ITask {
public:
    ITask(){}
    virtual ~ITask(){}

    virtual void execute() = 0;
    virtual std::chrono::time_point<std::chrono::high_resolution_clock> executionTime() = 0;
    virtual std::string name() const = 0;
};

class Scheduler {
public:
    Scheduler(const Scheduler& other) = delete;
    static Scheduler* getInstance(std::unique_ptr<ITimeProvider> timeProvider);

    void scheduleTask(std::unique_ptr<ITask> task);
    void processQueue();

private:
    Scheduler();
    Scheduler(std::unique_ptr<ITimeProvider> timeProvider);

    FRIEND_TEST(SchedulerTest, SortedTasksQueue);

    void processQueueWithTimeout(std::chrono::duration<double, std::milli> delay_ms);

    std::unique_ptr<ITimeProvider> m_timeProvider;
    std::priority_queue<std::unique_ptr<ITask>,
                        std::vector<std::unique_ptr<ITask>>,
                        decltype([](const std::unique_ptr<ITask>& l, const std::unique_ptr<ITask>& r) {
                            return l->executionTime() > r->executionTime();
                        })> m_queue;
};

} // namespace scheduler
