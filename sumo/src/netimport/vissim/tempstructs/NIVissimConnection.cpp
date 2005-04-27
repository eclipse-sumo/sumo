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
// Revision 1.18  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.17  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.16  2004/01/12 15:32:54  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.15  2003/11/11 08:24:51  dkrajzew
// debug values removed
//
// Revision 1.14  2003/10/30 09:12:59  dkrajzew
// further work on vissim-import
//
// Revision 1.13  2003/10/15 11:51:28  dkrajzew
// further work on vissim-import
//
// Revision 1.12  2003/09/23 14:16:36  dkrajzew
// further work on vissim-import
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
#include <utils/geom/Boundary.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBEdge.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/NBEdgeCont.h>
#include "NIVissimEdge.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimNodeDef.h"
#include "NIVissimConnection.h"

using namespace std;

NIVissimConnection::DictType NIVissimConnection::myDict;
int NIVissimConnection::myMaxID;

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
    if(myMaxID<id) {
        myMaxID = id;
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
            assert(e->myBoundary!=0&&e->myBoundary->xmax()>e->myBoundary->xmin());
            IntVector connections =
                NIVissimConnection::getWithin(*(e->myBoundary));
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
    Boundary *bound = new Boundary();
    bound->add(myFromDef.getGeomPosition());
    bound->add(myToDef.getGeomPosition());
    assert(myBoundary==0);
    myBoundary = bound;
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
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
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
        if(fromEdge==0||toEdge==0) {
            // !!! das gleiche wie oben
            fromEdge = ec.retrievePossiblySplitted(
                toString<int>(c->getFromEdgeID()),
                toString<int>(c->getToEdgeID()),
                true);
            toEdge = ec.retrievePossiblySplitted(
                toString<int>(c->getToEdgeID()),
                toString<int>(c->getFromEdgeID()),
                false);
            if(fromEdge==0||toEdge==0) {
                fromEdge = ec.retrievePossiblySplitted(
                    toString<int>(c->getFromEdgeID()),
                    toString<int>(c->getToEdgeID()),
                    true);

                WRITE_WARNING(string("Could not build connection between '")+ toString<int>(c->getFromEdgeID())+ string("' and '")+ toString<int>(c->getToEdgeID())+ string("'."));
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
        WRITE_WARNING(toString<size_t>(ref) + string(" of ")+ toString<size_t>(myDict.size())+ string(" connections could not be assigned."));
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
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


