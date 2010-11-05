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
#include <guisim/GUINet.h>
#include <utils/gui/globjects/GUIPolygon2D.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIShapeContainer::GUIShapeContainer(GUINet &net) throw()
        : myNet(net) {}


GUIShapeContainer::~GUIShapeContainer() throw() {}


bool
GUIShapeContainer::add(int layer, Polygon2D *p) throw() {
    myLock.lock();
    bool ret = ShapeContainer::add(layer, p);
    if (ret) {
        myNet.addAdditionalGLObject(static_cast<GUIPolygon2D*>(p));
    }
    myLock.unlock();
    return ret;
}


bool 
GUIShapeContainer::addPoI(const std::string &name, int layer, const std::string &type, const RGBColor &c, 
                       const Position2D &pos) throw() {
    PointOfInterest *p = new GUIPointOfInterest(GUIGlObjectStorage::gIDStorage, layer, name, type, pos, c);
    if (!add(layer, p)) {
        delete p;
        return false;
    }
    return true;
}


bool 
GUIShapeContainer::addPolygon(const std::string &name, int layer, const std::string &type, const RGBColor &c, 
                           bool filled, const Position2DVector &shape) throw() {
    Polygon2D *p = new GUIPolygon2D(GUIGlObjectStorage::gIDStorage, layer, name, type, c, shape, filled);
    if (!add(layer, p)) {
        delete p;
        return false;
    }
    return true;
}




bool
GUIShapeContainer::add(int layer, PointOfInterest *p) throw() {
    myLock.lock();
    bool ret = ShapeContainer::add(layer, p);
    if (ret) {
        myNet.addAdditionalGLObject(static_cast<GUIPointOfInterest*>(p));
    }
    myLock.unlock();
    return ret;
}


bool
GUIShapeContainer::removePolygon(int layer, const std::string &id) throw() {
    myLock.lock();
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        myLock.unlock();
        return false;
    }
    NamedObjectCont<Polygon2D*> &c = myPolygonLayers.find(layer)->second;
    Polygon2D *p = c.get(id);
    if (p==0) {
        myLock.unlock();
        return false;
    }
    myNet.removeAdditionalGLObject(static_cast<GUIPolygon2D*>(p));
    bool ret = c.remove(id);
    myLock.unlock();
    return ret;
}


bool
GUIShapeContainer::removePOI(int layer, const std::string &id) throw() {
    myLock.lock();
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        myLock.unlock();
        return false;
    }
    NamedObjectCont<PointOfInterest*> &c = myPOILayers.find(layer)->second;
    PointOfInterest *p = c.get(id);
    if (p==0) {
        myLock.unlock();
        return false;
    }
    myNet.removeAdditionalGLObject(static_cast<GUIPointOfInterest*>(p));
    bool ret = c.remove(id);
    myLock.unlock();
    return ret;
}



/****************************************************************************/

