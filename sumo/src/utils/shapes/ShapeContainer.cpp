/****************************************************************************/
/// @file    ShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Storage for geometrical objects, sorted by the layers they are in
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
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

#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <string>
#include <cmath>
#include <utils/common/NamedObjectCont.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/Polygon.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StdDefs.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ShapeContainer::ShapeContainer() {}

ShapeContainer::~ShapeContainer() {}

bool
ShapeContainer::addPolygon(const std::string& id, const std::string& type,
                           const RGBColor& color, SUMOReal layer,
                           SUMOReal angle, const std::string& imgFile,
                           const PositionVector& shape, bool fill) {
    Polygon* p = new Polygon(id, type, color, shape, fill, layer, angle, imgFile);
    if (!myPolygons.add(id, p)) {
        delete p;
        return false;
    }
    return true;
}


bool
ShapeContainer::addPOI(const std::string& id, const std::string& type,
                       const RGBColor& color, SUMOReal layer, SUMOReal angle, const std::string& imgFile,
                       const Position& pos, SUMOReal width, SUMOReal height) {
    PointOfInterest* p = new PointOfInterest(id, type, color, pos, layer, angle, imgFile, width, height);
    if (!myPOIs.add(id, p)) {
        delete p;
        return false;
    }
    return true;
}


bool
ShapeContainer::removePolygon(const std::string& id) {
    return myPolygons.remove(id);
}


bool
ShapeContainer::removePOI(const std::string& id) {
    return myPOIs.remove(id);
}



void
ShapeContainer::movePOI(const std::string& id, const Position& pos) {
    PointOfInterest* p = myPOIs.get(id);
    if (p != 0) {
        static_cast<Position*>(p)->set(pos);
    }
}


void
ShapeContainer::reshapePolygon(const std::string& id, const PositionVector& shape) {
    Polygon* p = myPolygons.get(id);
    if (p != 0) {
        p->setShape(shape);
    }
}

/****************************************************************************/

