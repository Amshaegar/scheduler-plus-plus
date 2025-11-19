// SPDX-License-Identifier: MIT

#include "TestTimeProvider.hpp"

/*static*/ std::chrono::time_point<std::chrono::high_resolution_clock> TestTimeProvider::time;

TestTimeProvider::TestTimeProvider() {}

TestTimeProvider::~TestTimeProvider() {}

std::chrono::time_point<std::chrono::high_resolution_clock> TestTimeProvider::currentTime() {
    return time;
}
