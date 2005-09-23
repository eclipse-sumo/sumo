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
// Revision 1.5  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:46:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/11/23 10:25:51  dkrajzew
// debugging
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work; documentation added
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
#include <config.h>
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


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROAbstractRouter
 * The interface for routing the vehicles over the network.
 */
class ROAbstractRouter {
public:
    /**
     * @class ROAbstractEdgeEffortRetriever
     * This interface has to be implemented in order to get the real efforts of edges
     */
    class ROAbstractEdgeEffortRetriever {
    public:
        /// Constructor
        ROAbstractEdgeEffortRetriever() { }

        /// Destructor
		virtual ~ROAbstractEdgeEffortRetriever() { }

		/// This function should return the effort to use
		virtual SUMOReal getEffort(SUMOTime time, const ROEdge * const edge,
            SUMOReal dist) = 0;

		/// This function should return true if the vehicle must not use the given edge
		virtual bool explicetlyOmit(ROEdge *e) const = 0;

		/// Returns the name of this retriever
		virtual const std::string &getID() const = 0;

	};

public:
    /// Constructor
    ROAbstractRouter();

    /// Destructor
    virtual ~ROAbstractRouter();

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual ROEdgeVector compute(ROEdge *from, ROEdge *to,
        SUMOTime time, bool continueOnUnbuild,
		ROAbstractEdgeEffortRetriever * const retriever=0) = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

