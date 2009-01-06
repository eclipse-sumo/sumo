/****************************************************************************/
/// @file    Polygon2D.h
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// A 2d-polygon
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
#ifndef Polygon2D_h
#define Polygon2D_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position2D.h>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
class Polygon2D
{


public:
    /// Constructor
    Polygon2D(const std::string name, const std::string type,
              const RGBColor &color, const Position2DVector &Pos, bool fill);

    /// Destructor
    virtual ~Polygon2D();

    /// add the incoming Position to the Polygon2D
    void addPolyPosition(Position2DVector &myNewPos);

    // return the name of the Polygon
    const std::string &getName() const;

    // return the type of the Polygon
    const std::string &getType() const;

    // return the Color of the polygon
    const RGBColor &getColor() const;

    // return the Positions Vector of the Polygon
    const Position2DVector &getPosition2DVector(void) const;

    bool fill() const;

protected:
    /// the name of the Polygon
    std::string myName;

    /// the type of the polygon
    std::string myType;

    /// the color of the Polygon
    RGBColor myColor;

    /// the positions of the Polygon
    Position2DVector myPos;

    /// Information whether the polygon has to be filled
    bool myFill;

};


#endif

/****************************************************************************/

