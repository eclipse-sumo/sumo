#ifndef RORouteDefCont_h
#define RORouteDefCont_h
//---------------------------------------------------------------------------//
//                        RORouteDefCont.h -
//  A container for route definitions
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
// Revision 1.8  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.7  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.6  2003/04/15 09:09:18  dkrajzew
// documentation added
//
// Revision 1.5  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/03/03 15:22:35  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utils/router/IDSupplier.h>
#include "RORouteDef.h"
#include <utils/common/NamedObjectCont.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteDefCont
 * A container for route definitions
 */
class RORouteDefCont : public NamedObjectCont<RORouteDef*> {
public:
    /// Constructor
    RORouteDefCont();

    /// Destructor
    ~RORouteDefCont();

    /// Adds a named route definition to the container
    bool add( RORouteDef *def);

    /// Writes the known routes into the given file using the XML-format
    static void writeXML(std::ostream &os);

    /// returns the information whether a route with the given name is known
    bool known(const std::string &name) const;

    /// Returns the information whether a route connecting the given edges is known
    bool known(ROEdge *from, ROEdge *to) const;

    /// returns the id of the route that connects the given edges (if known)
    std::string getID(ROEdge *from, ROEdge *to) const;

    /// Removes all route definitions from the container
    void clear();

private:
    /// Definitions of a container of route definitions from their start eand end points to the route definition
    typedef std::map<std::pair<ROEdge*, ROEdge*>, RORouteDef*> idMap;

    /// Container with routes definitions accessable by their starting and ending point
    idMap _known;

    /// A supplier of further ids
    IDSupplier _idSupplier;

private:
    /// we made the copy constructor invalid
    RORouteDefCont(const RORouteDefCont &src);

    /// we made the assignment operator invalid
    RORouteDefCont &operator=(const RORouteDefCont &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

