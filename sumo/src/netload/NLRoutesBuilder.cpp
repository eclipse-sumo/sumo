/***************************************************************************
                          NLRoutesBuilder.cpp
			  Container for MSNet::Route-structures during their
			  building
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
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
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.2  2002/10/21 09:52:58  dkrajzew
// support for route multireferencing added
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.5  2002/06/11 14:39:27  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:45  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:10  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include "NLRoutesBuilder.h"
#include <utils/xml/XMLBuildingExceptions.h>
#include "NLNetBuilder.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLRoutesBuilder::NLRoutesBuilder()
{
	m_pActiveRoute = new MSEdgeVector(); // !!! why a pointer
	m_pActiveRoute->reserve(100);
}

NLRoutesBuilder::~NLRoutesBuilder()
{
	delete m_pActiveRoute;
}

void
NLRoutesBuilder::openRoute(const string &id, bool multiReferenced)
{
	m_ActiveId = id;
    m_IsMultiReferenced = multiReferenced;
}

void
NLRoutesBuilder::addEdge(MSEdge *edge)
{
	m_pActiveRoute->push_back(edge);
}

void
NLRoutesBuilder::closeRoute()
{
    int size = m_pActiveRoute->size();
    if(/* NLNetBuilder::check&& */ size==0) {
		throw XMLListEmptyException();
    }
	MSRoute *route = new MSRoute(m_ActiveId, *m_pActiveRoute, m_IsMultiReferenced);
    m_pActiveRoute->clear();
    if(!MSRoute::dictionary(m_ActiveId, route)) {
	    delete route;
	    //if(NLNetBuilder::check)
	        throw XMLIdAlreadyUsedException("route", m_ActiveId);
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLRoutesBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:
