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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cmath>
#include <string>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include "RGBColor.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
const std::string RGBColor::DEFAULT_COLOR_STRING = "1,1,0";
const RGBColor RGBColor::DEFAULT_COLOR = RGBColor::parseColor(RGBColor::DEFAULT_COLOR_STRING);


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


std::ostream &
operator<<(std::ostream &os, const RGBColor &col)
{
    os
    << col.myRed << ","
    << col.myGreen << ","
    << col.myBlue;
    return os;
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


RGBColor
RGBColor::parseColor(const std::string &coldef)
{
    StringTokenizer st(coldef, ",");
    if (st.size()<3) {
        throw EmptyData();
    }
    SUMOReal r = TplConvert<char>::_2SUMOReal(st.next().c_str());
    SUMOReal g = TplConvert<char>::_2SUMOReal(st.next().c_str());
    SUMOReal b = TplConvert<char>::_2SUMOReal(st.next().c_str());
    return RGBColor(r, g, b);
}

RGBColor
RGBColor::interpolate(const RGBColor &minColor, const RGBColor &maxColor, SUMOReal weight)
{
    if (weight < 0) weight = 0;
    if (weight > 1) weight = 1;
    SUMOReal r = minColor.myRed + (maxColor.myRed - minColor.myRed) * weight;
    SUMOReal g = minColor.myGreen + (maxColor.myGreen - minColor.myGreen) * weight;
    SUMOReal b = minColor.myBlue + (maxColor.myBlue - minColor.myBlue) * weight;
    return RGBColor(r, g, b);
}

/****************************************************************************/

