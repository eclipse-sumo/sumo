/****************************************************************************/
/// @file    AbstractPoly.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The base class for polygons
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include "Position.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class AbstractPoly {
public:
    AbstractPoly() { }
    AbstractPoly(const AbstractPoly&) { }
    virtual ~AbstractPoly() { }
    virtual bool around(const Position& p, SUMOReal offset = 0) const = 0;
    virtual bool overlapsWith(const AbstractPoly& poly, SUMOReal offset = 0) const = 0;
    virtual bool partialWithin(const AbstractPoly& poly, SUMOReal offset = 0) const = 0;
    virtual bool crosses(const Position& p1,
                         const Position& p2) const = 0;
};


#endif

/****************************************************************************/

