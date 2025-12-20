// SPDX-License-Identifier: MIT

#include <scheduler-plus-plus.hpp>

#include "mocks/TestTask.hpp"
#include "mocks/TestTimeProvider.hpp"

#include <gtest/gtest.h>

namespace scheduler{

using namespace std::literals;

class SchedulerTest : public testing::Test {
protected:
    void TearDown() override {
        TestTask::isExecuted.clear();
    }

    std::map<std::string,std::function<ITask*()>> tasksRegistry;
};

TEST_F(SchedulerTest, ExecuteNow) {
    // Arrange
    auto nowTime = std::chrono::high_resolution_clock::now();
    TestTimeProvider::time = nowTime;
    std::string taskName = "TestTask";
    TestTask::isExecuted[taskName] = false;
    Scheduler scheduler = Scheduler(":memmory:", tasksRegistry, std::make_unique<TestTimeProvider>());

    // Act
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime, taskName));

    // Assert
    EXPECT_TRUE(TestTask::isExecuted.at(taskName));
}

TEST_F(SchedulerTest, ExecuteInSpecificTime) {
    // Arrange
    std::string taskName = "TestTask";
    TestTask::isExecuted[taskName] = false;
    auto nowTime = std::chrono::high_resolution_clock::now();
    auto taskExecutionTime = nowTime + 5min;
    TestTimeProvider::time = nowTime;
    Scheduler scheduler = Scheduler(":memmory:", tasksRegistry, std::make_unique<TestTimeProvider>());
    scheduler.scheduleTask(std::make_unique<TestTask>(taskExecutionTime, taskName));
    EXPECT_FALSE(TestTask::isExecuted.at(taskName));
    TestTimeProvider::time = taskExecutionTime + 1min;

    // Act
    scheduler.processQueue();

    // Assert
    EXPECT_TRUE(TestTask::isExecuted.at(taskName));
}

TEST_F(SchedulerTest, SortedTasksQueue) {
    // Arrange
    auto nowTime = std::chrono::high_resolution_clock::now();
    TestTimeProvider::time = nowTime;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> expectedTimes {{nowTime + 1min},
                                                                                           {nowTime + 5min},
                                                                                           {nowTime + 6min}};
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> actualTimes;
    std::string taskName = "TestTask";
    Scheduler scheduler = Scheduler(":memmory:", tasksRegistry, std::make_unique<TestTimeProvider>());

    // Act
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime + 5min, taskName));
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime + 1min, taskName));
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime + 6min, taskName));

    // Assert
    for (; !scheduler.m_queue.empty(); scheduler.m_queue.pop()) {
        actualTimes.push_back(scheduler.m_queue.top()->executionTime());
    }
    EXPECT_EQ(expectedTimes, actualTimes);
}

TEST_F(SchedulerTest, AllScheduledTasksAreExecuted) {
    // Arrange
    auto nowTime = std::chrono::high_resolution_clock::now();
    TestTimeProvider::time = nowTime;
    TestTask::isExecuted["TestTask0"] = false;
    TestTask::isExecuted["TestTask1"] = false;
    TestTask::isExecuted["TestTask2"] = false;
    TestTask::isExecuted["TestTask3"] = false;
    Scheduler scheduler = Scheduler(":memmory:", tasksRegistry, std::make_unique<TestTimeProvider>());
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime, "TestTask0"));
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime + 5min, "TestTask1"));
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime + 1min, "TestTask2"));
    scheduler.scheduleTask(std::make_unique<TestTask>(nowTime + 6min, "TestTask3"));
    TestTimeProvider::time = nowTime + 10min;

    // Act
    scheduler.processQueue();

    // Assert
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask0"));
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask1"));
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask2"));
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask3"));
}

TEST_F(SchedulerTest, PersistTasks) {
    // TODO add check of task execution time after scheduler recreate
    // TODO add check tasks list after scheduler recreate
    // Arrange
    std::string taskName = "TestTask";
    tasksRegistry[taskName] = [](){return new TestTask();}; // TODO move to fixture
    TestTask::isExecuted[taskName] = false;
    auto nowTime = std::chrono::high_resolution_clock::now();
    auto taskExecutionTime = nowTime + 5min;
    TestTimeProvider::time = nowTime;
    Scheduler* schedulerBefore = new Scheduler("scheduler.db", tasksRegistry, std::make_unique<TestTimeProvider>());
    schedulerBefore->scheduleTask(std::make_unique<TestTask>(taskExecutionTime, taskName));
    EXPECT_FALSE(TestTask::isExecuted.at(taskName));
    delete schedulerBefore;
    TestTimeProvider::time = taskExecutionTime + 1min;

    // Act
    Scheduler* schedulerAfter = new Scheduler("scheduler.db", tasksRegistry, std::make_unique<TestTimeProvider>());

    // Assert
    delete schedulerAfter;
    EXPECT_TRUE(TestTask::isExecuted.at(taskName));
}

} // namespace scheduler
