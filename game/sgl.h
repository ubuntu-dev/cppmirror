#if !defined(_SGL_H)

#define SGL_COMPILER_MSVC 0
#define SGL_COMPILER_CLANG 0
#define SGL_COMPILER_GCC 0

#define SGL_ENVIRONMENT64 1
#define SGL_ENVIRONMENT32 0

#define SGL_OS_WIN32 0
#define SGL_OS_LINUX 0

#if defined(__clang__)
    #undef SGL_COMPILER_CLANG
    #define SGL_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #undef SGL_COMPILER_MSVC
    #define SGL_COMPILER_MSVC 1
#elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.
    #undef SGL_COMPILER_GCC
    #define SGL_COMPILER_GCC 1
#else
    #error "Could not detect compiler."
#endif

#if defined(__linux__)
    #undef SGL_OS_LINUX
    #define SGL_OS_LINUX 1
#elif defined(_WIN32)
    #undef SGL_OS_WIN32
    #define SGL_OS_WIN32 1
#else
    #error "Could not detect OS"
#endif

#if SGL_OS_LINUX
    #if (__x86_64__ || __ppc64__)
        #undef SGL_ENVIRONMENT64
        #define SGL_ENVIRONMENT64 1
    #else
        #undef SGL_ENVIRONMENT32
        #define SGL_ENVIRONMENT32 1
    #endif
#elif SGL_OS_WIN32
    #if defined(_WIN64)
        #undef SGL_ENVIRONMENT64
        #define SGL_ENVIRONMENT64 1
    #else
        #undef SGL_ENVIRONMENT32
        #define SGL_ENVIRONMENT32 1
    #endif
#endif


#define _SGL_H
#endif