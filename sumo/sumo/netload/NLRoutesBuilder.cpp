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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
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
#include "../microsim/MSNet.h"
#include "../microsim/MSEdge.h"
#include "NLRoutesBuilder.h"
#include "../utils/XMLBuildingExceptions.h"
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
  m_pActiveRoute = new MSNet::Route();
  m_pActiveRoute->reserve(100);
}

NLRoutesBuilder::~NLRoutesBuilder() 
{
  delete m_pActiveRoute;
}

void 
NLRoutesBuilder::openRoute(string id) 
{
  m_ActiveId = id;
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
    if(NLNetBuilder::check && size==0) throw XMLListEmptyException();
    MSNet::Route *route = new MSNet::Route();
    route->reserve(size);
    for(MSNet::Route::iterator i1=m_pActiveRoute->begin(); i1!=m_pActiveRoute->end(); i1++)
	route->push_back(*i1);
    m_pActiveRoute->clear();
    if(!MSNet::routeDict(m_ActiveId, route)) {
	delete route;
	if(NLNetBuilder::check)
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
