cmake_minimum_required(VERSION 3.0)

project(parse_messages)

find_package(PkgConfig)

# Dependencies
find_package(Boost 1.57.0 COMPONENTS locale)
pkg_check_modules(XERCES_C xerces-c)

include_directories(${Boost_INCLUDE_DIRS} ${XERCES_C_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

add_executable(parse_messages parse_messages.cpp MySAX2Handler.cpp MyInputSource.cpp MyBinInputStream.cpp util.cpp)

target_link_libraries(parse_messages ${Boost_LIBRARIES} ${XERCES_C_LIBRARIES})
