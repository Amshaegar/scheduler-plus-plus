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

    static Scheduler* scheduler;
};

/*static*/ Scheduler* SchedulerTest::scheduler = Scheduler::getInstance(std::make_unique<TestTimeProvider>());

TEST_F(SchedulerTest, ExecuteNow) {
    // Arrange
    auto nowTime = std::chrono::high_resolution_clock::now();
    TestTimeProvider::time = nowTime;
    std::string taskName = "TestTask";
    TestTask::isExecuted[taskName] = false;

    // Act
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime, taskName));

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
    scheduler->scheduleTask(std::make_unique<TestTask>(taskExecutionTime, taskName));
    EXPECT_FALSE(TestTask::isExecuted.at(taskName));
    TestTimeProvider::time = taskExecutionTime + 1min;

    // Act
    scheduler->processQueue();

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

    // Act
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime + 5min, taskName));
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime + 1min, taskName));
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime + 6min, taskName));

    // Assert
    for (; !scheduler->m_queue.empty(); scheduler->m_queue.pop()) {
        actualTimes.push_back(scheduler->m_queue.top()->executionTime());
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
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime, "TestTask0"));
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime + 5min, "TestTask1"));
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime + 1min, "TestTask2"));
    scheduler->scheduleTask(std::make_unique<TestTask>(nowTime + 6min, "TestTask3"));
    TestTimeProvider::time = nowTime + 10min;

    // Act
    scheduler->processQueue();

    // Assert
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask0"));
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask1"));
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask2"));
    EXPECT_TRUE(TestTask::isExecuted.at("TestTask3"));
}

} // namespace scheduler
