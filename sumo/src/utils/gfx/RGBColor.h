/****************************************************************************/
/// @file    RGBColor.h
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
    /// Default constructor - the color is marked as being undefined
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

    /// Increases the color's brightness
    void brighten(SUMOReal by=0.25);

    /// Decreases the color's brightness
    void darken(SUMOReal by=0.25);

    friend RGBColor operator+(const RGBColor &c1, const RGBColor &c2);

    friend RGBColor operator-(const RGBColor &c1, const RGBColor &c2);

    friend RGBColor operator*(const RGBColor &c, const SUMOReal &v);

    friend RGBColor operator/(const RGBColor &c, const SUMOReal &v);

    bool operator==(const RGBColor &c) const;
    bool operator!=(const RGBColor &c) const;

private:
    static inline SUMOReal check(const SUMOReal &c)
    {
        if (c<0) {
            return 0;
        } else if (c>1.0) {
            return 1;
        } else {
            return c;
        }
    }

    static inline SUMOReal addChecking(const SUMOReal &c1, const SUMOReal &c2)
    {
        return check(c1+c2);
    }

    static inline SUMOReal subChecking(const SUMOReal &c1, const SUMOReal &c2)
    {
        return check(c1-c2);
    }

    static inline SUMOReal mulChecking(const SUMOReal &c1, const SUMOReal &c2)
    {
        return check(c1*c2);
    }

    static inline SUMOReal divChecking(const SUMOReal &c1, const SUMOReal &c2)
    {
        return check(c1/c2);
    }

private:
    /// The color amounts
    SUMOReal myRed, myGreen, myBlue;

};


#endif

/****************************************************************************/

