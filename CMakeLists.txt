cmake_minimum_required (VERSION 2.6)
project (substrate)

# The version number.
set (substrate_VERSION_MAJOR 0)
set (substrate_VERSION_MINOR 1)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -pedantic -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pedantic")

#add the include directory containing the header files
include_directories(${substrate_SOURCE_DIR}/include/)

###add osl to the build
##Comment out to use git submodule osl directory, else we use installed lib.
#set(osl_DIR ${substrate_SOURCE_DIR}/osl/)
#add_subdirectory(osl)
#include_directories(${osl_DIR}/include)
find_library(OSL_LIBRARY osl REQUIRED)

###add candl to the build
##Comment out to use git submodule candl directory, else we use installed lib.
#set(candl_DIR ${substrate_SOURCE_DIR}/candl/)
#add_subdirectory(candl)
#include_directories(${candl_DIR}/include)
find_library(CANDL_LIBRARY candl REQUIRED)

find_library(GMP_LIBRARY gmp)
if (GMP_LIBRARY)
    find_library(PIPLIB_GMP_LIBRARY piplib_gmp REQUIRED)
else()
    find_library(PIPLIB_DP_LIBRARY piplib_dp REQUIRED)
endif()

#find_package(GMP REQUIRED)

add_executable(substrate
    src/utils.c
    src/reuse_struct.c
    src/reuse.c
    src/parallelism_struct.c
    src/parallelism.c
    src/vectorization_struct.c
    src/vectorization.c
    src/statement_profile.c
    src/analyze.c
    src/optimization.c
    src/options.c
    src/substrate.c
    )

target_link_libraries(substrate ${OSL_LIBRARY})
target_link_libraries(substrate ${CANDL_LIBRARY})
if (GMP_LIBRARY)
    target_link_libraries(substrate ${GMP_LIBRARY})
    target_link_libraries(substrate ${PIPLIB_GMP_LIBRARY})
else()
    target_link_libraries(substrate ${PIPLIB_DP_LIBRARY})
endif()
