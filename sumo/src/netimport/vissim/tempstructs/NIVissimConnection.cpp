//---------------------------------------------------------------------------//
//                        NIVissimConnection.cpp -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2003/09/22 12:42:17  dkrajzew
// further work on vissim-import
//
// Revision 1.10  2003/07/07 08:28:48  dkrajzew
// adapted the importer to the new node type description; some further work
//
// Revision 1.9  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.8  2003/06/05 11:46:56  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include <iostream>
#include <cassert>
#include <utils/common/IntVector.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "NIVissimExtendedEdgePoint.h"
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Boundery.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBEdgeCont.h>
#include "NIVissimEdge.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimNodeDef.h"
#include "NIVissimConnection.h"

using namespace std;

NIVissimConnection::DictType NIVissimConnection::myDict;

NIVissimConnection::NIVissimConnection(int id,
        const std::string &name, const NIVissimExtendedEdgePoint &from_def,
        const NIVissimExtendedEdgePoint &to_def,
        const Position2DVector &geom, Direction direction,
        double dxnothalt, double dxeinordnen,
        double zuschlag1, double zuschlag2, double seglength,
        const IntVector &assignedVehicles, const NIVissimClosedLanesVector &clv)
        : NIVissimAbstractEdge(id, geom),
        myName(name), myFromDef(from_def), myToDef(to_def),
        myDirection(direction),
        myDXNothalt(dxnothalt), myDXEinordnen(dxeinordnen),
        myZuschlag1(zuschlag1), myZuschlag2(zuschlag2),
        myAssignedVehicles(assignedVehicles), myClosedLanes(clv)
{
}


NIVissimConnection::~NIVissimConnection()
{
    for(NIVissimClosedLanesVector::iterator i=myClosedLanes.begin(); i!=myClosedLanes.end(); i++) {
        delete (*i);
    }
    myClosedLanes.clear();
}


bool
NIVissimConnection::dictionary(int id, const std::string &name,
                               const NIVissimExtendedEdgePoint &from_def,
                               const NIVissimExtendedEdgePoint &to_def,
                               const Position2DVector &geom,
                               Direction direction,
                               double dxnothalt, double dxeinordnen,
                               double zuschlag1, double zuschlag2,
                               double seglength,
                               const IntVector &assignedVehicles,
                               const NIVissimClosedLanesVector &clv)
{
    NIVissimConnection *o = new NIVissimConnection(id, name, from_def, to_def,
        geom, direction, dxnothalt, dxeinordnen, zuschlag1, zuschlag2,
        seglength, assignedVehicles, clv);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}



bool
NIVissimConnection::dictionary(int id, NIVissimConnection *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}



NIVissimConnection *
NIVissimConnection::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimConnection::buildNodeClusters()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimConnection *e = (*i).second;
        if(!e->clustered()) {
            assert(e->myBoundery!=0&&e->myBoundery->xmax()>e->myBoundery->xmin());
            IntVector connections =
                NIVissimConnection::getWithin(*(e->myBoundery));
            int id = NIVissimNodeCluster::dictionary(-1, -1, connections,
                IntVector(), true); // 19.5.!!! should be on a single edge
        }
    }
}





IntVector
NIVissimConnection::getWithin(const AbstractPoly &poly)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->crosses(poly)) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}


void
NIVissimConnection::computeBounding()
{
    Boundery *bound = new Boundery();
    bound->add(myFromDef.getGeomPosition());
    bound->add(myToDef.getGeomPosition());
    assert(myBoundery==0);
    myBoundery = bound;
}


IntVector
NIVissimConnection::getForEdge(int edgeid, bool omitNodeAssigned)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        int connID = (*i).first;
        if( (*i).second->myFromDef.getEdgeID()==edgeid
            ||
            (*i).second->myToDef.getEdgeID()==edgeid) {
            if(!(*i).second->hasNodeCluster()) {
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


double
NIVissimConnection::getFromPosition() const
{
    return myFromDef.getPosition();
}


double
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
    if(myGeom.size()>0) {
        return;
    }
    myGeom.push_back(myFromDef.getGeomPosition());
    myGeom.push_back(myToDef.getGeomPosition());
}


void
NIVissimConnection::dict_buildNBEdgeConnections()
{
	size_t ref = 0;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
/*        int bla = (*i).first;
        if(bla==2000284||bla==2000283) {
            int bla = 0;
        }*/
        NIVissimConnection *c = (*i).second;
        NBEdge *fromEdge = NBEdgeCont::retrievePossiblySplitted(
            toString<int>(c->getFromEdgeID()),
            toString<int>(c->getToEdgeID()),
            true);
        NBEdge *toEdge = NBEdgeCont::retrievePossiblySplitted(
            toString<int>(c->getToEdgeID()),
            toString<int>(c->getFromEdgeID()),
            false);
        // check whether it is near to an already build node
/*
        if( NBEdgeCont::retrieve(toString<int>(c->getFromEdgeID()))==0
            ||
            NBEdgeCont::retrieve(toString<int>(c->getToEdgeID()))==0 ) {
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
        if(fromEdge==0||toEdge==0) {
            fromEdge = NBEdgeCont::retrievePossiblySplitted(
                toString<int>(c->getFromEdgeID()),
                toString<int>(c->getToEdgeID()),
                true);
            toEdge = NBEdgeCont::retrievePossiblySplitted(
                toString<int>(c->getToEdgeID()),
                toString<int>(c->getFromEdgeID()),
                false);
            if(fromEdge==0||toEdge==0) {
                fromEdge = NBEdgeCont::retrievePossiblySplitted(
                    toString<int>(c->getFromEdgeID()),
                    toString<int>(c->getToEdgeID()),
                    true);
                MsgHandler::getWarningInstance()->inform(
                    string("Could not build connection between '")
                    + toString<int>(c->getFromEdgeID())
                    + string("' and '")
                    + toString<int>(c->getToEdgeID())
                    + string("'."));
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
        for(IntVector::const_iterator j=fromLanes.begin(); j!=fromLanes.end(); j++) {
            for(IntVector::const_iterator k=toLanes.begin(); k!=toLanes.end(); k++) {
                fromEdge->addLane2LaneConnection((*j), toEdge, (*k));
            }
        }
    }
	if(ref!=0) {
        MsgHandler::getWarningInstance()->inform(
            toString<size_t>(ref) + string(" of ")
            + toString<size_t>(myDict.size())
            + string(" connections could not be assigned."));
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



const Boundery &
NIVissimConnection::getBoundingBox() const
{
    assert(myBoundery!=0&&myBoundery->xmax()>=myBoundery->xmin());
    return *myBoundery;
}


void
NIVissimConnection::dict_assignToEdges()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimConnection *c = (*i).second;
        NIVissimEdge::dictionary(c->getFromEdgeID())->addOutgoingConnection((*i).first);
        NIVissimEdge::dictionary(c->getToEdgeID())->addIncomingConnection((*i).first);
    }
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimConnection.icc"
//#endif

// Local Variables:
// mode:C++
// End:


