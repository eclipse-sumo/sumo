/****************************************************************************/
/// @file    AbstractPoly.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for polygons
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AbstractPoly_h
#define AbstractPoly_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Position2D.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class AbstractPoly {
public:
    AbstractPoly() { }
    AbstractPoly(const AbstractPoly &) { }
    virtual ~AbstractPoly() { }
    virtual bool around(const Position2D &p, SUMOReal offset=0) const = 0;
    virtual bool overlapsWith(const AbstractPoly &poly, SUMOReal offset=0) const = 0;
    virtual bool partialWithin(const AbstractPoly &poly, SUMOReal offset=0) const = 0;
    virtual bool crosses(const Position2D &p1,
                         const Position2D &p2) const = 0;
};


#endif

/****************************************************************************/

