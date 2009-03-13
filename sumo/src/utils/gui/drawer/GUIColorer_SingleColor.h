/****************************************************************************/
/// @file    GUIColorer_SingleColor.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUIColorer_SingleColor_h
#define GUIColorer_SingleColor_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIBaseColorer.h"
#include <utils/common/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIColorer_SingleColor
 * @brief Uses a single color for coloring
 */
template<class T>
class GUIColorer_SingleColor : public GUIBaseColorer<T> {
public:
    /// Constructor
    GUIColorer_SingleColor(const RGBColor &c) : myColor(c) { }

    /// Destructor
    virtual ~GUIColorer_SingleColor() { }


    /// @name inherited from GUIBaseColorer
    //@{

    /// Sets the color using a value from the given instance of T
    void setGlColor(const T&) const {
        glColor3d(myColor.red(), myColor.green(), myColor.blue());
    }

    /// Sets the color using the given value
    void setGlColor(SUMOReal) const {
        glColor3d(myColor.red(), myColor.green(), myColor.blue());
    }

    /// Returns the type of this class (CST_SINGLE)
    virtual ColorSetType getSetType() const {
        return CST_SINGLE;
    }
    //@}


    /// @name inherited from from GUIBaseColorerInterface
    //@{

    /// Sets the given color as the colors to use
    virtual void resetColor(const RGBColor &c) {
        myColor = c;
    }

    /// Returns the single color used
    virtual const RGBColor &getSingleColor() const {
        return myColor;
    }
    //@}

private:
    /// The color to use
    RGBColor myColor;

};


#endif

/****************************************************************************/

