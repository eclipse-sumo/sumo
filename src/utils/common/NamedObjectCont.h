/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
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

    ///@brief Destructor
    virtual ~NamedObjectCont() {
        // iterate over all elements to delete it
        for (auto i : myMap) {
            delete i.second;
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
     * @param[in] del delete item after removing of container
     * @return If the item could been removed (an item with the id was within the container before)
     */
    virtual bool remove(const std::string& id) {
        auto it = myMap.find(id);
        if (it == myMap.end()) {
            return false;
        } else {
            delete it->second;
            myMap.erase(it);
            myHaveChanged = true;
            return true;
        }
    }

    /** @brief Retrieves an item
     *
     * Returns 0 when no item with the given id is stored within the container
     *
     * @param[in] id The id of the item to retrieve
     * @return The item stored under the given id, or 0 if no such item exists
     */
    T get(const std::string& id) const {
        auto it = myMap.find(id);
        if (it == myMap.end()) {
            return 0;
        } else {
            return it->second;
        }
    }

    /// @brief Removes all items from the container (deletes them, too)
    void clear() {
        for (auto i : myMap) {
            delete i.second;
        }
        myMap.clear();
        myVector.clear();
        myHaveChanged = true;
    }

    /// @brief Returns the number of stored items within the container
    int size() const {
        return (int) myMap.size();
    }

    /** @brief Removes the named item from the container
     *
     * If the named object exists, it is deleted, the key is
     *  removed from the map, and true is returned. If the id was not
     *  known, false is returned.
     *
     * @param[in] id The id of the item to delete
     * @param[in] deleteObject delete object after removing it from container
     * @return Whether the object could be deleted (was within the map)
     */
    bool erase(const std::string& id, bool deleteObject = true) {
        auto i = myMap.find(id);
        if (i == myMap.end()) {
            return false;
        } else {
            T o = i->second;
            myMap.erase(i);
            // and from the vector
            typename ObjectVector::iterator i2 =
                find(myVector.begin(), myVector.end(), o);
            myHaveChanged = true;
            if (i2 != myVector.end()) {
                myVector.erase(i2);
            }
            if(deleteObject) {
                delete o;
            }
            return true;
        }
    }

    /* @brief Fills the given vector with the stored objects' ids
     * @param[in] into The container to fill
     */
    void insertIDs(std::vector<std::string>& into) const {
        for (auto i : myMap) {
            into.push_back(i.first);
        }
    }

    /// @brief change ID of a stored object
    bool changeID(const std::string& oldId, const std::string& newId) {
        auto i = myMap.find(oldId);
        if (i == myMap.end()) {
            return false;
        } else {
            // save Item, remove it from Map, and insert it again with the new ID
            T item = i->second;
            myMap.erase(i);
            myMap.insert(std::make_pair(newId, item));
            return true;
        }
    }

    /// @brief Returns a reference to the internal map
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

