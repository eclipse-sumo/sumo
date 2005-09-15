#ifndef StdDefs_h
#define StdDefs_h
//---------------------------------------------------------------------------//
//                        StdDefs.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
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
// $Log$
// Revision 1.5  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
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

