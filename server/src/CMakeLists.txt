# 源代码CMakeLists.txt

# 设置protobuf文件
set(PROTO_FILES message.proto)

# 生成protobuf文件
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# 生成gRPC文件
protobuf_generate_grpc_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_FILES})

# 共享源文件
set(SHARED_SOURCES
    config_manager.cpp
    io_context_pool.cpp
    redis_manager.cpp
    mysql_pool.cpp
    mysql_dao.cpp
    mysql_manager.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

# GateServer 源文件
set(GATESERVER_SOURCES
    cserver.cpp
    gate_server.cpp
    http_connection.cpp
    logic_system.cpp
    verify_grpc_client.cpp
    status_grpc_client.cpp
)

# StatusServer 源文件 (您需要创建 status_server.cpp)
set(STATUS_SERVER_SOURCES
    status_server.cpp
    status_server_impl.cpp
)

# 创建可执行文件 fchat-server
add_executable(gate_server
    ${GATESERVER_SOURCES}
    ${SHARED_SOURCES}
)

# 创建可执行文件 status_server
add_executable(status_server
    ${STATUS_SERVER_SOURCES}
    ${SHARED_SOURCES}
)

# 为 fchat-server 设置
target_include_directories(gate_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}  # 用于生成的protobuf头文件
)

target_link_libraries(gate_server PRIVATE
    Boost::system
    nlohmann_json::nlohmann_json
    protobuf::libprotobuf
    gRPC::grpc++
    Boost::property_tree
    hiredis::hiredis
    spdlog::spdlog
    unofficial::mysql-connector-cpp::connector
)

add_custom_command(
    TARGET gate_server
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        "$<TARGET_FILE_DIR:gate_server>/config.ini"
    COMMENT "Copying config.ini to output directory"
)

# 为 status_server 设置
target_include_directories(status_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(status_server PRIVATE
    Boost::system
    nlohmann_json::nlohmann_json
    protobuf::libprotobuf
    gRPC::grpc++
    Boost::property_tree
    hiredis::hiredis
    spdlog::spdlog
    unofficial::mysql-connector-cpp::connector
)

add_custom_command(
    TARGET status_server
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        "$<TARGET_FILE_DIR:status_server>/config.ini"
    COMMENT "Copying config.ini to output directory"
)


# Windows特定设置
if(WIN32)
    set(WIN_DEFS
        BOOST_BEAST_USE_STD_STRING_VIEW=1
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        BOOST_ASIO_WINDOWS
    )
    set(WIN_LIBS
        ws2_32
        winmm
    )
    target_compile_definitions(gate_server PRIVATE ${WIN_DEFS})
    target_compile_definitions(status_server PRIVATE ${WIN_DEFS})

    target_link_libraries(gate_server PRIVATE ${WIN_LIBS})
    target_link_libraries(status_server PRIVATE ${WIN_LIBS})
endif()

# 设置输出目录
set_target_properties(gate_server status_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)