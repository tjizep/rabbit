cmake_minimum_required (VERSION 3.1.0)
project (rabbit)

set (CMAKE_CXX_STANDARD 11)
SET(BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(RABBIT_TIME_HASH_MAPS         0  CACHE BOOL   "time hash maps.")
set(RABBIT_TESTS                  0  CACHE BOOL   "comparative tests.")
set(RABBIT_OWN_TESTS              0  CACHE BOOL   "own tests.")

if (RABBIT_TIME_HASH_MAPS)
add_executable (time_hash_maps time_hash_maps.cpp)
target_include_directories (time_hash_maps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(RABBIT_TESTS)
add_subdirectory(tests)
endif()

if(RABBIT_OWN_TESTS)
add_subdirectory(rabbit_tests)
endif()
