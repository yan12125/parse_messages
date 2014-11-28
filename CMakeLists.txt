cmake_minimum_required(VERSION 3.0)

project(parse_messages)

find_package(PkgConfig REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
# Dependencies
find_package(Boost 1.57.0 REQUIRED COMPONENTS locale filesystem system)
pkg_check_modules(XERCES_C REQUIRED xerces-c)
find_package(VSQLitePP)
find_package(Qt5Widgets REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${XERCES_C_INCLUDE_DIRS}
    ${VSQLITEPP_INCLUDE_DIR}
    ${Qt5Widgets_INCLUDE_DIRS}
)

set(CXX_FLAGS_COMMON "-std=c++14 -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS_COMMON}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS_COMMON} -DMY_DEBUG")

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

foreach(sub_folder parse_messages db2texts)
    aux_source_directory("${CMAKE_SOURCE_DIR}/${sub_folder}" SOURCES_${sub_folder})
    add_executable(${sub_folder} ${SOURCES_${sub_folder}})
endforeach(sub_folder)

target_link_libraries(parse_messages
    ${Boost_LIBRARIES}
    ${XERCES_C_LIBRARIES}
    ${VSQLITEPP_LIBRARY}
    Qt5::Widgets
)
target_link_libraries(db2texts ${Boost_LIBRARIES} ${VSQLITEPP_LIBRARY})
