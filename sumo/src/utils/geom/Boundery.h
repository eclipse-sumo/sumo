#ifndef Boundery_h
#define Boundery_h
//---------------------------------------------------------------------------//
//                        Boundery.h -
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
// $Log$
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

#include <iostream>
#include <utility>
#include "AbstractPoly.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class Boundery
        : public AbstractPoly {
public:
    Boundery();
    Boundery(double x1, double y1, double x2, double y2);
    ~Boundery();
    void add(double x, double y);
    void add(const Position2D &p);
    void add(const Boundery &p);
    std::pair<double, double> getCenter() const;
    double xmin() const;
    double xmax() const;
    double ymin() const;
    double ymax() const;
    double getWidth() const;
    double getHeight() const;
    bool around(const Position2D &p, double offset=0) const;
    bool overlapsWith(const AbstractPoly &poly, double offset=0) const;
    bool partialWithin(const AbstractPoly &poly, double offset=0) const;
    bool crosses(const Position2D &p1, const Position2D &p2) const;

    friend std::ostream &operator<<(std::ostream &os, const Boundery &b);
private:
    double _xmin, _xmax, _ymin, _ymax;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "Boundery.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

