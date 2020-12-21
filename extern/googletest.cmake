include(FetchContent)

FetchContent_Declare(
    googletest
    URL      https://github.com/google/googletest/archive/release-1.10.0.zip
    URL_HASH SHA1=9ea36bf6dd6383beab405fd619bdce05e66a6535
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # Prevent installation of GTest with your project
    set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

    # Add aliases for GTest and GMock libraries
    if(NOT TARGET GTest::GTest)
        add_library(GTest::GTest ALIAS gtest)
        add_library(GTest::Main ALIAS gtest_main)
    endif()

    if(NOT TARGET GTest::GMock)
        add_library(GMock::GMock ALIAS gmock)
        add_library(GMock::Main ALIAS gmock_main)
    endif()
endif()