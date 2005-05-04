#ifndef ROEdgeVector_h
#define ROEdgeVector_h
//---------------------------------------------------------------------------//
//                        ROEdgeVector.h -
//  A vector of edges (a route)
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
// Revision 1.8  2005/05/04 08:46:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for
//  an online-routing
//
// Revision 1.6  2004/04/02 11:25:34  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added
//
// Revision 1.5  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further
//  changes in order to make both new routers work; documentation added
//
// Revision 1.4  2003/04/10 15:47:01  dkrajzew
// random routes are now being prunned to avoid some stress with turning
//  vehicles
//
// Revision 1.3  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <deque>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROEdgeVector
 * A list of edges (to pass)
 */
class ROEdgeVector {
public:
    /// Definition of a list of edges
    typedef std::vector<ROEdge*> EdgeVector;

    /// Constructor
    ROEdgeVector();

    /// Constructor
    ROEdgeVector(size_t toReserve);

    /// Constructor
    ROEdgeVector(EdgeVector &edges);

    /// Destructor
    ~ROEdgeVector();

    /// Adds a further edge to the end
    void add(ROEdge *edge);

    /// Returns the first edge
    ROEdge *getFirst() const;

    /// Returns the last edge
    ROEdge *getLast() const;

    /// Returns a list of the edge ids in the order of the edges
    std::deque<std::string> getIDs() const;

    /// Computes the costs to pass the edges beginning at the given time
    double recomputeCosts(SUMOTime time) const;

    /** @brief Returns the infomration whether both lists are equal
        @deprecated (Should be a comparison operator */
    bool equals(const ROEdgeVector &vc) const;

    /// Returns the number of edges within the list
    size_t size() const;

    /// Empties the list
    void clear();

    /** @brief Returns the inverse list
        A list which starts at the last element of this list and ends with the first one is returned */
    ROEdgeVector getReverse() const;

    /** @brief removes the first and the last edge */
    void removeEnds();

    /** @brief removes the first edge */
    void removeFirst();

    /// Output operator
    friend std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev);

    const EdgeVector &getEdges() const;

private:
    /// The list of edges
    EdgeVector _edges;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

