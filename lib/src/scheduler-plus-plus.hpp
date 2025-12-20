// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <memory>
#include <queue>
#include <map>
#include <functional>

#include <sqlite3.h>
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
    ITask() = default;
    virtual ~ITask() = default;

    virtual void execute() = 0;
    virtual void setExecutionTime(std::chrono::time_point<std::chrono::high_resolution_clock> time) = 0;
    virtual std::chrono::time_point<std::chrono::high_resolution_clock> executionTime() = 0;
    virtual void setName(const std::string &name) = 0;
    virtual std::string name() const = 0;
    virtual void setId(long id) = 0;
    virtual long id() const = 0;
};

class Scheduler {
public:
    Scheduler() = delete;
    Scheduler(const Scheduler& other) = delete;
    Scheduler(const std::string& dbPath,
              const std::map<std::string, std::function<ITask*()>>& tasksRegistry,
              std::unique_ptr<ITimeProvider> timeProvider);
    ~Scheduler();

    void scheduleTask(std::unique_ptr<ITask> task);
    void processQueue();

private:
    FRIEND_TEST(SchedulerTest, SortedTasksQueue);

    void processQueueWithTimeout(std::chrono::duration<double, std::milli> delay_ms);

    void initDb();
    void createDbIfNeeded();
    void loadTasksFromDb();
    long saveTaskToDb(const std::string& taskName,
                      std::chrono::time_point<std::chrono::high_resolution_clock> taskExecutionTime);
    void removeTaskFromDb(long id);

    std::string m_dbName;
    // TODO use reflection instead of this registry approach
    std::map<std::string, std::function<ITask*()>> m_tasksRegistry;
    sqlite3* m_db = nullptr;
    std::unique_ptr<ITimeProvider> m_timeProvider;
    std::priority_queue<std::unique_ptr<ITask>,
                        std::vector<std::unique_ptr<ITask>>,
                        decltype([](const std::unique_ptr<ITask>& l, const std::unique_ptr<ITask>& r) {
                            return l->executionTime() > r->executionTime();
                        })> m_queue;
};

} // namespace scheduler
