/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIParam_PopupMenu.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mai 2003
/// @version $Id$
///
// A popup-menu for dynamic patameter table entries
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <string>
#include "GUIParameterTableWindow.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include "GUIParam_PopupMenu.h"
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/tracker/TrackerValueDesc.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIParam_PopupMenuInterface) GUIParam_PopupMenuInterfaceMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_OPENTRACKER, GUIParam_PopupMenuInterface::onCmdOpenTracker),
};

// Object implementation
FXIMPLEMENT(GUIParam_PopupMenuInterface, FXMenuPane, GUIParam_PopupMenuInterfaceMap, ARRAYNUMBER(GUIParam_PopupMenuInterfaceMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIParam_PopupMenuInterface::GUIParam_PopupMenuInterface(GUIMainWindow& app,
        GUIParameterTableWindow& parentWindow, GUIGlObject& o, const std::string& varName,
        ValueSource<double>* src)
    : FXMenuPane(&parentWindow), myObject(&o), myParentWindow(&parentWindow),
      myApplication(&app), myVarName(varName), mySource(src) {
}


GUIParam_PopupMenuInterface::~GUIParam_PopupMenuInterface() {
    delete mySource;
}


long
GUIParam_PopupMenuInterface::onCmdOpenTracker(FXObject*, FXSelector, void*) {
    std::string trackerName = myVarName + " from " + myObject->getFullName();
    GUIParameterTracker* tr = new GUIParameterTracker(*myApplication, trackerName);
    TrackerValueDesc* newTracked = new TrackerValueDesc(myVarName, RGBColor::BLACK, myApplication->getCurrentSimTime(), myApplication->getTrackerInterval());
    tr->addTracked(*myObject, mySource->copy(), newTracked);
    tr->create();
    tr->show();
    return 1;
}


/****************************************************************************/

