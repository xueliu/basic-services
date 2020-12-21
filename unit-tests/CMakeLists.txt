add_executable(unit-tests "")

target_sources(
    unit-tests
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/gtest_main.cpp
)

target_link_libraries(
    unit-tests
    PRIVATE
    GTest::GTest
    GTest::Main
)

target_include_directories(
    unit-tests
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_test(all_unit_tests unit-tests)