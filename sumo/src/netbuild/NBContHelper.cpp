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
// Revision 1.9  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.8  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.7  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.6  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
// Revision 1.5  2003/04/04 07:43:03  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
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
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:54  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor double to int conversions removed;
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <map>
#include <cassert>
#include "NBContHelper.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


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



double
NBContHelper::maxSpeed(const EdgeVector &ev)
{
    assert(ev.size()>0);
    double max = (*(ev.begin()))->getSpeed();
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



double
NBContHelper::edge_by_junction_angle_sorter::getConvAngle(NBEdge *e) const
{
    double angle;
    const Position2DVector &p = e->getGeometry();
    // convert angle if the edge is an outgoing edge
    if(e->getFromNode()==_node) {
        angle =
            atan2(
                (p.at(1).x()-p.at(0).x()),
                (p.at(1).y()-p.at(0).y()))*180.0/3.14159265;
        if(angle<0) {
            angle = 360 + angle;
        }
    } else {
        angle =
            atan2(
                (p.at(p.size()-2).x()-p.at(p.size()-1).x()),
                (p.at(p.size()-2).y()-p.at(p.size()-1).y()))*180.0/3.14159265;
        if(angle<0) {
            angle = 360 + angle;
        }
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBContHelper.icc"
//#endif

// Local Variables:
// mode:C++
// End:

