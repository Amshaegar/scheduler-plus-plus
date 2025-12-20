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

void TestTask::setExecutionTime(std::chrono::time_point<std::chrono::high_resolution_clock> time)
{
    m_executionTime = time;
}

std::chrono::time_point<std::chrono::high_resolution_clock> TestTask::executionTime() {
    return m_executionTime;
}

void TestTask::setName(const std::string &name)
{
    m_name = name;
}

std::string TestTask::name() const {
    return m_name;
}

void TestTask::setId(long id)
{
    m_id = id;
}

long TestTask::id() const
{
    return m_id;
}
