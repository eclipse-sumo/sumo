#include <iostream> // !!! debug
#include "NIVissimNodeDef.h"
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimTL.h"

using namespace std;

NIVissimNodeDef::DictType NIVissimNodeDef::myDict;

NIVissimNodeDef::NIVissimNodeDef(int id, const std::string &name)
    : myID(id), myName(name)
{
}


NIVissimNodeDef::~NIVissimNodeDef()
{
}


bool
NIVissimNodeDef::dictionary(int id, NIVissimNodeDef *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
//        o->computeBounding();
        return true;
    }
    return false;
}


NIVissimNodeDef *
NIVissimNodeDef::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

/*
int
NIVissimNodeDef::searchAndSetMatchingTLParent(int tlID,
        const NIVissimExtendedEdgePointVector &edges)
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->trafficLightMatches(edges)) {
            (*i).second->setTL(tlID);
            return (*i).second->myID;
        }
    }
    return -1;
}


void
NIVissimNodeDef::setTL(int tlID)
{
    myTLID = tlID;
}



int
NIVissimNodeDef::searchAndSetMatchingConnectionParent(int connID,
                                                      int fromEdge,
                                                      int toEdge)
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->connectionMatches(fromEdge, toEdge)) {
            (*i).second->addConnection(connID);
            return (*i).second->myID;
        }
    }
    return -1;
}


void
NIVissimNodeDef::addConnection(int connID)
{
    myConnections.push_back(connID);
}


void
NIVissimNodeDef::assignDisturbances()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->assignDisturbancesToNode();
    }
}

*/


void
NIVissimNodeDef::buildNodeClusters()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        int cluster = (*i).second->buildNodeCluster();
    }
}


int
NIVissimNodeDef::buildNodeCluster()
{
    bool changed = true;
    int id = -1;
    while(changed) {
        IntVector connectors = NIVissimConnection::getWithin(*myBoundery);
        IntVector disturbances = NIVissimDisturbance::getWithin(*myBoundery);
        IntVector tls = NIVissimTL::getWithin(*myBoundery);
        if(tls.size()>1) {
            cout << "NIVissimNodeDef: more than a single signal" << endl;
            throw 1;
        }
        int tlid = -1;
        if(tls.size()!=0) {
            tlid = tls[0];
        }
        id = NIVissimNodeCluster::dictionary(myID, tlid, connectors,
            disturbances);
        IntVector::iterator i;
        for(i=connectors.begin(); i!=connectors.end(); i++) {
            NIVissimConnection::dictionary(*i)->inCluster(id);
        }
        for(i=disturbances.begin(); i!=disturbances.end(); i++) {
            NIVissimDisturbance::dictionary(*i)->inCluster(id);
        }
    }
    return id;
}



IntVector
NIVissimNodeDef::getWithin(const AbstractPoly &p)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimNodeDef *d = (*i).second;
        if(d->partialWithin(p)) {
            ret.push_back((*i).first);
        }
    }
    return ret;
}


bool
NIVissimNodeDef::partialWithin(const AbstractPoly &p) const
{
    return myBoundery->partialWithin(p);
}


void
NIVissimNodeDef::dict_assignConnectionsToNodes()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->searchAndSetConnections();
    }
}


size_t
NIVissimNodeDef::dictSize()
{
    return myDict.size();
}



void
NIVissimNodeDef::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}
