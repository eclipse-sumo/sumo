/****************************************************************************/
/// @file    GUIParameterTableWindow.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// The window that holds the table of an object's parameter
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
#ifndef GUIParameterTableWindow_h
#define GUIParameterTableWindow_h
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

#include <vector>
#include <string>
#include <fx.h>
#include <utils/helpers/ValueSource.h>
#include "GUIParameterTableItem.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObject;
class GUIMainWindow;


// ===========================================================================
// class definitions
// ===========================================================================
class GUIParameterTableWindow : public FXMainWindow
{
    FXDECLARE(GUIParameterTableWindow)
public:
    /// Constructor
    GUIParameterTableWindow(GUIMainWindow &app,
                            GUIGlObject &o, size_t noRows);

    /// Destructor
    ~GUIParameterTableWindow();

    void closeBuilding();

    void mkItem(const char *name, bool dynamic, ValueSource<SUMOReal> *src);

    void mkItem(const char *name, bool dynamic, std::string value);

    void mkItem(const char *name, bool dynamic, SUMOReal value);

    long onSimStep(FXObject*,FXSelector,void*);
    long onTableSelected(FXObject*,FXSelector,void*);
    long onTableDeselected(FXObject*,FXSelector,void*);
    long onRightButtonPress(FXObject*,FXSelector,void*);

protected:

protected:

    void updateTable();

    /// The object to get the information from
    GUIGlObject *myObject;

    /// The table to display the information in
    FXTable *myTable;

    /// The main application
    GUIMainWindow *myApplication;

    std::vector<GUIParameterTableItem*> myItems;

    size_t myCurrentPos;

protected:
    GUIParameterTableWindow()
    { }
};


#endif

/****************************************************************************/

