#include <string>
#include <map>
#include <cassert>
#include <algorithm>
#include <utils/geom/Position2DVector.h>
#include "NIVissimEdge.h"
#include "NIVissimNodeDef.h"
#include "NIVissimNodeDef_Poly.h"
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
    assert(myBoundery!=0&&myBoundery->xmax()>myBoundery->xmin());
}


double
NIVissimNodeDef_Poly::getEdgePosition(int edgeid) const
{
    return -1;
}
