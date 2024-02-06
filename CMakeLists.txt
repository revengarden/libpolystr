cmake_minimum_required(VERSION 3.5)
project(libpolystr C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(PSTR_DEBUG "Setting build type to debug." OFF)
option(PSTR_BUILD_SHARED "Build shared library" ON)
option(PSTR_BUILD_STATIC "Build static library" ON)
option(PSTR_BUILD_OBJECT "Build object library" ON)
option(PSTR_BUILD_TESTS "Build test executables" ON)

include("cmake/libpolystr-config-version.cmake")

set(sources 
	src/pool.c)

# -----
# Build type
#------

message(STATUS "")
if (NOT CMAKE_BUILD_TYPE)
  if (PSTR_DEBUG)
    message(STATUS "No build type selected, default to: Debug")
    set(CMAKE_BUILD_TYPE "Debug")
  else()
    message(STATUS "No build type selected, default to: Release")
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endif()

# --
# Install info
# --
set(install_libdir "${CMAKE_INSTALL_LIBDIR}")
if (PSTR_INSTALL_TOPLEVEL)
  set(install_objdir     "${CMAKE_INSTALL_LIBDIR}")
  set(install_incdir     "${CMAKE_INSTALL_INCLUDEDIR}")
else()
  set(install_objdir     "${CMAKE_INSTALL_LIBDIR}/polystr-${version}")
  set(install_incdir     "${CMAKE_INSTALL_INCLUDEDIR}/polystr-${version}")
endif()

set(base_name "polystr")
set(cflags ${CMAKE_C_FLAGS})
set(libraries "")

if(PSTR_BUILD_SHARED)
  list(APPEND build_targets "shared")
endif()
if(PSTR_BUILD_STATIC)
  list(APPEND build_targets "static")
endif()
if(PSTR_BUILD_OBJECT)
  list(APPEND build_targets "object")
endif()
if(PSTR_BUILD_TESTS)
  list(APPEND build_targets "tests")
endif()

message(STATUS "")
message(STATUS "Library base name: ${base_name}")
message(STATUS "Version          : ${version}")
message(STATUS "Build type       : ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler       : ${CMAKE_C_COMPILER}")
message(STATUS "Compiler flags   : ${cflags}")
message(STATUS "Compiler defines : ${defines}")
message(STATUS "Link libraries   : ${libraries}")
message(STATUS "Build targets    : ${build_targets}")
message(STATUS "")

# ------------
# Build target
# ------------

if(PSTR_BUILD_SHARED)
	add_library(polystr SHARED ${sources})
	set_target_properties(polystr PROPERTIES VERSION ${version} SOVERSION ${version_major} OUTPUT_NAME ${base_name})
	target_compile_definitions(polystr PRIVATE ${defines} PSTR_SHARED_LIB PSTR_SHARED_LIB_EXPORT)
	target_compile_options(polystr PRIVATE ${cflags})
	target_link_libraries(polystr PRIVATE ${libraries})
  target_include_directories(polystr PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${install_incdir}>
  )
endif()

if (PSTR_BUILD_STATIC)
  add_library(polystr-static STATIC ${sources})
  set_property(TARGET polystr-static PROPERTY POSITION_INDEPENDENT_CODE ON)
	target_compile_definitions(polystr-static PRIVATE ${defines} PSTR_STATIC_LIB)
  target_compile_options(polystr-static PRIVATE ${cflags})
  target_link_libraries(polystr-static PRIVATE ${libraries})
  target_include_directories(polystr-static PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${install_incdir}>
  )
  if(WIN32)
    string(REPLACE "polystr" "polystr-static" output_name ${base_name})
    set_target_properties(polystr-static PROPERTIES OUTPUT_NAME ${output_name})
  else()
    set_target_properties(polystr-static PROPERTIES OUTPUT_NAME ${base_name})
  endif()
endif()

if(PSTR_BUILD_TESTS)
	include(CTest)
	enable_testing()

	foreach(TEST_NAME pool)
		add_executable(polystr-test-${TEST_NAME} ${CMAKE_SOURCE_DIR}/tests/test-${TEST_NAME}.c)
		target_compile_definitions(polystr-test-${TEST_NAME} PRIVATE ${defines})
    target_compile_options(polystr-test-${TEST_NAME} PRIVATE ${cflags})
		target_include_directories(polystr-test-${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(polystr-test-${TEST_NAME} PRIVATE polystr ${libraries})

    add_test(NAME test-${TEST_NAME} COMMAND polystr-test-${TEST_NAME})
  endforeach()
endif()
