# -- Minimum required version
cmake_minimum_required (VERSION 3.11.0)

# -- Project name
project (basic-services)

# -- Various includes
include (GenerateExportHeader)
include (CMakePackageConfigHelpers)
include (GNUInstallDirs)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# -- Versioning with git tag
include (GitTagVersion)

# -- Set cxx 14 standard default
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
if (NOT (${CMAKE_INSTALL_PREFIX} STREQUAL "/usr" ) )
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

if(WIN32)
    set(BASIC_SERVICES_STATIC_LIBS_BY_DEFAULT ON)
else()
    set(BASIC_SERVICES_STATIC_LIBS_BY_DEFAULT OFF)
endif()

# Compilation options
option(BASIC_SERVICES_STATIC_LIBS   "Build static libraries" ${BASIC_SERVICES_STATIC_LIBS_BY_DEFAULT})
option(BASIC_SERVICES_TESTS         "Build unit tests" OFF)
option(BASIC_SERVICES_EXAMPLES      "Build example applications" OFF)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

if(BASIC_SERVICES_STATIC_LIBS)
    set(BUILD_SHARED_LIBS OFF)
else()
    if(WIN32)
        message(FATAL_ERROR "building shared libraries not supported on Windows")
    endif()
    set(BUILD_SHARED_LIBS ON)
endif()

# External dependencies
include(extern/asio.cmake)

# Libraries
include(src/CMakeLists.txt)

# Unit-Tests
if(BASIC_SERVICES_TESTS)
    include(./extern/googletest.cmake)
    enable_testing()
    add_subdirectory(./unit-tests)
endif()

# Examples
if(BASIC_SERVICES_EXAMPLES)
    add_subdirectory(examples)
endif()
