
#include <string>
#include <map>
#include <algorithm>
#include <utils/geom/Boundery.h>
#include "NIVissimNodeParticipatingEdgeVector.h"
#include "NIVissimNodeDef.h"
#include "NIVissimEdge.h"
#include "NIVissimNodeDef_Edges.h"
#include "NIVissimDisturbance.h"
#include "NIVissimConnection.h"

using namespace std;

NIVissimNodeDef_Edges::NIVissimNodeDef_Edges(int id,
        const std::string &name, const NIVissimNodeParticipatingEdgeVector &edges)
    : NIVissimNodeDef(id, name), myEdges(edges)
{
}


NIVissimNodeDef_Edges::~NIVissimNodeDef_Edges()
{
}


bool
NIVissimNodeDef_Edges::dictionary(int id, const std::string &name,
        const NIVissimNodeParticipatingEdgeVector &edges)
{
    NIVissimNodeDef_Edges *o = new NIVissimNodeDef_Edges(id, name, edges);
    if(!NIVissimNodeDef::dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}

/*
bool
NIVissimNodeDef_Edges::trafficLightMatches(
        const NIVissimExtendedEdgePointVector &edges)
{
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        if(find_if(edges.begin(), edges.end(), lying_within_match((*i)))==edges.end()) {
            return false;
        }
    }
    return true;
}


bool
NIVissimNodeDef_Edges::connectionMatches(int fromID, int toID)
{
    return find_if(myEdges.begin(), myEdges.end(), id_matches(fromID))
        !=
        myEdges.end();
}


bool
NIVissimNodeDef_Edges::assignDisturbancesToNode()
{
    myDisturbances = NIVissimDisturbance::tryAssignToNode(myID, myEdges);
    return myDisturbances.size()!=0;
}
*/


void
NIVissimNodeDef_Edges::computeBounding()
{
    Boundery *boundery = new Boundery();
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        NIVissimConnection *c = NIVissimConnection::dictionary(edge->getID());
        if(c!=0) {
            boundery->add(c->getFromGeomPosition());
            boundery->add(c->getToGeomPosition());
        }
            /*
        Position2D p = edge->getFrom2DPosition();
        boundery->add(p.x(), p.y());
        p = edge->getTo2DPosition();
        boundery->add(p.x(), p.y());
        */
    }
    myBoundery = boundery;
}


void
NIVissimNodeDef_Edges::searchAndSetConnections()
{
    if(myID==1189||myID==1190) {
        int bla = 0;
    }
    IntVector connections;
    IntVector edges;
    Boundery boundery;
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        NIVissimConnection *c =
            NIVissimConnection::dictionary(edge->getID());
        if(c!=0) {
            connections.push_back(edge->getID());
            boundery.add(c->getFromGeomPosition());
            boundery.add(c->getToGeomPosition());
            c->setNodeCluster();
        } else {
            edges.push_back(edge->getID());
        }
    }
    NIVissimConnectionCluster *c =
        new NIVissimConnectionCluster(connections, boundery, myID, edges);
    for(IntVector::iterator j=edges.begin(); j!=edges.end(); j++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*j);
        edge->myConnectionClusters.push_back(c);
    }
}




