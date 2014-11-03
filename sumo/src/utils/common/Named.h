/****************************************************************************/
/// @file    Named.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Base class for objects which have an id.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <set>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Named
 * @brief Base class for objects which have an id.
 */
class Named {
public:
    /** @brief Constructor
     * @param[in] id The id of the object
     */
    Named(const std::string& id) : myID(id) { }


    /// @brief Destructor
    virtual ~Named() { }


    /** @brief Returns the id
     * @return The stored id
     */
    const std::string& getID() const {
        return myID;
    }


    /** @brief resets the id
     * @param[in] newID The new id of this object
     */
    void setID(const std::string& newID) {
        myID = newID;
    }


    /// @brief Function-object for stable sorting in containers
    struct ComparatorIdLess {
        bool operator()(Named* const a, Named* const b) const {
            return a->getID() < b->getID();
        }
    };

    /// @brief Function-object for stable sorting of objects acting like Named without being derived (SUMOVehicle)
    template <class NamedLike>
    struct NamedLikeComparatorIdLess {
        bool operator()(const NamedLike* const a, const NamedLike* const b) const {
            return a->getID() < b->getID();
        }
    };


    /** @class StoringVisitor
     * @brief Allows to store the object; used as context while traveling the rtree in TraCI
     */
    class StoringVisitor {
    public:
        /// @brief Contructor
        StoringVisitor(std::set<std::string>& ids) : myIDs(ids) {}

        /// @brief Destructor
        ~StoringVisitor() {}

        /// @brief Adds the given object to the container
        void add(const Named* const o) const {
            myIDs.insert(o->getID());
        }

        /// @brief The container
        std::set<std::string>& myIDs;

    private:
        /// @brief invalidated copy constructor
        StoringVisitor(const StoringVisitor& src);

        /// @brief invalidated assignment operator
        StoringVisitor& operator=(const StoringVisitor& src);
    };



    /** @brief Adds this object to the given container
     * @param[in, filled] cont The container to add this item to
     */
    void addTo(const StoringVisitor& cont) const {
        cont.add(this);
    }


protected:
    /// @brief The name of the object
    std::string myID;

};


#endif

/****************************************************************************/

