cmake_minimum_required(VERSION 3.13.4)
project(vtab)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE test_srcs ${PROJECT_SOURCE_DIR}/*/*test.cc)
file(GLOB_RECURSE srcs ${PROJECT_SOURCE_DIR}/*/*.cc)
list(REMOVE_ITEM srcs ${test_srcs})

add_library(vtab SHARED ${srcs} ${PROJECT_SOURCE_DIR}/vtab.cc)

add_custom_target(tests)
include(CTest)

foreach(test_src ${test_srcs})
    get_filename_component(test_file_name ${test_src} NAME)
    string(REPLACE ".cc" "" test_name ${test_file_name})

    add_executable(${test_name} EXCLUDE_FROM_ALL ${test_src})
    add_dependencies(tests ${test_name})

    target_link_libraries(${test_name} vtab gtest_main gtest)
    add_test(${test_name} ${CMAKE_BINARY_DIR}/${test_name} --gtest_color=yes)
endforeach(test_src ${test_srcs})
