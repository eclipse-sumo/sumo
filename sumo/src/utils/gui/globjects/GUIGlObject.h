#ifndef GUIGlObject_h
#define GUIGlObject_h
//---------------------------------------------------------------------------//
//                        GUIGlObject.h -
//  Base class for all objects that may be displayed within the openGL-gui
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
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:01:55  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.11  2004/07/02 08:27:09  dkrajzew
// possibility to manipulate objects added (preliminary, subject to change)
//
// Revision 1.10  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.9  2003/11/18 14:28:14  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.8  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.7  2003/11/11 08:41:27  dkrajzew
// logging moved from utils to microsim
//
// Revision 1.6  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.5  2003/06/05 11:37:30  dkrajzew
// class templates applied
//
// Revision 1.4  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.3  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.2  2003/04/14 08:24:55  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "GUIGlObjectTypes.h"
#include <utils/geom/Boundary.h>


 /* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;
class GUISUMOAbstractView;
class GUIParameterTableWindow;
class GUIMainWindow;
class GUIGLObjectPopupMenu;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIGlObject {
public:
    /// Constructor
    GUIGlObject(GUIGlObjectStorage &idStorage,
        std::string fullName);

    /// Constructor for objects joining gl-objects
    GUIGlObject(GUIGlObjectStorage &idStorage,
        std::string fullName, size_t glID);

    /// Destructor
    virtual ~GUIGlObject();

    /// Returns the full name apperaing in the tool tip
    const std::string &getFullName() const;

    /// Returns the numerical id of the object
    size_t getGlID() const;

    /// Returns an own popup-menu
    virtual GUIGLObjectPopupMenu *getPopUpMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent) = 0;

    /// Returns an own parameter window
    virtual GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent) = 0;

    /// Returns the type of the object as coded in GUIGlObjectType
    virtual GUIGlObjectType getType() const = 0;

    /// returns the id of the object as known to microsim
    virtual std::string microsimID() const = 0;

    /// Needed to set the id
    friend class GUIGlObjectStorage;

    /// Returns the information whether this object is still active
    virtual bool active() const = 0;

    /// Returns the boundary to which the object shall be centered
    virtual Boundary getCenteringBoundary() const = 0;

private:
    /// Sets the id of the object
    void setGlID(size_t id);


private:
    /// The numerical id of the object
    size_t myGlID;

    /// The name of the object
    std::string myFullName;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

