cmake_minimum_required (VERSION 3.1.0)
project (rabbit)

add_executable (rabbit_test main.cpp)
enable_testing()
add_test(NAME rabbit_test COMMAND rabbit_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (rabbit_test PUBLIC ${BASE_PATH})
