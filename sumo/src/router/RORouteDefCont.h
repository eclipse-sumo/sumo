/****************************************************************************/
/// @file    RORouteDefCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for route definitions
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
#ifndef RORouteDefCont_h
#define RORouteDefCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utils/common/IDSupplier.h>
#include "RORouteDef.h"
#include <utils/common/NamedObjectCont.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteDefCont
 * A container for route definitions
 */
class RORouteDefCont : public NamedObjectCont<RORouteDef*>
{
public:
    /// Constructor
    RORouteDefCont();

    /// Destructor
    ~RORouteDefCont();

    /// Adds a named route definition to the container
    bool add(RORouteDef *def);

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
    typedef std::map<std::pair<const ROEdge*, const ROEdge*>, RORouteDef*> idMap;

    /// Container with routes definitions accessable by their starting and ending point
    idMap myKnown;

    /// A supplier of further ids
    IDSupplier myIdSupplier;

private:
    /// we made the copy constructor invalid
    RORouteDefCont(const RORouteDefCont &src);

    /// we made the assignment operator invalid
    RORouteDefCont &operator=(const RORouteDefCont &src);

};


#endif

/****************************************************************************/

