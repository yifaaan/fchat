#pragma once

// 避免Windows.h与gRPC之间的宏定义冲突
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

// On Windows, gRPC's status codes can conflict with macros defined in Windows headers.
// Specifically, `INTERNAL` from `grpcpp/support/status_code_enum.h` conflicts with a macro
// often defined in `winioctl.h` (which is included by `windows.h`).
//
// To resolve this, we include `windows.h` first, then undefine the conflicting macro
// before any gRPC headers are included. This file should be included before any gRPC headers.
#ifdef _WIN32
#include <windows.h>

// Undefine macros that conflict with libraries like gRPC and Boost.
#ifdef INTERNAL
#undef INTERNAL
#endif

#ifdef ERROR_ALREADY_EXISTS
#undef ERROR_ALREADY_EXISTS
#endif

#ifdef ERROR_NOT_FOUND
#undef ERROR_NOT_FOUND
#endif

#endif

// 重新定义Windows宏，避免与gRPC枚举冲突
// 必须在包含任何Windows头文件前定义
#ifdef _WIN32
#define WIN_ERROR_CANCELLED ERROR_CANCELLED
#define WIN_ERROR_ABORTED ERROR_REQUEST_ABORTED
#define WIN_NO_ERROR NO_ERROR
#define WIN_ERROR_INVALID_PARAMETER ERROR_INVALID_PARAMETER
#define WIN_ERROR_NOT_FOUND ERROR_NOT_FOUND
#define WIN_INTERNAL INTERNAL
#define WIN_UNAVAILABLE ERROR_SERVICE_UNAVAILABLE
#define WIN_ALREADY_EXISTS ERROR_ALREADY_EXISTS

#undef ERROR_CANCELLED
#undef ERROR_REQUEST_ABORTED
#undef NO_ERROR
#undef ERROR_INVALID_PARAMETER
#undef ERROR_NOT_FOUND
#undef INTERNAL
#undef ERROR_SERVICE_UNAVAILABLE
#undef ERROR_ALREADY_EXISTS
#endif