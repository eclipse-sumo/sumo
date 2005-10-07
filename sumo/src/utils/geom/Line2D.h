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
// Revision 1.12  2005/10/07 11:44:16  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.11  2005/09/23 06:07:01  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 12:18:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/07/12 12:49:07  dkrajzew
// code style adapted
//
// Revision 1.8  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
    void extrapolateBy(SUMOReal length);
    void extrapolateFirstBy(SUMOReal length);
    void extrapolateSecondBy(SUMOReal length);
    const Position2D &p1() const;
    const Position2D &p2() const;
    Position2D getPositionAtDistance(SUMOReal offset) const;
    void move2side(SUMOReal amount);
    DoubleVector intersectsAtLengths(const Position2DVector &v);
    SUMOReal intersectsAtLength(const Line2D &v);
    SUMOReal atan2Angle() const;
    SUMOReal atan2DegreeAngle() const;
    SUMOReal atan2PositiveAngle() const;
    bool intersects(const Line2D &l) const;
    Position2D intersectsAt(const Line2D &l) const;
    SUMOReal length() const;
    void add(SUMOReal x, SUMOReal y);
    void add(const Position2D &p);
    void sub(SUMOReal x, SUMOReal y);
    SUMOReal distanceTo(const Position2D &p) const;
    Line2D &reverse();
    SUMOReal nearestPositionTo(const Position2D &p);

private:
    Position2D myP1, myP2;
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
