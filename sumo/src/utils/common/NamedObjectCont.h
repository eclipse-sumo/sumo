/****************************************************************************/
/// @file    NamedObjectCont.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A map of named object pointers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NamedObjectCont_h
#define NamedObjectCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <vector>
#include <algorithm>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NamedObjectCont
 * @brief A map of named object pointers
 *
 * An associative storage (map) for objects (pointers to them to be exact),
 *  which do have a name. In order to get the stored objects as a list,
 *  each insertion/deletion sets the internal state value "myHaveChanged"
 *  to true, indicating the list must be rebuild.
 */
template<class T>
class NamedObjectCont {
public:
    /// @brief Definition of the key to pointer map type
    typedef std::map< std::string, T > IDMap;

    /// @brief Constructor
    NamedObjectCont() : myHaveChanged(false) { }


    ///@brief  Destructor
    virtual ~NamedObjectCont() {
        for (typename IDMap::iterator i = myMap.begin(); i != myMap.end(); i++) {
            delete(*i).second;
        }
    }


    /** @brief Adds an item
     *
     * If another item with the same name is already known, false is reported
     *  and the item is not added.
     *
     * @param[in] id The id of the item to add
     * @param[in] item The item to add
     * @return If the item could been added (no item with the same id was within the container before)
     */
    virtual bool add(const std::string& id, T item) {
        if (myMap.find(id) != myMap.end()) {
            return false;
        }
        myMap.insert(std::make_pair(id, item));
        myHaveChanged = true;
        return true;
    }


    /** @brief Removes an item
     * @param[in] id The id of the item to remove
     * @return If the item could been removed (an item with the id was within the container before)
     */
    virtual bool remove(const std::string& id) {
        if (myMap.find(id) == myMap.end()) {
            return false;
        }
        typename std::map<std::string, T>::iterator i = myMap.find(id);
        delete i->second;
        myMap.erase(i);
        myHaveChanged = true;
        return true;
    }


    /** @brief Retrieves an item
     *
     * Returns 0 when no item with the given id is stored within the container
     *
     * @param[in] id The id of the item to retrieve
     * @return The item stored under the given id, or 0 if no such item exists
     */
    T get(const std::string& id) const {
        typename std::map<std::string, T>::const_iterator i = myMap.find(id);
        if (i == myMap.end()) {
            return 0;
        }
        return (*i).second;
    }


    /** @brief Removes all items from the container (deletes them, too) */
    void clear() {
        for (typename IDMap::iterator i = myMap.begin(); i != myMap.end(); i++) {
            delete(*i).second;
        }
        myMap.clear();
        myVector.clear();
        myHaveChanged = true;
    }


    /** @brief Returns the number of items within the container
     *
     * @return The number of stored items
     */
    unsigned int size() const {
        return (unsigned int) myMap.size();
    }


    /** @brief Removes the named item from the container
     *
     * If the named object exists, it is deleted, the key is
     *  removed from the map, and true is returned. If the id was not
     *  known, false is returned.
     *
     * @param[in] id The id of the item to delete
     * @return Whether the object could be deleted (was within the map)
     */
    bool erase(const std::string& id) {
        typename IDMap::iterator i = myMap.find(id);
        if (i == myMap.end()) {
            return false;
        }
        T o = (*i).second;
        myMap.erase(i);
        // and from the vector
        typename ObjectVector::iterator i2 =
            find(myVector.begin(), myVector.end(), o);
        myHaveChanged = true;
        if (i2 != myVector.end()) {
            myVector.erase(i2);
        }
        delete o;
        return true;
    }


    /* @brief Returns the reference to a vector that contains all objects.
     *
     * This method returns the reference to a vector which is stored within
     *  this class and contains all known objects stored within the map.
     * This vector is rebuild in prior if "myHaveChanged" indicates
     *  a change has taken place.
     *
     * @return Reference to a saved vector of objects within the map
     */
    const std::vector<T>& buildAndGetStaticVector() const {
        if (myHaveChanged) {
            myVector.clear();
            typename IDMap::const_iterator i;
            for (i = myMap.begin(); i != myMap.end(); ++i) {
                myVector.push_back((*i).second);
            }
            myHaveChanged = false;
        }
        return myVector;
    }


    /* @brief Returns a vector that contains all objects.
     *
     * This method builds and returns a vector which contains all known
     *  objects stored within the map.
     *
     * @return A vector of objects within the map
     */
    std::vector<T> getTempVector() const {
        std::vector<T> ret;
        typename IDMap::const_iterator i;
        for (i = myMap.begin(); i != myMap.end(); ++i) {
            ret.push_back((*i).second);
        }
        return ret;
    }


    /* @brief Fills the given vector with the stored objects' ids
     * @param[in] into The container to fill
     */
    void insertIDs(std::vector<std::string>& into) const {
        typename IDMap::const_iterator i;
        for (i = myMap.begin(); i != myMap.end(); ++i) {
            into.push_back((*i).first);
        }
    }


    /* @brief Returns a reference to the internal map
     *
     * @return A reference to the internal map
     */
    const IDMap& getMyMap() const {
        return myMap;
    }


private:
    /// @brief Definition of the container type iterator
    typedef typename IDMap::iterator myContIt;

    /// @brief The map from key to object
    IDMap myMap;

    /// @brief Definition objects vector
    typedef std::vector<T> ObjectVector;

    /// @brief The stored vector of all known items
    mutable ObjectVector myVector;

    /// @brief Information whether the vector is out of sync with the map
    mutable bool myHaveChanged;

};


#endif

/****************************************************************************/

