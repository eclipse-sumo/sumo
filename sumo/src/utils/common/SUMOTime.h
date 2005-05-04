#ifndef SUMOTime_h
#define SUMOTime_h
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)



typedef int SUMOTime;

// x*deltaT
#define SPEED2DIST(x) x

// x/deltaT
#define DIST2SPEED(x) x

// x*deltaT*deltaT
#define ACCEL2DIST(x) x

// x*deltaT
#define ACCEL2SPEED(x) x


//!!!
typedef unsigned long ExtSUMOTime;

#endif
