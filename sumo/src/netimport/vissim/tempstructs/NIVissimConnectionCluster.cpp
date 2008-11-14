/****************************************************************************/
/// @file    NIVissimConnectionCluster.cpp
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

#include <algorithm>
#include <iostream>
#include <cassert>
#include <utils/geom/Boundary.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/VectorHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimNodeCluster.h"
#include "NIVissimNodeDef.h"
#include "NIVissimEdge.h"
#include "NIVissimTL.h"
#include "NIVissimConnectionCluster.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static members
// ===========================================================================
NIVissimConnectionCluster::ContType NIVissimConnectionCluster::myClusters;
int NIVissimConnectionCluster::myFirstFreeID = 100000;
int NIVissimConnectionCluster::myStaticBlaID = 0;



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NIVissimConnectionCluster::NodeSubCluster - methods
// ---------------------------------------------------------------------------
NIVissimConnectionCluster::NodeSubCluster::NodeSubCluster(NIVissimConnection *c)
{
    add(c);
}


NIVissimConnectionCluster::NodeSubCluster::~NodeSubCluster()
{}


void
NIVissimConnectionCluster::NodeSubCluster::add(NIVissimConnection *c)
{
    myBoundary.add(c->getBoundingBox());
    myConnections.push_back(c);
}


void
NIVissimConnectionCluster::NodeSubCluster::add(const NIVissimConnectionCluster::NodeSubCluster &c)
{
    for (ConnectionCont::const_iterator i=c.myConnections.begin(); i!=c.myConnections.end(); i++) {
        add(*i);
    }
}


size_t
NIVissimConnectionCluster::NodeSubCluster::size() const
{
    return myConnections.size();
}


IntVector
NIVissimConnectionCluster::NodeSubCluster::getConnectionIDs() const
{
    IntVector ret;
    int id = NIVissimConnectionCluster::getNextFreeNodeID();
    for (ConnectionCont::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        ret.push_back((*i)->getID());
        (*i)->setNodeCluster(id);
    }
    return ret;
}


bool
NIVissimConnectionCluster::NodeSubCluster::overlapsWith(
    const NIVissimConnectionCluster::NodeSubCluster &c,
    SUMOReal offset)
{
    assert(myBoundary.xmax()>=myBoundary.xmin());
    assert(c.myBoundary.xmax()>=c.myBoundary.xmin());
    return myBoundary.overlapsWith(c.myBoundary, offset);
}



// ---------------------------------------------------------------------------
// NIVissimConnectionCluster - methods
// ---------------------------------------------------------------------------
NIVissimConnectionCluster::NIVissimConnectionCluster(
    const IntVector &connections, int nodeCluster, int edgeid)
        : myConnections(connections), myNodeCluster(nodeCluster),
        myBlaID(myStaticBlaID++)
{
    recomputeBoundary();
    myClusters.push_back(this);
    assert(edgeid>0);
    if (edgeid>=0) {
        myEdges.push_back(edgeid);
    }
    // add information about incoming and outgoing edges
    for (IntVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        assert(c!=0);
        myOutgoingEdges.push_back(c->getToEdgeID());
        myIncomingEdges.push_back(c->getFromEdgeID());
        assert(c->getFromEdgeID()==edgeid||c->getToEdgeID()==edgeid);
    }
    VectorHelper<int>::removeDouble(myIncomingEdges);
    VectorHelper<int>::removeDouble(myOutgoingEdges);
}


NIVissimConnectionCluster::NIVissimConnectionCluster(
    const IntVector &connections, const Boundary &boundary,
    int nodeCluster, const IntVector &edges)
        : myConnections(connections), myBoundary(boundary),
        myNodeCluster(nodeCluster), myEdges(edges)
{
    myClusters.push_back(this);
    recomputeBoundary();
    assert(myBoundary.xmax()>=myBoundary.xmin());
    // add information about incoming and outgoing edges
    for (IntVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        assert(c!=0);
        myOutgoingEdges.push_back(c->getToEdgeID());
        myIncomingEdges.push_back(c->getFromEdgeID());
        assert(find(edges.begin(), edges.end(), c->getFromEdgeID())!=edges.end()
               ||
               find(edges.begin(), edges.end(), c->getToEdgeID())!=edges.end());
    }
    VectorHelper<int>::removeDouble(myIncomingEdges);
    VectorHelper<int>::removeDouble(myOutgoingEdges);
}


NIVissimConnectionCluster::~NIVissimConnectionCluster()
{}



int
NIVissimConnectionCluster::getNextFreeNodeID()
{
    return myFirstFreeID++;
}


bool
NIVissimConnectionCluster::overlapsWith(NIVissimConnectionCluster *c,
                                        SUMOReal offset) const
{
    assert(myBoundary.xmax()>=myBoundary.xmin());
    assert(c->myBoundary.xmax()>=c->myBoundary.xmin());
    return c->myBoundary.overlapsWith(myBoundary, offset);
}


void
NIVissimConnectionCluster::add(NIVissimConnectionCluster *c)
{
    assert(myBoundary.xmax()>=myBoundary.xmin());
    assert(c->myBoundary.xmax()>=c->myBoundary.xmin());
    myBoundary.add(c->myBoundary);
    for (IntVector::iterator i=c->myConnections.begin(); i!=c->myConnections.end(); i++) {
        myConnections.push_back(*i);
    }
    VectorHelper<int>::removeDouble(myConnections);
    assert(myNodeCluster==-1||c->myNodeCluster==-1);
    if (myNodeCluster==-1) {
        myNodeCluster = c->myNodeCluster;
    }
    // inform edges about merging
    //  !!! merge should be done within one method
    for (IntVector::iterator j=c->myEdges.begin(); j!=c->myEdges.end(); j++) {
        NIVissimEdge::dictionary(*j)->mergedInto(c, this);
    }
    copy(c->myEdges.begin(), c->myEdges.end(), back_inserter(myEdges));
    copy(c->myIncomingEdges.begin(), c->myIncomingEdges.end(),
         back_inserter(myIncomingEdges));
    copy(c->myOutgoingEdges.begin(), c->myOutgoingEdges.end(),
         back_inserter(myOutgoingEdges));
    VectorHelper<int>::removeDouble(myEdges);
    VectorHelper<int>::removeDouble(myIncomingEdges);
    VectorHelper<int>::removeDouble(myOutgoingEdges);
}



void
NIVissimConnectionCluster::joinBySameEdges(SUMOReal offset)
{
    // !!! ...
    // Further, we try to omit joining of overlaping nodes. This is done by holding
    //  the lists of incoming and outgoing edges and incrementally building the nodes
    //  regarding this information
    std::vector<NIVissimConnectionCluster*> joinAble;
    size_t pos = 0;
    ContType::iterator i = myClusters.begin();
    // step1 - faster but no complete
    while (i!=myClusters.end()) {
        joinAble.clear();
        ContType::iterator j = i + 1;

        // check whether every combination has been processed
        while (j!=myClusters.end()) {
            // check whether the current clusters overlap
            if ((*i)->joinable(*j, offset)) {
                joinAble.push_back(*j);
            }
            j++;
        }
        for (std::vector<NIVissimConnectionCluster*>::iterator k=joinAble.begin();
                k!=joinAble.end(); k++) {
            // add the overlaping cluster
            (*i)->add(*k);
            // erase the overlaping cluster
            delete *k;
            myClusters.erase(find(myClusters.begin(), myClusters.end(), *k));
        }
        //
        if (joinAble.size()>0) {
            i = myClusters.begin() + pos;
            // clear temporary storages
            joinAble.clear();
        } else {
            i++;
            MsgHandler::getMessageInstance()->progressMsg("Checked(1): " + toString(pos) + "/" + toString(myClusters.size()) + "         ");
            pos++;
        }
    }
    //
    pos = 0;
    i = myClusters.begin();
    while (i!=myClusters.end()) {
        ContType::iterator j = i + 1;
        // check whether every combination has been processed
        while (j!=myClusters.end()) {
            // check whether the current clusters overlap
            if ((*i)->joinable(*j, offset)) {
                joinAble.push_back(*j);
            }
            j++;
        }
        for (std::vector<NIVissimConnectionCluster*>::iterator k=joinAble.begin();
                k!=joinAble.end(); k++) {
            // add the overlaping cluster
            (*i)->add(*k);
            // erase the overlaping cluster
            delete *k;
            myClusters.erase(find(myClusters.begin(), myClusters.end(), *k));
        }
        //
        if (joinAble.size()>0) {
            i = myClusters.begin();
            // clear temporary storages
            joinAble.clear();
            pos = 0;
        } else {
            i++;
            pos++;
            MsgHandler::getMessageInstance()->progressMsg("Checked(2): " + toString(pos) + "/" + toString(myClusters.size()) + "         ");
        }
    }
    // check for weak district connections
    //  (junctions made up by district connections, where prohibitions are not
    //   modelled properly)
    pos = 0;
    i = myClusters.begin();
    while (i!=myClusters.end()) {
        ContType::iterator j = i + 1;
        // check whether every combination has been processed
        while (j!=myClusters.end()) {
            // check whether the current clusters overlap
            if ((*i)->isWeakDistrictConnRealisation(*j)) {
                joinAble.push_back(*j);
            }
            j++;
        }
        for (std::vector<NIVissimConnectionCluster*>::iterator k=joinAble.begin();
                k!=joinAble.end(); k++) {
            // add the overlaping cluster
            (*i)->add(*k);
            // erase the overlaping cluster
            delete *k;
            myClusters.erase(find(myClusters.begin(), myClusters.end(), *k));
        }
        //
        if (joinAble.size()>0) {
            i = myClusters.begin();
            // clear temporary storages
            joinAble.clear();
            pos = 0;
        } else {
            i++;
            pos++;
            MsgHandler::getMessageInstance()->progressMsg("Checked(3): " + toString(pos) + "/" + toString(myClusters.size()) + "         ");
        }
    }
}


bool
NIVissimConnectionCluster::joinable(NIVissimConnectionCluster *c2, SUMOReal offset)
{
    // join clusters which have at least one connection in common
    if (VectorHelper<int>::subSetExists(myConnections, c2->myConnections)) {
        return true;
    }

    // connections shall overlap otherwise
    if (!overlapsWith(c2, offset)) {
        return false;
    }

    // at least one of the clusters shall not be assigned to a node in previous (!!!??)
    if (hasNodeCluster() && c2->hasNodeCluster()) {
        return false;
    }

    // join clusters which where connections do disturb each other
    if (VectorHelper<int>::subSetExists(c2->getDisturbanceParticipators(), myConnections)
            ||
            VectorHelper<int>::subSetExists(getDisturbanceParticipators(), c2->myConnections)) {

        return true;
    }


    // join clusters which do share the same incoming or outgoing edges (not mutually)
    IntVector extendedOutgoing1;
    IntVector extendedIncoming1;
    IntVector extendedOutgoing2;
    IntVector extendedIncoming2;
    if (myIncomingEdges.size()>1||c2->myIncomingEdges.size()>1) {
        extendedOutgoing1 =
            extendByToTreatAsSame(myOutgoingEdges, myIncomingEdges);
        extendedIncoming1 =
            extendByToTreatAsSame(myIncomingEdges, myOutgoingEdges);
        extendedOutgoing2 =
            extendByToTreatAsSame(c2->myOutgoingEdges, c2->myIncomingEdges);
        extendedIncoming2 =
            extendByToTreatAsSame(c2->myIncomingEdges, c2->myOutgoingEdges);
    } else {
        extendedOutgoing1 = myIncomingEdges;
        extendedIncoming1 = myOutgoingEdges;
        extendedOutgoing2 = c2->myIncomingEdges;
        extendedIncoming2 = c2->myOutgoingEdges;
    }

    if (VectorHelper<int>::subSetExists(extendedOutgoing1, extendedOutgoing2)
            ||
            VectorHelper<int>::subSetExists(extendedIncoming1, extendedIncoming2)
       ) {
        return true;
    }
    return false;
}


bool
NIVissimConnectionCluster::isWeakDistrictConnRealisation(NIVissimConnectionCluster *c2)
{
    if ((myIncomingEdges.size()==1&&myOutgoingEdges.size()==1)) {
        return false;
    }
    if ((c2->myIncomingEdges.size()==1&&c2->myOutgoingEdges.size()==1)) {
        return false;
    }

    // ok, may be the other way round
    if (myIncomingEdges.size()==1&&c2->myOutgoingEdges.size()==1) {
        return c2->isWeakDistrictConnRealisation(this);
    }
    // connections must cross
    bool crosses = false;
    for (IntVector::const_iterator j1=myConnections.begin(); j1!=myConnections.end()&&!crosses; j1++) {
        NIVissimConnection *c1 = NIVissimConnection::dictionary(*j1);
        const Position2DVector &g1 = c1->getGeometry();
        for (IntVector::const_iterator j2=c2->myConnections.begin(); j2!=c2->myConnections.end()&&!crosses; j2++) {
            NIVissimConnection *c2 = NIVissimConnection::dictionary(*j2);
            const Position2DVector &g2 = c2->getGeometry();
            if (g1.intersects(g2)) {
                crosses = true;
            }
        }
    }
    if (!crosses) {
        return false;
    }
    // ok, check for connection
    if (myOutgoingEdges.size()!=1||c2->myIncomingEdges.size()!=1) {
        return false;
    }
    // check whether the connection is bidirectional
    NIVissimEdge *oe = NIVissimEdge::dictionary(myOutgoingEdges[0]);
    NIVissimEdge *ie = NIVissimEdge::dictionary(c2->myIncomingEdges[0]);
    if (oe==0||ie==0) {
        return false;
    }
    Line2D l1(oe->getGeometry().getBegin(), oe->getGeometry().getEnd());
    Line2D l2(ie->getGeometry().getEnd(), ie->getGeometry().getBegin());
    SUMOReal a1 = l1.atan2DegreeAngle();
    SUMOReal a2 = l2.atan2DegreeAngle();
    return fabs(a1-a2)<5;
}


bool
NIVissimConnectionCluster::liesOnSameEdgesEnd(NIVissimConnectionCluster *cc2)
{
    //
    for (IntVector::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c1 = NIVissimConnection::dictionary(*i);
        for (IntVector::iterator j=cc2->myConnections.begin(); j!=cc2->myConnections.end(); j++) {
            NIVissimConnection *c2 = NIVissimConnection::dictionary(*j);
            if (c1->getFromEdgeID()==c2->getFromEdgeID()) {
                NIVissimEdge *e = NIVissimEdge::dictionary(c1->getFromEdgeID());
                const Position2DVector &g = e->getGeometry();
                SUMOReal pos1 = GeomHelper::nearest_position_on_line_to_point(
                                    g.getBegin(), g.getEnd(), c1->getBoundary().getCenter());
                SUMOReal pos2 = GeomHelper::nearest_position_on_line_to_point(
                                    g.getBegin(), g.getEnd(), c2->getBoundary().getCenter());
                if (pos1<=5.0&&pos2<=5.0) {
                    return true;
                }
            }
            if (c1->getToEdgeID()==c2->getToEdgeID()) {
                NIVissimEdge *e = NIVissimEdge::dictionary(c1->getFromEdgeID());
                const Position2DVector &g = e->getGeometry();
                SUMOReal pos1 = GeomHelper::nearest_position_on_line_to_point(
                                    g.getBegin(), g.getEnd(), c1->getBoundary().getCenter());
                SUMOReal pos2 = GeomHelper::nearest_position_on_line_to_point(
                                    g.getBegin(), g.getEnd(), c2->getBoundary().getCenter());
                if (pos1>=g.length()-5.0&&pos2>=g.length()-5.0) {
                    return true;
                }
            }
        }
    }
    return false;
}


IntVector
NIVissimConnectionCluster::extendByToTreatAsSame(const IntVector &iv1,
        const IntVector &iv2) const
{
    IntVector ret(iv1);
    for (IntVector::const_iterator i=iv1.begin(); i!=iv1.end(); i++) {
        NIVissimEdge *e = NIVissimEdge::dictionary(*i);
        const std::vector<NIVissimEdge*> treatAsSame = e->getToTreatAsSame();
        for (std::vector<NIVissimEdge*>::const_iterator j=treatAsSame.begin(); j!=treatAsSame.end(); j++) {
            if (find(iv2.begin(), iv2.end(), (*j)->getID())==iv2.end()) {
                ret.push_back((*j)->getID());
            }
        }
    }
    return ret;
}

IntVector
NIVissimConnectionCluster::getDisturbanceParticipators()
{
    IntVector ret;
    for (IntVector::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        const IntVector &disturbances = c->getDisturbances();
        for (IntVector::const_iterator j=disturbances.begin(); j!=disturbances.end(); j++) {
            NIVissimDisturbance *d = NIVissimDisturbance::dictionary(*j);
            ret.push_back(d->getEdgeID());
            ret.push_back(d->getDisturbanceID());
        }
    }
    return ret;
}


void
NIVissimConnectionCluster::buildNodeClusters()
{
    for (ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        IntVector disturbances;
        IntVector tls;
        IntVector nodes;
        int tlsid = -1;
        int nodeid = -1;
        if ((*i)->myConnections.size()>0) {
            (*i)->recomputeBoundary();
            disturbances = NIVissimDisturbance::getWithin((*i)->myBoundary);
        }
        nodes = (*i)->myNodes;//NIVissimTL::getWithin((*i)->myBoundary, 5.0);
        if (nodes.size()>1) {
            WRITE_WARNING("NIVissimConnectionCluster:More than a single node");
            //          throw 1; // !!! eigentlich sollte hier nur eine Ampelanlage sein
        }
        if (nodes.size()>0) {
            nodeid = nodes[0];
        }
        //
        //
        int id = NIVissimNodeCluster::dictionary(
                     nodeid, tlsid, (*i)->myConnections,
                     disturbances, (*i)->myIncomingEdges.size()<2);
        assert((*i)->myNodeCluster==id||(*i)->myNodeCluster<0);
        (*i)->myNodeCluster = id;
    }
}


void
NIVissimConnectionCluster::searchForConnection(int id)
{
    int pos = 0;
    for (ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        IntVector connections = (*i)->myConnections;
        if (find(connections.begin(), connections.end(), id)!=connections.end()) {
            for (IntVector::iterator j=connections.begin(); j!=connections.end(); j++) {
                int checkdummy = *j;
            }
        }
        pos++;
    }
}


void
NIVissimConnectionCluster::_debugOut(std::ostream &into)
{
    for (ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        IntVector connections = (*i)->myConnections;
        for (IntVector::iterator j=connections.begin(); j!=connections.end(); j++) {
            if (j!=connections.begin()) {
                into << ", ";
            }
            into << *j;
        }
        into << "(" << (*i)->myBoundary << ")" << endl;
    }
    into << "---------------------------" << endl;
}



bool
NIVissimConnectionCluster::hasNodeCluster() const
{
    return myNodeCluster != -1;
}


size_t
NIVissimConnectionCluster::dictSize()
{
    return myClusters.size();
}


void
NIVissimConnectionCluster::removeConnections(const NodeSubCluster &c)
{
    for (NodeSubCluster::ConnectionCont::const_iterator i=c.myConnections.begin(); i!=c.myConnections.end(); i++) {
        NIVissimConnection *conn = *i;
        int connid = conn->getID();
        IntVector::iterator j = find(myConnections.begin(), myConnections.end(), connid);
        if (j!=myConnections.end()) {
            myConnections.erase(j);
        }
    }
    recomputeBoundary();
}


void
NIVissimConnectionCluster::recomputeBoundary()
{
    myBoundary = Boundary();
    for (IntVector::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if (c!=0) {
            myBoundary.add(c->getFromGeomPosition());
            myBoundary.add(c->getToGeomPosition());
            if (c->getGeometry().size()!=0) {
                myBoundary.add(c->getGeometry().getBoxBoundary());
            }
        }
    }
    assert(myBoundary.xmax()>=myBoundary.xmin());
}


NBNode *
NIVissimConnectionCluster::getNBNode() const
{
    return NIVissimNodeCluster::dictionary(myNodeCluster)->getNBNode();
}


bool
NIVissimConnectionCluster::around(const Position2D &p, SUMOReal offset) const
{
    assert(myBoundary.xmax()>=myBoundary.xmin());
    return myBoundary.around(p, offset);
}



void
NIVissimConnectionCluster::recheckEdges()
{
    assert(myConnections.size()!=0);
    // remove the cluster from all edges at first
    IntVector::iterator i;
    for (i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*i);
        edge->removeFromConnectionCluster(this);
    }
    // clear edge information
    myEdges.clear();
    // recheck which edges do still participate and add edges
    for (i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        assert(myBoundary.xmax()>=myBoundary.xmin());
        if (myBoundary.around(c->getFromGeomPosition(), 5)) {
            myEdges.push_back(c->getFromEdgeID());
        }
        assert(myBoundary.xmax()>=myBoundary.xmin());
        if (myBoundary.around(c->getToGeomPosition(), 5)) {
            myEdges.push_back(c->getToEdgeID());
        }
    }
    // connect edges
    for (i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*i);
        edge->addToConnectionCluster(this);
    }
}


SUMOReal
NIVissimConnectionCluster::getPositionForEdge(int edgeid) const
{
    // return the middle of the connections when there are any
    if (myConnections.size()!=0) {
        SUMOReal sum = 0;
        size_t part = 0;
        IntVector::const_iterator i;
        for (i=myConnections.begin(); i!=myConnections.end(); i++) {
            NIVissimConnection *c = NIVissimConnection::dictionary(*i);
            if (c->getFromEdgeID()==edgeid) {
                part++;
                sum += c->getFromPosition();
            }
            if (c->getToEdgeID()==edgeid) {
                part++;
                sum += c->getToPosition();
            }
        }
        if (part>0) {
            return sum / (SUMOReal) part;
        }
    }
    // use the position of the node if possible
    if (myNodeCluster>=0) {
        // try to find the nearest point on the edge
        //  !!! only the main geometry is regarded
        NIVissimNodeDef *node =
            NIVissimNodeDef::dictionary(myNodeCluster);
        if (node!=0) {
            SUMOReal pos = node->getEdgePosition(edgeid);
            if (pos>=0) {
                return pos;
            }
        }
        /*
                SUMOReal try1 = GeomHelper::nearest_position_on_line_to_point(
                    edge->getBegin2D(), edge->getEnd2D(), node->getPos());
                if(try1>=0) {
                    return try1;
                }
                // try to use simple distance
                SUMOReal dist1 =
                    GeomHelper::distance(node->getPos(), edge->getBegin2D());
                SUMOReal dist2 =
                    GeomHelper::distance(node->getPos(), edge->getEnd2D());
                return dist1<dist2
                    ? 0 : edge->getLength();
                    */
    }
    // what else?
    WRITE_WARNING("NIVissimConnectionCluster: how to get an edge's position?");
    // !!!
    assert(myBoundary.xmin()<=myBoundary.xmax());
    NIVissimEdge *edge = NIVissimEdge::dictionary(edgeid);
    IntVector::const_iterator i = find(myEdges.begin(), myEdges.end(), edgeid);
    if (i==myEdges.end()) {
        // edge does not exist!?
        throw 1;
    }
    const Position2DVector &edgeGeom = edge->getGeometry();
    Position2D p = GeomHelper::crossPoint(myBoundary, edgeGeom);
    return GeomHelper::nearest_position_on_line_to_point(
               edgeGeom.getBegin(), edgeGeom.getEnd(), p);
}



void
NIVissimConnectionCluster::clearDict()
{
    for (ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        delete(*i);
    }
    myClusters.clear();
    myFirstFreeID = 100000;
}


Position2DVector
NIVissimConnectionCluster::getIncomingContinuationGeometry(NIVissimEdge *e) const
{
    // collect connection where this edge is the incoming one
    std::vector<NIVissimConnection*> edgeIsIncoming;
    for (IntVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if (c->getFromEdgeID()==e->getID()) {
            edgeIsIncoming.push_back(c);
        }
    }
    //
    if (edgeIsIncoming.size()==0) {
        return Position2DVector();
    }
    // sort connected edges in same direction
    sort(edgeIsIncoming.begin(), edgeIsIncoming.end(),
         same_direction_sorter(e->getGeometry().beginEndAngle()));
    NIVissimConnection *c = *(edgeIsIncoming.begin());
    return c->getGeometry();
}



NIVissimConnection *
NIVissimConnectionCluster::getIncomingContinuation(NIVissimEdge *e) const
{
    // collect connection where this edge is the incoming one
    std::vector<NIVissimConnection*> edgeIsIncoming;
    for (IntVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if (c->getFromEdgeID()==e->getID()) {
            edgeIsIncoming.push_back(c);
        }
    }
    //
    if (edgeIsIncoming.size()==0) {
        return 0;
    }
    // sort connected edges in same direction
    sort(edgeIsIncoming.begin(), edgeIsIncoming.end(),
         same_direction_sorter(e->getGeometry().beginEndAngle()));
    return *(edgeIsIncoming.begin());
}



Position2DVector
NIVissimConnectionCluster::getOutgoingContinuationGeometry(NIVissimEdge *e) const
{
    // collect connection where this edge is the outgoing one
    std::vector<NIVissimConnection*> edgeIsOutgoing;
    for (IntVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if (c->getToEdgeID()==e->getID()) {
            edgeIsOutgoing.push_back(c);
        }
    }
    //
    if (edgeIsOutgoing.size()==0) {
        return Position2DVector();
    }
    // sort connected edges in same direction
    sort(edgeIsOutgoing.begin(), edgeIsOutgoing.end(),
         same_direction_sorter(e->getGeometry().beginEndAngle()));
    NIVissimConnection *c = *(edgeIsOutgoing.begin());
    return c->getGeometry();
}


NIVissimConnection*
NIVissimConnectionCluster::getOutgoingContinuation(NIVissimEdge *e) const
{
    // collect connection where this edge is the outgoing one
    std::vector<NIVissimConnection*> edgeIsOutgoing;
    for (IntVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if (c->getToEdgeID()==e->getID()) {
            edgeIsOutgoing.push_back(c);
        }
    }
    //
    if (edgeIsOutgoing.size()==0) {
        return 0;
    }
    // sort connected edges in same direction
    sort(edgeIsOutgoing.begin(), edgeIsOutgoing.end(),
         same_direction_sorter(e->getGeometry().beginEndAngle()));
    return *(edgeIsOutgoing.begin());
}



/****************************************************************************/

