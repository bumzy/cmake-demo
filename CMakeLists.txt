cmake_minimum_required(VERSION 3.5)

########################################
# Project setup
########################################
project(cmake-demo)

########################################
# Set up output path
########################################
# SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/bin)

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
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/output/proto)

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
# Third Party
########################################
# nlohmann JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(third_party/json)

# Boost
add_subdirectory(third_party/boost-cmake)

# grpc
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
set(gRPC_BUILD_TESTS OFF CACHE INTERNAL "")
# set(gRPC_SSL_PROVIDER "package" CACHE STRING "Provider of ssl library")
# set(gRPC_BUILD_CODEGEN OFF CACHE INTERNAL "")
# set(gRPC_BUILD_CSHARP_EXT OFF CACHE INTERNAL "")
add_subdirectory(third_party/grpc)
message(STATUS "Using gRPC via add_subdirectory.")

# After using add_subdirectory, we can now use the grpc targets directly from
# this build.
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

# Proto file
get_filename_component(hw_proto "${PROJECT_SOURCE_DIR}/proto/helloworld.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/output/proto/helloworld.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/output/proto/helloworld.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/output/proto/helloworld.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/output/proto/helloworld.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/output/proto"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}/output/proto"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}/output/proto")

########################################
# Source files
########################################
file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

########################################
# Create a library
########################################
add_library(cmake-demo STATIC
    ${SOURCES}
    ${hw_proto_srcs}
    ${hw_grpc_srcs}
)

target_link_libraries(cmake-demo
    PUBLIC
        Threads::Threads
        nlohmann_json::nlohmann_json
        ${_GRPC_GRPCPP_UNSECURE}
        ${_PROTOBUF_LIBPROTOBUF}
        PUBLIC Boost::filesystem
        PUBLIC Boost::boost
)

set_target_properties(cmake-demo
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib
)

########################################
# Tool
########################################
file(GLOB TOOL_SRC_FILES ${PROJECT_SOURCE_DIR}/tool/*.cpp)
foreach(_tool_file ${TOOL_SRC_FILES})
    get_filename_component(_tool_name ${_tool_file} NAME_WE)
    add_executable(${_tool_name} ${_tool_file})
    target_link_libraries(${_tool_name}
        PRIVATE
            cmake-demo
    )
    set_target_properties(${_tool_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/bin
    )
endforeach()

# add_executable(cmake_demo_tool
#     tool/cmake_demo_tool.cpp
# )

# target_link_libraries(cmake_demo_tool
#     PRIVATE
#         cmake-demo
# )
# set_target_properties(cmake_demo_tool
#     PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/bin
# )

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

