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
// Revision 1.3  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <deque>

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
    /// Constructor
    ROEdgeVector();

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
    double recomputeCosts(long time) const;

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

    /// Output operator
    friend std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev);
private:

    /// Definition of a list of edges
    typedef std::vector<ROEdge*> EdgeVector;

    /// The list of edges
    EdgeVector _edges;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROEdgeVector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

