#include "SysUtils.h"


#ifndef WIN32
#include <time.h>
#else
#include <windows.h>
#endif

long
SysUtils::getCurrentMillis()
{
#ifndef WIN32
    struct timeval current;
    gettimeofday(&current, NULL);
    long nanosecs =
        (long) current.tv_sec * 1000L + (long) current.tv_usec / 1000L;
    return nanosecs;
#else
    LARGE_INTEGER val, val2;
    BOOL check = QueryPerformanceCounter(&val);
    check = QueryPerformanceFrequency(&val2);
//    val2.QuadPart = val2.QuadPart / 1000;
    return (long) (val.QuadPart*1000/val2.QuadPart);
#endif
}
