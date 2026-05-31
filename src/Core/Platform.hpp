#pragma once

// FORCEINLINE 强制内联
#if defined(_MSC_VER)
#define FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCEINLINE __attribute__((always_inline)) inline
#else
#define FORCEINLINE inline
#endif

// RESTRICT 指针
#if defined(_MSC_VER)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

// CRT 内存泄漏检测 (仅 MSVC Debug)
#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#define ENABLE_CRT_LEAK_CHECK() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
#define ENABLE_CRT_LEAK_CHECK() ((void)0)
#endif
