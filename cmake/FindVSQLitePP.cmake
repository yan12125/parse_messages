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


