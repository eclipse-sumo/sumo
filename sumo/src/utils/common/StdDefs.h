#ifndef StdDefs_h
#define StdDefs_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


inline double
MIN2(double a, double b)
{
    return a<b?a:b;
}

inline double
MAX2(double a, double b)
{
    return a>b?a:b;
}


inline double
MIN3(double a, double b, double c)
{
    return MIN2(c, a<b?a:b);
}


inline double
MAX3(double a, double b, double c)
{
    return MAX2(c, a>b?a:b);
}


inline double
MIN4(double a, double b, double c, double d)
{
    return MIN2(MIN2(a,b),MIN2(c,d));
}


inline double
MAX4(double a, double b, double c, double d)
{
    return MAX2(MAX2(a,b),MAX2(c,d));
}


/// !!! ins config.h
#define SUMOReal float

#endif

