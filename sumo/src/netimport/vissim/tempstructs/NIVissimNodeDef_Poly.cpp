/****************************************************************************/
/// @file    NIVissimNodeDef_Poly.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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
#include <map>
#include <cassert>
#include <algorithm>
#include <utils/geom/Position2DVector.h>
#include "NIVissimEdge.h"
#include "NIVissimNodeDef.h"
#include "NIVissimNodeDef_Poly.h"
#include "NIVissimConnection.h"
#include "NIVissimAbstractEdge.h"
#include <utils/geom/Boundary.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS
// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

NIVissimNodeDef_Poly::NIVissimNodeDef_Poly(int id, const std::string &name,
        const Position2DVector &poly)
        : NIVissimNodeDef_Edges(id, name, NIVissimNodeParticipatingEdgeVector()),
        myPoly(poly) {}


NIVissimNodeDef_Poly::~NIVissimNodeDef_Poly() {}


bool
NIVissimNodeDef_Poly::dictionary(int id, const std::string &name,
                                 const Position2DVector &poly) {
    NIVissimNodeDef_Poly *o = new NIVissimNodeDef_Poly(id, name, poly);
    if (!NIVissimNodeDef::dictionary(id, o)) {
        delete o;
        assert(false);
        return false;
    }
    return true;
}


/*
void
NIVissimNodeDef_Poly::computeBounding()
{
    // !!! compute participating edges
    // !!! call this method after loading!
    myBoundary = new Boundary(myPoly.getBoxBoundary());
    assert(myBoundary!=0&&myBoundary->xmax()>=myBoundary->xmin());
}

SUMOReal
NIVissimNodeDef_Poly::getEdgePosition(int edgeid) const
{
    NIVissimEdge *edge = NIVissimEdge::dictionary(edgeid);
    return edge->crossesAtPoint(
        Position2D(myBoundary->xmin(), myBoundary->ymin()),
        Position2D(myBoundary->xmax(), myBoundary->ymax()));
}
*/


void
NIVissimNodeDef_Poly::searchAndSetConnections(SUMOReal offset) {
    IntVector within = NIVissimAbstractEdge::getWithin(myPoly, offset);
    IntVector connections;
    IntVector edges;
    Boundary boundary(myPoly.getBoxBoundary());
    for (IntVector::const_iterator i=within.begin(); i!=within.end(); i++) {
        NIVissimConnection *c =
            NIVissimConnection::dictionary(*i);
        NIVissimEdge *e =
            NIVissimEdge::dictionary(*i);
        if (c!=0) {
            connections.push_back(*i);
            c->setNodeCluster(myID);
        }
        if (e!=0) {
            edges.push_back(*i);
        }
    }
    NIVissimConnectionCluster *c =
        new NIVissimConnectionCluster(connections, boundary, myID, edges);
    for (IntVector::iterator j=edges.begin(); j!=edges.end(); j++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*j);
        edge->myConnectionClusters.push_back(c);
    }
}



/****************************************************************************/

