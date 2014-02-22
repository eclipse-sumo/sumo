/****************************************************************************/
/// @file    NIVissimBoundedClusterObject.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <cassert>
#include <utils/geom/Boundary.h>
#include "NIVissimBoundedClusterObject.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

NIVissimBoundedClusterObject::ContType NIVissimBoundedClusterObject::myDict;

NIVissimBoundedClusterObject::NIVissimBoundedClusterObject()
    : myBoundary(0), myClusterID(-1) {
    myDict.insert(this);
}


NIVissimBoundedClusterObject::~NIVissimBoundedClusterObject() {
    delete myBoundary;
}


bool
NIVissimBoundedClusterObject::crosses(const AbstractPoly& poly,
                                      SUMOReal offset) const {
    assert(myBoundary != 0 && myBoundary->xmax() >= myBoundary->xmin());
    return myBoundary->overlapsWith(poly, offset);
}


void
NIVissimBoundedClusterObject::inCluster(int id) {
    myClusterID = id;
}


bool
NIVissimBoundedClusterObject::clustered() const {
    return myClusterID > 0;
}


void
NIVissimBoundedClusterObject::closeLoading() {
    for (ContType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        (*i)->computeBounding();
    }
}


const Boundary&
NIVissimBoundedClusterObject::getBoundary() const {
    return *myBoundary;
}



/****************************************************************************/

