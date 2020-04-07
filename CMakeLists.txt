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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -o3 -Wall")
set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

########################################
# Copy dictionary
########################################
file(COPY conf DESTINATION ${PROJECT_BINARY_DIR}/output)
file(COPY data DESTINATION ${PROJECT_BINARY_DIR}/output)
file(COPY include DESTINATION ${PROJECT_BINARY_DIR}/output)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/output/proto)

########################################
# Define include directories
########################################
set(ALL_INCLUDES
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include/cmake-demo
    ${PROJECT_BINARY_DIR}/output/proto
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
if (NOT TARGET grpc)
    add_subdirectory(third_party/grpc)
    message(STATUS "Using gRPC via add_subdirectory.")
endif()

# After using add_subdirectory, we can now use the grpc targets directly from
# this build.
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)


# Generated sources
set(_PROTO_SRCS "")
set(_GRPC_SRCS "")
set(_PROTO_OUTPUT ${PROJECT_BINARY_DIR}/output/proto)
file(GLOB PROTO_FILES ${PROJECT_SOURCE_DIR}/proto/*.proto)
foreach(_proto_file ${PROTO_FILES})
    # message("${_proto_file}")
    get_filename_component(_proto_file "${_proto_file}" ABSOLUTE)
    get_filename_component(_proto_file_path "${_proto_file}" PATH)
    get_filename_component(_proto_file_name "${_proto_file}" NAME_WE)
    set(_proto_src "${_PROTO_OUTPUT}/${_proto_file_name}.pb.cc")
    set(_proto_hdr "${_PROTO_OUTPUT}/${_proto_file_name}.pb.h")
    set(_grpc_src "${_PROTO_OUTPUT}/${_proto_file_name}.grpc.pb.cc")
    set(_grpc_hdr "${_PROTO_OUTPUT}/${_proto_file_name}.grpc.pb.h")
    # message("${_proto_src}")
    add_custom_command(
        OUTPUT "${_proto_src}" "${_proto_hdr}" "${_grpc_src}" "${_grpc_hdr}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS  --grpc_out "${_PROTO_OUTPUT}"
            --cpp_out "${_PROTO_OUTPUT}"
            -I "${_proto_file_path}"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${_proto_file}"
        DEPENDS "${_proto_file}"
    )
    list(APPEND _PROTO_SRCS "${_proto_src}")
    list(APPEND _GRPC_SRCS "${_grpc_src}")
endforeach()

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
    ${_PROTO_SRCS}
    ${_GRPC_SRCS}
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

target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(cmake-demo
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib
)

########################################
# Tool
########################################
file(GLOB TOOL_SRC_FILES ${PROJECT_SOURCE_DIR}/tool/*.cpp ${PROJECT_SOURCE_DIR}/tool/*.cc)
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

