#ifndef ROJPHelpers_h
#define ROJPHelpers_h
//---------------------------------------------------------------------------//
//                        ROJPHelpers.h -
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

#include <set>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class ROJPEdge;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROJPHelpers
 * Some functions commonly used within the junction-percentage router.
 */
class ROJPHelpers {
public:
    /** @brief Parses the names of given edges as a list of edge names, adds the edges into the container
        It is assumed, the names are divided by a ';' */
	static void parseROJPEdges(RONet &net, std::set<ROJPEdge*> &into,
		const std::string &chars);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


