/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIGLObjectPopupMenu.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// The popup menu of a globject
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <cassert>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/common/ToString.h>
#include "GUIGLObjectPopupMenu.h"
#include <utils/foxtools/MFXLinkLabel.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIGLObjectPopupMenu) GUIGLObjectPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CENTER,                  GUIGLObjectPopupMenu::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_NAME,               GUIGLObjectPopupMenu::onCmdCopyName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_TYPED_NAME,         GUIGLObjectPopupMenu::onCmdCopyTypedName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_EDGE_NAME,          GUIGLObjectPopupMenu::onCmdCopyEdgeName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_CURSOR_POSITION,    GUIGLObjectPopupMenu::onCmdCopyCursorPosition),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_CURSOR_GEOPOSITION, GUIGLObjectPopupMenu::onCmdCopyCursorGeoPosition),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_VIEW_GEOBOUNDARY,   GUIGLObjectPopupMenu::onCmdCopyViewGeoBoundary),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOW_GEOPOSITION_ONLINE, GUIGLObjectPopupMenu::onCmdShowCursorGeoPositionOnline),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPARS,                GUIGLObjectPopupMenu::onCmdShowPars),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTYPEPARS,            GUIGLObjectPopupMenu::onCmdShowTypePars),
    FXMAPFUNC(SEL_COMMAND,  MID_ADDSELECT,               GUIGLObjectPopupMenu::onCmdAddSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_REMOVESELECT,            GUIGLObjectPopupMenu::onCmdRemoveSelected)
};

// Object implementation
FXIMPLEMENT(GUIGLObjectPopupMenu, FXMenuPane, GUIGLObjectPopupMenuMap, ARRAYNUMBER(GUIGLObjectPopupMenuMap))


// ===========================================================================
// method definitions
// ===========================================================================

GUIGLObjectPopupMenu::GUIGLObjectPopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o) :
    FXMenuPane(&parent),
    myParent(&parent),
    myObject(&o),
    myApplication(&app),
    myPopupType(PopupType::ATTRIBUTES),
    myNetworkPosition(parent.getPositionInformation()) {
}


GUIGLObjectPopupMenu::GUIGLObjectPopupMenu(GUIMainWindow* app, GUISUMOAbstractView* parent, PopupType popupType) :
    FXMenuPane(parent),
    myParent(parent),
    myObject(nullptr),
    myApplication(app),
    myPopupType(popupType),
    myNetworkPosition(parent->getPositionInformation()) {
}


GUIGLObjectPopupMenu::~GUIGLObjectPopupMenu() {
    // Delete MenuPane children
    for (const auto& pane : myMenuPanes) {
        delete pane;
    }
}


void
GUIGLObjectPopupMenu::insertMenuPaneChild(FXMenuPane* child) {
    // Check that MenuPaneChild isn't NULL
    if (child == nullptr) {
        throw ProcessError("MenuPaneChild cannot be NULL");
    }
    // Check that MenuPaneChild wasn't already inserted
    for (const auto& pane : myMenuPanes) {
        if (pane == child) {
            throw ProcessError("MenuPaneChild already inserted");
        }
    }
    // Insert MenuPaneChild
    myMenuPanes.push_back(child);
}


void
GUIGLObjectPopupMenu::removePopupFromObject() {
    // remove popup menu from object
    if (myObject) {
        myObject->removedPopupMenu();
    }
}

GUISUMOAbstractView*
GUIGLObjectPopupMenu::getParentView() {
    return myParent;
}


GUIGLObjectPopupMenu::PopupType
GUIGLObjectPopupMenu::getPopupType() const {
    return myPopupType;
}


long
GUIGLObjectPopupMenu::onCmdCenter(FXObject*, FXSelector, void*) {
    // we already know where the object is since we clicked on it -> zoom on Boundary
    if (myObject) {
        myParent->centerTo(myObject->getGlID(), true, -1);
    } else {
        throw ProcessError("Object is NULL");
    }
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyName(FXObject*, FXSelector, void*) {
    if (myObject) {
        GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getMicrosimID());
    } else {
        throw ProcessError("Object is NULL");
    }
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyTypedName(FXObject*, FXSelector, void*) {
    if (myObject) {
        GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getFullName());
    } else {
        throw ProcessError("Object is NULL");
    }
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyEdgeName(FXObject*, FXSelector, void*) {
    if (myObject == nullptr) {
        throw ProcessError("Object is NULL");
    } else if (myObject->getType() != GLO_LANE) {
        throw ProcessError(TL("Object must be a lane"));
    } else {
        GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getParentName());
    }
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
    // formatted for pasting into google maps
    const std::string posString = toString(pos.y(), gPrecisionGeo) + ", " + toString(pos.x(), gPrecisionGeo);
    GUIUserIO::copyToClipboard(*myParent->getApp(), posString);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyViewGeoBoundary(FXObject*, FXSelector, void*) {
    const Boundary b = myParent->getVisibleBoundary();
    Position lowLeft(b.xmin(), b.ymin());
    GeoConvHelper::getFinal().cartesian2geo(lowLeft);
    Position upRight(b.xmax(), b.ymax());
    GeoConvHelper::getFinal().cartesian2geo(upRight);
    // formatted for usage with osmconvert
    const std::string posString = toString(lowLeft.x(), gPrecisionGeo) + "," + toString(lowLeft.y(), gPrecisionGeo) + "," +
                                  toString(upRight.x(), gPrecisionGeo) + "," + toString(upRight.y(), gPrecisionGeo);
    GUIUserIO::copyToClipboard(*myParent->getApp(), posString);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdShowCursorGeoPositionOnline(FXObject* item, FXSelector, void*) {
    FXMenuCommand* const mc = dynamic_cast<FXMenuCommand*>(item);
    Position pos = myNetworkPosition;
    GeoConvHelper::getFinal().cartesian2geo(pos);
    std::string url = myApplication->getOnlineMaps().find(mc->getText().rafter(' ').text())->second;
    url = StringUtils::replace(StringUtils::replace(url, "%lat", toString(pos.y(), gPrecisionGeo)), "%lon", toString(pos.x(), gPrecisionGeo));
    MFXLinkLabel::fxexecute(url.c_str());
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdShowPars(FXObject*, FXSelector, void*) {
    if (myObject) {
        myObject->getParameterWindow(*myApplication, *myParent);
    } else {
        throw ProcessError("Object is NULL");
    }
    return 1;
}



long
GUIGLObjectPopupMenu::onCmdShowTypePars(FXObject*, FXSelector, void*) {
    if (myObject) {
        myObject->getTypeParameterWindow(*myApplication, *myParent);
    } else {
        throw ProcessError("Object is NULL");
    }
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdAddSelected(FXObject*, FXSelector, void*) {
    if (myObject) {
        gSelected.select(myObject->getGlID());
        myParent->update();
    } else {
        throw ProcessError("Object is NULL");
    }
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdRemoveSelected(FXObject*, FXSelector, void*) {
    if (myObject) {
        gSelected.deselect(myObject->getGlID());
        myParent->update();
    } else {
        throw ProcessError("Object is NULL");
    }
    return 1;
}


GUIGLObjectPopupMenu::GUIGLObjectPopupMenu() :
    FXMenuPane(),
    myParent(nullptr),
    myObject(nullptr),
    myApplication(nullptr),
    myPopupType(PopupType::PROPERTIES) {
}

/****************************************************************************/
