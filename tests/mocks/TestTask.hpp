// SPDX-License-Identifier: MIT

#pragma once

#include <scheduler-plus-plus.hpp>

#include <map>

class TestTask : public scheduler::ITask {
public:
    TestTask(std::chrono::time_point<std::chrono::high_resolution_clock> time, const std::string& name);
    virtual ~TestTask(){}

    void execute() override;
    std::chrono::time_point<std::chrono::high_resolution_clock> executionTime() override;
    std::string name() const override;

    static std::map<std::string, bool> isExecuted;
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_executionTime;
    std::string m_name;
};
