#ifndef GUINetWrapper_h
#define GUINetWrapper_h
//---------------------------------------------------------------------------//
//                        GUINetWrapper.h -
//  No geometrical information is hold, here. Still, some methods for
//      displaying network-information are stored in here
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                :
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
// Revision 1.5  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.2  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.1  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
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
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/HaveBoundary.h>
#include <utils/foxtools/FXMutex.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObject.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class GUINetWrapper :
            public GUIGlObject,
            public HaveBoundary {
public:
    /// constructor
    GUINetWrapper( GUIGlObjectStorage &idStorage,
        GUINet &net);

    /// destructor
    virtual ~GUINetWrapper();

    /// Returns a popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent);

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    Boundary getBoundary() const;

    GUINet &getNet() const;

	//{
	Boundary getCenteringBoundary() const;
	//}

protected:

	bool active() const { return true; }

protected:

    GUINet &myNet;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

