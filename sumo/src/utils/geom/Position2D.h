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
// Revision 1.15  2005/10/07 11:44:16  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.14  2005/09/23 06:07:01  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.13  2005/09/15 12:18:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.12  2004/11/23 10:34:46  dkrajzew
// debugging
//
// Revision 1.11  2004/08/02 12:50:05  dkrajzew
// added the possibility to multiply each of the ccordinates with a value individually
//
// Revision 1.10  2004/03/19 13:01:11  dkrajzew
// methods needed for the new selection within the gui added; some style adaptions
//
// Revision 1.9  2003/11/11 08:01:23  dkrajzew
// some further methods implemented
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
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
    Position2D(SUMOReal x, SUMOReal y)
        : _x(x), _y(y) { }

    /// Destructor
    ~Position2D() { }

    /// Returns the x-position
    SUMOReal x() const { return _x; }

    /// Returns the y-position
    SUMOReal y() const { return _y; }

    ///
    void set(SUMOReal x, SUMOReal y) {
        _x = x;
        _y = y;
    }

    ///
    void set(const Position2D &pos) {
        _x = pos._x;
        _y = pos._y;
    }


    /// Multiplies both positions with the given value
    void mul(SUMOReal val) {
        _x *= val;
        _y *= val;
    }

    /// Multiplies position with the given values
    void mul(SUMOReal mx, SUMOReal my) {
        _x *= mx;
        _y *= my;
    }

    /// Adds the given position to this one
    void add(const Position2D &pos) {
        _x += pos._x;
        _y += pos._y;
    }

    /// Adds the given position to this one
    void add(SUMOReal dx, SUMOReal dy) {
        _x += dx;
        _y += dy;
    }

    /// Substracts the given position from this one
    void sub(SUMOReal dx, SUMOReal dy) {
        _x -= dx;
        _y -= dy;
    }

    /// Substracts the given position from this one
    void sub(const Position2D &pos) {
        _x -= pos._x;
        _y -= pos._y;
    }

    SUMOReal scalar() const {
        return sqrt(_x*_x + _y*_y);
    }

    void norm() {
        SUMOReal val = scalar();
        _x = _x / val;
        _y = _y / val;
    }

    void reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot) {
        SUMOReal x = _x * cos(rot) + _y * sin(rot) + xoff;
        SUMOReal y = _y * cos(rot) - _x * sin(rot) + yoff;
        _x = x;
        _y = y;
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
    SUMOReal _x;

    /// The y-position
    SUMOReal _y;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

