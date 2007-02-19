/****************************************************************************/
/// @file    GUIGlObject_AbstractAdd.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// Base class for additional objects (emitter, detectors etc.)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIGlObject_AbstractAdd_h
#define GUIGlObject_AbstractAdd_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIGlObjectTypes.h"
#include "GUIGlObject.h"
#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>
#include <vector>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGlObject_AbstractAdd
 */
class GUIGlObject_AbstractAdd :
            public GUIGlObject, public HaveBoundary
{
public:
    GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
                            std::string fullName, GUIGlObjectType type);

    /// Constructor for objects joining gl-objects
    GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
                            std::string fullName, size_t glID, GUIGlObjectType type);

    ~GUIGlObject_AbstractAdd();

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// Draws the detector in full-geometry mode
    virtual void drawGL(SUMOReal scale, SUMOReal upscale) = 0;

    /// Returns the detector's coordinates
    virtual Position2D getPosition() const = 0;

    /// Clears the dictionary (the objects will not be deleted)
    static void clearDictionary();

    /// Returns a named object
    static GUIGlObject_AbstractAdd *get(const std::string &name);

    /// Returns the list of all additional objects
    static const std::vector<GUIGlObject_AbstractAdd*> &getObjectList();

    /// Retusn the list of gl-ids of all additional objects
    static std::vector<size_t> getIDList();

    //{
    Boundary getCenteringBoundary() const;
    //}

protected:
    /// The object's type
    GUIGlObjectType myGlType;

protected:
    /// Map from names of loaded additional objects to the objects themselves
    static std::map<std::string, GUIGlObject_AbstractAdd*> myObjects;

    /// The list of all addtional objects currently loaded
    static std::vector<GUIGlObject_AbstractAdd*> myObjectList;

};


#endif

/****************************************************************************/

