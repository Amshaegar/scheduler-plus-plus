# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.15)

project(scheduler-plus-plus CXX)

add_library(scheduler-plus-plus STATIC
    DefaultTimeProvider.cpp
    DefaultTimeProvider.hpp
    scheduler-plus-plus.cpp
    scheduler-plus-plus.hpp
)

target_include_directories(
    scheduler-plus-plus PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(scheduler-plus-plus PRIVATE gtest)
target_compile_features(scheduler-plus-plus PUBLIC cxx_std_17)
set_target_properties(scheduler-plus-plus PROPERTIES CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(scheduler-plus-plus PUBLIC /W4 /WX)
else()
    target_compile_options(scheduler-plus-plus PUBLIC -Wall -Wextra -Werror -pedantic -std=c++20)
endif()
