#ifndef Position2D_h
#define Position2D_h
//---------------------------------------------------------------------------//
//                        Position2D.h -
//  A position in a 2D-world
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
// Revision 1.4  2003/03/20 16:41:10  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/12 16:35:39  dkrajzew
// some further functionality added needed by the artemis-import
//
// Revision 1.2  2003/02/07 10:50:20  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class Position2D {
public:
    /// default constructor
    Position2D() : _x(0.0), _y(0.0) { }

    /// parametrised constructor
    Position2D(double x, double y)
        : _x(x), _y(y) { }

    /// Destructor
    ~Position2D() { }

    /// Returns the x-position
    double x() const { return _x; }

    /// Returns the y-position
    double y() const { return _y; }

    /// Multiplies both positions with the given value
    void mul(double val) {
        _x *= val;
        _y *= val;
    }

    /// Adds the given position to this one
    void add(const Position2D &pos) {
        _x += pos._x;
        _y += pos._y;
    }

    /// Prints to the output
    friend std::ostream &operator<<(std::ostream &os, const Position2D &p) {
        os << "(" << p.x() << ", " << p.y() << ")";
        return os;
    }

private:
    /// The x-position
    double _x;

    /// The y-position
    double _y;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "Position2D.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

