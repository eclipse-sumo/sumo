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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <cassert>
#include <string>
#include <sstream>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
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
const RGBColor RGBColor::RED = RGBColor(255, 0, 0, 255);
const RGBColor RGBColor::GREEN = RGBColor(0, 255, 0, 255);
const RGBColor RGBColor::BLUE = RGBColor(0, 0, 255, 255);
const RGBColor RGBColor::YELLOW = RGBColor(255, 255, 0, 255);
const RGBColor RGBColor::CYAN = RGBColor(0, 255, 255, 255);
const RGBColor RGBColor::MAGENTA = RGBColor(255, 0, 255, 255);
const RGBColor RGBColor::WHITE = RGBColor(255, 255, 255, 255);
const RGBColor RGBColor::BLACK = RGBColor(0, 0, 0, 255);
const RGBColor RGBColor::GREY = RGBColor(128, 128, 128, 255);

const RGBColor RGBColor::DEFAULT_COLOR = RGBColor::YELLOW;
const std::string RGBColor::DEFAULT_COLOR_STRING = toString(RGBColor::DEFAULT_COLOR);


// ===========================================================================
// method definitions
// ===========================================================================
RGBColor::RGBColor()
    : myRed(0), myGreen(0), myBlue(0), myAlpha(0) {}


RGBColor::RGBColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    : myRed(red), myGreen(green), myBlue(blue), myAlpha(alpha) {}


RGBColor::RGBColor(const RGBColor& col)
    : myRed(col.myRed), myGreen(col.myGreen), myBlue(col.myBlue), myAlpha(col.myAlpha) {}


RGBColor::~RGBColor() {}


void
RGBColor::set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    myRed = r;
    myGreen = g;
    myBlue = b;
    myAlpha = a;
}


std::ostream&
operator<<(std::ostream& os, const RGBColor& col) {
    if (col == RGBColor::RED) {
        return os << "red";
    }
    if (col == RGBColor::GREEN) {
        return os << "green";
    }
    if (col == RGBColor::BLUE) {
        return os << "blue";
    }
    if (col == RGBColor::YELLOW) {
        return os << "yellow";
    }
    if (col == RGBColor::CYAN) {
        return os << "cyan";
    }
    if (col == RGBColor::MAGENTA) {
        return os << "magenta";
    }
    if (col == RGBColor::WHITE) {
        return os << "white";
    }
    if (col == RGBColor::BLACK) {
        return os << "black";
    }
    if (col == RGBColor::GREY) {
        return os << "grey";
    }
    os << static_cast<int>(col.myRed) << ","
       << static_cast<int>(col.myGreen) << ","
       << static_cast<int>(col.myBlue);
    if (col.myAlpha < 255) {
        os << "," << static_cast<int>(col.myAlpha);
    }
    return os;
}


bool
RGBColor::operator==(const RGBColor& c) const {
    return myRed == c.myRed && myGreen == c.myGreen && myBlue == c.myBlue && myAlpha == c.myAlpha;
}


bool
RGBColor::operator!=(const RGBColor& c) const {
    return myRed != c.myRed || myGreen != c.myGreen || myBlue != c.myBlue || myAlpha != c.myAlpha;
}


RGBColor
RGBColor::changedBrightness(int change, int toChange) const {
    const unsigned char red = (unsigned char)(MIN2(MAX2(myRed + change, 0), 255));
    const unsigned char blue = (unsigned char)(MIN2(MAX2(myBlue + change, 0), 255));
    const unsigned char green = (unsigned char)(MIN2(MAX2(myGreen + change, 0), 255));
    int changed = ((int)red - (int)myRed) + ((int)blue - (int)myBlue) + ((int)green - (int)myGreen);
    const RGBColor result(red, green, blue, myAlpha);
    if (changed == toChange * change) {
        return result;
    } else if (changed == 0) {
        return result;
    } else {
        const int maxedColors = (red != myRed + change ? 1 : 0) + (blue != myBlue + change ? 1 : 0) + (green != myGreen + change ? 1 : 0);
        if (maxedColors == 3) {
            return result;
        } else {
            const int toChangeNext = 3 - maxedColors;
            return result.changedBrightness((int)((toChange * change - changed) / toChangeNext), toChangeNext);
        }
    }
}

RGBColor
RGBColor::parseColor(std::string coldef) {
    std::transform(coldef.begin(), coldef.end(), coldef.begin(), tolower);
    if (coldef == "red") {
        return RED;
    }
    if (coldef == "green") {
        return GREEN;
    }
    if (coldef == "blue") {
        return BLUE;
    }
    if (coldef == "yellow") {
        return YELLOW;
    }
    if (coldef == "cyan") {
        return CYAN;
    }
    if (coldef == "magenta") {
        return MAGENTA;
    }
    if (coldef == "white") {
        return WHITE;
    }
    if (coldef == "black") {
        return BLACK;
    }
    if (coldef == "grey" || coldef == "gray") {
        return GREY;
    }
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 255;
    if (coldef[0] == '#') {
        const int coldesc = TplConvert::_hex2int(coldef.c_str());
        if (coldef.length() == 7) {
            r = static_cast<unsigned char>((coldesc & 0xFF0000) >> 16);
            g = static_cast<unsigned char>((coldesc & 0x00FF00) >> 8);
            b = coldesc & 0xFF;
        } else if (coldef.length() == 9) {
            r = static_cast<unsigned char>((coldesc & 0xFF000000) >> 24);
            g = static_cast<unsigned char>((coldesc & 0x00FF0000) >> 16);
            b = static_cast<unsigned char>((coldesc & 0x0000FF00) >> 8);
            a = coldesc & 0xFF;
        } else {
            throw EmptyData();
        }
    } else {
        std::vector<std::string> st = StringTokenizer(coldef, ",").getVector();
        if (st.size() == 3 || st.size() == 4) {
            try {
                r = static_cast<unsigned char>(TplConvert::_2int(st[0].c_str()));
                g = static_cast<unsigned char>(TplConvert::_2int(st[1].c_str()));
                b = static_cast<unsigned char>(TplConvert::_2int(st[2].c_str()));
                if (st.size() == 4) {
                    a = static_cast<unsigned char>(TplConvert::_2int(st[3].c_str()));
                }
                if (r <= 1 && g <= 1 && b <= 1 && (st.size() == 3 || a <= 1)) {
                    throw NumberFormatException();
                }
            } catch (NumberFormatException&) {
                r = static_cast<unsigned char>(TplConvert::_2SUMOReal(st[0].c_str()) * 255. + 0.5);
                g = static_cast<unsigned char>(TplConvert::_2SUMOReal(st[1].c_str()) * 255. + 0.5);
                b = static_cast<unsigned char>(TplConvert::_2SUMOReal(st[2].c_str()) * 255. + 0.5);
                if (st.size() == 4) {
                    a = static_cast<unsigned char>(TplConvert::_2SUMOReal(st[3].c_str()) * 255. + 0.5);
                }
            }
        } else {
            throw EmptyData();
        }
    }
    return RGBColor(r, g, b, a);
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
    const unsigned char r = (unsigned char)((int)minColor.myRed   + (((int)maxColor.myRed   - (int)minColor.myRed)   * weight));
    const unsigned char g = (unsigned char)((int)minColor.myGreen + (((int)maxColor.myGreen - (int)minColor.myGreen) * weight));
    const unsigned char b = (unsigned char)((int)minColor.myBlue  + (((int)maxColor.myBlue  - (int)minColor.myBlue)  * weight));
    const unsigned char a = (unsigned char)((int)minColor.myAlpha + (((int)maxColor.myAlpha - (int)minColor.myAlpha) * weight));
    return RGBColor(r, g, b, a);
}


RGBColor
RGBColor::fromHSV(SUMOReal h, SUMOReal s, SUMOReal v) {
    // H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
    // RGB are each returned on [0, 255].
    //float h = HSV.H, s = HSV.S, v = HSV.V,
    SUMOReal f;
    h /= 60.;
    int i;
    //if (h == UNDEFINED) RETURN_RGB(v, v, v);
    i = int(floor(h));
    f = float(h - i);
    if (!(i & 1)) {
        f = 1 - f;    // if i is even
    }
    const unsigned char m = static_cast<unsigned char>(v * (1 - s) * 255. + 0.5);
    const unsigned char n = static_cast<unsigned char>(v * (1 - s * f) * 255. + 0.5);
    const unsigned char vv = static_cast<unsigned char>(v * 255. + 0.5);
    switch (i) {
        case 6:
        case 0:
            return RGBColor(vv, n, m, 255);
        case 1:
            return RGBColor(n, vv, m, 255);
        case 2:
            return RGBColor(m, vv, n, 255);
        case 3:
            return RGBColor(m, n, vv, 255);
        case 4:
            return RGBColor(n, m, vv, 255);
        case 5:
            return RGBColor(vv, m, n, 255);
    }
    return RGBColor(255, 255, 255, 255);
}


/****************************************************************************/

