/****************************************************************************/
/// @file    GUIParam_PopupMenu.cpp
/// @author  Daniel Krajzewicz
/// @date    Mai 2003
/// @version $Id$
///
// A popup-menu for dynamic patameter table entries
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include "GUIParameterTableWindow.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include "GUIParam_PopupMenu.h"
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/tracker/TrackerValueDesc.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIParam_PopupMenuInterface) GUIParam_PopupMenuInterfaceMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_OPENTRACKER, GUIParam_PopupMenuInterface::onCmdOpenTracker),
};

// Object implementation
FXIMPLEMENT(GUIParam_PopupMenuInterface, FXMenuPane, GUIParam_PopupMenuInterfaceMap, ARRAYNUMBER(GUIParam_PopupMenuInterfaceMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIParam_PopupMenuInterface::GUIParam_PopupMenuInterface(GUIMainWindow &app,
        GUIParameterTableWindow &parentWindow,
        GUIGlObject &o, const std::string &varName,
        ValueSource<SUMOReal> *src)  throw()
        : FXMenuPane(&parentWindow), myObject(&o), myParentWindow(&parentWindow),
        myApplication(&app), myVarName(varName), mySource(src)
{
}


GUIParam_PopupMenuInterface::~GUIParam_PopupMenuInterface() throw()
{
    delete mySource;
}


long
GUIParam_PopupMenuInterface::onCmdOpenTracker(FXObject*,FXSelector,void*)
{
    string trackerName = myVarName + " from " + myObject->getFullName();
    GUIParameterTracker *tr = new GUIParameterTracker(*myApplication,
            trackerName, *myObject, 0, 0);
    TrackerValueDesc *newTracked = new TrackerValueDesc(
        myVarName, RGBColor(0, 0, 0), myObject, myApplication->getCurrentSimTime());
    tr->addTracked(*myObject, mySource->copy(), newTracked);
    tr->create();
    tr->show();
    return 1;
}


/****************************************************************************/

