#ifndef GUIGlObjectStorage_h
#define GUIGlObjectStorage_h
//---------------------------------------------------------------------------//
//                        GUIGlObjectStorage.h -
//  A storage for retrival of displayed object using a numerical id
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.3  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 09:19:51  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.4  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/11/18 14:28:14  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.2  2003/04/14 08:24:56  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <map>
#include <utils/foxtools/FXMutex.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObject;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIGlObjectStorage
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
    void registerObject(GUIGlObject *object, size_t id);

    /// retrievs a unique id for an object to display
    size_t getUniqueID();

    /** @brief Retunrs the object from the container locking it
        The lock prevents the object from being deleted while it is accessed */
    GUIGlObject *getObjectBlocking(size_t id);

    /** @brief Removes the named object from this container
        This function returns true if the object may be deleted;
        otherwise it's kept in an internal storage (for visualisation etc.) and will be removed by this class */
    bool remove(size_t id);

    /// Clears this container
    void clear();

    /// Marks an object as unblocked
    void unblockObject(size_t id);

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
    size_t myAktID;

    /// A lock to avoid parallel access on the storages
    FXEX::FXMutex _lock;

private:
    /// Invalidated copy constructor
    GUIGlObjectStorage(const GUIGlObjectStorage &s);

    /// Invalidate assignment operator
    GUIGlObjectStorage &operator=(const GUIGlObjectStorage &s);

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

