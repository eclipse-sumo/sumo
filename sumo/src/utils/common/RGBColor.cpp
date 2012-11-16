/****************************************************************************/
/// @file    RGBColor.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// A RGB-color definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <sstream>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
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
    : myRed(-1), myGreen(-1), myBlue(-1) {}


RGBColor::RGBColor(SUMOReal red, SUMOReal green, SUMOReal blue)
    : myRed(red), myGreen(green), myBlue(blue) {}


RGBColor::RGBColor(const RGBColor& col)
    : myRed(col.myRed), myGreen(col.myGreen), myBlue(col.myBlue) {}


RGBColor::~RGBColor() {}


void
RGBColor::set(SUMOReal r, SUMOReal g, SUMOReal b) {
    myRed = r;
    myGreen = g;
    myBlue = b;
}



std::ostream&
operator<<(std::ostream& os, const RGBColor& col) {
    os
            << col.myRed << ","
            << col.myGreen << ","
            << col.myBlue;
    return os;
}


bool
RGBColor::operator==(const RGBColor& c) const {
    return fabs(myRed - c.myRed) < 0.1 && fabs(myGreen - c.myGreen) < 0.1 && fabs(myBlue - c.myBlue) < 0.1;
    //return myRed==c.myRed&&myGreen==c.myGreen&&myBlue==c.myBlue;
}


bool
RGBColor::operator!=(const RGBColor& c) const {
    return fabs(myRed - c.myRed) > 0.1 || fabs(myGreen - c.myGreen) > 0.1 || fabs(myBlue - c.myBlue) > 0.1;
    //return myRed!=c.myRed||myGreen!=c.myGreen||myBlue!=c.myBlue;
}


RGBColor
RGBColor::changedBrightness(SUMOReal change) {
    SUMOReal red = MIN2(MAX2(myRed + change, (SUMOReal)0), (SUMOReal)1);
    SUMOReal blue = MIN2(MAX2(myBlue + change, (SUMOReal)0), (SUMOReal)1);
    SUMOReal green = MIN2(MAX2(myGreen + change, (SUMOReal)0), (SUMOReal)1);
    return RGBColor(red, green, blue);

}

RGBColor
RGBColor::parseColor(const std::string& coldef) throw(EmptyData, NumberFormatException) {
    StringTokenizer st(coldef, ",");
    if (st.size() < 3) {
        throw EmptyData();
    }
    const SUMOReal r = TplConvert::_2SUMOReal(st.next().c_str());
    const SUMOReal g = TplConvert::_2SUMOReal(st.next().c_str());
    const SUMOReal b = TplConvert::_2SUMOReal(st.next().c_str());
    return RGBColor(r, g, b);
}


RGBColor
RGBColor::parseColorReporting(
    const std::string& coldef, const std::string& objecttype,
    const char* objectid, bool report, bool& ok) {
    UNUSED_PARAMETER(report);
    try {
        return parseColor(coldef);
    } catch (NumberFormatException&) {
    } catch (EmptyData&) {
    }
    ok = false;
    std::ostringstream oss;
    oss << "Attribute 'color' in definition of ";
    if (objectid == 0) {
        oss << "a ";
    }
    oss << objecttype;
    if (objectid != 0) {
        oss << " '" << objectid << "'";
    }
    oss << " is not a valid color.";
    WRITE_ERROR(oss.str());
    return RGBColor();
}


RGBColor
RGBColor::interpolate(const RGBColor& minColor, const RGBColor& maxColor, SUMOReal weight) {
    if (weight < 0) {
        weight = 0;
    }
    if (weight > 1) {
        weight = 1;
    }
    SUMOReal r = minColor.myRed + (maxColor.myRed - minColor.myRed) * weight;
    SUMOReal g = minColor.myGreen + (maxColor.myGreen - minColor.myGreen) * weight;
    SUMOReal b = minColor.myBlue + (maxColor.myBlue - minColor.myBlue) * weight;
    return RGBColor(r, g, b);
}


RGBColor
RGBColor::fromHSV(SUMOReal h, SUMOReal s, SUMOReal v) {
    // H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
    // RGB are each returned on [0, 1].
    //float h = HSV.H, s = HSV.S, v = HSV.V,
    float m, n, f;
    h /= 60.;
    int i;
    //if (h == UNDEFINED) RETURN_RGB(v, v, v);
    i = int(floor(h));
    f = float(h - i);
    if (!(i & 1)) {
        f = 1 - f;    // if i is even
    }
    m = float(v * (1 - s));
    n = float(v * (1 - s * f));
    switch (i) {
        case 6:
        case 0:
            return RGBColor(v, n, m);
        case 1:
            return RGBColor(n, v, m);
        case 2:
            return RGBColor(m, v, n);
        case 3:
            return RGBColor(m, n, v);
        case 4:
            return RGBColor(n, m, v);
        case 5:
            return RGBColor(v, m, n);
    }
    return RGBColor(1, 1, 1);
}


/****************************************************************************/

