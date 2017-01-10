cmake_minimum_required (VERSION 3.1.0)
project (rabbit)

add_executable (test_ main.cpp)
enable_testing()
add_test(NAME test_ COMMAND test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (test_ PUBLIC ${BASE_PATH})