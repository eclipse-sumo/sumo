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
private:
    double _x;
    double _y;
public:
    Position2D() : _x(0.0), _y(0.0) { }
    Position2D(double x, double y)
        : _x(x), _y(y) { }
    ~Position2D() { }
    double x() const { return _x; }
    double y() const { return _y; }
    friend std::ostream &operator<<(std::ostream &os, const Position2D &p) {
        os << "(" << p.x() << ", " << p.y() << ")";
        return os;
    }
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "Position2D.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

