cmake_minimum_required(VERSION 3.5)

########################################
# Project setup
########################################
project(cmake-demo)

########################################
# Define include directories
########################################
set(ALL_INCLUDES
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include/cmake-demo
)
include_directories(${ALL_INCLUDES})

########################################
# Source files
########################################
file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

########################################
# Create a library
########################################
add_library(cmake-demo
    ${SOURCES}
)


########################################
# Unit tests
########################################
add_subdirectory(third_party/googletest)

# enable GTest testing
enable_testing()

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

# from list of files we'll create tests test_name.cpp -> test_name
foreach(_test_file ${TEST_SRC_FILES})
    get_filename_component(_test_name ${_test_file} NAME_WE)
    add_executable(${_test_name} ${_test_file})
    target_link_libraries(${_test_name}
        cmake-demo
        gtest
        gtest_main
    )
    set_target_properties(${_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/test
    )
    add_test(${_test_name} ${_test_name})
endforeach()

