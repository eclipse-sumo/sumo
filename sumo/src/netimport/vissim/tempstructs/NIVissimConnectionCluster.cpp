//---------------------------------------------------------------------------//
//                        NIVissimConnectionCluster.cpp -  ccc
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
// Revision 1.15  2003/09/22 12:42:17  dkrajzew
// further work on vissim-import
//
// Revision 1.14  2003/07/07 08:28:48  dkrajzew
// adapted the importer to the new node type description; some further work
//
// Revision 1.13  2003/06/24 08:19:35  dkrajzew
// some further work on importing traffic lights
//
// Revision 1.12  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.11  2003/06/05 11:46:56  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <algorithm>
#include <iostream>
#include <cassert>
#include <utils/geom/Boundery.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/IntVector.h>
#include <utils/common/MsgHandler.h>
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimNodeCluster.h"
#include "NIVissimNodeDef.h"
#include "NIVissimEdge.h"
#include "NIVissimTL.h"
#include "NIVissimConnectionCluster.h"


using namespace std;



NIVissimConnectionCluster::NodeSubCluster::NodeSubCluster(NIVissimConnection *c)
{
    add(c);
}


NIVissimConnectionCluster::NodeSubCluster::~NodeSubCluster()
{
}


void
NIVissimConnectionCluster::NodeSubCluster::add(NIVissimConnection *c)
{
    myBoundery.add(c->getBoundingBox());
    myConnections.push_back(c);
}


void
NIVissimConnectionCluster::NodeSubCluster::add(const NIVissimConnectionCluster::NodeSubCluster &c)
{
    for(ConnectionCont::const_iterator i=c.myConnections.begin(); i!=c.myConnections.end(); i++) {
        add(*i);
    }
}


size_t
NIVissimConnectionCluster::NodeSubCluster::size() const
{
    return myConnections.size();
}


void
NIVissimConnectionCluster::NodeSubCluster::setConnectionsFree()
{
    for(ConnectionCont::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = *i;
        c->unsetCluster();
    }
}


IntVector
NIVissimConnectionCluster::NodeSubCluster::getConnectionIDs() const
{
    IntVector ret;
    int id = NIVissimConnectionCluster::getNextFreeNodeID();
    for(ConnectionCont::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        ret.push_back((*i)->getID());
        (*i)->setNodeCluster(id);
    }
    return ret;
}


bool
NIVissimConnectionCluster::NodeSubCluster::overlapsWith(
        const NIVissimConnectionCluster::NodeSubCluster &c,
        double offset)
{
    assert(myBoundery.xmax()>=myBoundery.xmin());
    assert(c.myBoundery.xmax()>=c.myBoundery.xmin());
    return myBoundery.overlapsWith(c.myBoundery, offset);
}







NIVissimConnectionCluster::ContType NIVissimConnectionCluster::myClusters;

int NIVissimConnectionCluster::myFirstFreeID = 100000;
int NIVissimConnectionCluster::myStaticBlaID = 0;

NIVissimConnectionCluster::NIVissimConnectionCluster(
        const IntVector &connections, int nodeCluster, int edgeid)
    : myConnections(connections), myNodeCluster(nodeCluster),
    myBlaID(myStaticBlaID++)
{
    recomputeBoundery();
    myClusters.push_back(this);
    assert(edgeid>0);
    if(edgeid>=0) {
        myEdges.push_back(edgeid);
    }
    // add information about incoming and outgoing edges
    for(IntVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        assert(c!=0);
        myOutgoingEdges.push_back(c->getToEdgeID());
        myIncomingEdges.push_back(c->getFromEdgeID());
        assert(c->getFromEdgeID()==edgeid||c->getToEdgeID()==edgeid);
    }
}


NIVissimConnectionCluster::NIVissimConnectionCluster(
        const IntVector &connections, const Boundery &boundery,
        int nodeCluster, const IntVector &edges)
    : myConnections(connections), myBoundery(boundery),
    myNodeCluster(nodeCluster), myEdges(edges)
{
//    myBoundery.add(c->getFromGeomPosition());
    myClusters.push_back(this);
    assert(myBoundery.xmax()>=myBoundery.xmin());
    // add information about incoming and outgoing edges
    for(IntVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        assert(c!=0);
        myOutgoingEdges.push_back(c->getToEdgeID());
        myIncomingEdges.push_back(c->getFromEdgeID());
        assert(find(edges.begin(), edges.end(), c->getFromEdgeID())!=edges.end()
            ||
            find(edges.begin(), edges.end(), c->getToEdgeID())!=edges.end());
    }
}


NIVissimConnectionCluster::~NIVissimConnectionCluster()
{
}



int
NIVissimConnectionCluster::getNextFreeNodeID()
{
    return myFirstFreeID++;
}


bool
NIVissimConnectionCluster::overlapsWith(NIVissimConnectionCluster *c,
                                        double offset) const
{
    assert(myBoundery.xmax()>=myBoundery.xmin());
    assert(c->myBoundery.xmax()>=c->myBoundery.xmin());
    return c->myBoundery.overlapsWith(myBoundery, offset);
}


void
NIVissimConnectionCluster::add(NIVissimConnectionCluster *c)
{
    assert(myBoundery.xmax()>=myBoundery.xmin());
    assert(c->myBoundery.xmax()>=c->myBoundery.xmin());
    myBoundery.add(c->myBoundery);
    for(IntVector::iterator i=c->myConnections.begin(); i!=c->myConnections.end(); i++) {
        myConnections.push_back(*i);
    }
    IntVectorHelper::removeDouble(myConnections);
    assert(myNodeCluster==-1||c->myNodeCluster==-1);
    if(myNodeCluster==-1) {
        myNodeCluster = c->myNodeCluster;
    }
    // inform edges about merging
    //  !!! merge should be done within one method
    for(IntVector::iterator j=c->myEdges.begin(); j!=c->myEdges.end(); j++) {
        NIVissimEdge::dictionary(*j)->mergedInto(c, this);
    }
    copy(c->myEdges.begin(), c->myEdges.end(), back_inserter(myEdges));
}



void
NIVissimConnectionCluster::joinBySameEdges(double offset)
{
	// !!! ...
	// Further, we try to omit joining of overlaping nodes. This is done by holding
	//  the lists of incoming and outgoing edges and incrementally building the nodes
	//  regarding this information
    std::vector<NIVissimConnectionCluster*> joinAble;
    size_t pos = 0;
	IntVector incoming, outgoing;
    ContType::iterator i = myClusters.begin();
    // step1 - faster but no complete
    while(i!=myClusters.end()) {
        joinAble.clear();
        bool restart = false;
        ContType::iterator j = i + 1;

        // check whether every combination has been processed
        while(j!=myClusters.end()) {
			// check whether the current clusters overlap
			if((*i)->joinable(*j, offset)) {
                joinAble.push_back(*j);
            }
            j++;
        }
        for(std::vector<NIVissimConnectionCluster*>::iterator k=joinAble.begin();
                k!=joinAble.end(); k++) {
            // add the overlaping cluster
            (*i)->add(*k);
            // erase the overlaping cluster
            delete *k;
            myClusters.erase(find(myClusters.begin(), myClusters.end(), *k));
        }
        //
        if(joinAble.size()>0) {
            i = myClusters.begin() + pos;
			// clear temporary storages
            incoming.clear();
            outgoing.clear();
            joinAble.clear();
        } else {
            i++;
            cout << "Checked : " << pos << "/" << myClusters.size() << "         " << (char) 13;
            pos++;
        }
    }

    i = myClusters.begin();
    while(i!=myClusters.end()) {
        bool restart = false;
        ContType::iterator j = i + 1;
        // check whether every combination has been processed
        while(j!=myClusters.end()) {
            // check whether the current clusters overlap
			if((*i)->joinable(*j, offset)) {
                joinAble.push_back(*j);
            }
            j++;
        }
        for(std::vector<NIVissimConnectionCluster*>::iterator k=joinAble.begin();
                k!=joinAble.end(); k++) {
            // add the overlaping cluster
            (*i)->add(*k);
            // erase the overlaping cluster
            delete *k;
            myClusters.erase(find(myClusters.begin(), myClusters.end(), *k));
        }
        //
        if(joinAble.size()>0) {
            i = myClusters.begin();
			// clear temporary storages
            incoming.clear();
            outgoing.clear();
            joinAble.clear();
        } else {
            i++;
            pos++;
            cout << "Checked : " << pos << "/" << myClusters.size() << "         " << (char) 13;
        }
    }
}

bool
NIVissimConnectionCluster::joinable(NIVissimConnectionCluster *c2, double offset)
{
    // join clusters which have at least one connection in common
	if(IntVectorHelper::subSetExists(myConnections, c2->myConnections)) {
		return true;
	}

    // connections shall overlap otherwise
    if(!overlapsWith(c2, offset)) {
        return false;
    }

    // at least one of the clusters shall not be assigned to a node in previous (!!!??)
    if(hasNodeCluster() && c2->hasNodeCluster() ) {
        return false;
    }

    // join clusters which where connections do disturb each other
	if( IntVectorHelper::subSetExists(getDisturbanceParticipators(), myConnections)
        ||
        IntVectorHelper::subSetExists(c2->getDisturbanceParticipators(), c2->myConnections)) {

		return true;
	}


    // join clusters which do share the same incoming or outgoing edges (not mutually)
    if( IntVectorHelper::subSetExists(myOutgoingEdges, c2->myOutgoingEdges)
			||
	     IntVectorHelper::subSetExists(myIncomingEdges, c2->myIncomingEdges)
         ) {
		return true;
	}
	return false;
}



IntVector
NIVissimConnectionCluster::getDisturbanceParticipators()
{
    IntVector ret;
    for(IntVector::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        const IntVector &disturbances = c->getDisturbances();
        for(IntVector::const_iterator j=disturbances.begin(); j!=disturbances.end(); j++) {
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
    for(ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        IntVector disturbances;
        IntVector tls;
        IntVector nodes;
        int tlsid = -1;
        int nodeid = -1;

        if((*i)->myBlaID==296||(*i)->myBlaID==297) {
            int bla = 0;
        }

        if((*i)->myConnections.size()>0) {
            (*i)->recomputeBoundery();
//            assert((*i)->myBoundery.xmax()>(*i)->myBoundery.xmin());
            disturbances = NIVissimDisturbance::getWithin((*i)->myBoundery);
            //
        }
        nodes = (*i)->myNodes;//NIVissimTL::getWithin((*i)->myBoundery, 5.0);
        if(nodes.size()>1) {
            MsgHandler::getWarningInstance()->inform(
                "NIVissimConnectionCluster:More than a single node");
  //          throw 1; // !!! eigentlich sollte hier nur eine Ampelanlage sein
        }
        if(nodes.size()>0) {
            nodeid = nodes[0];
        }
        //
        //
        int id = NIVissimNodeCluster::dictionary(
            nodeid, tlsid, (*i)->myConnections,
            disturbances, (*i)->myIncomingEdges.size()<2);
        assert((*i)->myNodeCluster==id||(*i)->myNodeCluster<0);
        (*i)->myNodeCluster = id;
/*        for(IntVector::iterator j=disturbances.begin(); j!=disturbances.end(); j++) {
            NIVissimDisturbance::dictionary(*j)->
        }*/
    }
}


void
NIVissimConnectionCluster::searchForConnection(int id)
{
    int pos = 0;
    for(ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        IntVector connections = (*i)->myConnections;
        if(find(connections.begin(), connections.end(), id)!=connections.end()) {
            for(IntVector::iterator j=connections.begin(); j!=connections.end(); j++) {
                int checkdummy = *j;
            }
        }
        pos++;
    }
}


void
NIVissimConnectionCluster::_debugOut(std::ostream &into)
{
    for(ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        IntVector connections = (*i)->myConnections;
        for(IntVector::iterator j=connections.begin(); j!=connections.end(); j++) {
            if(j!=connections.begin()) {
                into << ", ";
            }
            into << *j;
        }
        into << "(" << (*i)->myBoundery << ")" << endl;
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
NIVissimConnectionCluster::dict_recheckNodes(double offset)
{
    // This method clusters connections into clusters which belong to
    //  a single node
    // The main assumption for doing this is, that connections are within nodes
    //  only and that if some connections are near beside each other, they
    //  should belong to the same node
    size_t pos = 0;
    for(ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i=myClusters.begin()+pos) {
        NIVissimConnectionCluster *current = *i;
        // get the connections from the cluster
        const IntVector &connections = current->myConnections;
        // recluster
        std::vector<NodeSubCluster> nodeClusters;
        std::vector<NodeSubCluster>::iterator k;
        // go through the connections of the current cluster
        for(IntVector::const_iterator j=connections.begin(); j!=connections.end(); j++) {
            // check whether the current connection may be added to a node
            NIVissimConnection *c1 = NIVissimConnection::dictionary(*j);
            bool found = false;
            for(k=nodeClusters.begin(); k!=nodeClusters.end()&&!found; k++) {
                assert((*k).myBoundery.xmax()>=(*k).myBoundery.xmin());
                if(c1->getBoundingBox().overlapsWith((*k).myBoundery, offset)) {
                    (*k).add(c1);
                    found = true;
                }
            }
            // build a new "node cluster" if not
            if(!found) {
                nodeClusters.push_back(NodeSubCluster(c1));
            }
        }
        // recluster cluster
        //  Go throught the list of build node clusters and check whether
        //  some of them may be joined
        bool changed = true;
        while(changed) {
            changed = false;
            for(k=nodeClusters.begin(); k!=nodeClusters.end()&&!changed; k++) {
                for(std::vector<NodeSubCluster>::iterator l=k+1; l!=nodeClusters.end()&&!changed; l++) {
                    if((*k).overlapsWith(*l, offset)) {
                        changed = true;
                        (*k).add(*l);
                        nodeClusters.erase(l);
                    }
                }
            }
        }

        // do nothing, when all connections are near together
        if(nodeClusters.size()<=1) {
            pos++;
            continue;
        }
        // Compute which cluster is the largest
        size_t maxSize = 0;
        int idx = -1;
        int akt = 0;
        for(k=nodeClusters.begin(); k!=nodeClusters.end(); k++) {
            if((*k).size()>maxSize) {
                maxSize = (*k).size();
                idx = akt;
            }
            akt++;
        }
        // Remove the largest cluster
        nodeClusters.erase(nodeClusters.begin()+idx);
        // Set all other connections free
        for(k=nodeClusters.begin(); k!=nodeClusters.end(); k++) {
            (*k).setConnectionsFree();
            current->removeConnections(*k);
            IntVector connections = (*k).getConnectionIDs();
            NIVissimConnectionCluster *newCluster =
                new NIVissimConnectionCluster(connections,
                    -1, -1);
            newCluster->recheckEdges();
        }
        current->recomputeBoundery();
        current->recheckEdges();
        pos++;
    }
}


void
NIVissimConnectionCluster::removeConnections(const NodeSubCluster &c)
{
    for(NodeSubCluster::ConnectionCont::const_iterator i=c.myConnections.begin(); i!=c.myConnections.end(); i++) {
        NIVissimConnection *conn = *i;
        int connid = conn->getID();
        IntVector::iterator j = find(myConnections.begin(), myConnections.end(), connid);
        if(j!=myConnections.end()) {
            myConnections.erase(j);
        }
    }
}


void
NIVissimConnectionCluster::recomputeBoundery()
{
    myBoundery = Boundery();
    for(IntVector::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if(c!=0) {
            myBoundery.add(c->getFromGeomPosition());
            myBoundery.add(c->getToGeomPosition());
        }
    }
    assert(myBoundery.xmax()>=myBoundery.xmin());
}


NBNode *
NIVissimConnectionCluster::getNBNode() const
{
    return NIVissimNodeCluster::dictionary(myNodeCluster)->getNBNode();
}


bool
NIVissimConnectionCluster::around(const Position2D &p, double offset) const
{
    assert(myBoundery.xmax()>=myBoundery.xmin());
    return myBoundery.around(p, offset);
}



void
NIVissimConnectionCluster::recheckEdges()
{
    assert(myConnections.size()!=0);
    // remove the cluster from all edges at first
    IntVector::iterator i;
    for(i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*i);
        edge->removeFromConnectionCluster(this);
    }
    // clear edge information
    myEdges.clear();
    // recheck which edges do still participate and add edges
    for(i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        assert(myBoundery.xmax()>=myBoundery.xmin());
        if(myBoundery.around(c->getFromGeomPosition(), 5)) {
            myEdges.push_back(c->getFromEdgeID());
        }
        assert(myBoundery.xmax()>=myBoundery.xmin());
        if(myBoundery.around(c->getToGeomPosition(), 5)) {
            myEdges.push_back(c->getToEdgeID());
        }
    }
    // connect edges
    for(i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*i);
        edge->addToConnectionCluster(this);
    }
}


double
NIVissimConnectionCluster::getPositionForEdge(int edgeid) const
{
    // return the middle of the connections when there are any
    if(myConnections.size()!=0) {
        double sum = 0;
        size_t part = 0;
        IntVector::const_iterator i;
        for(i=myConnections.begin(); i!=myConnections.end(); i++) {
            NIVissimConnection *c = NIVissimConnection::dictionary(*i);
            if(c->getFromEdgeID()==edgeid) {
                part++;
                sum += c->getFromPosition();
            }
            if(c->getToEdgeID()==edgeid) {
                part++;
                sum += c->getToPosition();
            }
        }
        if(part>0) {
            return sum / (double) part;
        }
    }
    // use the position of the node if possible
    if(myNodeCluster>=0) {
        // try to find the nearest point on the edge
        //  !!! only the main geometry is regarded
        NIVissimEdge *edge = NIVissimEdge::dictionary(edgeid);
        NIVissimNodeDef *node =
            NIVissimNodeDef::dictionary(myNodeCluster);
        if(node!=0) {
            double pos = node->getEdgePosition(edgeid);
            if(pos>=0) {
                return pos;
            }
        }
/*
        double try1 = GeomHelper::nearest_position_on_line_to_point(
            edge->getBegin2D(), edge->getEnd2D(), node->getPos());
        if(try1>=0) {
            return try1;
        }
        // try to use simple distance
        double dist1 =
            GeomHelper::distance(node->getPos(), edge->getBegin2D());
        double dist2 =
            GeomHelper::distance(node->getPos(), edge->getEnd2D());
        return dist1<dist2
            ? 0 : edge->getLength();
            */
    }
    // what else?
    MsgHandler::getWarningInstance()->inform(
        "NIVissimConnectionCluster: how to get an edge's position?");
    // !!!
    assert(myBoundery.xmin()<=myBoundery.xmax());
    NIVissimEdge *edge = NIVissimEdge::dictionary(edgeid);
    IntVector::const_iterator i = find(myEdges.begin(), myEdges.end(), edgeid);
    if(i==myEdges.end()) {
        // edge does not exist!?
        throw 1;
    }
    const Position2DVector &edgeGeom = edge->getGeometry();
    Position2D p = GeomHelper::crossPoint(myBoundery, edgeGeom);
    return GeomHelper::nearest_position_on_line_to_point(
        edgeGeom.getBegin(), edgeGeom.getEnd(), p);
}



void
NIVissimConnectionCluster::clearDict()
{
    for(ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        delete (*i);
    }
    myClusters.clear();
    myFirstFreeID = 100000;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimConnectionCluster.icc"
//#endif

// Local Variables:
// mode:C++
// End:


