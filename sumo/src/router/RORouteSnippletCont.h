#ifndef RORouteSnippletCont_h
#define RORouteSnippletCont_h
//---------------------------------------------------------------------------//
//                        RORouteSnippletCont.h -
//  A container with route snipplets for faster computation
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
// Revision 1.9  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.8  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:27  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:35  dkrajzew
// debugging
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <map>
#include "ROEdgeVector.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteSnippletCont
 * A container for route snipplets. Shall allow reusage of snipplets between
 * edges, but is not capable to handle changing network weights, yet.
 */
class RORouteSnippletCont {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

