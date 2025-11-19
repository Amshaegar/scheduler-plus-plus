// SPDX-License-Identifier: MIT

#include <scheduler-plus-plus.hpp>

#include <gtest/gtest.h>

class TestTask : public scheduler::ITask {
public:
    TestTask(std::chrono::time_point<std::chrono::high_resolution_clock> time)
        : m_executionTime(time)
    {}
    ~TestTask(){}

    void execute() override {
        isExecuted = true;
    }
    virtual std::chrono::time_point<std::chrono::high_resolution_clock> executionTime() override {
        return m_executionTime;
    }

    static bool isExecuted;
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_executionTime;
};
bool TestTask::isExecuted = false;

TEST(Scheduler, ExecuteNow) {
    // Arrange
    scheduler::Scheduler* scheduler = scheduler::Scheduler::getInstance();
    TestTask::isExecuted = false;

    // Act
    scheduler->scheduleTask(std::make_unique<TestTask>(std::chrono::high_resolution_clock::now()));

    // Assert
    EXPECT_TRUE(TestTask::isExecuted);
}

TEST(Scheduler, ExecuteInSpecificTime) {
    // Arrange
    using namespace std::literals;
    scheduler::Scheduler* scheduler = scheduler::Scheduler::getInstance();
    TestTask::isExecuted = false;

    // Act
    scheduler->scheduleTask(std::make_unique<TestTask>(std::chrono::high_resolution_clock::now() + 5min));

    // Assert
    EXPECT_TRUE(TestTask::isExecuted);
}
