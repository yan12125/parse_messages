# Auto link to qtmain.lib on Windows requires cmake 2.8.11+
# See CMP0020 http://www.cmake.org/cmake/help/v3.0/policy/CMP0020.html
cmake_minimum_required(VERSION 2.8.11)

project(parse_messages)

set(CMAKE_AUTOMOC ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
# Dependencies
find_package(Boost 1.54.0 REQUIRED COMPONENTS locale)
find_package(VSQLitePP)
find_package(Qt5 COMPONENTS Xml Sql Widgets REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${VSQLITEPP_INCLUDE_DIR} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/common")

# QT_NO_EMIT prevent Qt headers from defining emit, which is used in vsqlite++
# FIXME: QT_NO_EMIT is undocumented
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61841
# Debian-based system require pthread if using std::thread
set(CXX_FLAGS_COMMON "-std=c++14 -Wall -Wextra -DQT_NO_EMIT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS_COMMON}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS_COMMON} -DMY_DEBUG")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

foreach(sub_folder parse_messages db2texts merger)
    aux_source_directory("${CMAKE_SOURCE_DIR}/${sub_folder}" SOURCES_${sub_folder})
    add_executable(${sub_folder} ${SOURCES_${sub_folder}})
endforeach(sub_folder)

target_link_libraries(parse_messages ${Boost_LIBRARIES} ${VSQLITEPP_LIBRARY} Qt5::Widgets Qt5::Xml)
target_link_libraries(db2texts ${Boost_LIBRARIES} ${VSQLITEPP_LIBRARY} Qt5::Core)
target_link_libraries(merger Qt5::Widgets Qt5::Sql)
