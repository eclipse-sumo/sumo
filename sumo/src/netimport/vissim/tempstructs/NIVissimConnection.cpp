/****************************************************************************/
/// @file    NIVissimConnection.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
// used namespaces
// ===========================================================================

using namespace std;

NIVissimConnection::DictType NIVissimConnection::myDict;
int NIVissimConnection::myMaxID;

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
        myAssignedVehicles(assignedVehicles), myClosedLanes(clv)
{}


NIVissimConnection::~NIVissimConnection()
{
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
                               const NIVissimClosedLanesVector &clv)
{
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
NIVissimConnection::dictionary(int id, NIVissimConnection *o)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}



NIVissimConnection *
NIVissimConnection::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimConnection::buildNodeClusters()
{
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
NIVissimConnection::getWithin(const AbstractPoly &poly)
{
    IntVector ret;
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if ((*i).second->crosses(poly)) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}


void
NIVissimConnection::computeBounding()
{
    Boundary *bound = new Boundary();
    bound->add(myFromDef.getGeomPosition());
    bound->add(myToDef.getGeomPosition());
    assert(myBoundary==0);
    myBoundary = bound;
}


IntVector
NIVissimConnection::getForEdge(int edgeid, bool /*omitNodeAssigned*/)
{
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
NIVissimConnection::getFromEdgeID() const
{
    return myFromDef.getEdgeID();
}


int
NIVissimConnection::getToEdgeID() const
{
    return myToDef.getEdgeID();
}


SUMOReal
NIVissimConnection::getFromPosition() const
{
    return myFromDef.getPosition();
}


SUMOReal
NIVissimConnection::getToPosition() const
{
    return myToDef.getPosition();
}


Position2D
NIVissimConnection::getFromGeomPosition() const
{
    return myFromDef.getGeomPosition();
}



Position2D
NIVissimConnection::getToGeomPosition() const
{
    return myToDef.getGeomPosition();
}


void
NIVissimConnection::setNodeCluster(int nodeid)
{
    assert(myNode==-1);
    myNode = nodeid;
}


void
NIVissimConnection::unsetCluster()
{
    myNode = -1;
}


void
NIVissimConnection::buildGeom()
{
    if (myGeom.size()>0) {
        return;
    }
    myGeom.push_back(myFromDef.getGeomPosition());
    myGeom.push_back(myToDef.getGeomPosition());
}

/*
void
NIVissimConnection::dict_extendEdgesGeoms()
{
    typedef std::vector<NIVissimConnection*> ConnectionVector;
    typedef std::map<NBEdge*, ConnectionVector> Edge2ConnMap;
    Edge2ConnMap myOutgoing, myIncoming;
    // build lists of connections for each outgoing/incoming edge
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimConnection *c = (*i).second;
        NBEdge *fromEdge = myEdgeCont.retrievePossiblySplitted(
            toString<int>(c->getFromEdgeID()),
            toString<int>(c->getToEdgeID()),
            true);
        NBEdge *toEdge = myEdgeCont.retrievePossiblySplitted(
            toString<int>(c->getToEdgeID()),
            toString<int>(c->getFromEdgeID()),
            false);
        myOutgoing[fromEdge].push_back(c);
        myIncoming[toEdge].push_back(c);
    }
    // add information to edges
    Edge2ConnMap::iterator j;
    for(j=myOutgoing.begin(); j!=myOutgoing.end(); j++) {
        NBEdge *e = (*j).first;
        sort((*j).second.begin(), k!=(*j).second.end(),
            connection_most_opposite_to(e));
        NIVissimConnection *c = *((*j).second.begin());
        const Position2DVector &geom = c->getGeometry();
        e->addConnectionGeometryToEnd(geom);
    }

    for(j=myOutgoing.begin(); j!=myOutgoing.end(); j++) {
        NBEdge *e = (*j).first;
        sort((*j).second.begin(), k!=(*j).second.end(),
            connection_most_opposite_from(e));
        NIVissimConnection *c = *((*j).second.begin());
        const Position2DVector &geom = c->getGeometry();
        e->addConnectionGeometryToBegin(geom);
    }
}
*/

void
NIVissimConnection::dict_buildNBEdgeConnections(NBEdgeCont &ec)
{
    size_t ref = 0;
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimConnection *c = (*i).second;
        NBEdge *fromEdge = ec.retrievePossiblySplitted(
                               toString<int>(c->getFromEdgeID()),
                               toString<int>(c->getToEdgeID()),
                               true);
        NBEdge *toEdge = ec.retrievePossiblySplitted(
                             toString<int>(c->getToEdgeID()),
                             toString<int>(c->getFromEdgeID()),
                             false);
//        assert(fromEdge!=0&&toEdge!=0);
        // check whether it is near to an already build node
        /*
                if( myEdgeCont.retrieve(toString<int>(c->getFromEdgeID()))==0
                    ||
                    myEdgeCont.retrieve(toString<int>(c->getToEdgeID()))==0 ) {
                    NBEdge *tmpToEdge = toEdge;
                    NBEdge *tmpFromEdge =
                        fromEdge != 0
                        ? fromEdge->checkCorrectNode(toEdge)
                        : 0;
                    if(tmpFromEdge==fromEdge) {
                        tmpToEdge =
                            toEdge!=0
                            ? toEdge->checkCorrectNode(fromEdge)
                            : 0;
                    }
                    fromEdge = tmpFromEdge;
                    toEdge = tmpToEdge;
                }
        */
        if (fromEdge==0||toEdge==0) {
            // !!! das gleiche wie oben
            fromEdge = ec.retrievePossiblySplitted(
                           toString<int>(c->getFromEdgeID()),
                           toString<int>(c->getToEdgeID()),
                           true);
            toEdge = ec.retrievePossiblySplitted(
                         toString<int>(c->getToEdgeID()),
                         toString<int>(c->getFromEdgeID()),
                         false);
            if (fromEdge==0||toEdge==0) {
                fromEdge = ec.retrievePossiblySplitted(
                               toString<int>(c->getFromEdgeID()),
                               toString<int>(c->getToEdgeID()),
                               true);

                WRITE_WARNING("Could not build connection between '" + toString<int>(c->getFromEdgeID())+ "' and '" + toString<int>(c->getToEdgeID())+ "'.");
                ref++;
                continue;
            }
        }
        /*
                if(fromEdge==0) {
                    // This may occure when some connections were joined
                    //  into a node and the connected is outgoing at the very
                    //  beginning of the fromNode
                    // See network "Karlsruhe3d/_Mendel.inp", edges 3 & 4
                    // We use the really incoming nodes instead
                    NBNode *origin = toEdge->getFromNode();
                    const EdgeVector &incoming = origin->getIncomingEdges();
                    for(EdgeVector::const_iterator j=incoming.begin(); j!=incoming.end(); j++) {
                        (*j)->addEdge2EdgeConnection(toEdge);
                    }
                    continue;
                }
                if(toEdge==0) {
                    // See network "Rome_GradeSeparation/Soluz_A_2D.INP", edges 3 & 4
                    // We use the really outgoing nodes instead
                    NBNode *dest = fromEdge->getToNode();
                    const EdgeVector &outgoing = dest->getOutgoingEdges();
                    for(EdgeVector::const_iterator j=outgoing.begin(); j!=outgoing.end(); j++) {
                        fromEdge->addEdge2EdgeConnection(*j);
                    }
                    continue;
                }
                */
        const IntVector &fromLanes = c->getFromLanes();
        const IntVector &toLanes = c->getToLanes();
        for (IntVector::const_iterator j=fromLanes.begin(); j!=fromLanes.end(); j++) {
            for (IntVector::const_iterator k=toLanes.begin(); k!=toLanes.end(); k++) {

                if (!fromEdge->addLane2LaneConnection((*j), toEdge, (*k), false)) {
                    MsgHandler::getWarningInstance()->inform("Could not set connection between '" + fromEdge->getID() + "_" + toString(*j) + " and " + toEdge->getID() + "_" + toString(*k) + ".");
//                    throw ProcessError("Could not set connection!!!");
                }
            }
        }
    }
    if (ref!=0) {
        WRITE_WARNING(toString<size_t>(ref) + " of " + toString<size_t>(myDict.size())+ " connections could not be assigned.");
    }
}


const IntVector &
NIVissimConnection::getFromLanes() const
{
    return myFromDef.getLanes();
}


const IntVector &
NIVissimConnection::getToLanes() const
{
    return myToDef.getLanes();
}



const Boundary &
NIVissimConnection::getBoundingBox() const
{
    assert(myBoundary!=0&&myBoundary->xmax()>=myBoundary->xmin());
    return *myBoundary;
}


void
NIVissimConnection::dict_assignToEdges()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimConnection *c = (*i).second;
        NIVissimEdge::dictionary(c->getFromEdgeID())->addOutgoingConnection((*i).first);
        NIVissimEdge::dictionary(c->getToEdgeID())->addIncomingConnection((*i).first);
    }
}


int
NIVissimConnection::getMaxID()
{
    return myMaxID;
}



/****************************************************************************/

