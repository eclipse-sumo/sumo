/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Named.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Base class for objects which have an id.
/****************************************************************************/
#ifndef Named_h
#define Named_h


// ===========================================================================
// included modules
// ===========================================================================

#include <iostream>
#include <string>
#include <set>


/// @brief Function-object for stable sorting of objects acting like Named without being derived (SUMOVehicle)
// @note Numbers of different lengths will not be ordered by alphanumerical sorting
struct ComparatorIdLess {
    template<class T>
    bool operator()(const T* const a, const T* const b) const {
        return a->getID() < b->getID();
    }
};


/// @brief Function-object for stable sorting of objects with numerical ids
struct ComparatorNumericalIdLess {
    template<class T>
    bool operator()(const T* const a, const T* const b) const {
        return a->getNumericalID() < b->getNumericalID();
    }
};


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

    /// @brief get an identifier for Named-like object which may be Null
    template<class T>
    static std::string getIDSecure(const T* obj, const std::string& fallBack = "NULL") {
        return obj == 0 ? fallBack : obj->getID();
    }

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


    /** @class StoringVisitor
     * @brief Allows to store the object; used as context while traveling the rtree in TraCI
     */
    class StoringVisitor {
    public:
        /// @brief Contructor
        StoringVisitor(std::set<const Named*>& objects) : myIDs(nullptr), myObjects(&objects) {}
        StoringVisitor(std::set<std::string>& objects) : myIDs(&objects), myObjects(nullptr) {}

        /// @brief Destructor
        ~StoringVisitor() {}

        /// @brief Adds the given object to the container
        void add(const Named* const o) const {
            if (myObjects == nullptr) {
                myIDs->insert(o->getID());
            } else {
                myObjects->insert(o);
            }
        }

        /// @brief The container
        std::set<std::string>* myIDs;
        std::set<const Named*>* myObjects;

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

