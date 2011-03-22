/****************************************************************************/
/// @file    NIVissimEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A temporary storage for edges imported from Vissim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <algorithm>
#include <map>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iterator>
#include <utils/common/ToString.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/GeomHelper.h>
#include <utils/distribution/Distribution.h>
#include <netbuild/NBDistribution.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <utils/options/OptionsCont.h>
#include "NIVissimNodeCluster.h"
#include "NIVissimDistrictConnection.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimEdge.h"
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
NIVissimEdge::DictType NIVissimEdge::myDict;
int NIVissimEdge::myMaxID = 0;
std::vector<std::string> NIVissimEdge::myLanesWithMissingSpeeds;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimEdge::connection_position_sorter::connection_position_sorter(int edgeid)
        : myEdgeID(edgeid) {}


int
NIVissimEdge::connection_position_sorter::operator()(int c1id,
        int c2id) const {
    NIVissimConnection *c1 = NIVissimConnection::dictionary(c1id);
    NIVissimConnection *c2 = NIVissimConnection::dictionary(c2id);
    SUMOReal pos1 =
        c1->getFromEdgeID()==myEdgeID
        ? c1->getFromPosition() : c1->getToPosition();
    SUMOReal pos2 =
        c2->getFromEdgeID()==myEdgeID
        ? c2->getFromPosition() : c2->getToPosition();
    return pos1<pos2;
}








NIVissimEdge::connection_cluster_position_sorter::connection_cluster_position_sorter(int edgeid)
        : myEdgeID(edgeid) {}


int
NIVissimEdge::connection_cluster_position_sorter::operator()(
    NIVissimConnectionCluster *cc1,
    NIVissimConnectionCluster *cc2) const {
    SUMOReal pos1 = cc1->getPositionForEdge(myEdgeID);
    SUMOReal pos2 = cc2->getPositionForEdge(myEdgeID);
    if (pos2<0||pos1<0) {
        cc1->getPositionForEdge(myEdgeID);
        cc2->getPositionForEdge(myEdgeID);
    }
    assert(pos1>=0&&pos2>=0);
    return pos1<pos2;
}




NIVissimEdge::NIVissimEdge(int id, const std::string &name,
                           const std::string &type, int noLanes,
                           SUMOReal zuschlag1, SUMOReal zuschlag2,
                           SUMOReal /*length*/, const Position2DVector &geom,
                           const NIVissimClosedLanesVector &clv)
        : NIVissimAbstractEdge(id, geom),
        myName(name), myType(type), myNoLanes(noLanes),
        myZuschlag1(zuschlag1), myZuschlag2(zuschlag2),
        myClosedLanes(clv), myAmWithinJunction(false) { //, mySpeed(-1)
    assert(noLanes>=0);
    if (myMaxID<myID) {
        myMaxID = myID;
    }
    for (int i=0; i<noLanes; i++) {
        myLaneSpeeds.push_back(-1);
    }
}


NIVissimEdge::~NIVissimEdge() {
    for (NIVissimClosedLanesVector::iterator i=myClosedLanes.begin(); i!=myClosedLanes.end(); i++) {
        delete(*i);
    }
    myClosedLanes.clear();
}


bool
NIVissimEdge::dictionary(int id, const std::string &name,
                         const std::string &type, int noLanes,
                         SUMOReal zuschlag1, SUMOReal zuschlag2, SUMOReal length,
                         const Position2DVector &geom,
                         const NIVissimClosedLanesVector &clv) {
    NIVissimEdge *o = new NIVissimEdge(id, name, type, noLanes, zuschlag1,
                                       zuschlag2, length, geom, clv);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}



bool
NIVissimEdge::dictionary(int id, NIVissimEdge *o) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}



NIVissimEdge *
NIVissimEdge::dictionary(int id) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimEdge::buildConnectionClusters() throw() {
    const SUMOReal MAX_CLUSTER_DISTANCE = 10;
    // build clusters for all edges made up from not previously assigne
    //  connections
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        int edgeid = (*i).first;
        NIVissimEdge *edge = (*i).second;
        // get all connectors using this edge
        IntVector connectors = edge->myIncomingConnections;
        copy(edge->myOutgoingConnections.begin(), edge->myOutgoingConnections.end(), back_inserter(connectors));
        if (connectors.size()==0) {
            continue;
        }
        // sort the connectors by the place on the edge
        sort(connectors.begin(), connectors.end(), connection_position_sorter(edgeid));
        // try to cluster the connections participating within the current edge
        IntVector currentCluster;
        IntVector::iterator j=connectors.begin();
        bool outgoing = NIVissimConnection::dictionary(*j)->getFromEdgeID()==(*i).first;
        SUMOReal position = outgoing
                            ? NIVissimConnection::dictionary(*j)->getFromPosition()
                            : NIVissimConnection::dictionary(*j)->getToPosition();

        // skip connections already in a cluster
        // !!! (?)
        while (j!=connectors.end()&&NIVissimConnection::dictionary(*j)->hasNodeCluster()) {
            ++j;
        }
        if (j==connectors.end()) {
            continue;
        }
        currentCluster.push_back(*j);
        do {
            if (j+1!=connectors.end()&&!NIVissimConnection::dictionary(*j)->hasNodeCluster()) {
                bool n_outgoing = NIVissimConnection::dictionary(*(j+1))->getFromEdgeID()==edgeid;
                SUMOReal n_position = n_outgoing
                                      ? NIVissimConnection::dictionary(*(j+1))->getFromPosition()
                                      : NIVissimConnection::dictionary(*(j+1))->getToPosition();
                if (n_outgoing==outgoing && fabs(n_position-position)<MAX_CLUSTER_DISTANCE) {
                    // ok, in same cluster as prior
                    currentCluster.push_back(*(j+1));
                } else {
                    // start new cluster
                    VectorHelper<int>::removeDouble(currentCluster);
                    edge->myConnectionClusters.push_back(new NIVissimConnectionCluster(currentCluster, -1, edgeid));
                    currentCluster.clear();
                    currentCluster.push_back(*(j+1));
                }
                outgoing = n_outgoing;
                position = n_position;
            }
            j++;
        } while (j!=connectors.end());
        // add last connection
        if (currentCluster.size()>0) {
            VectorHelper<int>::removeDouble(currentCluster);
            edge->myConnectionClusters.push_back(new NIVissimConnectionCluster(currentCluster, -1, edgeid));
        }
    }
}


void
NIVissimEdge::dict_buildNBEdges(NBDistrictCont &dc, NBNodeCont &nc,
                                NBEdgeCont &ec, SUMOReal offset) {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->buildNBEdge(dc, nc, ec, offset);
    }
}


void
NIVissimEdge::dict_propagateSpeeds(/* NBDistribution &dc */) {
    DictType::iterator i;
    for (i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->setDistrictSpeed(/* dc */);
    }
    for (i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->propagateSpeed(/* dc */ -1, IntVector());
    }
    for (int j=0; j<3; j++) {
        for (i=myDict.begin(); i!=myDict.end(); i++) {
            NIVissimEdge *edge = (*i).second;
            edge->propagateOwn(/* dc */);
        }
        for (i=myDict.begin(); i!=myDict.end(); i++) {
            NIVissimEdge *edge = (*i).second;
            edge->checkUnconnectedLaneSpeeds(/* dc */);
        }
    }
}


void
NIVissimEdge::checkUnconnectedLaneSpeeds(/* NBDistribution &dc */) {
    for (int i=0; i<(int) myLaneSpeeds.size(); i++) {
        if (myLaneSpeeds[i]==-1) {
            SUMOReal speed = -1;
            int j1 = i - 1; // !!! recheck - j1 may become negative?
            int j2 = i;
            while (j2!=(int) myLaneSpeeds.size()&&myLaneSpeeds[j2]==-1) {
                j2++;
            }
            if (j1<0) {
                if (j2<(int) myLaneSpeeds.size()) {
                    speed = myLaneSpeeds[j2];
                }
            } else {
                if (j2>=(int) myLaneSpeeds.size()) {
                    speed = myLaneSpeeds[j1];
                } else {
                    speed = (myLaneSpeeds[j1] + myLaneSpeeds[j2]) / (SUMOReal) 2.0;
                }
            }
            if (speed==-1) {
                continue;
            }
            myLaneSpeeds[i] = speed;
            std::vector<NIVissimConnection*> connected = getOutgoingConnected(i);
            for (std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
                NIVissimConnection *c = *j;
                NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
                // propagate
                e->propagateSpeed(/*dc, */speed, c->getToLanes());
            }
        }
    }
}


void
NIVissimEdge::propagateOwn(/* NBDistribution &dc */) {
    for (int i=0; i<(int) myLaneSpeeds.size(); i++) {
        if (myLaneSpeeds[i]==-1) {
            continue;
        }
        std::vector<NIVissimConnection*> connected = getOutgoingConnected(i);
        for (std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
            NIVissimConnection *c = *j;
            NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
            // propagate
            e->propagateSpeed(/*dc, */myLaneSpeeds[i], c->getToLanes());
        }
    }
}


void
NIVissimEdge::propagateSpeed(/* NBDistribution &dc */ SUMOReal speed, IntVector forLanes) {
    // if no lane is given, all set be set
    if (forLanes.size()==0) {
        for (size_t i=0; i<myNoLanes; i++) {
            forLanes.push_back((int) i);
        }
    }
    // for the case of a first call
    // go through the lanes
    for (IntVector::const_iterator i=forLanes.begin(); i<forLanes.end(); i++) {
        // check whether a speed was set before
        if (myLaneSpeeds[*i]!=-1) {
            // do not reset it from incoming
            continue;
        }
        // check whether the lane has a new speed to set
        if ((int) myPatchedSpeeds.size()>*i&&myPatchedSpeeds[*i]!=-1) {
            // use it
            speed = getRealSpeed(/*dc, */myPatchedSpeeds[*i]);
        }
        // check whether a speed is given
        if (speed==-1) {
            // do nothing if not
            continue;
        }
        // set the lane's speed to the given
        myLaneSpeeds[*i] = speed;
        // propagate the speed further
        // get the list of connected edges
        std::vector<NIVissimConnection*> connected = getOutgoingConnected(*i);
        // go throught the list
        for (std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
            NIVissimConnection *c = *j;
            NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
            // propagate
            e->propagateSpeed(/*dc, */speed, c->getToLanes());
        }
    }
}



void
NIVissimEdge::setDistrictSpeed(/* NBDistribution &dc */) {
    if (myDistrictConnections.size()>0) {
        SUMOReal pos = *(myDistrictConnections.begin());
        if (pos<getLength()-pos) {
            NIVissimDistrictConnection *d =
                NIVissimDistrictConnection::dict_findForEdge(myID);
            if (d!=0) {
                SUMOReal speed = d->getMeanSpeed(/*dc*/);
                if (speed==-1) {
                    return;
                }
                for (unsigned int i=0; i<myNoLanes; i++) {
                    myLaneSpeeds[i] = speed;
                    // propagate the speed further
                    // get the list of connected edges
                    std::vector<NIVissimConnection*> connected = getOutgoingConnected(i);
                    // go throught the list
                    for (std::vector<NIVissimConnection*>::iterator j=connected.begin(); j!=connected.end(); j++) {
                        NIVissimConnection *c = *j;
                        NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
                        // propagate
                        e->propagateSpeed(/*dc, */speed, c->getToLanes());
                    }
                }
            }
        }
    }
}


std::vector<NIVissimConnection*>
NIVissimEdge::getOutgoingConnected(int lane) const {
    std::vector<NIVissimConnection*> ret;
    for (IntVector::const_iterator i=myOutgoingConnections.begin(); i!=myOutgoingConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        const IntVector &lanes = c->getFromLanes();
        if (find(lanes.begin(), lanes.end(), lane)!=lanes.end()) {
            NIVissimEdge *e = NIVissimEdge::dictionary(c->getToEdgeID());
            if (e!=0) {
                ret.push_back(c);
            }
        }
    }
    return ret;
}


void
NIVissimEdge::buildNBEdge(NBDistrictCont &dc, NBNodeCont &nc, NBEdgeCont &ec,
                          SUMOReal sameNodesOffset) throw(ProcessError) {
    // build the edge
    std::pair<NIVissimConnectionCluster*, NBNode *> fromInf, toInf;
    NBNode *fromNode, *toNode;
    fromNode = toNode = 0;
    sort(myConnectionClusters.begin(), myConnectionClusters.end(), connection_cluster_position_sorter(myID));
    sort(myDistrictConnections.begin(), myDistrictConnections.end());
    ConnectionClusters tmpClusters = myConnectionClusters;
    if (tmpClusters.size()!=0) {
        sort(tmpClusters.begin(), tmpClusters.end(), connection_cluster_position_sorter(myID));
        // get or build the from-node
        //  A node may have to be build when the edge starts or ends at
        //  a parking place or something like this
        fromInf = getFromNode(nc, tmpClusters);
        fromNode = fromInf.second;
        // get or build the to-node
        //if(tmpClusters.size()>0) {
        toInf = getToNode(nc, tmpClusters);
        toNode = toInf.second;
        if (fromInf.first!=0&&toNode!=0&&fromInf.first->around(toNode->getPosition())) {
            MsgHandler::getWarningInstance()->inform("Will not build edge '" + toString(myID) + "'.");
            myAmWithinJunction = true;
            return;
        }
        //}
        // if both nodes are the same, resolve the problem otherwise
        if (fromNode==toNode) {
            std::pair<NBNode*, NBNode*> tmp = resolveSameNode(nc, sameNodesOffset, fromNode, toNode);
            if (fromNode!=tmp.first) {
                fromInf.first = 0;
            }
            if (toNode!=tmp.second) {
                toInf.first = 0;
            }
            fromNode = tmp.first;
            toNode = tmp.second;
        }
    }

    //
    if (fromNode==0) {
        fromInf.first = 0;
        Position2D pos = myGeom[0];
        fromNode = new NBNode(toString<int>(myID) + "-SourceNode", pos, NODETYPE_NOJUNCTION);
        if (!nc.insert(fromNode)) {
            throw ProcessError("Could not insert node '" + fromNode->getID() + "' to nodes container.");
        }
    }
    if (toNode==0) {
        toInf.first = 0;
        Position2D pos = myGeom[-1];
        toNode = new NBNode(toString<int>(myID) + "-DestinationNode", pos, NODETYPE_NOJUNCTION);
        if (!nc.insert(toNode)) {
            throw ProcessError("Could not insert node '" + toNode->getID() + "' to nodes container.");
        }
    }

    // build the edge
    SUMOReal avgSpeed = 0;
    int i;
    for (i=0; i<(int) myNoLanes; i++) {
        if (myLaneSpeeds.size()<=(size_t) i||myLaneSpeeds[i]==-1) {
            myLanesWithMissingSpeeds.push_back(toString(myID) + "_" + toString(i));
            avgSpeed += OptionsCont::getOptions().getFloat("vissim.default-speed");
        } else {
            avgSpeed += myLaneSpeeds[i];
        }
    }
    avgSpeed /= (SUMOReal) myLaneSpeeds.size();
    avgSpeed *= OptionsCont::getOptions().getFloat("vissim.speed-norm");

    if (fromNode==toNode) {
        MsgHandler::getWarningInstance()->inform("Could not build edge '" + toString(myID) + "'; would connect same node.");
        return;
    }

    NBEdge *buildEdge = new NBEdge(
        toString<int>(myID), fromNode, toNode, myType,
        avgSpeed/(SUMOReal) 3.6, myNoLanes, -1, myGeom,
        NBEdge::LANESPREAD_CENTER, true);
    for (i=0; i<(int) myNoLanes; i++) {
        if ((int) myLaneSpeeds.size()<=i||myLaneSpeeds[i]==-1) {
            buildEdge->setLaneSpeed(i, OptionsCont::getOptions().getFloat("vissim.default-speed")/(SUMOReal) 3.6);
        } else {
            buildEdge->setLaneSpeed(i, myLaneSpeeds[i]/(SUMOReal) 3.6);
        }
    }
    ec.insert(buildEdge);
    // check whether the edge contains any other clusters
    if (tmpClusters.size()>0) {
        bool cont = true;
        for (ConnectionClusters::iterator j = tmpClusters.begin(); cont && j!=tmpClusters.end(); ++j) {
            // split the edge at the previously build node
            std::string nextID = buildEdge->getID() + "[1]";
            cont = ec.splitAt(dc, buildEdge, (*j)->getNBNode());
            // !!! what to do if the edge could not be split?
            buildEdge = ec.retrieve(nextID);
        }
    }
}


SUMOReal
NIVissimEdge::getRealSpeed(/* NBDistribution &dc */ int distNo) {
    std::string id = toString<int>(distNo);
    Distribution *dist = NBDistribution::dictionary("speed", id);
    if (dist==0) {
        WRITE_WARNING("The referenced speed distribution '" + id + "' is not known.");
        return -1;
    }
    assert(dist!=0);
    SUMOReal speed = dist->getMax();
    if (speed<0||speed>1000) {
        WRITE_WARNING("What about distribution '" + toString<int>(distNo) + "' ");
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
            cot << "Warning! Not all lanes are patched! (edge:" << myID << ")." << endl;
        }
        //
        if(DoubleVectorHelper::maxValue(myPatchedSpeeds)!=DoubleVectorHelper::minValue(myPatchedSpeeds)) {
            cot << "Warning! Not all lanes have the same speed!! (edge:" << myID << ")." << endl;
        }
        //
/        // !!! ist natürlich Quatsch - erst recht, wenn Edges zusammengefasst werden
        speed = DoubleVectorHelper::sum(myPatchedSpeeds);
        speed /= (SUMOReal) myPatchedSpeeds.size();*/
/*        return true;
    }
    if(myDistrictConnections.size()>0) {
        SUMOReal pos = *(myDistrictConnections.begin());
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

std::pair<NIVissimConnectionCluster*, NBNode*>
NIVissimEdge::getFromNode(NBNodeCont &nc, ConnectionClusters &clusters) {
    const SUMOReal MAX_DISTANCE = 10.;
    assert(clusters.size()>=1);
    const Position2D &beg = myGeom.getBegin();
    NIVissimConnectionCluster *c = *(clusters.begin());
    // check whether the edge starts within a already build node
    if (c->around(beg, MAX_DISTANCE)) {
        clusters.erase(clusters.begin());
        return std::pair<NIVissimConnectionCluster*, NBNode*>
               (c, c->getNBNode());
    }
    // check for a parking place at the begin
    if (myDistrictConnections.size()>0) {
        SUMOReal pos = *(myDistrictConnections.begin());
        if (pos<10) {
            NBNode *node = new NBNode(toString<int>(myID) + "-begin", beg, NODETYPE_NOJUNCTION);
            if (!nc.insert(node)) {
                throw 1;
            }
            while (myDistrictConnections.size()>0&&*(myDistrictConnections.begin())<10) {
                myDistrictConnections.erase(myDistrictConnections.begin());
            }
            return std::pair<NIVissimConnectionCluster*, NBNode*>(static_cast<NIVissimConnectionCluster*>(0), node);
        }
    }
    // build a new node for the edge's begin otherwise
    NBNode *node = new NBNode(toString<int>(myID) + "-begin", beg, NODETYPE_NOJUNCTION);
    if (!nc.insert(node)) {
        throw 1;
    }
    return std::pair<NIVissimConnectionCluster*, NBNode*>(static_cast<NIVissimConnectionCluster*>(0), node);
}


std::pair<NIVissimConnectionCluster*, NBNode *>
NIVissimEdge::getToNode(NBNodeCont &nc, ConnectionClusters &clusters) {
    const Position2D &end = myGeom.getEnd();
    if (clusters.size()>0) {
        const SUMOReal MAX_DISTANCE = 10.;
        assert(clusters.size()>=1);
        NIVissimConnectionCluster *c = *(clusters.end()-1);
        // check whether the edge ends within a already build node
        if (c->around(end, MAX_DISTANCE)) {
            clusters.erase(clusters.end()-1);
            return std::pair<NIVissimConnectionCluster*, NBNode *>(c, c->getNBNode());
        }
    }
    // check for a parking place at the end
    if (myDistrictConnections.size()>0) {
        SUMOReal pos = *(myDistrictConnections.end()-1);
        if (pos>myGeom.length()-10) {
            NBNode *node = new NBNode(toString<int>(myID) + "-end", end, NODETYPE_NOJUNCTION);
            if (!nc.insert(node)) {
                throw 1;
            }
            while (myDistrictConnections.size()>0&&*(myDistrictConnections.end()-1)<myGeom.length()-10) {
                myDistrictConnections.erase(myDistrictConnections.end()-1);
            }
            return std::pair<NIVissimConnectionCluster*, NBNode*>(static_cast<NIVissimConnectionCluster*>(0), node);
        }
    }

    // build a new node for the edge's end otherwise
    NBNode *node = new NBNode(toString<int>(myID) + "-end", end, NODETYPE_NOJUNCTION);
    if (!nc.insert(node)) {
        throw 1;
    }
    return std::pair<NIVissimConnectionCluster*, NBNode *>(static_cast<NIVissimConnectionCluster*>(0), node);
    /*
    if (clusters.size()>0) {
    NIVissimConnectionCluster *c = *(clusters.end()-1);
    clusters.erase(clusters.end()-1);
    return std::pair<NIVissimConnectionCluster*, NBNode*>(c, c->getNBNode());
    } else {
    // !!! dummy edge?!
    return std::pair<NIVissimConnectionCluster*, NBNode*>(static_cast<NIVissimConnectionCluster*>(0), (*(myConnectionClusters.begin()))->getNBNode());
    }
    */
}


std::pair<NBNode*, NBNode*>
NIVissimEdge::remapOneOfNodes(NBNodeCont &nc,
                              NIVissimDistrictConnection *d,
                              NBNode *fromNode, NBNode *toNode) {
    std::string nid = "ParkingPlace" + toString<int>(d->getID());
    if (d->geomPosition().distanceTo(fromNode->getPosition())
            <
            d->geomPosition().distanceTo(toNode->getPosition())) {

        NBNode *newNode = new NBNode(nid,
                                     fromNode->getPosition(),
                                     NODETYPE_NOJUNCTION);
        nc.erase(fromNode);
        nc.insert(newNode);
        return std::pair<NBNode*, NBNode*>(newNode, toNode);
    } else {
        NBNode *newNode = new NBNode(nid,
                                     toNode->getPosition(),
                                     NODETYPE_NOJUNCTION);
        nc.erase(toNode);
        nc.insert(newNode);
        return std::pair<NBNode*, NBNode*>(fromNode, newNode);
    }
}



std::pair<NBNode*, NBNode*>
NIVissimEdge::resolveSameNode(NBNodeCont &nc, SUMOReal offset,
                              NBNode *prevFrom, NBNode *prevTo) {
    // check whether the edge is connected to a district
    //  use it if so
    NIVissimDistrictConnection *d =
        NIVissimDistrictConnection::dict_findForEdge(myID);
    if (d!=0) {
        Position2D pos = d->geomPosition();
        SUMOReal position = d->getPosition();
        // the district is at the begin of the edge
        if (myGeom.length()-position>position) {
            std::string nid = "ParkingPlace" + toString<int>(d->getID());
            NBNode *node = nc.retrieve(nid);
            if (node==0) {
                node = new NBNode(nid,
                                  pos, NODETYPE_NOJUNCTION);
                if (!nc.insert(node)) {
                    throw 1;
                }
            }
            return std::pair<NBNode*, NBNode*>(node, prevTo);
        }
        // the district is at the end of the edge
        else {
            std::string nid = "ParkingPlace" + toString<int>(d->getID());
            NBNode *node = nc.retrieve(nid);
            if (node==0) {
                node = new NBNode(nid,
                                  pos, NODETYPE_NOJUNCTION);
                if (!nc.insert(node)) {
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
    if (myConnectionClusters.size()==1) {
        NBNode *node = prevFrom; // it is the same as getToNode()

        NIVissimConnectionCluster *c = *(myConnectionClusters.begin());
        // no end node given
        if (c->around(myGeom.getBegin(), offset) && !c->around(myGeom.getEnd(), offset)) {
            NBNode *end = new NBNode(
                toString<int>(myID) + "-End",
                myGeom.getEnd(),
                NODETYPE_NOJUNCTION);
            if (!nc.insert(end)) {
                throw 1;
            }
            return std::pair<NBNode*, NBNode*>(node, end);
        }

        // no begin node given
        if (!c->around(myGeom.getBegin(), offset) && c->around(myGeom.getEnd(), offset)) {
            NBNode *beg = new NBNode(
                toString<int>(myID) + "-Begin",
                myGeom.getBegin(),
                NODETYPE_NOJUNCTION);
            if (!nc.insert(beg)) {
                std::cout << "nope, NIVissimDisturbance" << std::endl;
                throw 1;
            }
            return std::pair<NBNode*, NBNode*>(beg, node);
        }

        // "dummy edge" - both points lie within the same cluster
        if (c->around(myGeom.getBegin()) && c->around(myGeom.getEnd())) {
            return std::pair<NBNode*, NBNode*>(node, node);
        }
    }
    // what to do in other cases?
    //  It simply is a dummy edge....
    return std::pair<NBNode*, NBNode*>(prevFrom, prevTo);
}




void
NIVissimEdge::setNodeCluster(int nodeid) {
    myNode = nodeid;
}


void
NIVissimEdge::buildGeom() {}


void
NIVissimEdge::addIncomingConnection(int id) {
    myIncomingConnections.push_back(id);
}


void
NIVissimEdge::addOutgoingConnection(int id) {
    myOutgoingConnections.push_back(id);
}



void
NIVissimEdge::mergedInto(NIVissimConnectionCluster *old,
                         NIVissimConnectionCluster *act) {
    ConnectionClusters::iterator i=
        find(myConnectionClusters.begin(), myConnectionClusters.end(), old);
    if (i!=myConnectionClusters.end()) {
        myConnectionClusters.erase(i);
    }
    i = find(myConnectionClusters.begin(), myConnectionClusters.end(), act);
    if (i==myConnectionClusters.end()) {
        myConnectionClusters.push_back(act);
    }
}



void
NIVissimEdge::removeFromConnectionCluster(NIVissimConnectionCluster *c) {
    ConnectionClusters::iterator i=
        find(myConnectionClusters.begin(), myConnectionClusters.end(), c);
    assert(i!=myConnectionClusters.end());
    myConnectionClusters.erase(i);
}


void
NIVissimEdge::addToConnectionCluster(NIVissimConnectionCluster *c) {
    ConnectionClusters::iterator i=
        find(myConnectionClusters.begin(), myConnectionClusters.end(), c);
    if (i==myConnectionClusters.end()) {
        myConnectionClusters.push_back(c);
    }
}


Position2D // !!! reference?
NIVissimEdge::getBegin2D() const {
    return myGeom[0];
}


Position2D // !!! reference?
NIVissimEdge::getEnd2D() const {
    return myGeom[-1];
}


SUMOReal
NIVissimEdge::getLength() const {
    return myGeom.length();
}


void
NIVissimEdge::checkDistrictConnectionExistanceAt(SUMOReal pos) {
    if (find(myDistrictConnections.begin(), myDistrictConnections.end(), pos)==myDistrictConnections.end()) {
        myDistrictConnections.push_back(pos);
        /*        int id = NIVissimConnection::getMaxID() + 1;
                IntVector currentCluster;
                currentCluster.push_back(id);
                myConnectionClusters.push_back(
                    new NIVissimConnectionCluster(currentCluster, -1, myID));*/
    }
}


void
NIVissimEdge::setSpeed(size_t lane, int speedDist) {
    while (myPatchedSpeeds.size()<=lane) {
        myPatchedSpeeds.push_back(-1);
    }
    myPatchedSpeeds[lane] = speedDist;
}


void
NIVissimEdge::dict_checkEdges2Join() {
    // go through the edges
    for (DictType::iterator i1=myDict.begin(); i1!=myDict.end(); i1++) {
        // retrieve needed values from the first edge
        NIVissimEdge *e1 = (*i1).second;
        const Position2DVector &g1 = e1->getGeometry();
        // check all other edges
        DictType::iterator i2=i1;
        i2++;
        for (; i2!=myDict.end(); i2++) {
            // retrieve needed values from the second edge
            NIVissimEdge *e2 = (*i2).second;
            const Position2DVector &g2 = e2->getGeometry();
            // get the connection description
            NIVissimConnection *c = e1->getConnectionTo(e2);
            if (c==0) {
                c = e2->getConnectionTo(e1);
            }
            // the edge must not be a direct contiuation of the other
            if (c!=0) {
                if ((c->getFromEdgeID()==e1->getID()&&fabs(c->getFromPosition()-e1->getGeometry().length())<5)
                        ||
                        (c->getFromEdgeID()==e2->getID()&&fabs(c->getFromPosition()-e2->getGeometry().length())<5)) {

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
            if (fabs(l1.atan2DegreeAngle()-l2.atan2DegreeAngle())>2.0) {
                // continue if the lines are not parallel
                continue;
            }

            // check whether the same node is approached
            //  (the distance between the ends should not be too large)
            //  !!! the usage of an explicite value is not very fine
            if (l1.p2().distanceTo(l2.p2())>10) {
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


bool
NIVissimEdge::addToTreatAsSame(NIVissimEdge *e) {
    if (e==this) {
        return false;
    }
    // check whether this edge already knows about the other
    if (find(myToTreatAsSame.begin(), myToTreatAsSame.end(), e)==myToTreatAsSame.end()) {
        myToTreatAsSame.push_back(e);
        return true;
    } else {
        return false; // !!! check this
    }
    //
    std::vector<NIVissimEdge*>::iterator i;
    // add to all other that shall be treated as same
    bool changed = true;
    while (changed) {
        changed = false;
        for (i=myToTreatAsSame.begin(); !changed&&i!=myToTreatAsSame.end(); i++) {
            changed |= (*i)->addToTreatAsSame(e);
        }
        for (i=myToTreatAsSame.begin(); !changed&&i!=myToTreatAsSame.end(); i++) {
            changed |= e->addToTreatAsSame(*i);
        }
    }
}

NIVissimConnection*
NIVissimEdge::getConnectionTo(NIVissimEdge *e) {
    IntVector::iterator i;
    for (i=myIncomingConnections.begin(); i!=myIncomingConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if (c->getFromEdgeID()==e->getID()) {
            return c;
        }
    }
    for (i=myOutgoingConnections.begin(); i!=myOutgoingConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if (c->getToEdgeID()==e->getID()) {
            return c;
        }
    }
    return 0;
}


const std::vector<NIVissimEdge*> &
NIVissimEdge::getToTreatAsSame() const {
    return myToTreatAsSame;
}


void
NIVissimEdge::reportUnsetSpeeds() throw() {
    if (myLanesWithMissingSpeeds.size()==0) {
        return;
    }
    std::ostringstream str;
    str << "The following lanes have no explicite speed information:\n  ";
    for (std::vector<std::string>::iterator i=myLanesWithMissingSpeeds.begin(); i!=myLanesWithMissingSpeeds.end(); ++i) {
        if (i!=myLanesWithMissingSpeeds.begin()) {
            str << ", ";
        }
        str << *i;
    }
    MsgHandler::getWarningInstance()->inform(str.str());
}


NIVissimEdge *
NIVissimEdge::getBestIncoming() const throw() {
    for (IntVector::const_iterator i=myIncomingConnections.begin(); i!=myIncomingConnections.end(); ++i) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        return NIVissimEdge::dictionary(c->getFromEdgeID());
    }
    return 0;
}


NIVissimEdge *
NIVissimEdge::getBestOutgoing() const throw() {
    for (IntVector::const_iterator i=myOutgoingConnections.begin(); i!=myOutgoingConnections.end(); ++i) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        return NIVissimEdge::dictionary(c->getToEdgeID());
    }
    return 0;
}



/****************************************************************************/

