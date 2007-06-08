#ifndef config_h
#define config_h

#ifndef _MSC_VER
#error This file is for MSVC compilation only. GCC should use configure generated config.h.
#endif

/* Disable "identifier truncated in debug info" warnings. */
#pragma warning(disable: 4786)
#pragma warning(disable: 4290)

/* Disable "unsafe" warnings for crt functions in VC++ 2005. */
#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_WARNINGS
#endif

/* Name of package */
#define PACKAGE "sumo"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* defines the precision of floats */
#define SUMOReal float

/* defines the epsilon to use on position comparison */
#define POSITION_EPS 0.1

/* defines the number of digits after the comma in output */
#define OUTPUT_ACCURACY 2

/* Version number of package */
#define VERSION "0.9.6"

/* define to use DataReel sockets library */
//#define USE_SOCKETS

#define HAVE_INTERNAL_LANES 1

#endif
