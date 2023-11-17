
option( ENABLE_ASAN "Enable address sanitizer" ON )
option( ENABLE_UBSAN "Enable undefined behavior sanitizer" ON )

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
  )

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

set(tests_target multi_lambda_tests)

add_executable( ${tests_target} )

target_sources( ${tests_target}
    PRIVATE
        traits_tests.cpp
    )

target_compile_features( ${tests_target}
    PRIVATE
        cxx_std_20
    )

set_target_properties( ${tests_target}
    PROPERTIES
        CXX_EXTENSIONS OFF
    )

# msvc optimizations and warnings
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/W4 /permissive- /Od /Ob0 /Oi /Ot /Oy /GL>)
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/W4 /permissive- /O2 /Ob2 /Oi /Ot /Oy /GL>)


# clang and gnu optimizations and warnings for debug builds:
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:-Wall -Wextra -Wpedantic -O0 -ggdb3>)
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-Wall -Wextra -Wpedantic -O0 -ggdb3>)

# clang and gnu optimizations and warnings for release builds:
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Release>>:-Wall -Wextra -Wpedantic -O3 -march=native -flto -fno-fat-lto-objects>)
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-Wall -Wextra -Wpedantic -O3 -march=native -flto -fno-fat-lto-objects>)

# sanitizers if enabled via cmake option
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${ENABLE_ASAN}>>:-fsanitize=address>)
target_link_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${ENABLE_ASAN}>>:-fsanitize=address>)
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${ENABLE_UBSAN}>>:-fsanitize=undefined>)
target_link_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${ENABLE_UBSAN}>>:-fsanitize=undefined>)

target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${ENABLE_ASAN}>>:-fsanitize=address>)
target_link_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${ENABLE_ASAN}>>:-fsanitize=address>)
target_compile_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${ENABLE_UBSAN}>>:-fsanitize=undefined>)
target_link_options(${tests_target} PRIVATE $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${ENABLE_UBSAN}>>:-fsanitize=undefined>)

target_link_libraries( multi_lambda_tests GTest::gtest_main )
target_link_libraries( multi_lambda_tests multi_lambda::multi_lambda )

include(GoogleTest)
gtest_discover_tests(${tests_target})