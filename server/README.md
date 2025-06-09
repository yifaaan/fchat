# FChat Server

一个基于gRPC和HTTP的聊天服务器，使用C++20和Boost.Beast开发。

## 依赖项

- C++20 编译器 (Visual Studio 2022 或更高版本)
- CMake 3.20+
- Git
- vcpkg



## 项目结构

```
fchat-server/
├── src/                     # 源代码目录
│   ├── CMakeLists.txt      # 源代码构建配置
│   ├── cserver.cpp         # 主程序入口
│   ├── cserver.h           # 主程序头文件
│   ├── gate_server.cpp     # 网关服务器实现
│   ├── http_connection.cpp # HTTP连接处理
│   ├── http_connection.h   # HTTP连接头文件
│   ├── logic_system.cpp    # 业务逻辑实现
│   ├── logic_system.h      # 业务逻辑头文件
│   ├── message.proto       # protobuf定义
│   ├── const.h             # 常量定义
│   ├── verify_server.h     # 验证服务器头文件
│   └── singleton.h         # 单例模式头文件
├── vcpkg.json              # vcpkg依赖清单
├── CMakeLists.txt          # 根CMake配置
└── README.md               # 本文件
```


## 使用的库

- **Boost.Beast**: HTTP/WebSocket服务器
- **gRPC**: RPC框架
- **Protocol Buffers**: 数据序列化
- **nlohmann/json**: JSON处理
- **OpenSSL**: TLS/SSL支持

## 运行

构建成功后，可执行文件位于：
```
build\bin\Release\fchat-server.exe  # 发布版本
build\bin\Debug\fchat-server.exe    # 调试版本
```

## 开发指南

### 添加新的源文件
在`src/CMakeLists.txt`中的相应列表添加新文件：
```cmake
# 添加到源文件列表
set(SERVER_SOURCES
    # ... 现有文件 ...
    your_new_file.cpp
)

# 添加到头文件列表
set(SERVER_HEADERS
    # ... 现有文件 ...
    your_new_file.h
)
```

### 修改protobuf定义
修改`src/message.proto`文件后，重新构建即可自动生成新的C++代码。

## 故障排除

### 常见问题

1. **vcpkg安装失败**
   - 确保网络连接正常
   - 检查防火墙设置
   - 尝试使用管理员权限运行

2. **CMake配置失败**
   - 确保CMake版本>=3.20
   - 检查Visual Studio C++工具链是否安装

3. **编译错误**
   - 确保使用C++20兼容的编译器
   - 检查所有依赖是否正确安装

4. **protobuf生成失败**
   - 检查protoc编译器是否正确安装
   - 确认gRPC插件可用