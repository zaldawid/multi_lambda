cmake_minimum_required(VERSION 3.21)

project(multi_lambdas)

option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_BENCHMARKS "Enable benchmarks" ON)

add_subdirectory(include)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif(ENABLE_TESTS)

if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif(ENABLE_BENCHMARKS)

