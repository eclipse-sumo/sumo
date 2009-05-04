/****************************************************************************/
/// @file    GUIGlObjectStorage.h
/// @author  Daniel Krajzewicz
/// @date    Oct 2002
/// @version $Id$
///
// A storage for retrieval of displayed object using a numerical id
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
 * @brief A storage for retrieval of displayed object using a numerical id
 *
 * This is a container for GUIGlObjects - objects, which may are displayed
 * and due to this may generate tooltips or be grapped in other ways.
 * As in case of vehicles (other, later implemented objects may have this
 * property, too) they may be deleted by the simulation while being accessed
 * - for example using a property window or something like that - this
 * container posesses three storages: one containing all objects that are not
 * accessed at all, one for objects currently accessed and one for objects that
 * are accessed but shall be deleted.
 */
class GUIGlObjectStorage {
public:
    /// Constructor
    GUIGlObjectStorage();

    /// Destructor
    ~GUIGlObjectStorage();

    /** @brief registers an object
        This done within the constructor of the GUIGlObject */
    void registerObject(GUIGlObject *object);

    /** @brief registers an object
        This done within the constructor of the GUIGlObject;
        this method may be used when several objects shall share the same id.
        The id must be previously retrieved using "getUniqueID" */
    //void registerObject(GUIGlObject *object, GLuint id);

    /// retrievs a unique id for an object to display
    GLuint getUniqueID();

    /** @brief Retunrs the object from the container locking it
        The lock prevents the object from being deleted while it is accessed */
    GUIGlObject *getObjectBlocking(GLuint id);

    /** @brief Removes the named object from this container
        This function returns true if the object may be deleted;
        otherwise it's kept in an internal storage (for visualisation etc.) and will be removed by this class */
    bool remove(GLuint id);

    /// Clears this container
    void clear();

    /// Marks an object as unblocked
    void unblockObject(GLuint id);

private:
    /// Definition of an associative container from numerical ids to objects
    typedef std::map<size_t, GUIGlObject *> ObjectMap;

    /// The known objects which are not accessed currently
    ObjectMap myMap;

    /// The accessed objects
    ObjectMap myBlocked;

    /// Objects to delete
    ObjectMap my2Delete;

    /** @brief The next id to give
        Initially zero, increased by one with each object registration */
    GLuint myAktID;

    /// A lock to avoid parallel access on the storages
    MFXMutex myLock;

private:
    /// Invalidated copy constructor
    GUIGlObjectStorage(const GUIGlObjectStorage &s);

    /// Invalidate assignment operator
    GUIGlObjectStorage &operator=(const GUIGlObjectStorage &s);

};


#endif

/****************************************************************************/

