/****************************************************************************/
/// @file    NBContHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Some methods for traversing lists of edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <map>
#include <cassert>
#include "NBContHelper.h"
#include <utils/geom/GeomHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * utility methods
 * ----------------------------------------------------------------------- */
void
NBContHelper::nextCW(const EdgeVector * edges, EdgeVector::const_iterator &from)
{
    from++;
    if (from==edges->end()) {
        from = edges->begin();
    }
}


void
NBContHelper::nextCCW(const EdgeVector * edges, EdgeVector::const_iterator &from)
{
    if (from==edges->begin()) {
        from = edges->end() - 1;
    } else {
        --from;
    }
}


std::ostream &
NBContHelper::out(std::ostream &os, const std::vector<bool> &v)
{
    for (std::vector<bool>::const_iterator i=v.begin(); i!=v.end(); i++) {
        os << *i;
    }
    return os;
}


NBEdge *
NBContHelper::findConnectingEdge(const EdgeVector &edges,
                                 NBNode *from, NBNode *to)
{
    for (EdgeVector::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        if ((*i)->getToNode()==to && (*i)->getFromNode()==from) {
            return *i;
        }
    }
    return 0;
}



SUMOReal
NBContHelper::maxSpeed(const EdgeVector &ev)
{
    assert(ev.size()>0);
    SUMOReal max = (*(ev.begin()))->getSpeed();
    for (EdgeVector::const_iterator i=ev.begin()+1; i!=ev.end(); i++) {
        max =
            max > (*i)->getSpeed()
            ? max : (*i)->getSpeed();
    }
    return max;
}



/* -------------------------------------------------------------------------
 * methods from edge_by_junction_angle_sorter
 * ----------------------------------------------------------------------- */
int
NBContHelper::edge_by_junction_angle_sorter::operator()(NBEdge *e1, NBEdge *e2) const
{
    return getConvAngle(e1) < getConvAngle(e2);
}



SUMOReal
NBContHelper::edge_by_junction_angle_sorter::getConvAngle(NBEdge *e) const
{

    SUMOReal angle;
    // convert angle if the edge is an outgoing edge
    if (e->getFromNode()==myNode) {
        angle = e->getNormedAngle(*myNode);
        angle += (SUMOReal) 180.;
        if (angle>=(SUMOReal) 360.) {
            angle -= (SUMOReal) 360.;
        }
    } else {
        angle = e->getNormedAngle(*myNode);
    }
    if (angle<0.1||angle>359.9) {
        angle = (SUMOReal) 0.;
    }
    assert(angle>=(SUMOReal)0 && angle<(SUMOReal)360);
    return angle;
}



/* -------------------------------------------------------------------------
 * methods from node_with_incoming_finder
 * ----------------------------------------------------------------------- */
NBContHelper::node_with_incoming_finder::node_with_incoming_finder(NBEdge *e)
        : myEdge(e)
{}


bool
NBContHelper::node_with_incoming_finder::operator()(const NBNode * const n) const
{
    const EdgeVector &incoming = n->getIncomingEdges();
    return std::find(incoming.begin(), incoming.end(), myEdge)!=incoming.end();
}



/* -------------------------------------------------------------------------
 * methods from node_with_outgoing_finder
 * ----------------------------------------------------------------------- */
NBContHelper::node_with_outgoing_finder::node_with_outgoing_finder(NBEdge *e)
        : myEdge(e)
{}


bool
NBContHelper::node_with_outgoing_finder::operator()(const NBNode * const n) const
{
    const EdgeVector &outgoing = n->getOutgoingEdges();
    return std::find(outgoing.begin(), outgoing.end(), myEdge)!=outgoing.end();
}



/* -------------------------------------------------------------------------
 * methods from !!!
 * ----------------------------------------------------------------------- */
NBContHelper::edge_with_destination_finder::edge_with_destination_finder(NBNode *dest)
        : myDestinationNode(dest)
{}


bool
NBContHelper::edge_with_destination_finder::operator()(NBEdge *e) const
{
    return e->getToNode()==myDestinationNode;
}


std::ostream &
operator<<(std::ostream &os, const EdgeVector &ev)
{
    for (EdgeVector::const_iterator i=ev.begin(); i!=ev.end(); i++) {
        if (i!=ev.begin()) {
            os << ", ";
        }
        os << (*i)->getID();
    }
    return os;
}




SUMOReal
NBContHelper::getMaxSpeed(const EdgeVector &edges)
{
    if (edges.size()==0) {
        return -1;
    }
    SUMOReal ret = (*(edges.begin()))->getSpeed();
    for (EdgeVector::const_iterator i=edges.begin()+1; i!=edges.end(); i++) {
        if ((*i)->getSpeed()>ret) {
            ret = (*i)->getSpeed();
        }
    }
    return ret;
}


SUMOReal
NBContHelper::getMinSpeed(const EdgeVector &edges)
{
    if (edges.size()==0) {
        return -1;
    }
    SUMOReal ret = (*(edges.begin()))->getSpeed();
    for (EdgeVector::const_iterator i=edges.begin()+1; i!=edges.end(); i++) {
        if ((*i)->getSpeed()<ret) {
            ret = (*i)->getSpeed();
        }
    }
    return ret;
}



/****************************************************************************/

