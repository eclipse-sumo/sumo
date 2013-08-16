/****************************************************************************/
/// @file    Line.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Line_h
#define Line_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Position.h"
#include <utils/common/VectorHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class PositionVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Class for line segments
 */
class Line {
public:
    Line();
    Line(const Position& p1, const Position& p2);
    ~Line();
    void extrapolateBy(SUMOReal length);
    void extrapolateFirstBy(SUMOReal length);
    void extrapolateSecondBy(SUMOReal length);
    const Position& p1() const;
    const Position& p2() const;
    Position getPositionAtDistance(SUMOReal offset) const;
    Position getPositionAtDistance2D(SUMOReal offset) const;
    void move2side(SUMOReal amount);
    std::vector<SUMOReal> intersectsAtLengths2D(const PositionVector& v);

    /// @brief returns distance between myP1 and intersection or -1 if line segments do not intersect
    SUMOReal intersectsAtLength2D(const Line& v);

    SUMOReal atan2Angle() const;
    SUMOReal atan2DegreeAngle() const;
    SUMOReal atan2PositiveAngle() const;
    SUMOReal atan2DegreeSlope() const;
    bool intersects(const Line& l) const;
    Position intersectsAt(const Line& l) const;
    SUMOReal length() const;
    SUMOReal length2D() const;
    void add(SUMOReal x, SUMOReal y);
    void add(const Position& p);
    void sub(SUMOReal x, SUMOReal y);
    Line& reverse();
    void rotateAtP1(SUMOReal rot);

private:
    Position myP1, myP2;
};


#endif

/****************************************************************************/

