#include <string>
#include <algorithm>
#include <map>
#include <cassert>
#include <cmath>
#include <utils/convert/ToString.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
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
    myClosedLanes(clv)
{
    if(myMaxID<myID) {
        myMaxID = myID;
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

/*
bool
NIVissimEdge::crosses(const Position2DVector &poly) const
{
    return GeomHelper::intersects(poly, myGeom);
}


std::pair<double, double>
NIVissimEdge::getCrossingRange(const Position2DVector &poly) const
{
    std::pair<double, double> positions(-1, -1);
    double length = 0;
    for(Position2DVector::const_iterator i=myGeom.begin(); i!=myGeom.end()-1; i++) {
        checkPosition1(*i, *(i+1), length, poly, positions);
        length += GeomHelper::distance(*i, *(i+1));
    }
    checkPosition1(*(myGeom.end()-1), *(myGeom.begin()),
        length, poly, positions);
    return positions;
}


void
NIVissimEdge::checkPosition1(const Position2D &p1, const Position2D &p2,
                             double length, const Position2DVector &poly,
                             std::pair<double, double> &positions) const
{
    Position2DVector::const_iterator i = poly.begin();
    while(i!=poly.end()) {
        i = GeomHelper::find_intersecting_line(p1, p2, poly, i);
        if(i!=poly.end()) {
            Position2D pos = GeomHelper::intersection_position(p1, p2,
                poly, i);
            double at = GeomHelper::distance(p1, p2)
                / GeomHelper::distance(p1, pos);
            if(positions.first==-1||at<positions.second) {
                positions.first = at;
            }
            if(positions.second==-1||at>positions.second) {
                positions.second = at;
            }
        }
    }
}


int
NIVissimEdge::getID() const
{
    return myID;
}


void
NIVissimEdge::assignConnectorsAndDisturbances()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->myOutgingConnectors =
            NIVissimConnection::getOutgoingForEdge((*i).second->myID);
        (*i).second->myIncomingConnectors =
            NIVissimConnection::getIncomingForEdge((*i).second->myID);
        (*i).second->myDisturbers =
            NIVissimDisturbance::getDisturbatorsForEdge((*i).second->myID);
        (*i).second->myDisturbings =
            NIVissimDisturbance::getDisturbtionsForEdge((*i).second->myID);
    }
}
*/


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
NIVissimEdge::dict_buildNBEdges()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimEdge *edge = (*i).second;
        edge->buildNBEdge();
    }
}


void
NIVissimEdge::buildNBEdge()
{
    if(myID==23) {
        int bla = 0;
    }
    NBNode *fromNode, *toNode;
    fromNode = toNode = 0;
    if(myConnectionClusters.size()!=0) {
/*        NBNode *from = new NBNode(
            toString<int>(myID) + "-Begin", 
            myGeom.getBegin().x(), myGeom.getBegin().y());
        NBNode *to = new NBNode(
            toString<int>(myID) + "-End", 
            myGeom.getEnd().x(), myGeom.getEnd().y());
    }
    // otherwise, build a connected edge
    else {*/
        sort(myConnectionClusters.begin(), myConnectionClusters.end(),
            connection_cluster_position_sorter(myID));
        // get or build the from-node
        //  A node may have to be build when the edge starts or ends at
        //  a parking place or something like this
        fromNode = getFromNode();
        // get or build the to-node
        toNode = getToNode();
        // if both nodes are the same, resolve the problem otherwise
        if(fromNode==toNode) {
            std::pair<NBNode*, NBNode*> tmp = resolveSameNode();
            fromNode = tmp.first;
            toNode = tmp.second;
        }
    }
    if(fromNode==0) {
        Position2D pos = myGeom.at(0);
        fromNode =
            new NBNode(toString<int>(myID) + string("-SourceNode"),
                pos.x(), -pos.y(), "no_junction");
        NBNodeCont::insert(fromNode);
    }
    if(toNode==0) {
        Position2D pos = myGeom.at(myGeom.size()-1);
        toNode =
            new NBNode(toString<int>(myID) + string("-DestinationNode"),
                pos.x(), -pos.y(), "no_junction");
        NBNodeCont::insert(toNode);
    }
    // build the edge
    NBEdge *buildEdge = new NBEdge(
        toString<int>(myID), myName, fromNode, toNode, myType,
        50.0/3.6, myNoLanes, myGeom.length(), NBEdge::EDGEFUNCTION_NORMAL);
    NBEdgeCont::insert(buildEdge);
    // check whether the edge contains any other clusters
    if(myConnectionClusters.size()>2) {
        for(ConnectionClusters::iterator j=myConnectionClusters.begin()+1; j!=myConnectionClusters.end()-1; j++) {
            // split the edge at the previously build node
            NBEdgeCont::splitAt(buildEdge, (*j)->getNBNode());
        }
    }
}


NBNode *
NIVissimEdge::getFromNode()
{
    assert(myConnectionClusters.size()!=0);
    return (*(myConnectionClusters.begin()))->getNBNode();
}


NBNode *
NIVissimEdge::getToNode()
{
    assert(myConnectionClusters.size()!=0);
    return (*(myConnectionClusters.end()-1))->getNBNode();
}


std::pair<NBNode*, NBNode*>
NIVissimEdge::resolveSameNode()
{
    // check whether the edge is connected to a district
    //  use it if so 
    NIVissimDistrictConnection *d =
        NIVissimDistrictConnection::dict_findForEdge(myID);
    if(d!=0) {
        double pos = d->getPosition();
        // the district is at the begin of the edge
        if(myGeom.length()-pos>pos) {
            Position2D pos = myGeom.at(0);
            NBNode *node = NBNodeCont::retrieve(pos.x(), -pos.y());
            if(node==0) {
                assert(NBNodeCont::retrieve(pos.x(), pos.y())==0);
                node = new NBNode(
                    toString<int>(myID) + string("DistrictSource"),
                    pos.x(), pos.y());
            }
            NBNodeCont::insert(node);
            return std::pair<NBNode*, NBNode*>(node, getToNode());
        }
        // the district is at the end of the edge
        else {
            Position2D pos = myGeom.at(myGeom.size()-1);
            NBNode *node = NBNodeCont::retrieve(pos.x(), -pos.y());
            if(node==0) {
                assert(NBNodeCont::retrieve(pos.x(), pos.y())==0);
                node = new NBNode(
                    toString<int>(myID) + string("DistrictSink"),
                    pos.x(), pos.y());
            }
            NBNodeCont::insert(node);
            return std::pair<NBNode*, NBNode*>(getFromNode(), node);
        }
    }
    // otherwise, check whether the edge is some kind of 
    //  a dead end...
    // check which end is nearer to the node centre
    if(myConnectionClusters.size()==1) {
        NBNode *node = getFromNode(); // it is the same as getToNode()

        NIVissimConnectionCluster *c = *(myConnectionClusters.begin());
        for(IntVector::iterator i=c->myConnections.begin(); i!=c->myConnections.end(); i++) {
            int bla = *i;
        }
        // no end node given
        if(c->around(myGeom.getBegin()) && !c->around(myGeom.getEnd())) {
            NBNode *end = new NBNode(
                toString<int>(myID) + "-End", 
                myGeom.getEnd().x(), myGeom.getEnd().y());
            NBNodeCont::insert(end);
            return std::pair<NBNode*, NBNode*>(node, end);
        }

        // no begin node given
        if(!c->around(myGeom.getBegin()) && c->around(myGeom.getEnd())) {
            NBNode *beg = new NBNode(
                toString<int>(myID) + "-Begin", 
                myGeom.getBegin().x(), myGeom.getBegin().y());
            NBNodeCont::insert(beg);
            return std::pair<NBNode*, NBNode*>(beg, node);
        }

        // "dummy edge" - both points lie within the same cluster
        if(c->around(myGeom.getBegin()) && c->around(myGeom.getEnd())) {
            return std::pair<NBNode*, NBNode*>(node, node);
        }
    }
    // what to do in other cases?
    throw 1;
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


