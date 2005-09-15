//---------------------------------------------------------------------------//
//                        ROJPHelpers.cpp -
//      A set of helping functions
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2005/09/15 12:05:34  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/02/06 08:43:46  dkrajzew
// new naming applied to the folders (jp-router is now called jtr-router)
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include "ROJPHelpers.h"
#include <router/RONet.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include "ROJPEdge.h"

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
void
ROJPHelpers::parseROJPEdges(RONet &net, std::set<ROJPEdge*> &into,
                            const std::string &chars)
{
	StringTokenizer st(chars, ";");
	while(st.hasNext()) {
		string name = st.next();
		ROJPEdge *edge = static_cast<ROJPEdge*>(net.getEdge(name));
		if(edge==0) {
			MsgHandler::getErrorInstance()->inform(
				string("The edge '") + name + string(" declared as a sink was not found in the network."));
			throw ProcessError();
		}
		into.insert(edge);
	}
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
