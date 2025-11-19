// SPDX-License-Identifier: MIT

#pragma once

#include "scheduler-plus-plus.hpp"

namespace scheduler{

class DefaultTimeProvider : public ITimeProvider {
public:
    DefaultTimeProvider();
    ~DefaultTimeProvider();

    std::chrono::time_point<std::chrono::high_resolution_clock> currentTime() override;
};

} // namespace scheduler
