//---------------------------------------------------------------------------//
//                        NIVissimNodeDef_Poly.cpp -  ccc
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
// Revision 1.6  2003/06/05 11:46:57  dkrajzew
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
NIVissimNodeDef_Poly::searchAndSetConnections(double offset)
{
    IntVector within = NIVissimAbstractEdge::getWithin(myPoly, offset);
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



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimNodeDef_Poly.icc"
//#endif

// Local Variables:
// mode:C++
// End:


