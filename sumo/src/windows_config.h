/****************************************************************************/
/// @file    windows_config.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// The general windows configuration file
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef config_h
#define config_h

#ifndef _MSC_VER
#error This file is for MSVC compilation only. GCC should use configure generated config.h.
#endif

/* Disable "identifier truncated in debug info" warnings. */
#pragma warning(disable: 4786)
/* Disable "C++ Exception Specification ignored" warnings */
#pragma warning(disable: 4290)

/* Disable "unsafe" warnings for crt functions in VC++ 2005. */
#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef WIN32
#define WIN32
#endif

/* Define for dynamic Fox linkage */
#define FOXDLL 1

/* defines the precision of floats */
#define SUMOReal float

/* defines the epsilon to use on position comparison */
#define POSITION_EPS 0.1

/* defines the number of digits after the comma in output */
#define OUTPUT_ACCURACY 2

/* Define if auto-generated version.h should be used. */
#define HAVE_VERSION_H 1

/* Version number of package */
#ifndef HAVE_VERSION_H
#define VERSION_STRING "0.9.10"
#endif

#define HAVE_INTERNAL_LANES 1

/* define to use nvwa for memory leak checking */
//#define CHECK_MEMORY_LEAKS 1

#endif
