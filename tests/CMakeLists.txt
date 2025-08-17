######################
##### UNIT TESTS #####
######################
add_compile_definitions(_POSIX_C_SOURCE=200809L)

set(GTEST_DIR "${PROJECT_SOURCE_DIR}/external/googletest")
add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest_build)

enable_testing()

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(test_source IN LISTS TEST_SOURCES)
  get_filename_component(test_name ${test_source} NAME_WE)
  add_executable(${test_name} ${test_source})
  target_link_libraries(${test_name} PRIVATE gtest gtest_main)
  target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
