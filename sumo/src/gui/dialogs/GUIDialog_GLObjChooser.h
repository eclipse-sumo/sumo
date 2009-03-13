/****************************************************************************/
/// @file    GUIDialog_GLObjChooser.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
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
#ifndef GUIDialog_GLObjChooser_h
#define GUIDialog_GLObjChooser_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOViewParent;
class GUIGlObjectStorage;
class GUIGlObject;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_GLObjChooser
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_GLObjChooser : public FXMainWindow {
    // FOX-declarations
    FXDECLARE(GUIDialog_GLObjChooser)

public:
    /// constructor
    GUIDialog_GLObjChooser(GUISUMOViewParent *parent, GUIGlObjectType type,
                           GUIGlObjectStorage &glStorage);

    /// destructor
    ~GUIDialog_GLObjChooser();

    /// Returns the chosen (selected) object
    GUIGlObject *getObject() const;

    /// The selected item shall be centered within the calling view
    long onCmdCenter(FXObject*,FXSelector,void*);

    /// The dialog shall be closed
    long onCmdClose(FXObject*,FXSelector,void*);

    /// Somthing has been typed into the the field
    long onChgText(FXObject*,FXSelector,void*);

    /// Selects to current item if enter is pressed
    long onCmdText(FXObject*,FXSelector,void*);

    /// Selects to current item if enter is pressed
    long onListKeyPress(FXObject*,FXSelector,void*);

private:
    /// the list that holds the ids
    FXList *myList;

    /// the artifact to choose
    GUIGlObjectType myObjectType;

    /// the parent window
    GUISUMOViewParent *myParent;

    /// the chosen id
    GUIGlObject *mySelected;

    /// The text field
    FXTextField *myTextEntry;

protected:
    /// FOX needs this
    GUIDialog_GLObjChooser() { }

};


#endif

/****************************************************************************/

