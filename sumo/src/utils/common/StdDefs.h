#ifndef StdDefs_h
#define StdDefs_h

inline double
MIN(double a, double b)
{
    return a<b?a:b;
}

inline double
MAX(double a, double b)
{
    return a>b?a:b;
}


inline double
MIN3(double a, double b, double c)
{
    return MIN(c, a<b?a:b);
}


inline double
MAX3(double a, double b, double c)
{
    return MAX(c, a>b?a:b);
}


inline double
MIN4(double a, double b, double c, double d)
{
    return MIN(MIN(a,b),MIN(c,d));
}


inline double
MAX4(double a, double b, double c, double d)
{
    return MAX(MAX(a,b),MAX(c,d));
}


#endif

