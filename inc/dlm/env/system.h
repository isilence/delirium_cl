/**
 Define compile-time macro constants, that determine target operating system (OS).
 All following macros are mutual exclusive.

 DLM_OS_LINUX       - linux
 DLM_OS_WINDOWS     - windows
 DLM_OS_UNKNOWN     - OS not determined

 DLM_PAGE_SIZE
*/

#ifndef DLM_ENV_SYSTEM_H_
#define DLM_ENV_SYSTEM_H_

#if defined(linux) || defined(__linux) || defined(__linux__)
    #define DLM_OS_LINUX 1
    #define DLM_OS_NAME "Linux"

#elif defined(WIN32) || defined(_WIN32) || defined(__TOS_WIN__) || defined(_MSC_VER)
    #define DLM_OS_WINDOWS 1
    #define DLM_OS_NAME "Windows"

#else
    #define DLM_OS_UNKNOWN

#endif

#define DLM_PAGE_SIZE (1ul << 12)

#endif /* DLM_ENV_SYSTEM_H_ */
