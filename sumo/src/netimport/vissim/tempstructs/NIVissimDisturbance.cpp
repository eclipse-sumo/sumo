#include <map>
#include <string>
#include <iostream>
#include <utils/convert/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundery.h>
#include "NIVissimConnection.h"
#include "NIVissimNodeDef.h"
#include "NIVissimDisturbance.h"
#include "NIVissimNodeParticipatingEdgeVector.h"

using namespace std;

NIVissimDisturbance::DictType NIVissimDisturbance::myDict;
int NIVissimDisturbance::myRunningID = 0;


NIVissimDisturbance::NIVissimDisturbance(int id,
                                         const std::string &name,
                                         const NIVissimExtendedEdgePoint &edge,
                                         const NIVissimExtendedEdgePoint &by,
                                         double timegap, double waygap,
                                         double vmax)
    : myID(id), myName(name), myEdge(edge), myDisturbance(by),
    myTimeGap(timegap), myWayGap(waygap), myVMax(vmax)
{
}


NIVissimDisturbance::~NIVissimDisturbance()
{
}
/*

bool
NIVissimDisturbance::tryAssignToNodeSingle(int nodeid,
        const NIVissimNodeParticipatingEdgeVector &edges)
{
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=edges.begin();
            i!=edges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        if( edge->getID()==myEdge.getEdgeID() &&
            edge->positionLiesWithin(myEdge.getPosition()) &&
            edge->getID()==myDisturbance.getEdgeID() &&
            edge->positionLiesWithin(myDisturbance.getPosition())) {
            myNodeID = nodeid;
            return true;
        }
        if( edge->getID()==myEdge.getEdgeID() &&
            edge->positionLiesWithin(myEdge.getPosition())) {
            cout << "Only the edge lies within the node!!!" << endl;
            cout << "  Querverkehrsstörung-ID: " << myID << endl;
            return true;
        }
        if( edge->getID()==myDisturbance.getEdgeID() &&
            edge->positionLiesWithin(myDisturbance.getPosition())) {
            cout << "Only the disturbing edge lies within the node!!!" << endl;
            cout << "  Querverkehrsstörung-ID: " << myID << endl;
            return true;
        }
    }
    return false;
}*/



bool
NIVissimDisturbance::dictionary(int id,
                                const std::string &name,
                                const NIVissimExtendedEdgePoint &edge,
                                const NIVissimExtendedEdgePoint &by,
                                double timegap, double waygap, double vmax)
{
    int nid = id;
    if(id<0) {
        nid = myRunningID++;
    }
    bool added = false;
    while(true) {
        NIVissimDisturbance *o =
            new NIVissimDisturbance(nid, name, edge, by,
                timegap, waygap, vmax);
        if(!dictionary(nid, o)) {
            delete o;
            nid = myRunningID++;
        } else {
            return true;
        }
    }
}


bool
NIVissimDisturbance::dictionary(int id, NIVissimDisturbance *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimDisturbance *
NIVissimDisturbance::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

/*
void
NIVissimDisturbance::buildNodeClusters()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimDisturbance *e = (*i).second;
        if(!e->clustered()) {
            IntVector disturbances = NIVissimDisturbance::getWithin(*(e->myBoundery));
            IntVector connections = NIVissimConnection::getWithin(*(e->myBoundery));
            int id = NIVissimNodeCluster::dictionary(-1, -1, connections,
                disturbances);
        }
    }
}
*/


/*
IntVector
NIVissimDisturbance::tryAssignToNode(int nodeid,
        const NIVissimNodeParticipatingEdgeVector &edges)
{
    IntVector assigned;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->tryAssignToNodeSingle(nodeid, edges)) {
            assigned.push_back((*i).second->myID);
        }
    }
    return assigned;
}


IntVector
NIVissimDisturbance::getDisturbatorsForEdge(int edgeid)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->myEdge.getEdgeID() == edgeid) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}

IntVector
NIVissimDisturbance::getDisturbtionsForEdge(int edgeid)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->myDisturbance.getEdgeID() == edgeid) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}
*/

IntVector
NIVissimDisturbance::getWithin(const AbstractPoly &poly)
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
NIVissimDisturbance::computeBounding()
{
    Boundery *bound = new Boundery();
    bound->add(myEdge.getGeomPosition());
    bound->add(myDisturbance.getGeomPosition());
    myBoundery = bound;
}







