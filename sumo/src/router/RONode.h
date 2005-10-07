#ifndef RONode_h
#define RONode_h
//---------------------------------------------------------------------------//
//                        RONode.h -
//  A single router's node
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
// $Log$
// Revision 1.5  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
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
#include "ROEdgeVector.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RONode
 * Not used yet, but kept as later on, nodes may be handled as vehicle
 *  decelerators that do affect the travelling speed.
 */
class RONode {
public:
    /// Constructor
    RONode(const std::string &id, const ROEdgeVector &edges);

    /// Destructor
    ~RONode();

private:
    /// The list of edges that do participate in this junction
    ROEdgeVector _edges;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

