#ifndef Line2D_h
#define Line2D_h
//---------------------------------------------------------------------------//
//                        Line2D.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.8  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "Position2D.h"
#include <utils/common/DoubleVector.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class Position2DVector;
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class Line2D {
public:
    Line2D();
    Line2D(const Position2D &p1, const Position2D &p2);
    ~Line2D();
    void extrapolateBy(double length);
    void extrapolateFirstBy(double length);
    void extrapolateSecondBy(double length);
    const Position2D &p1() const;
    const Position2D &p2() const;
    Position2D getPositionAtDistance(double offset) const;
    void move2side(double amount);
    DoubleVector intersectsAtLengths(const Position2DVector &v);
    double intersectsAtLength(const Line2D &v);
    double atan2Angle() const;
    double atan2DegreeAngle() const;
    double atan2PositiveAngle() const;
    bool intersects(const Line2D &l) const;
    Position2D intersectsAt(const Line2D &l) const;
    double length() const;
    void add(double x, double y);
    void add(const Position2D &p);
    void sub(double x, double y);
    double distanceTo(const Position2D &p) const;
    Line2D &reverse();
    double nearestPositionTo(const Position2D &p);

private:
    Position2D myP1, myP2;
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
