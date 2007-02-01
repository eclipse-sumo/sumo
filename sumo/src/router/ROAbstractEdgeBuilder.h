/****************************************************************************/
/// @file    ROAbstractEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 21 Jan 2004
/// @version $Id: $
///
// Interface for building edges
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
#ifndef ROAbstractEdgeBuilder_h
#define ROAbstractEdgeBuilder_h
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

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROAbstractEdgeBuilder
 * This interface is used to build instances of router-edges. As the
 *  different routing algorithms may need certain types of edges, edges are
 *  build via a factory object derived from this class.
 */
class ROAbstractEdgeBuilder
{
public:
    /// Constructor
    ROAbstractEdgeBuilder() : myCurrentIndex(0)
    { }

    /// Destructor
    virtual ~ROAbstractEdgeBuilder()
    { }

    /// Builds an edge with the given name
    virtual ROEdge *buildEdge(const std::string &name) = 0;

    size_t getCurrentIndex()
    {
        return myCurrentIndex++;
    }

private:
    size_t myCurrentIndex;

};


#endif

/****************************************************************************/

