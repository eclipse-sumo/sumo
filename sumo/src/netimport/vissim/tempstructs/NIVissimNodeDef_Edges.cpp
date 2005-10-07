//---------------------------------------------------------------------------//
//                        NIVissimNodeDef_Edges.cpp -  ccc
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
// Revision 1.11  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.8  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.7  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.6  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H



#include <string>
#include <map>
#include <algorithm>
#include <cassert>
#include <utils/geom/Boundary.h>
#include "NIVissimNodeParticipatingEdgeVector.h"
#include "NIVissimNodeDef.h"
#include "NIVissimEdge.h"
#include "NIVissimNodeDef_Edges.h"
#include "NIVissimDisturbance.h"
#include "NIVissimConnection.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

NIVissimNodeDef_Edges::NIVissimNodeDef_Edges(int id,
        const std::string &name, const NIVissimNodeParticipatingEdgeVector &edges)
    : NIVissimNodeDef(id, name), myEdges(edges)
{
}


NIVissimNodeDef_Edges::~NIVissimNodeDef_Edges()
{
    for(NIVissimNodeParticipatingEdgeVector::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        delete (*i);
    }
    myEdges.clear();
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
void
NIVissimNodeDef_Edges::computeBounding()
{
    assert(myBoundary==0);
    Boundary *boundary = new Boundary();
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        NIVissimConnection *c = NIVissimConnection::dictionary(edge->getID());
        NIVissimEdge *e = NIVissimEdge::dictionary(edge->getID());
        if(c!=0) {
            // both connected edges should be a part of the junction? !!!
            boundary->add(c->getFromGeomPosition());
            boundary->add(c->getToGeomPosition());
        }
        if(e!=0) {
            boundary->add(e->getGeomPosition(edge->getFromPos()));
            boundary->add(e->getGeomPosition(edge->getToPos()));
        }
    }
    myBoundary = boundary;
    assert(myBoundary!=0&&myBoundary->xmax()>=myBoundary->xmin());
}
*/

void
NIVissimNodeDef_Edges::searchAndSetConnections()
{
    IntVector connections;
    IntVector edges;
    Boundary boundary;
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        NIVissimConnection *c =
            NIVissimConnection::dictionary(edge->getID());
        NIVissimEdge *e =
            NIVissimEdge::dictionary(edge->getID());
        if(c!=0) {
            connections.push_back(edge->getID());
            boundary.add(c->getFromGeomPosition());
            boundary.add(c->getToGeomPosition());
            c->setNodeCluster(myID);
        }
        if(e!=0) {
            edges.push_back(edge->getID());
            boundary.add(e->getGeomPosition(edge->getFromPos()));
            boundary.add(e->getGeomPosition(edge->getToPos()));
        }
    }
    NIVissimConnectionCluster *c =
        new NIVissimConnectionCluster(connections, boundary, myID, edges);
    for(IntVector::iterator j=edges.begin(); j!=edges.end(); j++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*j);
        edge->myConnectionClusters.push_back(c);
    }
}



SUMOReal
NIVissimNodeDef_Edges::getEdgePosition(int edgeid) const
{
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        if(edge->getID()==edgeid) {
            return (edge->getFromPos() + edge->getToPos()) / (SUMOReal) 2.0;
        }
    }
    return -1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


