cmake_minimum_required(VERSION 3.0)

project(parse_messages)

find_package(PkgConfig REQUIRED)

# Dependencies
find_package(Boost 1.57.0 REQUIRED COMPONENTS locale)
pkg_check_modules(XERCES_C REQUIRED xerces-c)

find_path(VSQLITEPP_INCLUDE_DIR sqlite/connection.hpp)

if(NOT DEFINED VSQLITEPP_INCLUDE_DIR)
    message(FATAL_ERROR "VSQlite++ include path not found")
endif()

find_library(VSQLITEPP_LIBRARY vsqlitepp)

if(NOT DEFINED VSQLITEPP_LIBRARY)
    message(FATAL_ERROR "VSQlite++ library not found")
endif()

message(STATUS "VSQlite++ found.")
message(STATUS "  Include path: ${VSQLITEPP_INCLUDE_DIR}")
message(STATUS "  Library path: ${VSQLITEPP_LIBRARY}")

include_directories(${Boost_INCLUDE_DIRS} ${XERCES_C_INCLUDE_DIRS} ${VSQLITEPP_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")

aux_source_directory(${CMAKE_SOURCE_DIR} SOURCES)
add_executable(parse_messages ${SOURCES})

target_link_libraries(parse_messages ${Boost_LIBRARIES} ${XERCES_C_LIBRARIES} ${VSQLITEPP_LIBRARY})
