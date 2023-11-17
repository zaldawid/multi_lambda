option(USE_LIBCXX "Use libc++ with Clang" OFF)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(BENCHMARK_USE_LIBCXX ${USE_LIBCXX})
endif()

set(BENCHMARK_ENABLE_LIBPFM OFF)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

include(FetchContent)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)


FetchContent_MakeAvailable(googlebenchmark)