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
 *
 */
class ROEdgeVector {
private:
    typedef std::vector<ROEdge*> EdgeVector;
    EdgeVector _edges;
public:
    ROEdgeVector();
    ~ROEdgeVector();
    void add(ROEdge *edge);
    ROEdge *getFirst() const;
    ROEdge *getLast() const;
    std::deque<std::string> getIDs() const;
    double recomputeCosts(long time) const;
    bool equals(const ROEdgeVector &vc) const;
    size_t size() const;
    ROEdgeVector getReverse() const;
    friend std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROEdgeVector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

