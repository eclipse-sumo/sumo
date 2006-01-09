//---------------------------------------------------------------------------//
//                        RGBColor.cpp -
//  A RGB-color definition
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.8  2006/01/09 13:35:32  dkrajzew
// debugging vehicle color usage
//
// Revision 1.7  2005/10/07 11:44:28  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:07:13  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:18:33  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/11/23 10:35:01  dkrajzew
// debugging
//
// Revision 1.3  2003/07/16 15:38:51  dkrajzew
// handling of colors improved
//
// Revision 1.2  2003/02/07 10:50:53  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "RGBColor.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
RGBColor::RGBColor()
    : myRed(-1), myGreen(-1), myBlue(-1)
{
}


RGBColor::RGBColor(SUMOReal red, SUMOReal green, SUMOReal blue)
    : myRed(red), myGreen(green), myBlue(blue)
{
}


RGBColor::~RGBColor()
{
}


SUMOReal
RGBColor::red() const
{
    return myRed;
}


SUMOReal
RGBColor::green() const
{
    return myGreen;
}


SUMOReal
RGBColor::blue() const
{
    return myBlue;
}


void
RGBColor::brighten(SUMOReal by)
{
    SUMOReal r = myRed + by;
    SUMOReal g = myGreen + by;
    SUMOReal b = myBlue + by;
    if(r>1.0) r = 1.0;
    if(g>1.0) g = 1.0;
    if(b>1.0) b = 1.0;
    myRed = r;
    myGreen = g;
    myBlue = b;
}


void
RGBColor::darken(SUMOReal by)
{
    SUMOReal r = myRed - by;
    SUMOReal g = myGreen - by;
    SUMOReal b = myBlue - by;
    if(r<0) r = 0;
    if(g<0) g = 0;
    if(b<0) b = 0;
    myRed = r;
    myGreen = g;
    myBlue = b;
}


std::ostream &
operator<<(std::ostream &os, const RGBColor &col)
{
    os
        << col.myRed << ","
        << col.myGreen << ","
        << col.myBlue;
    return os;
}


RGBColor
operator+(const RGBColor &c1, const RGBColor &c2)
{
    return RGBColor(
        RGBColor::addChecking(c1.myRed, c2.myRed),
        RGBColor::addChecking(c1.myGreen, c2.myGreen),
        RGBColor::addChecking(c1.myBlue, c2.myBlue));
}


RGBColor
operator-(const RGBColor &c1, const RGBColor &c2)
{
    return RGBColor(
        RGBColor::subChecking(c1.myRed, c2.myRed),
        RGBColor::subChecking(c1.myGreen, c2.myGreen),
        RGBColor::subChecking(c1.myBlue, c2.myBlue));
}


RGBColor
operator*(const RGBColor &c, const SUMOReal &v)
{
    return RGBColor(
        RGBColor::mulChecking(c.myRed, v),
        RGBColor::mulChecking(c.myGreen, v),
        RGBColor::mulChecking(c.myBlue, v));
}


RGBColor
operator/(const RGBColor &c, const SUMOReal &v)
{
    return RGBColor(
        RGBColor::divChecking(c.myRed, v),
        RGBColor::divChecking(c.myGreen, v),
        RGBColor::divChecking(c.myBlue, v));
}


bool
RGBColor::operator==(const RGBColor &c) const
{
    return myRed==c.myRed&&myGreen==c.myGreen&&myBlue==c.myBlue;
}


bool
RGBColor::operator!=(const RGBColor &c) const
{
    return myRed!=c.myRed||myGreen!=c.myGreen||myBlue!=c.myBlue;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


