# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.15)

project(scheduler-plus-plus_tests CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.16.0
)
FetchContent_MakeAvailable(googletest)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(scheduler-plus-plus_tests
    main.cpp
    mocks/TestTask.cpp
    mocks/TestTask.hpp
    mocks/TestTimeProvider.cpp
    mocks/TestTimeProvider.hpp
    scheduler-plus-plus_tests.cpp
)

target_include_directories(
    scheduler-plus-plus_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

target_link_libraries(scheduler-plus-plus_tests PRIVATE gtest)
target_link_libraries(scheduler-plus-plus_tests PUBLIC scheduler-plus-plus)
target_compile_features(scheduler-plus-plus_tests PUBLIC cxx_std_17)
set_target_properties(scheduler-plus-plus_tests PROPERTIES CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(scheduler-plus-plus_tests PUBLIC /W4 /WX)
else()
    target_compile_options(scheduler-plus-plus_tests PUBLIC -Wall -Wextra -Werror -pedantic -std=c++20)
endif()
