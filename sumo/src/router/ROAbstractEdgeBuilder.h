/****************************************************************************/
/// @file    ROAbstractEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 21 Jan 2004
/// @version $Id$
///
// Interface for building instances of router-edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROAbstractEdgeBuilder_h
#define ROAbstractEdgeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RONode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROAbstractEdgeBuilder
 * @brief Interface for building instances of router-edges
 *
 * As the different routing algorithms may need certain types of edges,
 *  edges are build via a factory object derived from this class.
 *
 * The only method to be implemented is "buildEdge" which builds an edge
 *  of the needed ROEdge-subtype.
 *
 * The built edges are numbered in the order they are built, the current
 *  number (index) is stored in "myCurrentIndex" and the next to use may
 *  be obtained via "getNextIndex".
 */
class ROAbstractEdgeBuilder {
public:
    /// @brief Constructor
    ROAbstractEdgeBuilder() throw() : myCurrentIndex(0) { }


    /// @brief Destructor
    virtual ~ROAbstractEdgeBuilder() throw() { }


    /// @name Methods to be implemented
    /// @{

    /** @brief Builds an edge with the given name
     *
     * @param[in] name The name of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @return A proper instance of the named edge
     */
    virtual ROEdge *buildEdge(const std::string &name, RONode *from, RONode *to) throw() = 0;
    /// @}


protected:
    /** @brief Returns the index of the edge to built
     * @return Next valid edge index
     */
    unsigned int getNextIndex() throw() {
        return myCurrentIndex++;
    }


private:
    /// @brief The next edge's index
    unsigned int myCurrentIndex;


private:
    /// @brief Invalidated copy constructor
    ROAbstractEdgeBuilder(const ROAbstractEdgeBuilder &src);

    /// @brief Invalidated assignment operator
    ROAbstractEdgeBuilder &operator=(const ROAbstractEdgeBuilder &src);

};


#endif

/****************************************************************************/

