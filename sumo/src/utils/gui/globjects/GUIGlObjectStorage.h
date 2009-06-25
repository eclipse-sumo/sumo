/****************************************************************************/
/// @file    GUIGlObjectStorage.h
/// @author  Daniel Krajzewicz
/// @date    Oct 2002
/// @version $Id$
///
// A storage for of displayed objects via their numerical id
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
#ifndef GUIGlObjectStorage_h
#define GUIGlObjectStorage_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <fx.h>
#include <utils/foxtools/MFXMutex.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObject;


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
    GUIGlObjectStorage() throw();


    /// @brief Destructor
    ~GUIGlObjectStorage() throw();


    /** @brief Registers an object
     *
     * This done within the constructor of the GUIGlObject; The object's "setGLID"
     *  method is called giving the next free id.
     *
     * @param[in] object The object to register
     */
    void registerObject(GUIGlObject *object) throw();


    /** @brief retrieves a unique id for an object to display
     * @return The next free gl-id
     */
    GLuint getUniqueID() throw();


    /** @brief Returns the object from the container locking it
     *
     * The lock prevents the object from being deleted while it is accessed.
     * The object is moved from "myMap" to "myBlocked".
     *
     * @param[in] id The id of the object to return
     * @return The object with the given id or 0 if no such object is known
     */
    GUIGlObject *getObjectBlocking(GLuint id) throw();


    /** @brief Removes the named object from this container
     *
     * This function returns true if the object may be deleted;
     *  otherwise it's kept in an internal storage (for visualisation etc.) 
     *  and will be removed by this class
     *
     * @param[in] id The id of the object to remove
     * @return Whether the object could be removed (and may be deleted)
     */
    bool remove(GLuint id) throw();


    /** @brief Clears this container
     *
     * The objects are not deleted.
     */
    void clear() throw();


    /** @brief Marks an object as unblocked
     * 
     * The object is moved from "myBlocked" to "myMap".
     * @param[in] id The id of the object to unblock
     */
    void unblockObject(GLuint id) throw();


    /** @brief Sets the given object as the "network" object
     * @param[in] object The object to set as network object
     */
    void setNetObject(GUIGlObject *object) throw() {
        myNetObject = object;
    }


    /** @brief Returns the network object
     * @return The network object
     */
    GUIGlObject * const getNetObject() const throw() {
        return myNetObject;
    }


    /** @brief A single static instance of this class
     */
    static GUIGlObjectStorage gIDStorage;

private:
    /// @brief Definition of a container from numerical ids to objects
    typedef std::map<size_t, GUIGlObject *> ObjectMap;

    /// @brief The known objects which are not accessed currently
    ObjectMap myMap;

    /// @brief The currently accessed objects
    ObjectMap myBlocked;

    /// @brief Objects to delete
    ObjectMap my2Delete;

    /// @brief The next id to give; initially zero, increased by one with each object registration
    GLuint myAktID;

    /// @brief A lock to avoid parallel access on the storages
    MFXMutex myLock;

    /// @brief The network object
    GUIGlObject *myNetObject;


private:
    /// @brief invalidated copy constructor
    GUIGlObjectStorage(const GUIGlObjectStorage &s);

    /// @brief invalidate assignment operator
    GUIGlObjectStorage &operator=(const GUIGlObjectStorage &s);


};


#endif

/****************************************************************************/

