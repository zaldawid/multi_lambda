add_library(multi_lambda INTERFACE)

target_include_directories(multi_lambda INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_features(multi_lambda INTERFACE cxx_std_20)

set_target_properties(multi_lambda PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON)

add_library(multi_lambda::multi_lambda ALIAS multi_lambda)