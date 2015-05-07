cmake_minimum_required (VERSION 2.6)
project (substrate)

# The version number.
set (substrate_VERSION_MAJOR 0)
set (substrate_VERSION_MINOR 1)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

#-fgnu89-inline is added to remove pluto's warning about inline functions
#not being defined.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fgnu89-inline")
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

include(ExternalProject)
ExternalProject_Add(
  pluto
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pluto
  CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/pluto/ && 
    git submodule init && 
    git submodule update && 
    bash autogen.sh && 
    ./configure --enable-debug
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/pluto
  BUILD_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/pluto && make
  INSTALL_COMMAND echo "Pluto won't be installed : bundle mode"
  BUILD_IN_SOURCE 1
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pluto/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pluto/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pluto/isl/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pluto/clan/include)
add_library(libpluto SHARED IMPORTED)
set_target_properties(libpluto PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/pluto/src/.libs/libpluto.so )

add_executable(substrate
    src/utils.c
    src/reuse_struct.c
    src/reuse.c
    src/parallelism_struct.c
    src/parallelism.c
    src/vectorization_struct.c
    src/vectorization.c
    src/tiling_hyperplane_struct.c
    src/tiling_hyperplane.c
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
target_link_libraries(substrate libpluto)
