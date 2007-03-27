/****************************************************************************/
/// @file    RGBColor.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A RGB-color definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cmath>
#include "RGBColor.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RGBColor::RGBColor()
        : myRed(-1), myGreen(-1), myBlue(-1)
{}


RGBColor::RGBColor(SUMOReal red, SUMOReal green, SUMOReal blue)
        : myRed(red), myGreen(green), myBlue(blue)
{}


RGBColor::RGBColor(const RGBColor& col)
        : myRed(col.myRed), myGreen(col.myGreen), myBlue(col.myBlue)
{}


RGBColor::~RGBColor()
{}


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
    if (r>1.0) r = 1.0;
    if (g>1.0) g = 1.0;
    if (b>1.0) b = 1.0;
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
    if (r<0) r = 0;
    if (g<0) g = 0;
    if (b<0) b = 0;
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
    return fabs(myRed-c.myRed)<0.1 && fabs(myGreen-c.myGreen)<0.1 && fabs(myBlue-c.myBlue)<0.1;
    //return myRed==c.myRed&&myGreen==c.myGreen&&myBlue==c.myBlue;
}


bool
RGBColor::operator!=(const RGBColor &c) const
{
    return fabs(myRed-c.myRed)>0.1 || fabs(myGreen-c.myGreen)>0.1 || fabs(myBlue-c.myBlue)>0.1;
    //return myRed!=c.myRed||myGreen!=c.myGreen||myBlue!=c.myBlue;
}



/****************************************************************************/

