#include <algorithm>
#include <iostream>
#include <cassert>
#include <utils/geom/Boundery.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/IntVector.h>
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


bool
NIVissimConnectionCluster::NodeSubCluster::overlapsWith(const NIVissimConnectionCluster::NodeSubCluster &c)
{
    return myBoundery.overlapsWith(c.myBoundery);
}







NIVissimConnectionCluster::ContType NIVissimConnectionCluster::myClusters;

NIVissimConnectionCluster::NIVissimConnectionCluster(
        const IntVector &connections, int nodeCluster, int edgeid)
    : myConnections(connections), myNodeCluster(nodeCluster)
{
    recomputeBoundery();
    myClusters.push_back(this);
    myEdges.push_back(edgeid);
    if(myNodeCluster==1189||myNodeCluster==1190) {
        int bla = 0;
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
    if(myNodeCluster==1189||myNodeCluster==1190) {
        int bla = 0;
    }
}


NIVissimConnectionCluster::~NIVissimConnectionCluster()
{
}


bool
NIVissimConnectionCluster::overlapsWith(NIVissimConnectionCluster *c, double offset) const
{
    return c->myBoundery.overlapsWith(myBoundery, offset);
}


void
NIVissimConnectionCluster::add(NIVissimConnectionCluster *c)
{
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
NIVissimConnectionCluster::join()
{
    std::vector<NIVissimConnectionCluster*> joinAble;
    size_t pos = 0;
    ContType::iterator i = myClusters.begin() + pos;
    // step1 - faster but no complete
    while(i!=myClusters.end()) {
        if((*i)->myNodeCluster==1189||(*i)->myNodeCluster==1190) {
            int bla = 0;
        }
        cout << pos << "/" << myClusters.size() << endl;
        joinAble.clear();
        bool restart = false;
        ContType::iterator j = i + 1;
        // check whether every combination has been processed
        while(j!=myClusters.end()) {
            // check whether the current clusters overlap
            if( (*i)->overlapsWith(*j, 5)
                 &&
                 (!(*i)->hasNodeCluster() || !(*j)->hasNodeCluster() ) ) {
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
        } else {
            i++;
            pos++;
        }
    }
    // step2 - slower but complete
    while(i!=myClusters.end()) {
        cout << pos << "/" << myClusters.size() << endl;
        joinAble.clear();
        bool restart = false;
        ContType::iterator j = i + 1;
        // check whether every combination has been processed
        while(j!=myClusters.end()) {
            // check whether the current clusters overlap
            if( (*i)->overlapsWith(*j, 5)
                 &&
                 (!(*i)->hasNodeCluster() || !(*j)->hasNodeCluster() ) ) {
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
        } else {
            i++;
        }
    }
}


void
NIVissimConnectionCluster::buildNodeClusters()
{
    for(ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        IntVector disturbances =
            NIVissimDisturbance::getWithin((*i)->myBoundery);
        //
        IntVector tls = NIVissimTL::getWithin((*i)->myBoundery);
        if(tls.size()>1) {
            throw 1; // !!! eigentlich sollte hier nur eine Ampelanlage sein
        }
        int tlsid = -1;
        if(tls.size()>0) {
            tlsid = tls[0];
        }
        //
        //
        int id = NIVissimNodeCluster::dictionary(
            (*i)->myNodeCluster, tlsid, (*i)->myConnections, disturbances);
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
                int bla = *j;
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
NIVissimConnectionCluster::dict_recheckNodes()
{
    for(ContType::iterator i=myClusters.begin(); i!=myClusters.end(); i++) {
        if((*i)->myNodeCluster==1189||(*i)->myNodeCluster==1190) {
            int bla = 0;
        }
        // get the connections from the cluster
        const IntVector &connections = (*i)->myConnections;
        // recluster
        std::vector<NodeSubCluster> nodeClusters;
        std::vector<NodeSubCluster>::iterator k;
        for(IntVector::const_iterator j=connections.begin(); j!=connections.end(); j++) {
            NIVissimConnection *c1 = NIVissimConnection::dictionary(*j);
            bool found = false;
            for(k=nodeClusters.begin(); k!=nodeClusters.end()&&!found; k++) {
                if(c1->getBoundingBox().overlapsWith((*k).myBoundery, 2.0)) {
                    (*k).add(c1);
                    found = true;
                }
            }
            if(!found) {
                nodeClusters.push_back(NodeSubCluster(c1));
            }
        }
        // recluster cluster
        bool changed = true;
        while(changed) {
            changed = false;
            for(k=nodeClusters.begin(); k!=nodeClusters.end()&&!changed; k++) {
                for(std::vector<NodeSubCluster>::iterator l=k+1; l!=nodeClusters.end()&&!changed; l++) {
                    if((*k).overlapsWith(*l)) {
                        changed = true;
                        (*k).add(*l);
                        nodeClusters.erase(l);
                    }
                }
            }
        }

        // do nothing, when the connections are near together
        if(nodeClusters.size()<=1) {
            continue;
        }
        // Retrieve the largest cluster
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
        // Set all othe connections free
        for(k=nodeClusters.begin(); k!=nodeClusters.end(); k++) {
            (*k).setConnectionsFree();
            (*i)->removeConnections(*k);
        }
        (*i)->recomputeBoundery();
        (*i)->recheckEdges();
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
    if(myNodeCluster==1189||myNodeCluster==1190) {
        int bla = 0;
    }
    for(IntVector::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        NIVissimConnection *c = NIVissimConnection::dictionary(*i);
        if(c!=0) {
            myBoundery.add(c->getFromGeomPosition());
            myBoundery.add(c->getToGeomPosition());
        }
    }
}


NBNode *
NIVissimConnectionCluster::getNBNode() const
{
    return NIVissimNodeCluster::dictionary(myNodeCluster)->getNBNode();
}


bool
NIVissimConnectionCluster::around(const Position2D &p, double offset) const
{
    return myBoundery.around(p, offset);
}



void
NIVissimConnectionCluster::recheckEdges()
{
    assert(myConnections.size()!=0);
    if(myNodeCluster==1189||myNodeCluster==1190) {
        int bla = 0;
    }
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
        if(myBoundery.around(c->getFromGeomPosition(), 5)) {
            myEdges.push_back(c->getFromEdgeID());
        }
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
        return sum / (double) part;
    }
    // use the position of the node if possible
/*
tralala
    if(myNodeCluster>=0) {
        // try to find the nearest point on the edge
        //  !!! only the main geometry is regarded
        NIVissimEdge *edge = NIVissimEdge::dictionary(edgeid);
        NIVissimNodeCluster *node =
            NIVissimNodeCluster::dictionary(myNodeCluster);
        double try1 = GeomHelper::nearest_position_on_line_to_point(
            edge->getBegin2D(), edge->getEnd2D, node->getPos());
        if(try1>=0) {
            return try1;
        }
        double distance
    }
    // what else?
*/
}


