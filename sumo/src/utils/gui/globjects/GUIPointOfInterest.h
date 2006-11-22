#ifndef GUIPointOfInterest_h
#define GUIPointOfInterest_h
/***************************************************************************
                          GUIPointOfInterest.cpp
    The GUI-version of a point of interest
                             -------------------
    project              : SUMO - Simulation of Urban MObility
    begin                : June 2006
    copyright            : (C) 2006 by Daniel Krajzewicz
    organisation         : IVF/DLR http://ivf.dlr.de
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2006/11/22 13:06:46  dkrajzew
// patching problems on choosing an object when using shapes within different layers
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/shapes/PointOfInterest.h>
#include <utils/gui/globjects/GUIGlObject.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/*
 * @class GUIPointOfInterest
 * @brief The GUI-version of a point of interest
 */
class GUIPointOfInterest : public PointOfInterest, public GUIGlObject {
public:
    GUIPointOfInterest(GUIGlObjectStorage &idStorage, int layer,
        const std::string &id, const std::string &type,
        const Position2D &p, const RGBColor &c);

    virtual ~GUIPointOfInterest();


    //@{ From GUIGlObject
    /// Returns the popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the parameter window
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
   const std::string &microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;

	/// Returns the boundary to which the object shall be centered
	Boundary getCenteringBoundary() const;
    //@}

    /// Returns the layer the object is located in
    int getLayer() const;

protected:
    /** @brief The layer this object is located in
     *
     * This value is used for determining which object to choose as being on top under the cursor
     */
    int myLayer;

};

#endif
