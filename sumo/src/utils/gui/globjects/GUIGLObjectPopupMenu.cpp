/****************************************************************************/
/// @file    GUIGLObjectPopupMenu.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The popup menu of a globject
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <cassert>
#include <utils/geom/GeoConvHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/common/ToString.h>
#include "GUIGLObjectPopupMenu.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIGLObjectPopupMenu) GUIGLObjectPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CENTER,                  GUIGLObjectPopupMenu::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_NAME,               GUIGLObjectPopupMenu::onCmdCopyName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_TYPED_NAME,         GUIGLObjectPopupMenu::onCmdCopyTypedName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_CURSOR_POSITION,    GUIGLObjectPopupMenu::onCmdCopyCursorPosition),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_CURSOR_GEOPOSITION, GUIGLObjectPopupMenu::onCmdCopyCursorGeoPosition),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPARS,                GUIGLObjectPopupMenu::onCmdShowPars),
    FXMAPFUNC(SEL_COMMAND,  MID_ADDSELECT,               GUIGLObjectPopupMenu::onCmdAddSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_REMOVESELECT,            GUIGLObjectPopupMenu::onCmdRemoveSelected)
};

// Object implementation
FXIMPLEMENT(GUIGLObjectPopupMenu, FXMenuPane, GUIGLObjectPopupMenuMap, ARRAYNUMBER(GUIGLObjectPopupMenuMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIGLObjectPopupMenu::GUIGLObjectPopupMenu(GUIMainWindow& app,
        GUISUMOAbstractView& parent,
        GUIGlObject& o)
    : FXMenuPane(&parent), myParent(&parent), myObject(&o),
      myApplication(&app), myNetworkPosition(parent.getPositionInformation()) {
}


GUIGLObjectPopupMenu::~GUIGLObjectPopupMenu() {}


long
GUIGLObjectPopupMenu::onCmdCenter(FXObject*, FXSelector, void*) {
    // we already know where the object is since we clicked on it -> zoom on Boundary
    myParent->centerTo(myObject->getGlID(), true, -1);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyName(FXObject*, FXSelector, void*) {
    GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getMicrosimID());
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyTypedName(FXObject*, FXSelector, void*) {
    GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getFullName());
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyCursorPosition(FXObject*, FXSelector, void*) {
    GUIUserIO::copyToClipboard(*myParent->getApp(), toString(myNetworkPosition));
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyCursorGeoPosition(FXObject*, FXSelector, void*) {
    Position pos = myNetworkPosition;
    GeoConvHelper::getFinal().cartesian2geo(pos);
    // formated for pasting into google maps
    const std::string posString = toString(pos.y(), GEO_OUTPUT_ACCURACY) + ", " + toString(pos.x(), GEO_OUTPUT_ACCURACY);
    GUIUserIO::copyToClipboard(*myParent->getApp(), posString);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdShowPars(FXObject*, FXSelector, void*) {
    myObject->getParameterWindow(*myApplication, *myParent); // !!! showParameterWindow would be more appropriate
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdAddSelected(FXObject*, FXSelector, void*) {
    gSelected.select(myObject->getGlID());
    myParent->update();
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdRemoveSelected(FXObject*, FXSelector, void*) {
    gSelected.deselect(myObject->getGlID());
    myParent->update();
    return 1;
}


/****************************************************************************/

