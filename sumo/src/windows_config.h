#ifndef config_h
#define config_h

#ifdef __GNUC__
#error This file is for MSVC compilation only. GCC should use configure generated config.h.
#endif

/* Disable "unsafe" warnings for crt functions in VC++ 2005. */
#if defined _MSC_VER && _MSC_VER >= 1400
#define _CRT_SECURE_NO_WARNINGS
#endif

/* Name of package */
#define PACKAGE "sumo"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

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

/* define to use DataReel sockets libray */
//#define USE_SOCKETS

/* define to use XML-RPC */
//#define _RPC

#define HAVE_INTERNAL_LANES 1

#endif
