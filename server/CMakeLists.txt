cmake_minimum_required(VERSION 3.20)
project(fchat-server VERSION 1.0.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_MAKE_PROGRAM "Ninja")

# 查找依赖包
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS property_tree)

# 自定义函数：生成gRPC代码
function(protobuf_generate_grpc_cpp SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: protobuf_generate_grpc_cpp() called without any proto files")
        return()
    endif()

    set(${SRCS})
    set(${HDRS})

    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        
        set(GRPC_SRC "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc")
        set(GRPC_HDR "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h")
        
        list(APPEND ${SRCS} ${GRPC_SRC})
        list(APPEND ${HDRS} ${GRPC_HDR})
        
        add_custom_command(
            OUTPUT ${GRPC_SRC} ${GRPC_HDR}
            COMMAND protobuf::protoc
            ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
                 --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
                 --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
                 --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
                 ${ABS_FIL}
            DEPENDS ${ABS_FIL} protobuf::protoc gRPC::grpc_cpp_plugin
            COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
            VERBATIM)
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()



# 创建bin目录用于输出
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加src子目录
add_subdirectory(src)

# 输出构建信息
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}/bin") 