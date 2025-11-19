// SPDX-License-Identifier: MIT

#pragma once

#include <scheduler-plus-plus.hpp>

class TestTimeProvider : public scheduler::ITimeProvider {
public:
    TestTimeProvider();
    ~TestTimeProvider();

    std::chrono::time_point<std::chrono::high_resolution_clock> currentTime() override;

    static std::chrono::time_point<std::chrono::high_resolution_clock> time;
};
