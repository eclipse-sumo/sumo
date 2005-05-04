#include "SysUtils.h"


/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif

long
SysUtils::getCurrentMillis()
{
#ifndef WIN32
   timeval current;
   gettimeofday(&current, 0);
   long nanosecs =
        (long) current.tv_sec * 1000L + (long) current.tv_usec / 1000L;
   return nanosecs;
#else
    LARGE_INTEGER val, val2;
    BOOL check = QueryPerformanceCounter(&val);
    check = QueryPerformanceFrequency(&val2);
    return (long) (val.QuadPart*1000/val2.QuadPart);
#endif
}
