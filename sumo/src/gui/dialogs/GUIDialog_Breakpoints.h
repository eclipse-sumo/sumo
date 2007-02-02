/****************************************************************************/
/// @file    GUIDialog_Breakpoints.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Jun 2004
/// @version $Id: $
///
//
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
#ifndef GUIDialog_Breakpoints_h
#define GUIDialog_Breakpoints_h
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
#include <gui/GUIAddWeightsStorage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIApplicationWindow;
class MFXAddEditTypedTable;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_Breakpoints
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_Breakpoints : public FXMainWindow
{
    // FOX-declarations
    FXDECLARE(GUIDialog_Breakpoints)
public:
    /// constructor
    GUIDialog_Breakpoints(GUIMainWindow *parent);

    /// destructor
    ~GUIDialog_Breakpoints();

    FXbool close(FXbool notify=FALSE);

    long onCmdLoad(FXObject*,FXSelector,void*);
    long onCmdSave(FXObject*,FXSelector,void*);
    long onCmdDeselect(FXObject*,FXSelector,void*);
    long onCmdClear(FXObject*,FXSelector,void*);
    long onCmdClose(FXObject*,FXSelector,void*);
    long onCmdEditTable(FXObject*,FXSelector,void*);
//    long onUpdSave(FXObject*,FXSelector,void*);

private:
    void rebuildList();
    std::string encode2TXT();

private:
    /// the list that holds the ids
    MFXAddEditTypedTable *myTable;

    /// the parent window
    GUIMainWindow *myParent;

protected:
    GUIDialog_Breakpoints()
    { }

};


#endif

/****************************************************************************/

