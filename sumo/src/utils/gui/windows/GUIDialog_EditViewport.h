/****************************************************************************/
/// @file    GUIDialog_EditViewport.h
/// @author  Daniel Krajzewicz
/// @date    2005-05-04
/// @version $Id$
///
// A dialog to change the viewport
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
#ifndef GUIDialog_EditViewport_h
#define GUIDialog_EditViewport_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/FXRealSpinDial.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
class GUIDialog_EditViewport : public FXDialogBox {
    // FOX-declarations
    FXDECLARE(GUIDialog_EditViewport)
public:
    enum {
        MID_CHANGED = FXDialogBox::ID_LAST
    };

    /// Constructor
    GUIDialog_EditViewport(GUISUMOAbstractView* parent,  const char* name,
                           SUMOReal zoom, SUMOReal xoff, SUMOReal yoff, int x, int y);

    /// Destructor
    ~GUIDialog_EditViewport();

    /// Called when the user changes the viewport
    long onCmdChanged(FXObject*,FXSelector,void*);

    /// Called when the user wants to keep the viewport
    long onCmdOk(FXObject*,FXSelector,void*);

    /// Called when the user wants to restore the viewport
    long onCmdCancel(FXObject*,FXSelector,void*);

    /// Sets the given values into the dialog
    void setValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff);

    /// Returns the information whether one of the spin dialers is grabbed
    bool haveGrabbed() const;

    /// Resets old values
    void setOldValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff);

protected:
    /// The calling view
    GUISUMOAbstractView *myParent;

    /// The old zoom
    SUMOReal myOldZoom;

    /// The old offsets
    SUMOReal myOldXOff, myOldYOff;

    /// The spin dialers used to change the view
    FXRealSpinDial *myZoom, *myXOff, *myYOff;

protected:
    /// FOX needs this
    GUIDialog_EditViewport() { }

};


#endif

/****************************************************************************/

