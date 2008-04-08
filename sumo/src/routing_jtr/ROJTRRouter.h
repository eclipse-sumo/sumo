/****************************************************************************/
/// @file    ROJTRRouter.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// The junction-percentage router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROJTRRouter_h
#define ROJTRRouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOAbstractRouter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class ROEdge;
class ROJTREdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTRRouter
 * Lays the given route over the edges using the dijkstra algorithm
 */
class ROJTRRouter : public SUMOAbstractRouter<ROEdge,ROVehicle>
{
public:
    /// Constructor
    ROJTRRouter(RONet &net, bool unbuildIsWarningOnly,
                bool acceptAllDestinations);

    /// Destructor
    ~ROJTRRouter();

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    void compute(const ROEdge *from, const ROEdge *to, const ROVehicle * const vehicle,
                 SUMOTime time, std::vector<const ROEdge*> &into);

private:
    /// The network to use
    RONet &myNet;

    /// The maximum number of edges a route may have
    int myMaxEdges;

    bool myUnbuildIsWarningOnly;

    bool myAcceptAllDestination;

    bool myIgnoreClasses;

};


#endif

/****************************************************************************/

