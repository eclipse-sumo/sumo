#include <map>
#include <algorithm>
#include <utils/common/IntVector.h>
#include <utils/convert/ToString.h>
#include <utils/geom/Position2DVector.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NIVissimDisturbance.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeCluster.h"

using namespace std;

NIVissimNodeCluster::DictType NIVissimNodeCluster::myDict;
int NIVissimNodeCluster::myCurrentID = 1;



NIVissimNodeCluster::NIVissimNodeCluster(int id, int nodeid, int tlid,
                                         const IntVector &connectors,
                                         const IntVector &disturbances)
    : myID(id), myNodeID(nodeid), myTLID(tlid),
    myConnectors(connectors), myDisturbances(disturbances),
    myNBNode(0)
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
    return false;
}


int
NIVissimNodeCluster::dictionary(int nodeid, int tlid,
                                const IntVector &connectors,
                                const IntVector &disturbances)
{
    int id = myCurrentID++;
    if(nodeid>0) {
        id = nodeid;
        myCurrentID = nodeid + 1;
    }
    NIVissimNodeCluster *o = new NIVissimNodeCluster(id,
        nodeid, tlid, connectors, disturbances);
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



void
NIVissimNodeCluster::assignToEdges()
{ // !!!!
/*    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimNodeCluster *c = (*i).second;
        NIVissimEdgePosMap edges = c->getParticipatingEdgePositions();
        for(NIVissimEdgePosMap::iterator j=edges.begin(); j!=edges.end(); j++) {
            NIVissimAbstractEdge *e =
                NIVissimAbstractEdge::dictionary((*j).first);
            e->addNodeClusterInformation(c->getID(), (*j).second);
        }
    }
*/
}


 // !!!!
/*
NIVissimEdgePosMap
NIVissimNodeCluster::getParticipatingEdgePositions()
{
    NIVissimEdgePosMap ret;
    // add positions from node
    if(myNodeID>0) {
        ret.join(NIVissimNodeDef::getParticipatingEdgePositions(myNodeID));
    }
    // add positions from the traffic light
    if(myTLID>0) {
        ret.join(NIVissimTL::getParticipatingEdgePositions(myTLID));
    }
    // add positions from the connectors
    IntVector::iterator i;
    for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
        ret.join(NIVissimConnection::getParticipatingEdgePositions(*i));
    }
    // add positions from the disturbances
    IntVector::iterator i;
    for(i=myDisturbances.begin(); i!=myDisturbances.end(); i++) {
        ret.join(NIVissimDisturbance::getParticipatingEdgePositions(*i));
    }
    return ret;
}

*/

/*
void
NIVissimNodeCluster::container_computePositions()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimNodeCluster *c = (*i).second;
    }
}
*/

void
NIVissimNodeCluster::buildNBNode()
{
    if(myConnectors.size()==0) {
        return; // !!! Check, when this is the case
    }
    // compute the position
        // compute the places the connections cross
    Position2DVector crossings;
    IntVector::iterator i, j;
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
        // alternative way:
            // compute via positions of crossings
    if(crossings.size()==0) {
        for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
            NIVissimConnection *c1 = NIVissimConnection::dictionary(*i);
            crossings.push_back(c1->getFromGeomPosition());
            crossings.push_back(c1->getToGeomPosition());
        }
    }
        // compute the position
    Position2D pos = crossings.center();

    // build the node
    // !!!
    cout << toString<int>(myID) << ": ";
    for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
        if(i!=myConnectors.begin()) {
            cout << ", ";
        }
        cout << (*i);
    }
    cout << "- - - - - - - - - - " << endl;
    // !!!
    NBNode *node = new NBNode(toString<int>(myID), pos.x(), -pos.y());
    NBNodeCont::insert(node);
    myNBNode = node;
}

/*
void
NIVissimNodeCluster::addNodesEdges()
{
    NBNode *node = NBNodeCont::retrieve(toString<int>(myID));
    // add edges
    IntVector tmpEdges;
    for(IntVector::iterator i=myConnectors.begin(); i!=myConnectors.end(); i++) {
        NIVissimConnection *c1 = NIVissimConnection::dictionary(*i);
        int from = c1->getFromEdgeID();
        NBEdge *fromEdge = NBEdgeCont::retrieve(toString<int>(from));
        if(find(tmpEdges.begin(), tmpEdges.end(), from)==tmpEdges.end()) {
            node->addIncomingEdge(fromEdge);
            tmpEdges.push_back(from);
        }
        int to = c1->getToEdgeID();
        NBEdge *toEdge = NBEdgeCont::retrieve(toString<int>(to));
        if(find(tmpEdges.begin(), tmpEdges.end(), to)==tmpEdges.end()) {
            node->addIncomingEdge(toEdge);
            tmpEdges.push_back(to);
        }
    }
}
*/

void
NIVissimNodeCluster::buildNBNodes()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->buildNBNode();
    }
}



bool
NIVissimNodeCluster::recheckEdgeChanges()
{
    // do nothing when there is only one way - it is a junction
    if(myConnectors.size()<=1) {
        return false;
    }
    // We will now try to remove all connections which are not a part of the
    //  junction's logic. We hope, these are those edges, which do lay within
    //  the junction (due to overlapping), but have only one connected, following
    //  edge which has no other predecessors. Further, none of both edges
    //  must be blocked by any of the other within the junction
    IntVector::iterator i, j;
    IntVector connections2Keep;
    NIVissimConnection *c1, *c2;
    int from1, from2, to1, to2;
    // try to find all connections which shall be kept
    for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
        c1 = NIVissimConnection::dictionary(*i);
        from1 = c1->getFromEdgeID();
        to1 = c1->getToEdgeID();
        bool found = false;
        for(j=i+1; j!=myConnectors.end(); j++) {
            c2 = NIVissimConnection::dictionary(*j);
            from2 = c2->getFromEdgeID();
            to2 = c2->getToEdgeID();
            if(from1==from2 && to1!=to2) {
                if(find(connections2Keep.begin(), connections2Keep.end(), *j)==connections2Keep.end()) {
                    connections2Keep.push_back(*j);
                    found = true;
                }
            }
        }
        if(found) {
            if(find(connections2Keep.begin(), connections2Keep.end(), *i)==connections2Keep.end()) {
                connections2Keep.push_back(*i);
            }
        }
    }
    // return when all connections have distinct outgoing and incoming edges
    if(connections2Keep.size()==myConnectors.size()) {
        return false;
    }
    // it is also possible, that a street is connected with another
    //  using multiple connectors. In this case, we assume it
    //  to be a single junction, too
    c1 = NIVissimConnection::dictionary(*(myConnectors.begin()));
    from1 = c1->getFromEdgeID();
    to1 = c1->getToEdgeID();
    bool allEdgesSame = true;
    for(i=myConnectors.begin()+1; i!=myConnectors.end()&&allEdgesSame; i++) {
        c2 = NIVissimConnection::dictionary(*i);
        if(c2->getFromEdgeID()!=from1 || c2->getToEdgeID()!=to1) {
            allEdgesSame = false;
        }
    }
    if(allEdgesSame) {
        return false;
    }
    // remove the others, building new node clusters around them
    for(i=myConnectors.begin(); i!=myConnectors.end(); i++) {
        j = find(connections2Keep.begin(), connections2Keep.end(), *i);
        if(j!=connections2Keep.end()) {
            // go to the next connection if the current shall be kept
            continue;
        }
        // check whether othe connections do connect the same edges
        c1 = NIVissimConnection::dictionary(*i);
/*
        if(c1->interactsWith(myDisturbances)) {
            // keep the connection if it interacts with any of the node's
            //  disturbances
            continue;
        }
        */
        IntVector allFromThisEdge;
        allFromThisEdge.push_back(*i);
        for(j=i+1; j!=myConnectors.end(); j++) {
            c2 = NIVissimConnection::dictionary(*j);
            if( c1->getFromEdgeID()==c2->getFromEdgeID()
                &&
                c1->getToEdgeID()==c2->getToEdgeID() )
            {
                allFromThisEdge.push_back(*j);
            }
        }
        // by now, we assume such connections are not
        //  disturbed by anything
        NIVissimNodeCluster::dictionary(-1, -1,
            allFromThisEdge, IntVector());
    }
    myConnectors = connections2Keep;
    return true;
}


void
NIVissimNodeCluster::dict_recheckEdgeChanges()
{
    return;
    /*
    DictType::iterator i;
    do {
        i=myDict.begin();
        size_t bla = myDict.size();
        size_t posbla = 0;
        while(i!=myDict.end()&&!(*i).second->recheckEdgeChanges()) {
            posbla++;
            i++;
        }
    } while(i!=myDict.end());
    */
}

/*
void
NIVissimNodeCluster::dict_addNodesEdges()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->addNodesEdges();
    }
}
*/

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
                    cout << "DoubleNode:" << ret << endl;
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
        NBNode *node = NBNodeCont::retrieve(toString<int>((*i).first));
        for(IntVector::const_iterator j=disturbances.begin(); j!=disturbances.end(); j++) {
            NIVissimDisturbance *disturbance = NIVissimDisturbance::dictionary(*j);
            disturbance->addToNode(node);
        }
    }
}


void 
NIVissimNodeCluster::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}
