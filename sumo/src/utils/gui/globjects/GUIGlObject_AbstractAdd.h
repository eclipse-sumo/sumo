/****************************************************************************/
/// @file    GUIGlObject_AbstractAdd.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// Base class for additional objects (detectors etc.)
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
#ifndef GUIGlObject_AbstractAdd_h
#define GUIGlObject_AbstractAdd_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIGlObjectTypes.h"
#include "GUIGlObject.h"
#include <vector>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGlObject_AbstractAdd
 */
class GUIGlObject_AbstractAdd :
    public GUIGlObject {
public:
    GUIGlObject_AbstractAdd(
        const std::string& prefix,
        GUIGlObjectType type,
        const std::string& id);

    ~GUIGlObject_AbstractAdd();

    /// Clears the dictionary (the objects will not be deleted)
    static void clearDictionary();

    /// Returns a named object
    static GUIGlObject_AbstractAdd* get(const std::string& name);

    /// Removes an object
    static void remove(GUIGlObject_AbstractAdd* o);

    /// Returns the list of all additional objects
    static const std::vector<GUIGlObject_AbstractAdd*>& getObjectList();

    /// Returns the list of gl-ids of all additional objects
    static std::vector<GUIGlID> getIDList();


protected:
    /// Map from names of loaded additional objects to the objects themselves
    static std::map<std::string, GUIGlObject_AbstractAdd*> myObjects;

    /// The list of all addtional objects currently loaded
    static std::vector<GUIGlObject_AbstractAdd*> myObjectList;

};


#endif

/****************************************************************************/

