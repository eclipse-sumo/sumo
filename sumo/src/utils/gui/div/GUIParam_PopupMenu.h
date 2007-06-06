/****************************************************************************/
/// @file    GUIParam_PopupMenu.h
/// @author  Daniel Krajzewicz
/// @date    Mai 2003
/// @version $Id$
///
// missing_desc
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
#ifndef GUIParam_PopupMenu_h
#define GUIParam_PopupMenu_h


// ===========================================================================
// included modules
// ===========================================================================
#include <string>

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/helpers/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
//class GUIParameterTable;
class GUIParameterTableWindow;
class GUIGlObject;
class GUIMainWindow;


// ===========================================================================
// class declarataions
// ===========================================================================
/**
 * @class GUIParam_PopupMenu
 * A popup menu holding the context of a parameter table entry
 */
class GUIParam_PopupMenu : public FXMenuPane
{
    FXDECLARE(GUIParam_PopupMenu)
public:
    /// Constructor
    GUIParam_PopupMenu(GUIMainWindow &app,
                       /*GUIParameterTable &parent, */GUIParameterTableWindow &parentWindow,
                       GUIGlObject &o, const std::string &varName, ValueSource<SUMOReal> *src);

    /// Destructor
    ~GUIParam_PopupMenu();

    long onCmdOpenTracker(FXObject*,FXSelector,void*);

private:
    /// The object the table displays
    GUIGlObject *myObject;

//    GUIParameterTable *myParent;

    GUIParameterTableWindow *myParentWindow;

    /** @brief The main application window
        holder of some needed values */
    GUIMainWindow *myApplication;

    std::string myVarName;


    ValueSource<SUMOReal> *mySource;

protected:
    GUIParam_PopupMenu()
    { }

};


#endif

/****************************************************************************/

