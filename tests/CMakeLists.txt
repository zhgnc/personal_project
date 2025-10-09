######################
##### UNIT TESTS #####
######################

add_compile_definitions(_POSIX_C_SOURCE=200809L)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(unit_tests ${TEST_SOURCES})

target_compile_options(unit_tests PRIVATE
  -Wno-deprecated-declarations
)

target_include_directories(unit_tests
    PRIVATE
      ${PROJECT_SOURCE_DIR}/src
    SYSTEM
      ${PROJECT_SOURCE_DIR}/external/eigen
      ${PROJECT_SOURCE_DIR}/external/googletest/gmock/include
      ${PROJECT_SOURCE_DIR}/external/googletest/gtest/include
)

target_link_libraries(unit_tests
    PRIVATE
    ${PROJECT_SOURCE_DIR}/external/googletest/lib/libgtest.a
    ${PROJECT_SOURCE_DIR}/external/googletest/lib/libgmock.a
)

add_test(NAME unit_tests COMMAND unit_tests)