/****************************************************************************/
/// @file    RORouteSnippletCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container with route snipplets for faster computation
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
#ifndef RORouteSnippletCont_h
#define RORouteSnippletCont_h
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

#include <map>
#include "ROEdgeVector.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteSnippletCont
 * A container for route snipplets. Shall allow reusage of snipplets between
 * edges, but is not capable to handle changing network weights, yet.
 */
class RORouteSnippletCont
{
public:
    /// Constructor
    RORouteSnippletCont();

    /// Destructor
    ~RORouteSnippletCont();

    /// Adds a snipplet
    bool add(const ROEdgeVector &item);

    /// Retrieves a snipplet
    const ROEdgeVector &get(ROEdge *from, ROEdge *to) const;

    /// Returns the information whether the specified snipplet is known
    bool knows(ROEdge *from, ROEdge *to) const;

private:
    /// Definition of a snipplet container
    typedef std::map<std::pair<const ROEdge*, const ROEdge*>, ROEdgeVector > MapType;

    /// The snipplet container
    MapType _known;

private:
    /// we made the copy constructor invalid
    RORouteSnippletCont(const RORouteSnippletCont &src);

    /// we made the assignment operator invalid
    RORouteSnippletCont &operator=(const RORouteSnippletCont &src);
};


#endif

/****************************************************************************/

