//---------------------------------------------------------------------------//
//                        Boundery.cpp -
//  A class that stores the 2D geometrical boundery
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
// Revision 1.6  2003/11/11 08:00:30  dkrajzew
// consequent usage of Position2D instead of two doubles
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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <utility>

#include "GeomHelper.h"
#include "Boundery.h"
#include "Position2DVector.h"
#include "Position2D.h"

/* =========================================================================
 * method definitions
 * ======================================================================= */
Boundery::Boundery()
    : _xmin(10000000000.0), _xmax(-10000000000.0),
    _ymin(10000000000.0), _ymax(-10000000000.0)
{
}

Boundery::Boundery(double x1, double y1, double x2, double y2)
    : _xmin(10000000000.0), _xmax(-10000000000.0),
    _ymin(10000000000.0), _ymax(-10000000000.0)
{
    add(x1, y1);
    add(x2, y2);
}


Boundery::~Boundery()
{
}


void
Boundery::add(double x, double y)
{
    _xmin = _xmin < x ? _xmin : x;
    _xmax = _xmax > x ? _xmax : x;
    _ymin = _ymin < y ? _ymin : y;
    _ymax = _ymax > y ? _ymax : y;
}


void
Boundery::add(const Position2D &p)
{
    add(p.x(), p.y());
}


void
Boundery::add(const Boundery &p)
{
    add(p.xmin(), p.ymin());
    add(p.xmax(), p.ymax());
}



Position2D
Boundery::getCenter() const
{
    return Position2D( (_xmin+_xmax)/2.0, (_ymin+_ymax)/2.0);
}

double
Boundery::xmin() const
{
    return _xmin;
}


double
Boundery::xmax() const
{
    return _xmax;
}


double
Boundery::ymin() const
{
    return _ymin;
}


double
Boundery::ymax() const
{
    return _ymax;
}


double
Boundery::getWidth() const
{
    return _xmax - _xmin;
}


double
Boundery::getHeight() const
{
    return _ymax - _ymin;
}


bool
Boundery::around(const Position2D &p, double offset) const
{
    return
        (p.x()<=_xmax+offset && p.x()>=_xmin-offset) &&
        (p.y()<=_ymax+offset && p.y()>=_ymin-offset);
}


bool
Boundery::overlapsWith(const AbstractPoly &p, double offset) const
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
Boundery::crosses(const Position2D &p1, const Position2D &p2) const
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
Boundery::partialWithin(const AbstractPoly &poly, double offset) const
{
    return
        poly.around(Position2D(_xmax, _ymax), offset) ||
        poly.around(Position2D(_xmin, _ymax), offset) ||
        poly.around(Position2D(_xmax, _ymin), offset) ||
        poly.around(Position2D(_xmin, _ymin), offset);
}


void
Boundery::grow(double by)
{
    _xmax += by;
    _ymax += by;
    _xmin -= by;
    _ymin -= by;
}


void
Boundery::flipY()
{
    _ymin *= -1.0;
    _ymax *= -1.0;
    double tmp = _ymin;
    _ymin = _ymax;
    _ymax = tmp;
}



std::ostream &
operator<<(std::ostream &os, const Boundery &b)
{
    os << "((" << b._xmin << ", " << b._ymin
        << "), (" << b._xmax << ", " << b._ymax
        << "))";
    return os;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "Boundery.icc"
//#endif

// Local Variables:
// mode:C++
// End:


