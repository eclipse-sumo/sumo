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
// Revision 1.2  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.1  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/HaveBoundery.h>
#include <utils/foxtools/FXMutex.h>
#include <gui/GUIGlObjectStorage.h>
#include <gui/popup/GUIGLObjectPopupMenu.h>
#include <gui/GUIGlObject.h>


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
            public HaveBoundery {
public:
    /// constructor
    GUINetWrapper( GUIGlObjectStorage &idStorage,
        GUINet &net);

    /// destructor
    virtual ~GUINetWrapper();

    /// Returns a popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    GUIParameterTableWindow *getParameterWindow(
        GUIApplicationWindow &app, GUISUMOAbstractView &parent);

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    Boundery getBoundery() const;

    GUINet &getNet() const;

protected:

	bool active() const { return true; }

protected:

    GUINet &myNet;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUINetWrapper.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

