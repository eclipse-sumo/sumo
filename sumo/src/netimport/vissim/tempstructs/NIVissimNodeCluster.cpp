//---------------------------------------------------------------------------//
//                        NIVissimNodeCluster.cpp -  ccc
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
// Revision 1.12  2004/01/12 15:33:02  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.11  2003/07/07 08:28:48  dkrajzew
// adapted the importer to the new node type description; some further work
//
// Revision 1.10  2003/06/24 08:19:35  dkrajzew
// some further work on importing traffic lights
//
// Revision 1.9  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <map>
#include <algorithm>
#include <cassert>
#include <utils/common/IntVector.h>
#include <utils/convert/ToString.h>
#include <utils/geom/Position2DVector.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVissimTL.h"
#include "NIVissimDisturbance.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeCluster.h"

using namespace std;

NIVissimNodeCluster::DictType NIVissimNodeCluster::myDict;
int NIVissimNodeCluster::myCurrentID = 1;



NIVissimNodeCluster::NIVissimNodeCluster(int id, int nodeid, int tlid,
                                         const IntVector &connectors,
                                         const IntVector &disturbances,
										 bool amEdgeSplitOnly)
    : myID(id), myNodeID(nodeid), myTLID(tlid),
    myConnectors(connectors), myDisturbances(disturbances),
    myNBNode(0), myAmEdgeSplit(amEdgeSplitOnly)
{
}


NIVissimNodeCluster::~NIVissimNodeCluster()
{
}




bool
NIVissimNodeCluster::dictionary(int id, NIVissimNodeCluster *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    assert(false);
    return false;
}


int
NIVissimNodeCluster::dictionary(int nodeid, int tlid,
                                const IntVector &connectors,
                                const IntVector &disturbances,
								bool amEdgeSplitOnly)
{
    int id = nodeid;
    if(nodeid<0) {
        id = myCurrentID++;
    }
    NIVissimNodeCluster *o = new NIVissimNodeCluster(id,
        nodeid, tlid, connectors, disturbances, amEdgeSplitOnly);
    dictionary(id, o);
    return id;
}


NIVissimNodeCluster *
NIVissimNodeCluster::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}



size_t
NIVissimNodeCluster::contSize()
{
    return myDict.size();
}



std::string
NIVissimNodeCluster::getNodeName() const
{
    if(myTLID==-1) {
        return toString<int>(myID);
    } else {
        return toString<int>(myID)
            + string("LSA ")
            + toString<int>(myTLID);
    }
}


void
NIVissimNodeCluster::buildNBNode()
{
    if(myConnectors.size()==0) {
        return; // !!! Check, whether this can happen
    }

    // compute the position
    Position2DVector crossings;
	IntVector::iterator i, j;
	// check whether this is a split of an edge only
	if(myAmEdgeSplit) {
// !!! should be		assert(myTLID==-1);
        for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
            NIVissimConnection *c1 = NIVissimConnection::dictionary(*i);
            crossings.push_back(c1->getFromGeomPosition());
		}
	} else {
        // compute the places the connections cross
		for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
			NIVissimAbstractEdge *c1 = NIVissimAbstractEdge::dictionary(*i);
			c1->buildGeom();
			for(j=i+1; j!=myConnectors.end(); j++) {
	            NIVissimAbstractEdge *c2 = NIVissimAbstractEdge::dictionary(*j);
				c2->buildGeom();
	            if(c1->crossesEdge(c2)) {
		            crossings.push_back(c1->crossesEdgeAtPoint(c2));
			    }
	        }
		}
        // alternative way: compute via positions of crossings
		if(crossings.size()==0) {
	        for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
		        NIVissimConnection *c1 = NIVissimConnection::dictionary(*i);
			    crossings.push_back(c1->getFromGeomPosition());
				crossings.push_back(c1->getToGeomPosition());
			}
		}
	}
        // get the position (center)
    Position2D pos = crossings.center();
    // build the node
/*    if(myTLID!=-1) {
 !!!        NIVissimTL *tl = NIVissimTL::dictionary(myTLID);
        if(tl->getType()=="festzeit") {
            node = new NBNode(getNodeName(), pos.x(), pos.y(),
                "traffic_light");
        } else {
            node = new NBNode(getNodeName(), pos.x(), pos.y(),
                "actuated_traffic_light");
        }
    }*/
    NBNode *node = new NBNode(getNodeName(), pos.x(), pos.y(),
        NBNode::NODETYPE_PRIORITY_JUNCTION);
    if(!NBNodeCont::insert(node)) {
        delete node;
        throw 1;
    }
    myNBNode = node;
}


void
NIVissimNodeCluster::buildNBNodes()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->buildNBNode();
    }
}



void
NIVissimNodeCluster::dict_recheckEdgeChanges()
{
    return;
}


int
NIVissimNodeCluster::getFromNode(int edgeid)
{
    int ret = -1;
    bool mult = false;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimNodeCluster *c = (*i).second;
        for(IntVector::iterator j=c->myConnectors.begin(); j!=c->myConnectors.end(); j++) {
            NIVissimConnection *conn = NIVissimConnection::dictionary(*j);
            if(conn!=0&&conn->getToEdgeID()==edgeid) {
//                return (*i).first;
                if(ret!=-1&&(*i).first!=ret) {
                    mult = true;
//                     "NIVissimNodeCluster:DoubleNode:" << ret << endl;
                    throw 1; // an edge should not outgo from two different nodes
// but actually, a joined cluster may posess a connections more than once
                }
                ret = (*i).first;
            }
        }
    }
    return ret;
}


int
NIVissimNodeCluster::getToNode(int edgeid)
{
    int ret = -1;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimNodeCluster *c = (*i).second;
        for(IntVector::iterator j=c->myConnectors.begin(); j!=c->myConnectors.end(); j++) {
            NIVissimConnection *conn = NIVissimConnection::dictionary(*j);
            if(conn!=0&&conn->getFromEdgeID()==edgeid) {
//                return (*i).first;
                if(ret!=-1&&ret!=(*i).first) {
//                  << "NIVissimNodeCluster: multiple to-nodes" << endl;
                    throw 1; // an edge should not outgo from two different nodes
// but actually, a joined cluster may posess a connections more than once

                }
                ret = (*i).first;
            }
        }
    }
    return ret;
}


void
NIVissimNodeCluster::_debugOut(std::ostream &into)
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimNodeCluster *c = (*i).second;
        into << endl << c->myID << ":";
        for(IntVector::iterator j=c->myConnectors.begin(); j!=c->myConnectors.end(); j++) {
            if(j!=c->myConnectors.begin()) {
                into << ", ";
            }
            into << (*j);
        }
    }
    into << "=======================" << endl;
}



NBNode *
NIVissimNodeCluster::getNBNode() const
{
    return myNBNode;
}


Position2D
NIVissimNodeCluster::getPos() const
{
    return myPosition;
}


void
NIVissimNodeCluster::dict_addDisturbances()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        const IntVector &disturbances = (*i).second->myDisturbances;
        NBNode *node = NBNodeCont::retrieve((*i).second->getNodeName());
        for(IntVector::const_iterator j=disturbances.begin(); j!=disturbances.end(); j++) {
            NIVissimDisturbance *disturbance = NIVissimDisturbance::dictionary(*j);
            disturbance->addToNode(node);
        }
    }
    NIVissimDisturbance::reportRefused();
}


void
NIVissimNodeCluster::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


void
NIVissimNodeCluster::setCurrentVirtID(int id)
{
    myCurrentID = id;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimNodeCluster.icc"
//#endif

// Local Variables:
// mode:C++
// End:


