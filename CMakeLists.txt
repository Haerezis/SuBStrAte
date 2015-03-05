cmake_minimum_required (VERSION 2.6)
project (substrate)

# The version number.
set (substrate_VERSION_MAJOR 0)
set (substrate_VERSION_MINOR 1)


# User's settings - C Flags
# set(release "TRUE")
set(release "FALSE")
# Release
if (release)
set(CMAKE_C_FLAGS "-O3")
# Debug # valgrind --show-reachable=yes --leak-check=full -v exe
else()
set(CMAKE_C_FLAGS "-O0 -g3")
endif()
# User's settings - General C Flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c99")

set(EXECUTABLE_OUTPUT_PATH bin)

include_directories(${substrate_SOURCE_DIR}/include/)

add_executable(substrate
    src/substrate.c
    )
