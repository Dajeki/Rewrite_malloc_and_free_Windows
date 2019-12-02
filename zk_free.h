#include <stdio.h>
#include <stdlib.h>

#if (defined(_WIN32) || defined(__CYGWIN__))
    #define OS "Windows"

#elif (defined(__linux__))
    #define OS "Linux"

#else
    #define OS "Unknown"
#endif

