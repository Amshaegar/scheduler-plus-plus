// SPDX-License-Identifier: MIT

#include "DefaultTimeProvider.hpp"

namespace scheduler{

DefaultTimeProvider::DefaultTimeProvider() {}

DefaultTimeProvider::~DefaultTimeProvider() {}

std::chrono::time_point<std::chrono::high_resolution_clock> scheduler::DefaultTimeProvider::currentTime() {
    return std::chrono::high_resolution_clock::now();
}

} // namespace scheduler
