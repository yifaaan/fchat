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

#endif