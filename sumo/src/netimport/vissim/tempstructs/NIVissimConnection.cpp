/****************************************************************************/
/// @file    NIVissimConnection.cpp
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

#include <string>
#include <map>
#include <iostream>
#include <cassert>
#include <utils/common/VectorHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NIVissimExtendedEdgePoint.h"
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBEdgeCont.h>
#include "NIVissimEdge.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimNodeDef.h"
#include "NIVissimConnection.h"
#include <utils/common/UtilExceptions.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
NIVissimConnection::DictType NIVissimConnection::myDict;
int NIVissimConnection::myMaxID;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimConnection::NIVissimConnection(int id,
                                       const std::string &name, const NIVissimExtendedEdgePoint &from_def,
                                       const NIVissimExtendedEdgePoint &to_def,
                                       const Position2DVector &geom, Direction direction,
                                       SUMOReal dxnothalt, SUMOReal dxeinordnen,
                                       SUMOReal zuschlag1, SUMOReal zuschlag2, SUMOReal /*seglength*/,
                                       const IntVector &assignedVehicles, const NIVissimClosedLanesVector &clv)
        : NIVissimAbstractEdge(id, geom),
        myName(name), myFromDef(from_def), myToDef(to_def),
        myDirection(direction),
        myDXNothalt(dxnothalt), myDXEinordnen(dxeinordnen),
        myZuschlag1(zuschlag1), myZuschlag2(zuschlag2),
        myAssignedVehicles(assignedVehicles), myClosedLanes(clv) {}


NIVissimConnection::~NIVissimConnection() {
    for (NIVissimClosedLanesVector::iterator i=myClosedLanes.begin(); i!=myClosedLanes.end(); i++) {
        delete(*i);
    }
    myClosedLanes.clear();
}


bool
NIVissimConnection::dictionary(int id, const std::string &name,
                               const NIVissimExtendedEdgePoint &from_def,
                               const NIVissimExtendedEdgePoint &to_def,
                               const Position2DVector &geom,
                               Direction direction,
                               SUMOReal dxnothalt, SUMOReal dxeinordnen,
                               SUMOReal zuschlag1, SUMOReal zuschlag2,
                               SUMOReal seglength,
                               const IntVector &assignedVehicles,
                               const NIVissimClosedLanesVector &clv) {
    NIVissimConnection *o = new NIVissimConnection(id, name, from_def, to_def,
            geom, direction, dxnothalt, dxeinordnen, zuschlag1, zuschlag2,
            seglength, assignedVehicles, clv);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    if (myMaxID<id) {
        myMaxID = id;
    }
    return true;
}



bool
NIVissimConnection::dictionary(int id, NIVissimConnection *o) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}



NIVissimConnection *
NIVissimConnection::dictionary(int id) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimConnection::buildNodeClusters() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimConnection *e = (*i).second;
        if (!e->clustered()) {
            assert(e->myBoundary!=0&&e->myBoundary->xmax()>e->myBoundary->xmin());
            IntVector connections =
                NIVissimConnection::getWithin(*(e->myBoundary));
            NIVissimNodeCluster::dictionary(-1, -1, connections,
                                            IntVector(), true); // 19.5.!!! should be on a single edge
        }
    }
}





IntVector
NIVissimConnection::getWithin(const AbstractPoly &poly) {
    IntVector ret;
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if ((*i).second->crosses(poly)) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}


void
NIVissimConnection::computeBounding() {
    Boundary *bound = new Boundary();
    bound->add(myFromDef.getGeomPosition());
    bound->add(myToDef.getGeomPosition());
    assert(myBoundary==0);
    myBoundary = bound;
}


IntVector
NIVissimConnection::getForEdge(int edgeid, bool /*omitNodeAssigned*/) {
    IntVector ret;
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        int connID = (*i).first;
        if ((*i).second->myFromDef.getEdgeID()==edgeid
                ||
                (*i).second->myToDef.getEdgeID()==edgeid) {
            if (!(*i).second->hasNodeCluster()) {
                ret.push_back(connID);
            }
        }
    }
    return ret;
}


int
NIVissimConnection::getFromEdgeID() const {
    return myFromDef.getEdgeID();
}


int
NIVissimConnection::getToEdgeID() const {
    return myToDef.getEdgeID();
}


SUMOReal
NIVissimConnection::getFromPosition() const {
    return myFromDef.getPosition();
}


SUMOReal
NIVissimConnection::getToPosition() const {
    return myToDef.getPosition();
}


Position2D
NIVissimConnection::getFromGeomPosition() const {
    return myFromDef.getGeomPosition();
}



Position2D
NIVissimConnection::getToGeomPosition() const {
    return myToDef.getGeomPosition();
}


void
NIVissimConnection::setNodeCluster(int nodeid) {
    assert(myNode==-1);
    myNode = nodeid;
}


void
NIVissimConnection::buildGeom() {
    if (myGeom.size()>0) {
        return;
    }
    myGeom.push_back(myFromDef.getGeomPosition());
    myGeom.push_back(myToDef.getGeomPosition());
}


unsigned int
NIVissimConnection::buildEdgeConnections(NBEdgeCont &ec) {
    unsigned int unsetConnections = 0;
    // try to determine the connected edges
    NBEdge *fromEdge = 0;
    NBEdge *toEdge = 0;
    NIVissimEdge *vissimFrom = NIVissimEdge::dictionary(getFromEdgeID());
    if (vissimFrom->wasWithinAJunction()) {
        // this edge was not built, try to get one that approaches it
        vissimFrom = vissimFrom->getBestIncoming();
        if (vissimFrom!=0) {
            fromEdge = ec.retrievePossiblySplitted(toString(vissimFrom->getID()), toString(getFromEdgeID()), true);
        }
    } else {
        // this edge was built, try to get the proper part
        fromEdge = ec.retrievePossiblySplitted(toString(getFromEdgeID()), toString(getToEdgeID()), true);
    }
    NIVissimEdge *vissimTo = NIVissimEdge::dictionary(getToEdgeID());
    if (vissimTo->wasWithinAJunction()) {
        vissimTo = vissimTo->getBestOutgoing();
        if (vissimTo!=0) {
            toEdge = ec.retrievePossiblySplitted(toString(vissimTo->getID()), toString(getToEdgeID()), true);
        }
    } else {
        toEdge = ec.retrievePossiblySplitted(toString(getToEdgeID()), toString(getFromEdgeID()), false);
    }

    // try to get the edges the current connection connects
    /*
    NBEdge *fromEdge = ec.retrievePossiblySplitted(toString(getFromEdgeID()), toString(getToEdgeID()), true);
    NBEdge *toEdge = ec.retrievePossiblySplitted(toString(getToEdgeID()), toString(getFromEdgeID()), false);
    */
    if (fromEdge==0||toEdge==0) {
        WRITE_WARNING("Could not build connection between '" + toString(getFromEdgeID())+ "' and '" + toString(getToEdgeID())+ "'.");
        return 1; // !!! actually not 1
    }
    recheckLanes(fromEdge, toEdge);
    const IntVector &fromLanes = getFromLanes();
    const IntVector &toLanes = getToLanes();
    if (fromLanes.size()!=toLanes.size()) {
        MsgHandler::getWarningInstance()->inform("Lane sizes differ for connection '" + toString(getID()) + "'.");
    } else {
        for (unsigned int index=0; index<fromLanes.size(); ++index) {
            if (fromEdge->getNoLanes()<=static_cast<unsigned int>(fromLanes[index])) {
                MsgHandler::getWarningInstance()->inform("Could not set connection between '" + fromEdge->getID() + "_" + toString(fromLanes[index]) + "' and '" + toEdge->getID() + "_" + toString(toLanes[index]) + "'.");
                ++unsetConnections;
            } else if (!fromEdge->addLane2LaneConnection(fromLanes[index], toEdge, toLanes[index], NBEdge::L2L_VALIDATED)) {
                MsgHandler::getWarningInstance()->inform("Could not set connection between '" + fromEdge->getID() + "_" + toString(fromLanes[index]) + "' and '" + toEdge->getID() + "_" + toString(toLanes[index]) + "'.");
                ++unsetConnections;
            }
        }
    }
    return unsetConnections;
}


void
NIVissimConnection::dict_buildNBEdgeConnections(NBEdgeCont &ec) {
    unsigned int unsetConnections = 0;
    // go through connections
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        unsetConnections += (*i).second->buildEdgeConnections(ec);
    }
    if (unsetConnections!=0) {
        WRITE_WARNING(toString<size_t>(unsetConnections) + " of " + toString<size_t>(myDict.size())+ " connections could not be assigned.");
    }
}


const IntVector &
NIVissimConnection::getFromLanes() const {
    return myFromDef.getLanes();
}


const IntVector &
NIVissimConnection::getToLanes() const {
    return myToDef.getLanes();
}


void
NIVissimConnection::recheckLanes(const NBEdge * const fromEdge, const NBEdge * const toEdge) throw() {
    myFromDef.recheckLanes(fromEdge);
    myToDef.recheckLanes(toEdge);
}


const Boundary &
NIVissimConnection::getBoundingBox() const {
    assert(myBoundary!=0&&myBoundary->xmax()>=myBoundary->xmin());
    return *myBoundary;
}


void
NIVissimConnection::dict_assignToEdges() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimConnection *c = (*i).second;
        NIVissimEdge::dictionary(c->getFromEdgeID())->addOutgoingConnection((*i).first);
        NIVissimEdge::dictionary(c->getToEdgeID())->addIncomingConnection((*i).first);
    }
}


int
NIVissimConnection::getMaxID() {
    return myMaxID;
}


/****************************************************************************/


