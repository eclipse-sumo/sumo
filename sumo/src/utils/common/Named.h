/****************************************************************************/
/// @file    Named.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Base class for objects which do have an id.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
    Named(const std::string& id) : myID(id) { }


    /// @brief Destructor
    virtual ~Named() { }


    /** @brief Returns the id
     *
     * @return The stored id
     */
    const std::string& getID() const {
        return myID;
    }


    /// @brief resets the id
    void setID(const std::string& newID) {
        myID = newID;
    }


    /** Function-object for stable sorting in containers. */
    struct ComparatorIdLess {
        bool operator()(Named* const a, Named* const b) {
            return (a->getID() < b->getID());
        }
    };



protected:
    /// @brief The name of the object
    std::string myID;

};


#endif

/****************************************************************************/

