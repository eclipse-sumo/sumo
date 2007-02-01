/****************************************************************************/
/// @file    GUIRoute.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id: $
///
// A vehicle route
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
#ifndef GUIRoute_h
#define GUIRoute_h
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

#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gui/div/GUIColorSetter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIRoute
 * Just a MSRoute extended by a color
 */
class GUIRoute : public MSRoute
{
public:
    /// Constructor
    GUIRoute(const RGBColor &c,
             const std::string &id, const MSEdgeVector &edges,
             bool multipleReferenced);

    /// Destructor
    ~GUIRoute();

    /// Returns the color
    const RGBColor &getColor() const;

    /// Sets the color in openGL
    inline void setColor() const
    {
        mglColor(myColor);
    }

private:
    /// The color
    RGBColor myColor;

};


#endif

/****************************************************************************/

