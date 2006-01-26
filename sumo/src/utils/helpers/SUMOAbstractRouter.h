#ifndef SUMOAbstractRouter_h
#define SUMOAbstractRouter_h
//---------------------------------------------------------------------------//
//                        SUMOAbstractRouter.h -
//  The dijkstra-router
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 25.Jan 2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/01/26 08:39:57  dkrajzew
// made the abstract router usable within microsim and routers
//
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

#include <string>
#include <vector>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SUMOAbstractRouter
 * The interface for routing the vehicles over the network.
 */
template<class E, class V>
class SUMOAbstractRouter {
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
		virtual SUMOReal getEffort(SUMOTime time, const E * const edge,
            SUMOReal dist) = 0;

		/// Returns the name of this retriever
		virtual const std::string &getID() const = 0;

	};

public:
    /// Constructor
    SUMOAbstractRouter() { }

    /// Destructor
    virtual ~SUMOAbstractRouter() { }

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual void compute(const E *from, const E *to, const V * const vehicle,
        SUMOTime time, std::vector<const E*> &into) = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

