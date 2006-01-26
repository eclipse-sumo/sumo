#ifndef ROJTRRouter_h
#define ROJTRRouter_h
//---------------------------------------------------------------------------//
//                        ROJTRRouter.h -
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
// Revision 1.2  2006/01/26 08:47:17  dkrajzew
// adapted the new router API
//
// Revision 1.1  2005/10/10 12:09:36  dkrajzew
// renamed ROJP*-classes to ROJTR*
//
// Revision 1.5  2005/10/07 11:42:39  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:05:34  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/11/23 10:26:59  dkrajzew
// debugging
//
// Revision 1.1  2004/02/06 08:43:46  dkrajzew
// new naming applied to the folders (jp-router is now called jtr-router)
//
// Revision 1.3  2004/01/28 14:19:20  dkrajzew
// allowed to specify the maximum edge number in a route by a factor
//
// Revision 1.2  2004/01/26 09:58:15  dkrajzew
// sinks are now simply marked as these instead of the usage of a further
//  container
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <router/ROAbstractRouter.h>
#include <router/ROEdgeVector.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class ROEdge;
class ROJTREdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROJTRRouter
 * Lays the given route over the edges using the dijkstra algorithm
 */
class ROJTRRouter : public ROAbstractRouter {
public:
    /// Constructor
    ROJTRRouter(RONet &net, bool unbuildIsWarningOnly);

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

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

