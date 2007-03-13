/****************************************************************************/
/// @file    GUIDialog_GLChosenEditor.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 11.03.2004
/// @version $Id$
///
// Editor for the list of chosen objects
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
#ifndef GUIDialog_GLChosenEditor_h
#define GUIDialog_GLChosenEditor_h
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

#include <string>
#include <vector>
#include <fx.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIApplicationWindow;
class GUISelectedStorage;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_GLChosenEditor
 * @brief Editor for the list of chosen objects
 */
class GUIDialog_GLChosenEditor : public FXMainWindow
{
    // FOX-declarations
    FXDECLARE(GUIDialog_GLChosenEditor)
public:
    /// constructor
    GUIDialog_GLChosenEditor(GUIMainWindow *parent,
                             GUISelectedStorage *str);

    /// destructor
    ~GUIDialog_GLChosenEditor();

    /// Called when the user presses the Load-button
    long onCmdLoad(FXObject*,FXSelector,void*);

    /// Called when the user presses the Save-button
    long onCmdSave(FXObject*,FXSelector,void*);

    /// Called when the user presses the Deselect-button
    long onCmdDeselect(FXObject*,FXSelector,void*);

    /// Called when the user presses the Clear-button
    long onCmdClear(FXObject*,FXSelector,void*);

    /// Called when the user presses the Close-button
    long onCmdClose(FXObject*,FXSelector,void*);

    /// Rebuilds the entire list
    void rebuildList();

private:
    /// the list that holds the ids
    FXList *myList;

    /// the parent window
    GUIMainWindow *myParent;

    /// the storage
    GUISelectedStorage *myStorage;

protected:
    /// FOX needs this
    GUIDialog_GLChosenEditor()
    { }

};


#endif

/****************************************************************************/

