#include <string>
#include <map>
#include <cassert>
#include <algorithm>
#include <utils/geom/Position2DVector.h>
#include "NIVissimEdge.h"
#include "NIVissimNodeDef.h"
#include "NIVissimNodeDef_Poly.h"
#include "NIVissimConnection.h"
#include "NIVissimAbstractEdge.h"
#include <utils/geom/Boundery.h>

using namespace std;

NIVissimNodeDef_Poly::NIVissimNodeDef_Poly(int id, const std::string &name,
                                           const Position2DVector &poly)
    : NIVissimNodeDef_Edges(id, name, NIVissimNodeParticipatingEdgeVector()),
    myPoly(poly)
{
}


NIVissimNodeDef_Poly::~NIVissimNodeDef_Poly()
{
}


bool
NIVissimNodeDef_Poly::dictionary(int id, const std::string &name,
                                 const Position2DVector &poly)
{
    NIVissimNodeDef_Poly *o = new NIVissimNodeDef_Poly(id, name, poly);
    if(!NIVissimNodeDef::dictionary(id, o)) {
        delete o;
        assert(false);
        return false;
    }
    return true;
}

/*
bool
NIVissimNodeDef_Poly::matches(const IntVector &edges)
{
    for(IntVector::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        NIVissimAbstractEdge *edge = NIVissimAbstractEdge::dictionary(*i);
        if(!edge->crosses(myPoly)) {
            return false;
        }
        int edgeid = edge->getID();
        std::pair<double, double> positions = edge->getCrossingRange(myPoly);
        NIVissimNodeParticipatingEdge *desc = new NIVissimNodeParticipatingEdge(edgeid, positions.first, positions.second);
        myEdges.push_back(desc);
    }
    return true;
}


*/



void
NIVissimNodeDef_Poly::computeBounding()
{
    // !!! compute participating edges
    // !!! call this method after loading!
    myBoundery = new Boundery(myPoly.getBoxBoundery());
    assert(myBoundery!=0&&myBoundery->xmax()>=myBoundery->xmin());
}


double
NIVissimNodeDef_Poly::getEdgePosition(int edgeid) const
{
    NIVissimEdge *edge = NIVissimEdge::dictionary(edgeid);
    return edge->crossesAtPoint(
        Position2D(myBoundery->xmin(), myBoundery->ymin()),
        Position2D(myBoundery->xmax(), myBoundery->ymax()));
}


void
NIVissimNodeDef_Poly::searchAndSetConnections()
{
    IntVector within = NIVissimAbstractEdge::getWithin(myPoly, 5.0);
    IntVector connections;
    IntVector edges;
    Boundery boundery(myPoly.getBoxBoundery());
    for(IntVector::const_iterator i=within.begin(); i!=within.end(); i++) {
        NIVissimConnection *c =
            NIVissimConnection::dictionary(*i);
        NIVissimEdge *e =
            NIVissimEdge::dictionary(*i);
        if(c!=0) {
            connections.push_back(*i);
            c->setNodeCluster(myID);
        }
        if(e!=0) {
            edges.push_back(*i);
        }
    }
    NIVissimConnectionCluster *c =
        new NIVissimConnectionCluster(connections, boundery, myID, edges);
    for(IntVector::iterator j=edges.begin(); j!=edges.end(); j++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*j);
        edge->myConnectionClusters.push_back(c);
    }
}

