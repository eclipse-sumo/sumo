/****************************************************************************/
/// @file    Named.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for objects which do have an id.
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
#ifndef Named_h
#define Named_h


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
// class definitions
// ===========================================================================
/**
 * @class Named
 * @brief Base class for objects which do have an id.
 */
class Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the object
     */
    Named(const std::string &id) throw() : myID(id) { }


    /// @brief Destructor
    virtual ~Named() throw() { }


    /** @brief Returns the id
     *
     * @return The stored id
     */
    const std::string &getID() const throw() {
        return myID;
    }


protected:
    /// @brief The name of the object
    std::string myID;

};


#endif

/****************************************************************************/

