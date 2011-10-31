/****************************************************************************/
/// @file    Line.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
 *
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
    void move2side(SUMOReal amount);
    DoubleVector intersectsAtLengths(const PositionVector& v);
    SUMOReal intersectsAtLength(const Line& v);
    SUMOReal atan2Angle() const;
    SUMOReal atan2DegreeAngle() const;
    SUMOReal atan2PositiveAngle() const;
    bool intersects(const Line& l) const;
    Position intersectsAt(const Line& l) const;
    SUMOReal length() const;
    void add(SUMOReal x, SUMOReal y);
    void add(const Position& p);
    void sub(SUMOReal x, SUMOReal y);
    Line& reverse();
    void rotateAtP1(SUMOReal rot);
    void rotateAround(const Position& at, SUMOReal rot);

private:
    Position myP1, myP2;
};


#endif

/****************************************************************************/

