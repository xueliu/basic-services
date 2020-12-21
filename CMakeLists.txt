# -- Minimum required version
cmake_minimum_required (VERSION 3.11.0)

# -- Project name
project (basic-services)

# -- Various includes
include (GenerateExportHeader)
include (CMakePackageConfigHelpers)

# -- Set cxx 14 standard default
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(BASIC_SERVICES_STATIC_LIBS_BY_DEFAULT ON)
else()
    set(BASIC_SERVICES_STATIC_LIBS_BY_DEFAULT OFF)
endif()

# Compilation options
option(BASIC_SERVICES_STATIC_LIBS   "Build static libraries" ${BASIC_SERVICES_STATIC_LIBS_BY_DEFAULT})
option(BASIC_SERVICES_TESTS         "Build unit tests" ON)
option(BASIC_SERVICES_EXAMPLES      "Build example applications" OFF)

if(BASIC_SERVICES_STATIC_LIBS)
    set(BUILD_SHARED_LIBS OFF)
else()
    if(WIN32)
        message(FATAL_ERROR "building shared libraries not supported on Windows")
    endif()
    set(BUILD_SHARED_LIBS ON)
endif()

# External dependencies
include(./extern/asio.cmake)

# Libraries


# Unit-Tests
if(BASIC_SERVICES_TESTS)
    include(./extern/googletest.cmake)
    enable_testing()
    add_subdirectory(./unit-tests)
endif()

# Examples
if(BASIC_SERVICES_EXAMPLES)

endif()