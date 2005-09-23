#ifndef Boundary_h
#define Boundary_h
//---------------------------------------------------------------------------//
//                        Boundary.h -
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
// $Log$
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
// Revision 1.1  2004/10/22 12:50:44  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.8  2004/07/02 09:44:40  dkrajzew
// changes for 0.8.0.2
//
// Revision 1.7  2004/03/19 13:01:11  dkrajzew
// methods needed for the new selection within the gui added; some style adaptions
//
// Revision 1.6  2003/11/11 08:00:31  dkrajzew
// consequent usage of Position2D instead of two SUMOReals
//
// Revision 1.5  2003/05/20 09:50:20  dkrajzew
// further work and debugging
//
// Revision 1.4  2003/03/20 16:41:09  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2002/06/11 15:58:25  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/02/07 10:50:20  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>
#include "AbstractPoly.h"
#include "Position2D.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class Boundary
        : public AbstractPoly {
public:
    Boundary();
    Boundary(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2);
    ~Boundary();
    void add(SUMOReal x, SUMOReal y);
    void add(const Position2D &p);
    void add(const Boundary &p);
    Position2D getCenter() const;
    SUMOReal xmin() const;
    SUMOReal xmax() const;
    SUMOReal ymin() const;
    SUMOReal ymax() const;
    SUMOReal getWidth() const;
    SUMOReal getHeight() const;
    bool around(const Position2D &p, SUMOReal offset=0) const;
    bool overlapsWith(const AbstractPoly &poly, SUMOReal offset=0) const;
    bool partialWithin(const AbstractPoly &poly, SUMOReal offset=0) const;
    bool crosses(const Position2D &p1, const Position2D &p2) const;
    /** brief extends the boundary by the given amount
        The method returns a reference to the instance for further use */
    Boundary &grow(SUMOReal by);
    void flipY();
    void set(SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax);

    friend std::ostream &operator<<(std::ostream &os, const Boundary &b);
private:
    SUMOReal _xmin, _xmax, _ymin, _ymax;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

