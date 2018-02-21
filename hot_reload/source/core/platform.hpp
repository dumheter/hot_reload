/**
 * Thanks Filip for most things in this header.
 */

#pragma once

// ============================================================ //
// Platform Detection
// ============================================================ //

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64)
#define LIGHTCTRL_PLATFORM_WINDOWS
#elif defined(__linux__)
#define LIGHTCTRL_PLATFORM_LINUX
#elif defined (__APPLE__)
#define LIGHTCTRL_PLATFORM_APPLE
#endif

// ============================================================ //
// Platform Specific Headers
// ============================================================ //

#if defined(LIGHTCTRL_PLATFORM_WINDOWS)

#elif defined(LIGHTCTRL_PLATFORM_LINUX)

#elif defined(LIGHTCTRL_PLATFORM_APPLE)
#endif
