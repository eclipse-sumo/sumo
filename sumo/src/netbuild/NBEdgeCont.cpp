/***************************************************************************
                          NBEdgeCont.cpp
			  A container for all of the nets edges
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.6  2002/07/25 08:31:42  dkrajzew
// Report methods transfered from loader to the containers
//
// Revision 1.5  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
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
// Revision 1.1  2001/12/06 13:38:00  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include "NBEdgeCont.h"
#include "NBNodeCont.h"
#include "NBHelpers.h"
#include <cmath>
#include "NBTypeCont.h"
#include <iostream>
#include <strstream>

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
 * static members
 * ======================================================================= */
NBEdgeCont::EdgeCont NBEdgeCont::_edges;

/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
NBEdgeCont::insert(NBEdge *edge)
{
    EdgeCont::iterator i = _edges.find(edge->getID());
    if(i!=_edges.end()) return false;
    _edges.insert(EdgeCont::value_type(edge->getID(), edge));
    return true;
}


NBEdge *
NBEdgeCont::retrieve(const string &id)
{
    EdgeCont::iterator i = _edges.find(id);
    if(i==_edges.end()) return 0;
    return (*i).second;
}



bool 
NBEdgeCont::computeTurningDirections(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeTurningDirections();
    }
    return true;
}


bool
NBEdgeCont::sortOutgoingLanesConnections(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->sortOutgoingLanesConnections();
    }
    return true;
}

bool
NBEdgeCont::computeEdge2Edges()
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeEdge2Edges();
    }
    return true;
}


bool
NBEdgeCont::recheckLanes(bool verbose) {
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->recheckLanes(verbose);
    }
    return true;
}

bool
NBEdgeCont::computeLinkPriorities(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeLinkPriorities();
    }
    return true;
}

bool
NBEdgeCont::appendTurnarounds(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->appendTurnaround();
    }
    return true;
}


void
NBEdgeCont::writeXMLEdgeList(ostream &into)
{
    into << "   <edges no=\"" << _edges.size() << "\">";
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        if(i!=_edges.begin()) {
            into << ' ';
        }
        into << (*i).first;
    }
    into << "</edges>" << endl;
}


void
NBEdgeCont::writeXMLStep1(std::ostream &into)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep1(into);
    }
    into << endl;
}


void
NBEdgeCont::writeXMLStep2(std::ostream &into)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep2(into);
    }
    into << endl;
}


void
NBEdgeCont::writeXMLStep3(std::ostream &into)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep3(into);
    }
    into << endl;
}


int NBEdgeCont::size() {
    return _edges.size();
}

int
NBEdgeCont::getNo() {
    return _edges.size();
}

void
NBEdgeCont::clear() {
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++)
        delete((*i).second);
    _edges.clear();
}

void
NBEdgeCont::report(bool verbose)
{
    if(verbose) {
        cout << "   " << getNo() << " edges loaded." << endl;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBEdgeCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:

