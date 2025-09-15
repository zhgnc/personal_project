######################
##### UNIT TESTS #####
######################

add_compile_definitions(_POSIX_C_SOURCE=200809L)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(unit_tests ${TEST_SOURCES})

target_include_directories(unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        SYSTEM ${PROJECT_SOURCE_DIR}/external/Eigen
)

target_link_libraries(unit_tests PRIVATE gtest_main)

add_test(NAME unit_tests COMMAND unit_tests)