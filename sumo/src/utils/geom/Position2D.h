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
// Revision 1.8  2003/10/15 11:56:30  dkrajzew
// further work on vissim-import
//
// Revision 1.7  2003/09/05 15:27:38  dkrajzew
// changes from adding internal lanes and further work on node geometry
//
// Revision 1.6  2003/08/14 14:05:50  dkrajzew
// functions to process a nodes geometry added
//
// Revision 1.5  2003/04/07 12:22:31  dkrajzew
// first steps towards a junctions geometry
//
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
#include <cmath>

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

    /// Adds the given position to this one
    void add(double dx, double dy) {
        _x += dx;
        _y += dy;
    }

    /// Adds the given position to this one
    void sub(double dx, double dy) {
        _x -= dx;
        _y -= dy;
    }

    void reshiftRotate(double xoff, double yoff, double rot) {
        _x = _x * cos(rot) + _y * sin(rot) + xoff;
        _y = _y * cos(rot) - _x * sin(rot) + yoff;
    }


    /// Prints to the output
    friend std::ostream &operator<<(std::ostream &os, const Position2D &p) {
        os << p.x() << "," << p.y();
        return os;
    }

    friend bool operator==(const Position2D &p1, const Position2D &p2) {
        return p1.x()==p2.x() && p1.y()==p2.y();
    }

    friend bool operator!=(const Position2D &p1, const Position2D &p2) {
        return p1.x()!=p2.x() || p1.y()!=p2.y();
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

