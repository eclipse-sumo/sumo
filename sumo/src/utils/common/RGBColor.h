/****************************************************************************/
/// @file    RGBColor.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A RGB-color definition
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
#ifndef RGBColor_h
#define RGBColor_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RGBColor
 * The definition of a color in the RGB-space.
 * The cube is meant to lie between (0, 0, 0) and (1, 1, 1)
 */
class RGBColor
{
public:
    /// Standard constructor
    RGBColor();

    /// Parametrised constructor
    RGBColor(SUMOReal red, SUMOReal green, SUMOReal blue);

    /// Copy constructor
    RGBColor(const RGBColor &col);

    /// Destructor
    ~RGBColor();

    /// Returns the red-amount of the color
    SUMOReal red() const;

    /// Returns the green-amount of the color
    SUMOReal green() const;

    /// Returns the blue-amount of the color
    SUMOReal blue() const;

    /// Writes the color to the given stream
    friend std::ostream &operator<<(std::ostream &os, const RGBColor &col);

    bool operator==(const RGBColor &c) const;
    bool operator!=(const RGBColor &c) const;

    /** @brief parses a color information
        It is assumed that the color is stored as "<RED>,<GREEN>,<BLUE>"
        And each color is represented as a SUMOReal. */
    static RGBColor parseColor(const std::string &coldef);

    /** @brief interpolates between two colors
        The interpolated color is calculated as a weighted average of the RGB values of minColor and
        maxColor, giving weight to maxColor and 1-weight to minColor. */
    static RGBColor interpolate(const RGBColor &minColor, const RGBColor &maxColor, SUMOReal weight);

    /// The string description of the default color
    static const std::string DEFAULT_COLOR_STRING;

    /// The default color (for vehicle types and vehicles)
    static const RGBColor DEFAULT_COLOR;
private:
    /// The color amounts
    SUMOReal myRed, myGreen, myBlue;

};


#endif

/****************************************************************************/

