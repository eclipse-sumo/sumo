/****************************************************************************/
/// @file    ROEdgeVector.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vector of edges (a route)
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
#ifndef ROEdgeVector_h
#define ROEdgeVector_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <deque>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROEdgeVector
 * A list of edges (to pass)
 */
class ROEdgeVector
{
public:
    /// Definition of a list of edges
    typedef std::vector<const ROEdge*> EdgeVector;

    /// Constructor
    ROEdgeVector();

    /// Constructor
    ROEdgeVector(size_t toReserve);

    /// Constructor
    ROEdgeVector(const EdgeVector &edges);

    /// Destructor
    ~ROEdgeVector();

    /// Adds a further edge to the end
    void add(const ROEdge *edge);

    /// Returns the first edge
    const ROEdge *getFirst() const;

    /// Returns the last edge
    const ROEdge *getLast() const;

    /// Returns a list of the edge ids in the order of the edges
    std::deque<std::string> getIDs() const;

    /// Computes the costs to pass the edges beginning at the given time
    SUMOReal recomputeCosts(const ROVehicle *const, SUMOTime time) const;

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

    void recheckForLoops();

private:
    /// The list of edges
    EdgeVector _edges;

};


#endif

/****************************************************************************/

