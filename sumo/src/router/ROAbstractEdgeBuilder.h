#ifndef ROAbstractEdgeBuilder_h
#define ROAbstractEdgeBuilder_h
//---------------------------------------------------------------------------//
//                        ROAbstractEdgeBuilder.h -
//      Interface for building edges
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 21 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.4  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
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


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROAbstractEdgeBuilder
 * This interface is used to build instances of router-edges. As the
 *  different routing algorithms may need certain types of edges, edges are
 *  build via a factory object derived from this class.
 */
class ROAbstractEdgeBuilder {
public:
    /// Constructor
    ROAbstractEdgeBuilder() : myCurrentIndex(0) { }

    /// Destructor
    virtual ~ROAbstractEdgeBuilder() { }

    /// Builds an edge with the given name
    virtual ROEdge *buildEdge(const std::string &name) = 0;

    size_t getCurrentIndex() { return myCurrentIndex++; }

private:
    size_t myCurrentIndex;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
