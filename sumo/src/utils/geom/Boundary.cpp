//---------------------------------------------------------------------------//
//                        Boundary.cpp -
//  A class that stores the 2D geometrical boundary
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/10/07 11:44:16  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 06:07:01  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:18:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:34:46  dkrajzew
// debugging
//
// Revision 1.1  2004/10/29 06:25:23  dksumo
// boundery renamed to boundary
//
// Revision 1.1  2004/10/22 12:50:43  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.8  2004/07/02 09:44:40  dkrajzew
// changes for 0.8.0.2
//
// Revision 1.7  2004/03/19 13:01:11  dkrajzew
// methods needed for the new selection within the gui added; some style adaptions
//
// Revision 1.6  2003/11/11 08:00:30  dkrajzew
// consequent usage of Position2D instead of two SUMOReals
//
// Revision 1.5  2003/05/20 09:50:19  dkrajzew
// further work and debugging
//
// Revision 1.4  2003/03/18 13:16:57  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/12 16:35:41  dkrajzew
// some further functionality added needed by the artemis-import
//
// Revision 1.2  2003/02/07 10:50:20  dkrajzew
// updated
//
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
#include <utility>

#include "GeomHelper.h"
#include "Boundary.h"
#include "Position2DVector.h"
#include "Position2D.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
Boundary::Boundary()
    : _xmin(10000000000.0), _xmax(-10000000000.0),
    _ymin(10000000000.0), _ymax(-10000000000.0)
{
}

Boundary::Boundary(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2)
    : _xmin(10000000000.0), _xmax(-10000000000.0),
    _ymin(10000000000.0), _ymax(-10000000000.0)
{
    add(x1, y1);
    add(x2, y2);
}


Boundary::~Boundary()
{
}


void
Boundary::add(SUMOReal x, SUMOReal y)
{
    _xmin = _xmin < x ? _xmin : x;
    _xmax = _xmax > x ? _xmax : x;
    _ymin = _ymin < y ? _ymin : y;
    _ymax = _ymax > y ? _ymax : y;
}


void
Boundary::add(const Position2D &p)
{
    add(p.x(), p.y());
}


void
Boundary::add(const Boundary &p)
{
    add(p.xmin(), p.ymin());
    add(p.xmax(), p.ymax());
}



Position2D
Boundary::getCenter() const
{
    return Position2D( (_xmin+_xmax)/(SUMOReal) 2.0, (_ymin+_ymax)/(SUMOReal) 2.0);
}

SUMOReal
Boundary::xmin() const
{
    return _xmin;
}


SUMOReal
Boundary::xmax() const
{
    return _xmax;
}


SUMOReal
Boundary::ymin() const
{
    return _ymin;
}


SUMOReal
Boundary::ymax() const
{
    return _ymax;
}


SUMOReal
Boundary::getWidth() const
{
    return _xmax - _xmin;
}


SUMOReal
Boundary::getHeight() const
{
    return _ymax - _ymin;
}


bool
Boundary::around(const Position2D &p, SUMOReal offset) const
{
    return
        (p.x()<=_xmax+offset && p.x()>=_xmin-offset) &&
        (p.y()<=_ymax+offset && p.y()>=_ymin-offset);
}


bool
Boundary::overlapsWith(const AbstractPoly &p, SUMOReal offset) const
{
    if(
        // check whether one of my points lies within the given poly
        partialWithin(p, offset) ||
        // check whether the polygon lies within me
        p.partialWithin(*this, offset) ) {
        return true;
    }
    // check whether the bounderies cross
    return
        p.crosses(Position2D(_xmax+offset, _ymax+offset), Position2D(_xmin-offset, _ymax+offset))
        ||
        p.crosses(Position2D(_xmin-offset, _ymax+offset), Position2D(_xmin-offset, _ymin-offset))
        ||
        p.crosses(Position2D(_xmin-offset, _ymin-offset), Position2D(_xmax+offset, _ymin-offset))
        ||
        p.crosses(Position2D(_xmax+offset, _ymin-offset), Position2D(_xmax+offset, _ymax+offset));
}


bool
Boundary::crosses(const Position2D &p1, const Position2D &p2) const
{
    return
        GeomHelper::intersects(p1, p2, Position2D(_xmax, _ymax), Position2D(_xmin, _ymax))
        ||
        GeomHelper::intersects(p1, p2, Position2D(_xmin, _ymax), Position2D(_xmin, _ymin))
        ||
        GeomHelper::intersects(p1, p2, Position2D(_xmin, _ymin), Position2D(_xmax, _ymin))
        ||
        GeomHelper::intersects(p1, p2, Position2D(_xmax, _ymin), Position2D(_xmax, _ymax));
}


bool
Boundary::partialWithin(const AbstractPoly &poly, SUMOReal offset) const
{
    return
        poly.around(Position2D(_xmax, _ymax), offset) ||
        poly.around(Position2D(_xmin, _ymax), offset) ||
        poly.around(Position2D(_xmax, _ymin), offset) ||
        poly.around(Position2D(_xmin, _ymin), offset);
}


Boundary &
Boundary::grow(SUMOReal by)
{
    _xmax += by;
    _ymax += by;
    _xmin -= by;
    _ymin -= by;
    return *this;
}


void
Boundary::flipY()
{
    _ymin *= -1.0;
    _ymax *= -1.0;
    SUMOReal tmp = _ymin;
    _ymin = _ymax;
    _ymax = tmp;
}



std::ostream &
operator<<(std::ostream &os, const Boundary &b)
{
    os << "((" << b._xmin << ", " << b._ymin
        << "), (" << b._xmax << ", " << b._ymax
        << "))";
    return os;
}


void
Boundary::set(SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax)
{
    _xmin = xmin;
    _ymin = ymin;
    _xmax = xmax;
    _ymax = ymax;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


