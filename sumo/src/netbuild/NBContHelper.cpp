/***************************************************************************
                          NBContHelper.cpp
              Some methods for traversing lists of edges
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.20  2005/10/17 09:02:44  dkrajzew
// got rid of the old MSVC memory leak checker; memory leaks removed
//
// Revision 1.19  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.18  2005/09/23 06:01:05  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.17  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.16  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.15  2004/11/23 10:21:40  dkrajzew
// debugging
//
// Revision 1.14  2004/08/02 13:11:39  dkrajzew
// made some deprovements or so
//
// Revision 1.13  2004/07/02 09:32:26  dkrajzew
// mapping of joined edges names added; removal of edges with a too low
//  speed added
//
// Revision 1.12  2004/02/16 13:58:22  dkrajzew
// some further work on edge geometry
//
// Revision 1.11  2003/11/11 08:33:54  dkrajzew
// consequent position2D instead of two SUMOReals added
//
// Revision 1.10  2003/09/22 12:40:11  dkrajzew
// further work on vissim-import
//
// Revision 1.9  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.8  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.7  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.6  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
// Revision 1.5  2003/04/04 07:43:03  dkrajzew
// Yellow phases must be now explicetely given; comments added;
//  order of edge sorting (false lane connections) debugged
//
// Revision 1.4  2003/03/17 14:22:32  dkrajzew
// further debug and windows eol removed
//
// Revision 1.3  2003/03/12 16:47:52  dkrajzew
// extension for artemis-import
//
// Revision 1.2  2003/02/07 10:43:43  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.4  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on
//  the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:54  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:21:24  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
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

#include <vector>
#include <map>
#include <cassert>
#include "NBContHelper.h"
#include <utils/geom/GeomHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * utility methods
 * ----------------------------------------------------------------------- */
void
NBContHelper::nextCW(const EdgeVector * edges, EdgeVector::const_iterator &from)
{
    from++;
    if(from==edges->end()) {
        from = edges->begin();
    }
}


void
NBContHelper::nextCCW(const EdgeVector * edges, EdgeVector::const_iterator &from)
{
    if(from==edges->begin()) {
        from = edges->end() - 1;
    } else {
        --from;
    }
}


int
NBContHelper::countPriorities(const EdgeVector &s)
{
    if(s.size()==0)
        return 0;
    map<int, int> knownPrios;
    for(EdgeVector::const_iterator i=s.begin(); i!=s.end(); i++) {
        knownPrios.insert(map<int, int>::value_type((*i)->getPriority(), 0));
    }
    return knownPrios.size();
}


std::ostream &
NBContHelper::out(std::ostream &os, const std::vector<bool> &v)
{
    for(std::vector<bool>::const_iterator i=v.begin(); i!=v.end(); i++) {
        os << *i;
    }
    return os;
}


NBEdge *
NBContHelper::findConnectingEdge(const EdgeVector &edges,
                                 NBNode *from, NBNode *to)
{
    for(EdgeVector::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        if((*i)->getToNode()==to && (*i)->getFromNode()==from) {
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
    for(EdgeVector::const_iterator i=ev.begin()+1; i!=ev.end(); i++) {
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
NBContHelper::edge_by_junction_angle_sorter::operator() (NBEdge *e1, NBEdge *e2) const
{
    return getConvAngle(e1) < getConvAngle(e2);
}



SUMOReal
NBContHelper::edge_by_junction_angle_sorter::getConvAngle(NBEdge *e) const
{

    SUMOReal angle;
    // convert angle if the edge is an outgoing edge
    if(e->getFromNode()==_node) {
        angle = e->getNormedAngle(*_node);
        angle += 180;
        if(angle>=360) {
            angle -= 360;
        }
    } else {
        angle = e->getNormedAngle(*_node);
    }
    assert(angle>=0&&angle<360);
    return angle;
}



/* -------------------------------------------------------------------------
 * methods from node_with_incoming_finder
 * ----------------------------------------------------------------------- */
NBContHelper::node_with_incoming_finder::node_with_incoming_finder(NBEdge *e)
    : _edge(e)
{
}


bool
NBContHelper::node_with_incoming_finder::operator() (const NBNode * const n) const
{
    const EdgeVector &incoming = n->getIncomingEdges();
    return std::find(incoming.begin(), incoming.end(), _edge)!=incoming.end();
}



/* -------------------------------------------------------------------------
 * methods from node_with_outgoing_finder
 * ----------------------------------------------------------------------- */
NBContHelper::node_with_outgoing_finder::node_with_outgoing_finder(NBEdge *e)
    : _edge(e)
{
}


bool
NBContHelper::node_with_outgoing_finder::operator() (const NBNode * const n) const
{
    const EdgeVector &outgoing = n->getOutgoingEdges();
    return std::find(outgoing.begin(), outgoing.end(), _edge)!=outgoing.end();
}



/* -------------------------------------------------------------------------
 * methods from node_with_incoming_finder
 * ----------------------------------------------------------------------- */
NBContHelper::edgelane_finder::edgelane_finder(NBEdge *toEdge, int toLane)
    : myDestinationEdge(toEdge), myDestinationLane(toLane)
{
}


bool
NBContHelper::edgelane_finder::operator() (const EdgeLane &el) const
{
    return el.edge==myDestinationEdge
        &&
        ((int) el.lane ==  myDestinationLane || myDestinationLane<0);
}





NBContHelper::edge_with_destination_finder::edge_with_destination_finder(NBNode *dest)
    : myDestinationNode(dest)
{
}


bool
NBContHelper::edge_with_destination_finder::operator() (NBEdge *e) const
{
    return e->getToNode()==myDestinationNode;
}


std::ostream &
operator<<(std::ostream &os, const EdgeVector &ev)
{
    for(EdgeVector::const_iterator i=ev.begin(); i!=ev.end(); i++) {
        if(i!=ev.begin()) {
            os << ", ";
        }
        os << (*i)->getID();
    }
    return os;
}


NBContHelper::edge_to_lane_sorter::edge_to_lane_sorter(NBNode *from, NBNode *to)
{
    flip = atan2(
        from->getPosition().x()-to->getPosition().x(),
        from->getPosition().y()-to->getPosition().y())<0;
}


int
NBContHelper::edge_to_lane_sorter::operator() (NBEdge *e1, NBEdge *e2) const {
    Position2D p = e1->getGeometry().center();
    if(flip) {
        return 0<GeomHelper::DistancePointLine(p,
            e2->getGeometry().getBegin(), e2->getGeometry().getEnd());
    } else {
        return 0>GeomHelper::DistancePointLine(p,
            e2->getGeometry().getBegin(), e2->getGeometry().getEnd());
    }
}


SUMOReal
NBContHelper::getMaxSpeed(const EdgeVector &edges)
{
    if(edges.size()==0) {
        return -1;
    }
    SUMOReal ret = (*(edges.begin()))->getSpeed();
    for(EdgeVector::const_iterator i=edges.begin()+1; i!=edges.end(); i++) {
        if((*i)->getSpeed()>ret) {
            ret = (*i)->getSpeed();
        }
    }
    return ret;
}


SUMOReal
NBContHelper::getMinSpeed(const EdgeVector &edges)
{
    if(edges.size()==0) {
        return -1;
    }
    SUMOReal ret = (*(edges.begin()))->getSpeed();
    for(EdgeVector::const_iterator i=edges.begin()+1; i!=edges.end(); i++) {
        if((*i)->getSpeed()<ret) {
            ret = (*i)->getSpeed();
        }
    }
    return ret;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

