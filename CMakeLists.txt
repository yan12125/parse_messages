cmake_minimum_required(VERSION 3.0)

project(parse_messages)

find_package(PkgConfig REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
# Dependencies
find_package(Boost 1.57.0 REQUIRED COMPONENTS locale filesystem system)
pkg_check_modules(XERCES_C REQUIRED xerces-c)
find_package(VSQLitePP)
find_package(Qt4 REQUIRED QtGui)

include_directories(${Boost_INCLUDE_DIRS} ${XERCES_C_INCLUDE_DIRS} ${VSQLITEPP_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")

foreach(sub_folder parse_messages db2texts)
    aux_source_directory("${CMAKE_SOURCE_DIR}/${sub_folder}" SOURCES_${sub_folder})
    add_executable(${sub_folder} ${SOURCES_${sub_folder}})
endforeach(sub_folder)

target_link_libraries(parse_messages ${Boost_LIBRARIES} ${XERCES_C_LIBRARIES} ${VSQLITEPP_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
target_link_libraries(db2texts ${Boost_LIBRARIES} ${VSQLITEPP_LIBRARY})
