#ifndef ROAbstractRouter_h
#define ROAbstractRouter_h
//---------------------------------------------------------------------------//
//                        ROAbstractRouter.h -
//  The dijkstra-router
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
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// --------------------------------------------
// Revision 1.4  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <utils/common/InstancePool.h>
#include "ROEdgeVector.h"
#include "ROEdge.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class ROEdgeCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROAbstractRouter
 * The interface for routing the vehicles over the network.
 */
class ROAbstractRouter {
public:
    /// Constructor
    ROAbstractRouter();

    /// Destructor
    virtual ~ROAbstractRouter();

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual ROEdgeVector compute(ROEdge *from, ROEdge *to,
        long time, bool continueOnUnbuild) = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

