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


# Build doxygen
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		add_custom_target(
			doxygen
			${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc/"
			COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	else()
		message (STATUS "Doxygen not found :( API documentation can not be built")
	endif()


###add osl to the build
#If the "osl" directory is not empty, we assume that it is filled with
#the openscop project. If this is the case, substrate will build and use
#this version of libosl, instead of the system version.
set(osl_DIR ${substrate_SOURCE_DIR}/osl/*)
file(GLOB RESULT ${osl_DIR})
if(RESULT)
    add_subdirectory(osl)
    include_directories(${osl_DIR}/include)
    set(OSL_LIBRARY osl)
else()
    find_library(OSL_LIBRARY osl REQUIRED)
endif()

###add candl to the build
#If the "candl" directory is not empty, we assume that it is filled with
#the candl project. If this is the case, substrate will build and use
#this version of libcandl, instead of the system version.

set(candl_DIR ${substrate_SOURCE_DIR}/candl/*)
file(GLOB RESULT ${candl_DIR})
if(RESULT)
    add_subdirectory(candl)
    include_directories(${candl_DIR}/include)
    set(CANDL_LIBRARY candl)
else()
    find_library(CANDL_LIBRARY candl REQUIRED)
endif()

###add clay to the build
#If the "clay" directory is not empty, we assume that it is filled with
#the clay project. If this is the case, substrate will build and use
#this version of libclay, instead of the system version.
set(clay_DIR ${substrate_SOURCE_DIR}/clay/*)
file(GLOB RESULT ${clay_DIR})
if(RESULT)
    add_subdirectory(clay)
    include_directories(${clay_DIR}/include)
    set(CLAY_LIBRARY clay)
else()
    find_library(CLAY_LIBRARY clay REQUIRED)
endif()

find_library(GMP_LIBRARY gmp)
if (GMP_LIBRARY)
    find_library(PIPLIB_GMP_LIBRARY piplib_gmp REQUIRED)
else()
    find_library(PIPLIB_DP_LIBRARY piplib_dp REQUIRED)
endif()

###add pluto to the build
#If the "pluto" directory is not empty, we assume that it is filled with
#the pluto project. If this is the case, substrate will build and use
#this version of libpluto, instead of the system version.
set(pluto_DIR ${substrate_SOURCE_DIR}/pluto/*)
file(GLOB RESULT ${pluto_DIR})
if(RESULT)
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
    SET(PLUTO_LIBRARY libpluto)
else()
    find_library(PLUTO_LIBRARY pluto REQUIRED)
endif()


# files .c
set(
  sources
  src/adjacency_matrix.c
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
  #src/substrate.c
)

# substrate program
add_executable(substrate_exe
  ${sources}
  src/substrate.c
)
set_target_properties(substrate_exe PROPERTIES OUTPUT_NAME "substrate")


# substrate static library
add_library(substrate_lib_static STATIC
  ${sources}
)
set_target_properties(substrate_lib_static PROPERTIES OUTPUT_NAME "substrate")

# substrate shared library
add_library(substrate_lib_shared SHARED
  ${sources}
)
set_target_properties(substrate_lib_shared PROPERTIES OUTPUT_NAME "substrate")


target_link_libraries(substrate_exe ${OSL_LIBRARY})
target_link_libraries(substrate_exe ${CANDL_LIBRARY})
target_link_libraries(substrate_exe ${CLAY_LIBRARY})
if (GMP_LIBRARY)
    target_link_libraries(substrate_exe ${GMP_LIBRARY})
    target_link_libraries(substrate_exe ${PIPLIB_GMP_LIBRARY})
else()
    target_link_libraries(substrate_exe ${PIPLIB_DP_LIBRARY})
endif()
target_link_libraries(substrate_exe ${PLUTO_LIBRARY})
