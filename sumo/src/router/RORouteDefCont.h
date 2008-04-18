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
 * @brief A container for route definitions
 *
 * @todo Recheck whether this is really needed; seems to be a NamedObjectCont with no additional functionality (besides a buggy implementation of id reassignment)
 */
class RORouteDefCont : public NamedObjectCont<RORouteDef*>
{
public:
    /// @brief Constructor
    RORouteDefCont() throw();


    /// @brief Destructor
    ~RORouteDefCont() throw();


    /// Adds a named route definition to the container
    bool add(RORouteDef *def) throw();

    /// returns the information whether a route with the given name is known
    bool known(const std::string &name) const throw();

    /// Removes all route definitions from the container
    void clear() throw();

private:
    /// A supplier of further ids
    IDSupplier myIdSupplier;


private:
    /// @brief Invalidated copy constructor
    RORouteDefCont(const RORouteDefCont &src);

    /// @brief Invalidated assignment operator
    RORouteDefCont &operator=(const RORouteDefCont &src);

};


#endif

/****************************************************************************/

