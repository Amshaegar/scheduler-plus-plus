// SPDX-License-Identifier: MIT

#include "TestTask.hpp"

/*static*/ std::map<std::string, bool> TestTask::isExecuted;

TestTask::TestTask(std::chrono::time_point<std::chrono::high_resolution_clock> time, const std::string &name)
    : m_executionTime(time)
    , m_name(name)
{}

void TestTask::execute() {
    isExecuted[m_name] = true;
}

std::chrono::time_point<std::chrono::high_resolution_clock> TestTask::executionTime() {
    return m_executionTime;
}

std::string TestTask::name() const {
    return m_name;
}
