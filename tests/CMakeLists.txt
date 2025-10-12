######################
##### UNIT TESTS #####
######################

add_compile_definitions(_POSIX_C_SOURCE=200809L)

# Links all the cpp files in the tests/ and src/ directories other than 
# src/main.cpp which will conflict with main() in test_main.cpp 
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"          
    "${PROJECT_SOURCE_DIR}/src/logger/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/sensor_models/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/sim_framework/*.cpp"            
)

add_executable(unit_tests ${TEST_SOURCES})

target_include_directories(unit_tests
    PRIVATE
      ${PROJECT_SOURCE_DIR}/src
    SYSTEM
      ${PROJECT_SOURCE_DIR}/external/eigen
      ${PROJECT_SOURCE_DIR}/external/google_test/gmock/include
      ${PROJECT_SOURCE_DIR}/external/google_test/gtest/include
)

target_link_libraries(unit_tests
    PRIVATE
    ${PROJECT_SOURCE_DIR}/external/google_test/lib/libgtest.a
    ${PROJECT_SOURCE_DIR}/external/google_test/lib/libgmock.a
    yaml-cpp
    hdf5_cpp  
    hdf5) 

add_test(NAME unit_tests COMMAND unit_tests)