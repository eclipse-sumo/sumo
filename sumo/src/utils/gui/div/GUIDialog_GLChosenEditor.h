/****************************************************************************/
/// @file    GUIDialog_GLChosenEditor.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 11.03.2004
/// @version $Id: $
///
// Editor for the  list of chosen objects
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
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
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

    FXbool close(FXbool notify=FALSE);

    long onCmdLoad(FXObject*,FXSelector,void*);
    long onCmdSave(FXObject*,FXSelector,void*);
    long onCmdDeselect(FXObject*,FXSelector,void*);
    long onCmdClear(FXObject*,FXSelector,void*);
    long onCmdClose(FXObject*,FXSelector,void*);
    void rebuildList();

private:
    /// the list that holds the ids
    FXList *myList;

    /// the parent window
    GUIMainWindow *myParent;

    /// the storage
    GUISelectedStorage *myStorage;

protected:
    GUIDialog_GLChosenEditor()
    { }

};


#endif

/****************************************************************************/

