/****************************************************************************/
/// @file    GUIRoute.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id$
///
// A vehicle route
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
#ifndef GUIRoute_h
#define GUIRoute_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <utils/common/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIRoute
 * Just a MSRoute extended by a color
 */
class GUIRoute : public MSRoute {
public:
    /// Constructor
    GUIRoute(const RGBColor &c,
             const std::string &id, const MSEdgeVector &edges,
             bool multipleReferenced) throw();

    /// Destructor
    ~GUIRoute() throw();

    /// Returns the color
    const RGBColor &getColor() const;

    /// Sets the color in openGL
    inline void setColor() const {
        glColor3d(myColor.red(), myColor.green(), myColor.blue());
    }

private:
    /// The color
    RGBColor myColor;

};


#endif

/****************************************************************************/

