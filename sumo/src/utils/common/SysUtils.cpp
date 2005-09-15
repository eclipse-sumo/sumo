//---------------------------------------------------------------------------//
//                        SysUtils.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * member method definitions
 * ======================================================================= */
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

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
