/****************************************************************************/
/// @file    MSJunction.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// The base class for an intersection
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class declarations
// ===========================================================================
class MSLink;


// ===========================================================================
// member method definition
// ===========================================================================
MSJunction::MSJunction(const std::string &id, const Position2D &position,
                       const Position2DVector &shape) throw()
        : myID(id), myPosition(position), myShape(shape) {}


MSJunction::~MSJunction() {}


const Position2D &
MSJunction::getPosition() const {
    return myPosition;
}


void
MSJunction::postloadInit() throw(ProcessError) {}


const std::string &
MSJunction::getID() const {
    return myID;
}


/****************************************************************************/

