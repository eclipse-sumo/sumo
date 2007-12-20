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
#include <utils/common/ValueSource.h>
#include <utils/gui/div/GUIParameterTableWindow.h>


// ===========================================================================
// class definitions
// ===========================================================================
class GUIGlObject;
class GUIMainWindow;


// ===========================================================================
// class declarataions
// ===========================================================================
class GUIParam_PopupMenuInterface : public FXMenuPane
{
    FXDECLARE(GUIParam_PopupMenuInterface)
public:
    GUIParam_PopupMenuInterface(GUIMainWindow &app,
                                GUIParameterTableWindow &parentWindow,
                                GUIGlObject &o, const std::string &varName) : FXMenuPane(&parentWindow),
            myObject(&o), //myParent(&parent),
            myParentWindow(&parentWindow), myApplication(&app), myVarName(varName) { }

    virtual ~GUIParam_PopupMenuInterface() {}

    long onCmdOpenTracker(FXObject*,FXSelector,void*);

    virtual ValueSource<SUMOReal> *getSUMORealSourceCopy() = 0;

protected:
    /// The object the table displays
    GUIGlObject *myObject;

    GUIParameterTableWindow *myParentWindow;

    /** @brief The main application window
        holder of some needed values */
    GUIMainWindow *myApplication;

    std::string myVarName;


};


/**
 * @class GUIParam_PopupMenu
 * A popup menu holding the context of a parameter table entry
 */
template<class T>
class GUIParam_PopupMenu : public GUIParam_PopupMenuInterface
{
public:
    /// Constructor
    GUIParam_PopupMenu(GUIMainWindow &app,
                       GUIParameterTableWindow &parentWindow,
                       GUIGlObject &o, const std::string &varName, ValueSource<T> *src)
            : GUIParam_PopupMenuInterface(app, parentWindow, o, varName),
            mySource(src) {}

    /// Destructor
    ~GUIParam_PopupMenu() {
        delete mySource;
    }

    ValueSource<SUMOReal> *getSUMORealSourceCopy() {
        return mySource->makeSUMORealReturningCopy();
    }


private:
    ValueSource<T> *mySource;

protected:
    GUIParam_PopupMenu() { }

};


#endif

/****************************************************************************/

