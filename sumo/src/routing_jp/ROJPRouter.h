#ifndef ROJPRouter_h
#define ROJPRouter_h
//---------------------------------------------------------------------------//
//                        ROJPRouter.h -
//      The junction-percentage router
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
// Revision 1.3  2004/01/28 14:19:20  dkrajzew
// allowed to specify the maximum edge number in a route by a factor
//
// Revision 1.2  2004/01/26 09:58:15  dkrajzew
// sinks are now simply marked as these instead of the usage of a further container
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <router/ROAbstractRouter.h>
#include <router/ROEdgeVector.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class ROEdge;
class ROJPEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROJPRouter
 * Lays the given route over the edges using the dijkstra algorithm
 */
class ROJPRouter : public ROAbstractRouter {
public:
    /// Constructor
    ROJPRouter(RONet &net);

    /// Destructor
    ~ROJPRouter();

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    ROEdgeVector compute(ROEdge *from, ROEdge *to,
        long time, bool continueOnUnbuild);

private:
    /// Performs the computation
    ROEdgeVector jpCompute(ROJPEdge *from, long time, bool continueOnUnbuild);


private:
    /// The network to use
    RONet &myNet;

    /// The maximum number of edges a route may have
    int myMaxEdges;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

