# 源代码CMakeLists.txt

# 设置protobuf文件
set(PROTO_FILES message.proto)

# 生成protobuf文件
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# 生成gRPC文件  
protobuf_generate_grpc_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_FILES})

# 设置源文件
set(SERVER_SOURCES
    cserver.cpp
    gate_server.cpp
    http_connection.cpp
    logic_system.cpp
    config_manager.cpp
)

# 设置头文件
set(SERVER_HEADERS
    cserver.h
    const.h
    verify_server.h
    logic_system.h
    http_connection.h
    singleton.h
    config_manager.h
    verify_server.h
)

# 将生成的protobuf文件添加到源文件列表
list(APPEND SERVER_SOURCES ${PROTO_SRCS} ${GRPC_SRCS})

# 创建可执行文件
add_executable(${PROJECT_NAME} 
    ${SERVER_SOURCES} 
    ${SERVER_HEADERS}
)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}  # 用于生成的protobuf头文件
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    Boost::system
    nlohmann_json::nlohmann_json
    protobuf::libprotobuf
    gRPC::grpc++
    # gRPC::grpc++_reflection
    Boost::property_tree
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/config.ini"
    COMMENT "Copying config.ini to output directory"
)

# Windows特定设置
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        BOOST_BEAST_USE_STD_STRING_VIEW=1
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    target_link_libraries(${PROJECT_NAME}
        ws2_32
        winmm
    )
endif()

# 设置编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /W4 
        /utf-8  # 支持UTF-8编码
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall -Wextra -pedantic
    )
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
) 