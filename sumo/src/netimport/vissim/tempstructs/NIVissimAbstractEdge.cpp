/****************************************************************************/
/// @file    NIVissimAbstractEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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


#include <map>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/GeoConvHelper.h>
#include "NIVissimAbstractEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


NIVissimAbstractEdge::DictType NIVissimAbstractEdge::myDict;

NIVissimAbstractEdge::NIVissimAbstractEdge(int id,
        const Position2DVector &geom)
        : myID(id), myNode(-1) {
    // convert/publicate geometry
    std::deque<Position2D>::const_iterator i;
    const std::deque<Position2D> &geomC = geom.getCont();
    for (i=geomC.begin(); i!=geomC.end(); ++i) {
        Position2D p = *i;
        if (!GeoConvHelper::x2cartesian(p)) {
            MsgHandler::getWarningInstance()->inform("Unable to project coordinates for edge '" + toString(id) + "'.");
        }
        myGeom.push_back_noDoublePos(p);
    }
    //
    dictionary(id, this);
}


NIVissimAbstractEdge::~NIVissimAbstractEdge() {}


bool
NIVissimAbstractEdge::dictionary(int id, NIVissimAbstractEdge *e) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = e;
        return true;
    }
    return false;
}


NIVissimAbstractEdge *
NIVissimAbstractEdge::dictionary(int id) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}



Position2D
NIVissimAbstractEdge::getGeomPosition(SUMOReal pos) const {
    if (myGeom.length()>pos) {
        return myGeom.positionAtLengthPosition(pos);
    } else if (myGeom.length()==pos) {
        return myGeom[-1];
    } else {
        Position2DVector g(myGeom);
        SUMOReal amount = pos - myGeom.length();
        Position2D ne = GeomHelper::extrapolate_second(g[-2], g[-1], amount*2);
        g.pop_back();
        g.push_back(ne);
        return g.positionAtLengthPosition(pos);
    }
}


void
NIVissimAbstractEdge::splitAndAssignToNodes() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimAbstractEdge *e = (*i).second;
        e->splitAssigning();
    }
}

void
NIVissimAbstractEdge::splitAssigning() {}





bool
NIVissimAbstractEdge::crossesEdge(NIVissimAbstractEdge *c) const {
    return myGeom.intersects(c->myGeom);
}


Position2D
NIVissimAbstractEdge::crossesEdgeAtPoint(NIVissimAbstractEdge *c) const {
    return myGeom.intersectsAtPoint(c->myGeom);
}


SUMOReal
NIVissimAbstractEdge::crossesAtPoint(const Position2D &p1,
                                     const Position2D &p2) const {
    // !!! not needed
    Position2D p = GeomHelper::intersection_position(
                       myGeom.getBegin(), myGeom.getEnd(), p1, p2);
    return GeomHelper::nearest_position_on_line_to_point(
               myGeom.getBegin(), myGeom.getEnd(), p);
}



IntVector
NIVissimAbstractEdge::getWithin(const AbstractPoly &p, SUMOReal offset) {
    IntVector ret;
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimAbstractEdge *e = (*i).second;
        if (e->overlapsWith(p, offset)) {
            ret.push_back(e->myID);
        }
    }
    return ret;
}


bool
NIVissimAbstractEdge::overlapsWith(const AbstractPoly &p, SUMOReal offset) const {
    return myGeom.overlapsWith(p, offset);
}


bool
NIVissimAbstractEdge::hasNodeCluster() const {
    return myNode!=-1;
}


int
NIVissimAbstractEdge::getID() const {
    return myID;
}

void
NIVissimAbstractEdge::clearDict() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}


const Position2DVector &
NIVissimAbstractEdge::getGeometry() const {
    return myGeom;
}


void
NIVissimAbstractEdge::addDisturbance(int disturbance) {
    myDisturbances.push_back(disturbance);
}


const IntVector &
NIVissimAbstractEdge::getDisturbances() const {
    return myDisturbances;
}



/****************************************************************************/

