#ifndef RGBColor_h
#define RGBColor_h
//---------------------------------------------------------------------------//
//                        RGBColor.h -
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
// $Log$
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

#include <iostream>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RGBColor
 * The definition of a color in the RGB-space.
 * The cube is meant to lie between (0, 0, 0) and (1, 1, 1)
 */
class RGBColor {
public:
    /// Default constructor - the color is marked as being undefined
    RGBColor();

    /// Parametrised constructor
    RGBColor(SUMOReal red, SUMOReal green, SUMOReal blue);

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

private:
    static inline SUMOReal check(const SUMOReal &c) {
        if(c<0) {
            return 0;
        } else if(c>1.0) {
            return 1;
        } else {
            return c;
        }
    }

    static inline SUMOReal addChecking(const SUMOReal &c1, const SUMOReal &c2) {
        return check(c1+c2);
    }

    static inline SUMOReal subChecking(const SUMOReal &c1, const SUMOReal &c2) {
        return check(c1-c2);
    }

    static inline SUMOReal mulChecking(const SUMOReal &c1, const SUMOReal &c2) {
        return check(c1*c2);
    }

    static inline SUMOReal divChecking(const SUMOReal &c1, const SUMOReal &c2) {
        return check(c1/c2);
    }

private:
    /// The color amounts
    SUMOReal myRed, myGreen, myBlue;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

