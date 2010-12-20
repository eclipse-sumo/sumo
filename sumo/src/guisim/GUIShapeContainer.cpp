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
#include <foreign/rtree/SUMORTree.h>
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
GUIShapeContainer::GUIShapeContainer(SUMORTree &vis) throw()
        : myVis(vis) {}


GUIShapeContainer::~GUIShapeContainer() throw() {}


bool
GUIShapeContainer::addPoI(const std::string &name, int layer, const std::string &type, const RGBColor &c,
                          const Position2D &pos) throw() {
    GUIPointOfInterest *p = new GUIPointOfInterest(GUIGlObjectStorage::gIDStorage, layer, name, type, pos, c);
    myLock.lock();
    const bool ret = add(layer, p);
    if (ret) {
        myVis.addAdditionalGLObject(p);
    } else {
        delete p;
    }
    myLock.unlock();
    return ret;
}


bool
GUIShapeContainer::addPolygon(const std::string &name, int layer, const std::string &type, const RGBColor &c,
                              bool filled, const Position2DVector &shape) throw() {
    GUIPolygon2D *p = new GUIPolygon2D(GUIGlObjectStorage::gIDStorage, layer, name, type, c, shape, filled);
    myLock.lock();
    const bool ret = add(layer, p);
    if (ret) {
        myVis.addAdditionalGLObject(p);
    } else {
        delete p;
    }
    myLock.unlock();
    return ret;
}



bool
GUIShapeContainer::removePoI(int layer, const std::string &id) throw() {
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
    myVis.removeAdditionalGLObject(static_cast<GUIPointOfInterest*>(p));
    bool ret = c.remove(id);
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
    myVis.removeAdditionalGLObject(static_cast<GUIPolygon2D*>(p));
    bool ret = c.remove(id);
    myLock.unlock();
    return ret;
}



void
GUIShapeContainer::movePoI(int layer, const std::string &id, const Position2D &pos) throw() {
    myLock.lock();
    if (myPOILayers.find(layer)!=myPOILayers.end()) {
        PointOfInterest *p = myPOILayers.find(layer)->second.get(id);
        if (p!=0) {
            myVis.removeAdditionalGLObject(static_cast<GUIPointOfInterest*>(p));
            static_cast<Position2D*>(p)->set(pos);
            myVis.addAdditionalGLObject(static_cast<GUIPointOfInterest*>(p));
        }
    }
    myLock.unlock();
}


void
GUIShapeContainer::reshapePolygon(int layer, const std::string &id, const Position2DVector &shape) throw() {
    myLock.lock();
    if (myPolygonLayers.find(layer)!=myPolygonLayers.end()) {
        Polygon2D *p = myPolygonLayers.find(layer)->second.get(id);
        if (p!=0) {
            myVis.removeAdditionalGLObject(static_cast<GUIPolygon2D*>(p));
            p->setShape(shape);
            myVis.addAdditionalGLObject(static_cast<GUIPolygon2D*>(p));
        }
    }
    myLock.unlock();
}


/****************************************************************************/

