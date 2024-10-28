/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIGlObjectStorage.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Oct 2002
///
// A storage for displayed objects via their numerical id
/****************************************************************************/
#pragma once
#include <config.h>

#include <map>
#include <string>
#include <set>
#include <utils/foxtools/fxheader.h>
#include "GUIGlObject.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGlObjectStorage
 * @brief A storage for of displayed objects via their numerical id
 *
 * This is a container for GUIGlObject - objects, which may be displayed
 *  and due to this may generate tooltips or be grapped in other ways.
 *
 * As in case of vehicles (other, later implemented objects may have this
 *  property, too) they may be deleted by the simulation while being accessed
 *  - for example using a property window or something like that - this
 *  container posesses three storages: one containing all objects that are not
 *  accessed at all, one for objects currently accessed and one for objects that
 *  are accessed but shall be deleted.
 */
class GUIGlObjectStorage {
public:
    /// @brief Constructor
    GUIGlObjectStorage();


    /// @brief Destructor
    ~GUIGlObjectStorage();


    /** @brief Registers an object
     *
     * This is done within the constructor of the GUIGlObject.
     * The next free id is calculated as well.
     *
     * @param[in] object The object to register
     */
    GUIGlID registerObject(GUIGlObject* object);

    void changeName(GUIGlObject* object, const std::string& fullName);

    /** @brief Returns the object from the container locking it
     *
     * The lock prevents the object from being deleted while it is accessed.
     *
     * @param[in] id The id of the object to return
     * @return The object with the given id or nullptr if no such object is known
     */
    GUIGlObject* getObjectBlocking(GUIGlID id) const;

    /** @brief Returns the object from the container locking it
     *
     * The lock prevents the object from being deleted while it is accessed.
     *
     * @param[in] id The id of the object to return
     * @return The object with the given id or nullptr if no such object is known
     */
    GUIGlObject* getObjectBlocking(const std::string& fullName) const;

    /** @brief Removes the named object from this container
     *
     * This function returns true if the object may be deleted;
     *  otherwise it's kept in an internal storage (for visualisation etc.)
     *  and will be removed by this class
     *
     * @param[in] id The id of the object to remove
     * @return Whether the object could be removed (and may be deleted)
     */
    bool remove(GUIGlID id);

    /** @brief Clears this container
     *
     * The objects are not deleted.
     */
    void clear();


    /** @brief Marks an object as unblocked
     *
     * The object is moved from "myBlocked" to "myMap".
     * @param[in] id The id of the object to unblock
     */
    void unblockObject(GUIGlID id);


    /** @brief Sets the given object as the "network" object
     * @param[in] object The object to set as network object
     */
    void setNetObject(GUIGlObject* object) {
        myNetObject = object;
    }

    /** @brief Returns the network object
     * @return The network object
     */
    GUIGlObject* getNetObject() const {
        return myNetObject;
    }

    /// @brief A single static instance of this class
    static GUIGlObjectStorage gIDStorage;

    /// @brief Returns the set of all known objects
    const std::vector<GUIGlObject*>& getAllGLObjects() const;

private:
    /// @brief The known objects
    std::vector<GUIGlObject*> myObjects;

    /// @brief The known objects by their full name
    std::map<std::string, GUIGlObject*> myFullNameMap;

    /// @brief The next id to give; initially one, increased by one with each object registration
    GUIGlID myNextID;

    /// @brief A lock to avoid parallel access on the storages
    mutable FXMutex myLock;

    /// @brief The network object
    GUIGlObject* myNetObject;

private:
    /// @brief invalidated copy constructor
    GUIGlObjectStorage(const GUIGlObjectStorage& s) = delete;

    /// @brief invalidate assignment operator
    GUIGlObjectStorage& operator=(const GUIGlObjectStorage& s) = delete;
};
