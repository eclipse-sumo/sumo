/****************************************************************************/
/// @file    GUIShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @date    08.10.2009
/// @version $Id$
///
// Storage for geometrical objects extended by mutexes
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

#include "GUIShapeContainer.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIShapeContainer::GUIShapeContainer() throw() {}


GUIShapeContainer::~GUIShapeContainer() throw() {}


bool
GUIShapeContainer::add(int layer, Polygon2D *p) throw() {
    myLock.lock();
    bool ret = ShapeContainer::add(layer, p);
    myLock.unlock();
    return ret;
}


bool
GUIShapeContainer::add(int layer, PointOfInterest *p) throw() {
    myLock.lock();
    bool ret = ShapeContainer::add(layer, p);
    myLock.unlock();
    return ret;
}


bool
GUIShapeContainer::removePolygon(int layer, const std::string &id) throw() {
    myLock.lock();
    bool ret = ShapeContainer::removePolygon(layer, id);
    myLock.unlock();
    return ret;
}


bool
GUIShapeContainer::removePOI(int layer, const std::string &id) throw() {
    myLock.lock();
    bool ret = ShapeContainer::removePOI(layer, id);
    myLock.unlock();
    return ret;
}



/****************************************************************************/

