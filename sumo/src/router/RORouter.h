#ifndef RORouter_h
#define RORouter_h
//---------------------------------------------------------------------------//
//                        RORouter.h -
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
// Revision 1.4  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could not be computed or not; not very sphisticated, in fact
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <functional>
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
 * @class RORouter
 * A router over edges.
 */
class RORouter {
public:
    /// Constructor
    RORouter(RONet &net, ROEdgeCont *source);

    /// Destructor
    ~RORouter();

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    ROEdgeVector compute(ROEdge *from, ROEdge *to,
        long time, bool continueOnUnbuild);

private:
    /// Class to compare (and so sort) nodes by their effort
    class NodeByEffortComperator : public std::less<ROEdge*> {
    public:
        /// Constructor
        explicit NodeByEffortComperator() { }

        /// Destructor
        ~NodeByEffortComperator() { }

        /// Comparing method
        bool operator()(ROEdge *nod1, ROEdge *nod2) const {
            return nod1->getEffort()>nod2->getEffort();
        }
    };

    /// Computes the route using Dijkstra's algorithm
    ROEdgeVector dijkstraCompute(ROEdge *from, ROEdge *to,
        long time, bool continueOnUnbuild);

    /// Builds the path from marked edges
    ROEdgeVector buildPathFrom(ROEdge *rbegin);

private:
    /// The network to use
    RONet &_net;

    /// The container of routes; used within the Dijkstra-initialisation
    ROEdgeCont *_source;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RORouter.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

