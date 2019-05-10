cmake_minimum_required(VERSION 3.13)
project(vtab)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})


add_library(coverage_config INTERFACE)

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(coverage_config INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(coverage_config INTERFACE --coverage)
  else()
    target_link_libraries(coverage_config INTERFACE --coverage)
  endif()
endif()


include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE test_srcs ${PROJECT_SOURCE_DIR}/*/*test.cc)
list(APPEND test_srcs ${PROJECT_SOURCE_DIR}/vtab_test.cc)
file(GLOB_RECURSE srcs ${PROJECT_SOURCE_DIR}/*/*.cc)
list(REMOVE_ITEM srcs ${test_srcs})

add_library(vtab SHARED ${srcs} ${PROJECT_SOURCE_DIR}/vtab.cc)
target_compile_options(vtab PUBLIC -Wall -fPIC)
target_link_libraries(vtab PUBLIC coverage_config)

add_custom_target(tests)
include(CTest)

foreach(test_src ${test_srcs})
    get_filename_component(test_file_name ${test_src} NAME)
    string(REPLACE ".cc" "" test_name ${test_file_name})

    add_executable(${test_name} EXCLUDE_FROM_ALL ${test_src})
    add_dependencies(tests ${test_name})
    target_link_libraries(${test_name} vtab gtest_main gtest sqlite3)

    add_test(${test_name} ${CMAKE_BINARY_DIR}/${test_name} --gtest_color=yes)
endforeach(test_src ${test_srcs})
