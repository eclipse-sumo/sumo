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
#include <algorithm>
#include <cassert>
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



void
NIVissimNodeDef_Edges::computeBounding()
{
    assert(myBoundery==0);
    Boundery *boundery = new Boundery();
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        NIVissimConnection *c = NIVissimConnection::dictionary(edge->getID());
        NIVissimEdge *e = NIVissimEdge::dictionary(edge->getID());
        if(c!=0) {
            // both connected edges should be a part of the junction? !!!
            boundery->add(c->getFromGeomPosition());
            boundery->add(c->getToGeomPosition());
        }
        if(e!=0) {
            boundery->add(e->getGeomPosition(edge->getFromPos()));
            boundery->add(e->getGeomPosition(edge->getToPos()));
        }
            /*
        Position2D p = edge->getFrom2DPosition();
        boundery->add(p.x(), p.y());
        p = edge->getTo2DPosition();
        boundery->add(p.x(), p.y());
        */
    }
    myBoundery = boundery;
    assert(myBoundery!=0&&myBoundery->xmax()>=myBoundery->xmin());
}


void
NIVissimNodeDef_Edges::searchAndSetConnections()
{
    IntVector connections;
    IntVector edges;
    Boundery boundery;
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        NIVissimConnection *c =
            NIVissimConnection::dictionary(edge->getID());
        NIVissimEdge *e =
            NIVissimEdge::dictionary(edge->getID());
        if(c!=0) {
            connections.push_back(edge->getID());
            boundery.add(c->getFromGeomPosition());
            boundery.add(c->getToGeomPosition());
            c->setNodeCluster(myID);
        }
        if(e!=0) {
            edges.push_back(edge->getID());
            boundery.add(e->getGeomPosition(edge->getFromPos()));
            boundery.add(e->getGeomPosition(edge->getToPos()));
        }
    }
    NIVissimConnectionCluster *c =
        new NIVissimConnectionCluster(connections, boundery, myID, edges);
    for(IntVector::iterator j=edges.begin(); j!=edges.end(); j++) {
        NIVissimEdge *edge = NIVissimEdge::dictionary(*j);
        edge->myConnectionClusters.push_back(c);
    }
}



double
NIVissimNodeDef_Edges::getEdgePosition(int edgeid) const
{
    for(NIVissimNodeParticipatingEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NIVissimNodeParticipatingEdge *edge = *i;
        if(edge->getID()==edgeid) {
            return (edge->getFromPos() + edge->getToPos()) / 2.0;
        }
    }
    return -1;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimNodeDef_Edges.icc"
//#endif

// Local Variables:
// mode:C++
// End:


