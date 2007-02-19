/****************************************************************************/
/// @file    GUIColorer_SingleColor.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUIColorer_SingleColor_h
#define GUIColorer_SingleColor_h
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

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

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
template<class _T>
class GUIColorer_SingleColor : public GUIBaseColorer<_T>
{
public:
    /// Constructor
    GUIColorer_SingleColor(const RGBColor &c) : myColor(c)
    { }

    /// Destructor
    virtual ~GUIColorer_SingleColor()
    { }


    ///{ from GUIBaseColorer
    /// Sets the color using a value from the given instance of _T
    void setGlColor(const _T&) const
    {
        glColor3d(myColor.red(), myColor.green(), myColor.blue());
    }

    /// Sets the color using the given value
    void setGlColor(SUMOReal) const
    {
        glColor3d(myColor.red(), myColor.green(), myColor.blue());
    }

    /// Returns the type of this class (CST_SINGLE)
    virtual ColorSetType getSetType() const
    {
        return CST_SINGLE;
    }
    ///}


    ///{ from GUIBaseColorerInterface
    /// Sets the given color as the colors to use
    virtual void resetColor(const RGBColor &c)
    {
        myColor = c;
    }

    /// Returns the single color used
    virtual const RGBColor &getSingleColor() const
    {
        return myColor;
    }
    ///}

private:
    /// The color to use
    RGBColor myColor;

};


#endif

/****************************************************************************/

