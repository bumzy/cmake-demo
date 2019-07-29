cmake_minimum_required(VERSION 3.5)

########################################
# Project setup
########################################
project(cmake-demo)

########################################
# Set up output path
########################################
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/bin)

########################################
# Set up the compiler flags
########################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")
set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

########################################
# Copy dictionary
########################################
file(COPY conf DESTINATION ${PROJECT_BINARY_DIR}/output)
file(COPY data DESTINATION ${PROJECT_BINARY_DIR}/output)

########################################
# Define include directories
########################################
set(ALL_INCLUDES
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include/cmake-demo
)
include_directories(${ALL_INCLUDES})

########################################
# Third Party JSON
########################################
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(third_party/json)

set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
set(gRPC_BUILD_TESTS OFF CACHE INTERNAL "")
set(gRPC_BUILD_CODEGEN OFF CACHE INTERNAL "")
add_subdirectory(third_party/grpc)
message(STATUS "Using gRPC via add_subdirectory.")

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

target_link_libraries(cmake-demo
    PRIVATE
        Threads::Threads
        nlohmann_json::nlohmann_json
        ${_PROTOBUF_LIBPROTOBUF}
)

########################################
# Tools
########################################
add_executable(cmake_demo_tool
    tool/cmake_demo_tool.cpp
)

target_link_libraries(cmake_demo_tool
    PRIVATE
        cmake-demo
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

