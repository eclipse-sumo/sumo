//---------------------------------------------------------------------------//
//                        NIVissimEdge.cpp -
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
// Revision 1.24  2004/02/10 07:15:25  dkrajzew
// removed some debug-variables
//
// Revision 1.23  2004/01/28 12:39:23  dkrajzew
// work on reading and setting speeds in vissim-networks
//
// Revision 1.22  2004/01/12 15:33:02  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.21  2003/11/17 07:27:16  dkrajzew
// false heuristics removed
//
// Revision 1.20  2003/11/11 08:24:52  dkrajzew
// debug values removed
//
// Revision 1.19  2003/10/30 09:12:59  dkrajzew
// further work on vissim-import
//
// Revision 1.18  2003/10/28 07:24:47  dkrajzew
// false sorting of used connection clusters patched
//
// Revision 1.17  2003/10/27 10:51:55  dkrajzew
// edges speed setting implemented (only on an edges begin)
//
// Revision 1.16  2003/10/15 11:51:28  dkrajzew
// further work on vissim-import
//
// Revision 1.15  2003/09/23 14:16:37  dkrajzew
// further work on vissim-import
//
// Revision 1.14  2003/09/22 12:42:18  dkrajzew
// further work on vissim-import
//
// Revision 1.13  2003/07/07 08:28:48  dkrajzew
// adapted the importer to the new node type description; some further work
//
// Revision 1.12  2003/06/05 11:46:56  dkrajzew
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
#include <algorithm>
#include <map>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <utils/convert/ToString.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/GeomHelper.h>
#include <utils/distribution/Distribution.h>
#include <netbuild/NBDistribution.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include "NIVissimNodeCluster.h"
#include "NIVissimDistrictConnection.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimEdge.h"


using namespace std;



NIVissimEdge::connection_position_sorter::connection_position_sorter(int edgeid)
    : myEdgeID(edgeid)
{
}


int
NIVissimEdge::connection_position_sorter::operator() (int c1id,
                                                      int c2id) const
{
    NIVissimConnection *c1 = NIVissimConnection::dictionary(c1id);
    NIVissimConnection *c2 = NIVissimConnection::dictionary(c2id);
    double pos1 =
        c1->getFromEdgeID()==myEdgeID
        ? c1->getFromPosition() : c1->getToPosition();
    double pos2 =
        c2->getFromEdgeID()==myEdgeID
        ? c2->getFromPosition() : c2->getToPosition();
    return pos1<pos2;
}








NIVissimEdge::connection_cluster_position_sorter::connection_cluster_position_sorter(int edgeid)
    : myEdgeID(edgeid)
{
}


int
NIVissimEdge::connection_cluster_position_sorter::operator() (
             NIVissimConnectionCluster *cc1,
             NIVissimConnectionCluster *cc2) const
{
    double pos1 = cc1->getPositionForEdge(myEdgeID);
    double pos2 = cc2->getPositionForEdge(myEdgeID);
    if(pos2<0||pos1<0) {
        cc1->getPositionForEdge(myEdgeID);
        cc2->getPositionForEdge(myEdgeID);
    }
    assert(pos1>=0&&pos2>=0);
    return pos1<pos2;
}





NIVissimEdge::DictType NIVissimEdge::myDict;
int NIVissimEdge::myMaxID = 0;


NIVissimEdge::NIVissimEdge(int id, const std::string &name,
                           const std::string &type, int noLanes,
                           double zuschlag1, double zuschlag2,
                           double length, const Position2DVector &geom,
                           const NIVissimClosedLanesVector &clv)
    : NIVissimAbstractEdge(id, geom),
        myName(name), myType(type), myNoLanes(noLanes),
        myZuschlag1(zuschlag1), myZuschlag2(zuschlag2),
    myClosedLanes(clv)//, mySpeed(-1)
{
    if(myMaxID<myID) {
        myMaxID = myID;
    }
    for(int i=0; i<noLanes; i++) {
        myLaneSpeeds.push_back(-1);
    }
}


NIVissimEdge::~NIVissimEdge()
{
    for(NIVissimClosedLanesVector::iterator i=myClosedLanes.begin(); i!=myClosedLanes.end(); i++) {
        delete (*i);
    }
    myClosedLanes.clear();
}


bool
NIVissimEdge::dictionary(int id, const std::string &name,
                         const std::string &type, int noLanes,
                         double zuschlag1, double zuschlag2, double length,
                         const Position2DVector &geom,
                         const NIVissimClosedLanesVector &clv)
{
    NIVissimEdge *o = new NIVissimEdge(id, name, type, noLanes, zuschlag1,
        zuschlag2, length, geom, clv);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}



bool
NIVissimEdge::dictionary(int id, NIVissimEdge *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}



NIVissimEdge *
NIVissimEdge::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimEdge::buildConnectionClusters()
{
    // build clusters for all edges made up from not previously assigne
    //  connections
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        int edgeid = (*i).first;
        NIVissimEdge *edge = (*i).second;
        // get all connectors using this edge
        IntVector connectors = edge->myIncomingConnections;
        copy(edge->myOutgoingConnections.begin(),
            edge->myOutgoingConnections.end(),
            back_inserter(connectors));
        if(connectors.size()==0) {
            continue;
        }
        // sort the connectors by the place on the edge
        sort(connectors.begin(), connectors.end(),
            connection_position_sorter(edgeid));
        // try to cluster the connections participating within the
        //  current edge
        IntVector currentCluster;
        IntVector::iterator j=connectors.begin();
        bool outgoing =
            NIVissimConnection::dictionary(*j)->getFromEdgeID()==(*i).first;
        double position =
            outgoing
            ? NIVissimConnection::dictionary(*j)->getFromPosition()
            : NIVissimConnection::dictionary(*j)->getToPosition();
        bool foundUnset = false;
        while(j!=connectors.end()&&NIVissimConnection::dictionary(*j)->hasNodeCluster()) {
            j++;
        }
        if(j==connectors.end()) {
            continue;
        }
        currentCluster.push_back(*j);
        do {
            if(j+1!=connectors.end()&&!NIVissimConnection::dictionary(*j)->hasNodeCluster()) {
                bool n_outgoing =
                    NIVissimConnection::dictionary(*(j+1))->getFromEdgeID()==edgeid;
                double n_position =
                    n_outgoing
                    ? NIVissimConnection::dictionary(*(j+1))->getFromPosition()
                    : NIVissimConnection::dictionary(*(j+1))->getToPosition();
                if(n_outgoing==outgoing && fabs(n_position-position)<10) {
                    currentCluster.push_back(*(j+1));
                } else {
                    IntVectorHelper::removeDouble(currentCluster);
                    (*i).second->myConnectionClusters.push_back(
                        new NIVissimConnectionCluster(currentCluster, -1,
                            (*i).second->myID));
                    currentCluster.clear();
                    currentCluster.push_back(*(j+1));
                }
                outgoing = n_outgoing;
                position = n_position;
            }
            j++;
        } while(j!=connectors.end());
        if(currentCluster.size()>0) {
            IntVectorHelper::removeDouble(currentCluster);
            (*i).second->myConnectionClusters.push_back(
                new NIVissimConnectionCluster(currentCluster, -1,
                    (*i).second->myID));
        }
    }
}


void
NIVissimEdge::dict_buildNBEdges(double offset)
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->buildNBEdge(offset);
    }
}

void
NIVissimEdge::dict_propagateSpeeds()
{
    DictType::iterator i;
    for(i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->setDistrictSpeed();
    }
    for(i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->propagateSpeed(-1, IntVector());
    }
    for(int j=0; j<3; j++) {
        for(i=myDict.begin(); i!=myDict.end(); i++) {
            NIVissimEdge *edge = (*i).second;
            edge->propagateOwn();
        }
        for(i=myDict.begin(); i!=myDict.end(); i++) {
            NIVissimEdge *edge = (*i).second;
            edge->checkUnconnectedLaneSpeeds();
        }
    }
/*    for(i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->setUnsetSpeed(5000);
    }*/
}


void
NIVissimEdge::checkUnconnectedLaneSpeeds()
{
    for(int i=0; i<myLaneSpeeds.size(); i++) {
        if(myLaneSpeeds[i]==-1) {
            double speed = -1;
            int j1 = i - 1;
            int j2 = i;
            while(j2!=myLaneSpeeds.size()&&myLaneSpeeds[j2]==-1) {
                j2++;
            }
            if(j1<0) {
                if(j2<myLaneSpeeds.size()) {
                    speed = myLaneSpeeds[j2];
                }
            } else {
                if(j2>=myLaneSpeeds.size()) {
                    speed = myLaneSpeeds[j1];
                } else {
                    speed = (myLaneSpeeds[j1] + myLaneSpeeds[j2]) / 2.0;
                }
            }
            if(speed==-1) {
                continue;
            }
            myLaneSpeeds[i] = speed;
            std::vector<NIVissimConnection*> connected = getOutgoingConnected(i);
            for(std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
                NIVissimConnection *c = *j;
                NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
                // propagate
                e->propagateSpeed(speed, c->getToLanes());
            }
        }
    }
}


void
NIVissimEdge::propagateOwn()
{
    for(int i=0; i<myLaneSpeeds.size(); i++) {
        if(myLaneSpeeds[i]==-1) {
            continue;
        }
        std::vector<NIVissimConnection*> connected = getOutgoingConnected(i);
        for(std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
            NIVissimConnection *c = *j;
            NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
            // propagate
            e->propagateSpeed(myLaneSpeeds[i], c->getToLanes());
        }
    }
}


void
NIVissimEdge::propagateSpeed(double speed, IntVector forLanes)
{
    // if no lane is given, all set be set
    if(forLanes.size()==0) {
        for(size_t i=0; i<myNoLanes; i++) {
            forLanes.push_back(i);
        }
    }
    // for the case of a first call
    // go through the lanes
    for(IntVector::const_iterator i=forLanes.begin(); i<forLanes.end(); i++) {
        // check whether a speed was set before
        if(myLaneSpeeds[*i]!=-1) {
            // do not reset it from incoming
            continue;
        }
        // check whether the lane has a new speed to set
        if(myPatchedSpeeds.size()>*i&&myPatchedSpeeds[*i]!=-1) {
            // use it
            speed = getRealSpeed(myPatchedSpeeds[*i]);
        }
        // check whether a speed is given
        if(speed==-1) {
            // do nothing if not
            continue;
        }
        // set the lane's speed to the given
        myLaneSpeeds[*i] = speed;
        // propagate the speed further
            // get the list of connected edges
        std::vector<NIVissimConnection*> connected = getOutgoingConnected(*i);
            // go throught the list
        for(std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
            NIVissimConnection *c = *j;
            NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
            // propagate
            e->propagateSpeed(speed, c->getToLanes());
        }
    }
}



void
NIVissimEdge::setDistrictSpeed()
{
    if(myDistrictConnections.size()>0) {
        double pos = *(myDistrictConnections.begin());
        if(pos<getLength()-pos) {
            NIVissimDistrictConnection *d =
                NIVissimDistrictConnection::dict_findForEdge(myID);
            if(d!=0) {
                double speed = d->getMeanSpeed();
                if(speed==-1) {
                    return;
                }
                for(size_t i=0; i<myNoLanes; i++) {
                    myLaneSpeeds[i] = speed;
                    // propagate the speed further
                        // get the list of connected edges
                    std::vector<NIVissimConnection*> connected = getOutgoingConnected(i);
                        // go throught the list
                    for(std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
                        NIVissimConnection *c = *j;
                        NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
                        // propagate
                        e->propagateSpeed(speed, c->getToLanes());
                    }
                }
            }
        }
    }
}


std::vector<NIVissimConnection*>
NIVissimEdge::getOutgoingConnected(int lane) const
{
    std::vector<NIVissimConnection*> ret;
    for(IntVector::const_iterator i=myOutgoingConnections.begin(); i!=myOutgoingConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        const IntVector &lanes = c->getFromLanes();
        if(find(lanes.begin(), lanes.end(), lane)!=lanes.end()) {
            NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
            if(e!=0) {
                ret.push_back(c);
            }
        }
    }
    return ret;
}


void
NIVissimEdge::buildNBEdge(double offset)
{
    // build the edge
    std::pair<NIVissimConnectionCluster*, NBNode *> fromInf, toInf;
    NBNode *fromNode, *toNode;
    fromNode = toNode = 0;
    sort(myConnectionClusters.begin(), myConnectionClusters.end(),
        connection_cluster_position_sorter(myID));
    sort(myDistrictConnections.begin(), myDistrictConnections.end());
    ConnectionClusters tmpClusters = myConnectionClusters;
   if(tmpClusters.size()!=0) {
/*        NBNode *from = new NBNode(
            toString<int>(myID) + "-Begin",
            myGeom.getBegin().x(), myGeom.getBegin().y());
        NBNode *to = new NBNode(
            toString<int>(myID) + "-End",
            myGeom.getEnd().x(), myGeom.getEnd().y());
    }
    // otherwise, build a connected edge
    else {*/
        sort(tmpClusters.begin(), tmpClusters.end(),
            connection_cluster_position_sorter(myID));
        // get or build the from-node
        //  A node may have to be build when the edge starts or ends at
        //  a parking place or something like this
        fromInf = getFromNode(tmpClusters);
        fromNode = fromInf.second;
        // get or build the to-node
        toInf = getToNode(tmpClusters);
        toNode = toInf.second;
        // check whether one of the nodes should be a district node
/*        NIVissimDistrictConnection *d =
            NIVissimDistrictConnection::dict_findForEdge(myID);
        if(d!=0&&fromNode!=toNode) {
            std::pair<NBNode*, NBNode*> tmp = remapOneOfNodes(d, fromNode, toNode);
            fromNode = tmp.first;
            toNode = tmp.second;
        }*/

        // if both nodes are the same, resolve the problem otherwise
        if(fromNode==toNode) {
            std::pair<NBNode*, NBNode*> tmp =
                resolveSameNode(offset, fromNode, toNode);
            if(fromNode!=tmp.first) {
                fromInf.first = 0;
            }
            if(toNode!=tmp.second) {
                toInf.first = 0;
            }
            fromNode = tmp.first;
            toNode = tmp.second;
        }
    }

    if(fromNode==0) {
        fromInf.first = 0;
        Position2D pos = myGeom.at(0);
        fromNode =
            new NBNode(toString<int>(myID) + string("-SourceNode"),
                pos.x(), pos.y(), NBNode::NODETYPE_NOJUNCTION);
        if(!NBNodeCont::insert(fromNode)) {
            throw 1;
        }
    } /* !_! else if(fromNode->getPosition()!=myGeom.at(0)) {
        myGeom.push_front(fromNode->getPosition());
    }*/
    if(toNode==0) {
        toInf.first = 0;
        Position2D pos = myGeom.at(myGeom.size()-1);
        toNode =
            new NBNode(toString<int>(myID) + string("-DestinationNode"),
                pos.x(), pos.y(), NBNode::NODETYPE_NOJUNCTION);
        if(!NBNodeCont::insert(toNode)) {
            throw 1;
        }
    } /* !_! else if(toNode->getPosition()!=myGeom.getEnd()) {
        myGeom.push_back(toNode->getPosition());
    }*/

    // extend the edge's shape
/*    if(toInf.first!=0) {
        NIVissimConnection *c1 = toInf.first->getIncomingContinuation(this);
        if(c1!=0) {
            const Position2DVector &g1 = c1->getGeometry();
            myGeom.eraseAt(myGeom.size()-1);
            if(myGeom.at(myGeom.size()-1)!=g1.at(g1.size()-1)) {
                myGeom.push_back(g1.at(0));
            }
            if(myGeom.at(myGeom.size()-1)!=g1.at(g1.size()-1)) {
                myGeom.push_back(g1.at(g1.size()-1));
            }
   if(myID==6) {
       cout << setprecision(10) << "To1:" << g1 << endl;
   }*/
/*        if(g1.size()>=2) {
            Position2D cp = g1.intersectsAtPoint(myGeom);
*/
/*            if(cp.x()!=-1||cp.y()!=-1) {
                double pos = g1.nearest_position_on_line_to_point(cp);
                Position2DVector gs1 = g1.getSubpart(pos, g1.length());*/
/*   if(myID==6) {
       cout << "To2:" << gs1 << endl;
   }
                gs1.eraseAt(0);
   if(myID==6) {
       cout << "To3:" << gs1 << endl;
   }
                if(myGeom.at(myGeom.size()-1)==gs1.at(0)) {
                    gs1.eraseAt(0);
                }
                for(size_t o=0; o<gs1.size(); o++) {
                    const Position2D &p = gs1.at(o);
                    if(p!=myGeom.at(myGeom.size()-1)) {
                        myGeom.push_back(p);
                    }
                }
//                myGeom.push_back(gs1);
            } else {
                Position2DVector g2(g1);
                if(myGeom.at(myGeom.size()-1)==g1.at(0)) {
                    g2.eraseAt(0);
                }
                for(size_t o=0; o<g2.size(); o++) {
                    const Position2D &p = g2.at(o);
                    if(p!=myGeom.at(myGeom.size()-1)) {
                        myGeom.push_back(p);
                    }
                }
            }
        } */
/*        }
    }
   if(myID==6) {
       cout << "Own2:" << myGeom << endl;
   }
    if(fromInf.first!=0) {
        NIVissimConnection *c1 = fromInf.first->getOutgoingContinuation(this);
        if(c1!=0) {
        const Position2DVector &g1 = c1->getGeometry();
        myGeom.eraseAt(0);
        if(g1.at(g1.size()-1)!=myGeom.at(0)) {
            myGeom.push_front(g1.at(g1.size()-1));
        }
        if(g1.at(0)!=myGeom.at(0)) {
            myGeom.push_front(g1.at(0));
        }
   if(myID==6) {
       cout << "From1:" << g1 << endl;
   }*/
/*        if(g1.size()>=2) {
            Position2D cp = g1.intersectsAtPoint(myGeom);
            if(cp.x()!=-1||cp.y()!=-1) {
                double pos = g1.nearest_position_on_line_to_point(cp);
                Position2DVector gs1 = g1.getSubpart(0, pos);
   if(myID==6) {
       cout << "From2:" << gs1 << endl;
   }*/
/*
        Position2DVector g =
            fromInf.first->getOutgoingContinuationGeometry(this);
        if(g.size()>0) {
            g.eraseAt(g.size()-1);
        }*/
/*               gs1.eraseAt(gs1.size()-1);
   if(myID==6) {
       cout << "From3:" << gs1 << endl;
   }
                if(myGeom.at(myGeom.size()-1)==gs1.at(gs1.size()-1)) {
                    gs1.eraseAt(gs1.size()-1);
                }
                for(int o=gs1.size()-1; o>=0; o--) {
                    const Position2D &p = gs1.at(o);
                    if(p!=myGeom.at(0)) {
                        myGeom.push_front(p);
                    }
                }
//                gs1.push_back(myGeom);
//                myGeom = gs1;
            } else {
                Position2DVector g2(g1);
                if(g2.at(g2.size()-1)==myGeom.at(0)) {
                    myGeom.eraseAt(0);
                }
                for(int o=g2.size()-1; o>=0; o--) {
                    const Position2D &p = g2.at(o);
                    if(p!=myGeom.at(0)) {
                        myGeom.push_front(p);
                    }
                }
//                g2.push_back(myGeom);
//                myGeom = g2;
            }
        }*/
/*        }
   if(myID==6) {
       cout << "Own3:" << myGeom << endl;
   }
    }*/
    //
    // build the edge
//    assert(mySpeed!=-1);
    double bla = 0;
    int i;
    for(i=0; i<myNoLanes; i++) {
        if(myLaneSpeeds.size()<=i||myLaneSpeeds[i]==-1) {
            cout << "Unset speed on edge:" << myID << ", lane:" << i
                << ". Using default." << endl;
            bla += OptionsSubSys::getOptions().getFloat("vissim-default-speed");
        } else {
            bla += myLaneSpeeds[i];
        }
    }
    bla /= (double) myLaneSpeeds.size();
    bla *= OptionsSubSys::getOptions().getFloat("vissim-speed-norm");

    NBEdge *buildEdge = new NBEdge(
        toString<int>(myID), myName, fromNode, toNode, myType,
        bla/3.6, myNoLanes, myGeom.length(), 0, myGeom,
        NBEdge::LANESPREAD_CENTER, NBEdge::EDGEFUNCTION_NORMAL);
    for(i=0; i<myNoLanes; i++) {
        if(myLaneSpeeds.size()<=i||myLaneSpeeds[i]==-1) {
            buildEdge->setLaneSpeed(i,
                OptionsSubSys::getOptions().getFloat("vissim-default-speed"));
        } else {
            buildEdge->setLaneSpeed(i, myLaneSpeeds[i]);
        }
    }
    NBEdgeCont::insert(buildEdge);
    // check whether the edge contains any other clusters
    if(tmpClusters.size()>0) {
		bool cont = true;
        ConnectionClusters::iterator j = tmpClusters.begin();
        // check whether the first node was already build
/*        if(!fromInf.first) {
            j++;
        }*/
        ConnectionClusters::iterator end = tmpClusters.end();
        // check whether the last node was already build
/*        if(!toInf.first) {
            end--;
        }*/

        for(; cont && j!=end; j++) {
            // split the edge at the previously build node
            string nextID = buildEdge->getID() + "[1]";
            cont = NBEdgeCont::splitAt(buildEdge, (*j)->getNBNode());
			// !!! what to do if the edge could not be split?
            buildEdge = NBEdgeCont::retrieve(nextID);
        }
    }
}


double
NIVissimEdge::getRealSpeed(int distNo)
{
    string id = toString<int>(distNo);
    Distribution *dist = NBDistribution::dictionary("speed", id);
    if(dist==0) {
        cout << "The referenced speed distribution '" << id << "' is not known." << endl;
        return -1;
    }
    assert(dist!=0);
    double speed = dist->getMax();
    if(speed<0||speed>1000) {
        cout << "What about distribution '" << distNo << "' " << endl;
    }
    return speed;
}

/*
bool
NIVissimEdge::recheckSpeedPatches()
{
//    size_t speed_idx = -1;
    // check set speeds
    if(myPatchedSpeeds.size()!=0) {
        DoubleVector::iterator i =
            find(myPatchedSpeeds.begin(), myPatchedSpeeds.end(), -1);
        if(myPatchedSpeeds.size()!=myNoLanes||i!=myPatchedSpeeds.end()) {
            cout << "Warning! Not all lanes are patched! (edge:" << myID << ")." << endl;
        }
        //
        if(DoubleVectorHelper::maxValue(myPatchedSpeeds)!=DoubleVectorHelper::minValue(myPatchedSpeeds)) {
            cout << "Warning! Not all lanes have the same speed!! (edge:" << myID << ")." << endl;
        }
        //
/*        // !!! ist natürlich Quatsch - erst recht, wenn Edges zusammengefasst werden
        speed = DoubleVectorHelper::sum(myPatchedSpeeds);
        speed /= (double) myPatchedSpeeds.size();*/
/*        return true;
    }
    if(myDistrictConnections.size()>0) {
        double pos = *(myDistrictConnections.begin());
//        if(pos<10) {
            NIVissimDistrictConnection *d =
                NIVissimDistrictConnection::dict_findForEdge(myID);
            if(d!=0) {
                return true;
//                speed = d->getMeanSpeed();
            }
//        }
//        return true;
    }
    return false;
}
*/

std::pair<NIVissimConnectionCluster*, NBNode *>
NIVissimEdge::getFromNode(ConnectionClusters &clusters)
{
    assert(clusters.size()>=1);
    const Position2D &beg = myGeom.getBegin();
    NIVissimConnectionCluster *c = *(clusters.begin());
    // check whether the edge starts within a already build node
    if(c->around(beg, 5.0)) {
        clusters.erase(clusters.begin());
        return std::pair<NIVissimConnectionCluster*, NBNode *>
            (c, c->getNBNode());
    }
    // check for a parking place at the begin
    if(myDistrictConnections.size()>0) {
        double pos = *(myDistrictConnections.begin());
        if(pos<10) {
            NBNode *node = new NBNode(toString<int>(myID) + "-begin",
                beg.x(), beg.y(), NBNode::NODETYPE_NOJUNCTION);
            if(!NBNodeCont::insert(node)) {
                throw 1;
            }
            while(myDistrictConnections.size()>0&&*(myDistrictConnections.begin())<10) {
                myDistrictConnections.erase(myDistrictConnections.begin());
            }
            return std::pair<NIVissimConnectionCluster*, NBNode *>(0, node);
        }
    }

/*    // build a new node for the edge's begin otherwise
    NBNode *node = new NBNode(toString<int>(myID) + "-begin",
        beg.x(), beg.y(), NBNode::NODETYPE_NOJUNCTION);
    if(!NBNodeCont::insert(node)) {
        throw 1;
    }
    return std::pair<bool, NBNode *>(true, node);*/
    clusters.erase(clusters.begin());
    return std::pair<NIVissimConnectionCluster*, NBNode *>(c, c->getNBNode());
}


std::pair<NIVissimConnectionCluster*, NBNode *>
NIVissimEdge::getToNode(ConnectionClusters &clusters)
{
//    assert(clusters.size()>=1);
    const Position2D &end = myGeom.getEnd();
    if(clusters.size()>0) {
        NIVissimConnectionCluster *c = *(clusters.end()-1);
        // check whether the edge ends within a already build node
        if(c->around(end, 5.0)) {
            clusters.erase(clusters.end()-1);
            return std::pair<NIVissimConnectionCluster*, NBNode *>(c, c->getNBNode());
        }
    }

    // check for a parking place at the end
    if(myDistrictConnections.size()>0) {
        double pos = *(myDistrictConnections.end()-1);
        if(pos>myGeom.length()-10) {
            NBNode *node = new NBNode(toString<int>(myID) + "-end",
                end.x(), end.y(), NBNode::NODETYPE_NOJUNCTION);
            if(!NBNodeCont::insert(node)) {
                throw 1;
            }
            while(myDistrictConnections.size()>0&&*(myDistrictConnections.end()-1)<myGeom.length()-10) {
                myDistrictConnections.erase(myDistrictConnections.end()-1);
            }
            return std::pair<NIVissimConnectionCluster*, NBNode *>(0, node);
        }
    }
/*    // build a new node for the edge's end otherwise
    NBNode *node = new NBNode(toString<int>(myID) + "-end",
        end.x(), end.y(), NBNode::NODETYPE_NOJUNCTION);
    if(!NBNodeCont::insert(node)) {
        throw 1;
    }
    return std::pair<bool, NBNode *>(true, node);*/
    if(clusters.size()>0) {
        NIVissimConnectionCluster *c = *(clusters.end()-1);
        clusters.erase(clusters.end()-1);
        return std::pair<NIVissimConnectionCluster*, NBNode *>(c, c->getNBNode());
    } else {
        // !!! dummy edge?!
        return std::pair<NIVissimConnectionCluster*, NBNode *>(0, (*(myConnectionClusters.begin()))->getNBNode());
    }
}


std::pair<NBNode*, NBNode*>
NIVissimEdge::remapOneOfNodes(NIVissimDistrictConnection *d,
                              NBNode *fromNode, NBNode *toNode)
{
    string nid = string("ParkingPlace") + toString<int>(d->getID());
    if( GeomHelper::distance(d->geomPosition(), fromNode->getPosition())
        <
        GeomHelper::distance(d->geomPosition(), toNode->getPosition()) ) {

        NBNode *newNode = new NBNode(nid,
            fromNode->getPosition().x(), fromNode->getPosition().y(),
            NBNode::NODETYPE_NOJUNCTION);
        NBNodeCont::erase(fromNode);
        NBNodeCont::insert(newNode);
        return std::pair<NBNode*, NBNode*>(newNode, toNode);
    } else {
        NBNode *newNode = new NBNode(nid,
            toNode->getPosition().x(), toNode->getPosition().y(),
            NBNode::NODETYPE_NOJUNCTION);
        NBNodeCont::erase(toNode);
        NBNodeCont::insert(newNode);
        return std::pair<NBNode*, NBNode*>(fromNode, newNode);
    }
}



std::pair<NBNode*, NBNode*>
NIVissimEdge::resolveSameNode(double offset, NBNode *prevFrom, NBNode *prevTo)
{
    // check whether the edge is connected to a district
    //  use it if so
    NIVissimDistrictConnection *d =
        NIVissimDistrictConnection::dict_findForEdge(myID);
    if(d!=0) {
        Position2D pos = d->geomPosition();
        double position = d->getPosition();
        // the district is at the begin of the edge
        if(myGeom.length()-position>position) {
            string nid = string("ParkingPlace") + toString<int>(d->getID());
            NBNode *node = NBNodeCont::retrieve(nid);
            if(node==0) {
                node = new NBNode(nid,
                    pos.x(), pos.y(), NBNode::NODETYPE_NOJUNCTION);
                if(!NBNodeCont::insert(node)) {
                    throw 1;
                }
            }
            return std::pair<NBNode*, NBNode*>(node, prevTo);
        }
        // the district is at the end of the edge
        else {
            string nid = string("ParkingPlace") + toString<int>(d->getID());
            NBNode *node = NBNodeCont::retrieve(nid);
            if(node==0) {
                node = new NBNode(nid,
                    pos.x(), pos.y(), NBNode::NODETYPE_NOJUNCTION);
                if(!NBNodeCont::insert(node)) {
                    throw 1;
                }
            }
            assert(node!=0);
            return std::pair<NBNode*, NBNode*>(prevFrom, node);
        }
    }
    // otherwise, check whether the edge is some kind of
    //  a dead end...
    // check which end is nearer to the node centre
    if(myConnectionClusters.size()==1) {
        NBNode *node = prevFrom; // it is the same as getToNode()

        NIVissimConnectionCluster *c = *(myConnectionClusters.begin());
        // no end node given
        if(c->around(myGeom.getBegin(), offset) && !c->around(myGeom.getEnd(), offset)) {
            NBNode *end = new NBNode(
                toString<int>(myID) + "-End",
                myGeom.getEnd().x(), myGeom.getEnd().y(),
                NBNode::NODETYPE_NOJUNCTION);
            if(!NBNodeCont::insert(end)) {
                throw 1;
            }
            return std::pair<NBNode*, NBNode*>(node, end);
        }

        // no begin node given
        if(!c->around(myGeom.getBegin(), offset) && c->around(myGeom.getEnd(), offset)) {
            NBNode *beg = new NBNode(
                toString<int>(myID) + "-Begin",
                myGeom.getBegin().x(), myGeom.getBegin().y(),
                NBNode::NODETYPE_NOJUNCTION);
            if(!NBNodeCont::insert(beg)) {
                cout << "nope, NIVissimDisturbance" << endl;
                throw 1;
            }
            return std::pair<NBNode*, NBNode*>(beg, node);
        }

        // "dummy edge" - both points lie within the same cluster
        if(c->around(myGeom.getBegin()) && c->around(myGeom.getEnd())) {
            return std::pair<NBNode*, NBNode*>(node, node);
        }
    }
    // what to do in other cases?
    //  It simply is a dummy edge....
    return std::pair<NBNode*, NBNode*>(prevFrom, prevTo);
}




void
NIVissimEdge::setNodeCluster(int nodeid)
{
    myNode = nodeid;
}


void
NIVissimEdge::buildGeom()
{
}


void
NIVissimEdge::addIncomingConnection(int id)
{
    myIncomingConnections.push_back(id);
}


void
NIVissimEdge::addOutgoingConnection(int id)
{
    myOutgoingConnections.push_back(id);
}



void
NIVissimEdge::mergedInto(NIVissimConnectionCluster *old,
                         NIVissimConnectionCluster *act)
{
    ConnectionClusters::iterator i=
        find(myConnectionClusters.begin(), myConnectionClusters.end(), old);
    if(i!=myConnectionClusters.end()) {
        myConnectionClusters.erase(i);
    }
    i = find(myConnectionClusters.begin(), myConnectionClusters.end(), act);
    if(i==myConnectionClusters.end()) {
        myConnectionClusters.push_back(act);
    }
}



void
NIVissimEdge::removeFromConnectionCluster(NIVissimConnectionCluster *c)
{
    ConnectionClusters::iterator i=
        find(myConnectionClusters.begin(), myConnectionClusters.end(), c);
    assert(i!=myConnectionClusters.end());
    myConnectionClusters.erase(i);
}


void
NIVissimEdge::addToConnectionCluster(NIVissimConnectionCluster *c)
{
    ConnectionClusters::iterator i=
        find(myConnectionClusters.begin(), myConnectionClusters.end(), c);
    if(i==myConnectionClusters.end()) {
        myConnectionClusters.push_back(c);
    }
}


Position2D
NIVissimEdge::getBegin2D() const
{
    return myGeom.at(0);
}


Position2D
NIVissimEdge::getEnd2D() const
{
    return myGeom.at(myGeom.size()-1);
}


double
NIVissimEdge::getLength() const
{
    return myGeom.length();
}


void
NIVissimEdge::checkDistrictConnectionExistanceAt(double pos)
{
    if(find(myDistrictConnections.begin(), myDistrictConnections.end(), pos)==myDistrictConnections.end()) {
        myDistrictConnections.push_back(pos);
/*        int id = NIVissimConnection::getMaxID() + 1;
        IntVector currentCluster;
        currentCluster.push_back(id);
        myConnectionClusters.push_back(
            new NIVissimConnectionCluster(currentCluster, -1, myID));*/
    }
}

/*
void
NIVissimEdge::replaceSpeed(int id, int lane, double speed)
{
    DictType::iterator i = myDict.find(id);
    assert(i!=myDict.end());
    (*i).second->replaceSpeed(lane, speed);
}
*/

void
NIVissimEdge::setSpeed(int lane, int speedDist)
{
    while(myPatchedSpeeds.size()<=lane) {
        myPatchedSpeeds.push_back(-1);
    }
    myPatchedSpeeds[lane] = speedDist;
}


void
NIVissimEdge::dict_checkEdges2Join()
{
    // go through the edges
    for(DictType::iterator i1=myDict.begin(); i1!=myDict.end(); i1++) {
        // retrieve needed values from the first edge
        NIVissimEdge *e1 = (*i1).second;
        const Position2DVector &g1 = e1->getGeometry();
        // check all other edges
        DictType::iterator i2=i1;
        i2++;
        for(; i2!=myDict.end(); i2++) {
            // retrieve needed values from the second edge
            NIVissimEdge *e2 = (*i2).second;
            const Position2DVector &g2 = e2->getGeometry();
            // get the connection description
            NIVissimConnection *c = e1->getConnectionTo(e2);
            if(c==0) {
                c = e2->getConnectionTo(e1);
            }
            // the edge must not be a direct contiuation of the other
            if(c!=0) {
                if( (c->getFromEdgeID()==e1->getID()&&fabs(c->getFromPosition()-e1->getGeometry().length())<5)
                    ||
                    (c->getFromEdgeID()==e2->getID()&&fabs(c->getFromPosition()-e2->getGeometry().length())<5) ) {

                    continue;
                }
            }
            // only parallel edges which do end at the same node
            //  should be joined
            // retrieve the "approximating" lines first
            Line2D l1 = Line2D(g1.getBegin(), g1.getEnd());
            Line2D l2 = Line2D(g2.getBegin(), g2.getEnd());
            // check for parallelity
            //  !!! the usage of an explicite value is not very fine
            if(fabs(l1.atan2DegreeAngle()-l2.atan2DegreeAngle())>2.0) {
                // continue if the lines are not parallel
                continue;
            }

            // check whether the same node is approached
            //  (the distance between the ends should not be too large)
            //  !!! the usage of an explicite value is not very fine
            if(GeomHelper::distance(l1.p2(), l2.p2())>10) {
                // continue if the lines do not end at the same length
                continue;
            }
            // ok, seem to be different lanes for the same edge
            //  mark as possibly joined later
            e1->addToTreatAsSame(e2);
            e2->addToTreatAsSame(e1);
        }
    }
}


void
NIVissimEdge::addToTreatAsSame(NIVissimEdge *e)
{
    if(e==this) {
        return;
    }
    // check whether this edge already knows about the other
    if(find(myToTreatAsSame.begin(), myToTreatAsSame.end(), e)==myToTreatAsSame.end()) {
        myToTreatAsSame.push_back(e);
    } else {
        return; // !!! check this
    }
    //
    std::vector<NIVissimEdge*>::iterator i;
    // add to all other that shall be treated as same
    for(i=myToTreatAsSame.begin(); i!=myToTreatAsSame.end(); i++) {
        (*i)->addToTreatAsSame(e);
    }
    for(i=myToTreatAsSame.begin(); i!=myToTreatAsSame.end(); i++) {
        e->addToTreatAsSame(*i);
    }
}

NIVissimConnection*
NIVissimEdge::getConnectionTo(NIVissimEdge *e)
{
    IntVector::iterator i;
    for(i=myIncomingConnections.begin(); i!=myIncomingConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if(c->getFromEdgeID()==e->getID()) {
            return c;
        }
    }
    for(i=myOutgoingConnections.begin(); i!=myOutgoingConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if(c->getToEdgeID()==e->getID()) {
            return c;
        }
    }
    return 0;
}


const std::vector<NIVissimEdge*> &
NIVissimEdge::getToTreatAsSame() const
{
    return myToTreatAsSame;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimEdge.icc"
//#endif

// Local Variables:
// mode:C++
// End:


