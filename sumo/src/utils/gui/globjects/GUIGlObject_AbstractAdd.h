#ifndef GUIGlObject_AbstractAdd_h
#define GUIGlObject_AbstractAdd_h
//---------------------------------------------------------------------------//
//                        GUIGlObject_AbstractAdd.h -
//  Base class for additional objects (emitter, detectors etc.)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2005/05/04 09:18:58  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:01:55  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
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

#include "GUIGlObjectTypes.h"
#include "GUIGlObject.h"
#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>
#include <vector>
#include <map>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIGlObject_AbstractAdd
 */
class GUIGlObject_AbstractAdd :
    public GUIGlObject, public HaveBoundary {
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
    virtual void drawGL_FG(double scale) = 0;

    /// Draws the detector in simple-geometry mode
    virtual void drawGL_SG(double scale) = 0;

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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

