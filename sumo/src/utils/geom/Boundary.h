/****************************************************************************/
/// @file    Boundary.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A class that stores the 2D geometrical boundary
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Boundary_h
#define Boundary_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utility>
#include "AbstractPoly.h"
#include "Position2D.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class Boundary
            : public AbstractPoly
{
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
    void moveby(SUMOReal x, SUMOReal y);

    friend std::ostream &operator<<(std::ostream &os, const Boundary &b);
private:
    SUMOReal _xmin, _xmax, _ymin, _ymax;
};


#endif

/****************************************************************************/

