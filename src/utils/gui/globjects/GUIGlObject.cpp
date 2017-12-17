/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GUIGlObject.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// Base class for all objects that may be displayed within the openGL-gui
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <stack>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"

// ===========================================================================
// static members
// ===========================================================================

StringBijection<GUIGlObjectType>::Entry GUIGlObject::GUIGlObjectTypeNamesInitializer[] = {
    {"network",       GLO_NETWORK},
    {"edge",          GLO_EDGE},
    {"lane",          GLO_LANE},
    {"junction",      GLO_JUNCTION},
    {"crossing",      GLO_CROSSING},
    {"connection",    GLO_CONNECTION},
    {"prohibition",   GLO_PROHIBITION},
    {"tlLogic",       GLO_TLLOGIC},
    {"detector",      GLO_DETECTOR},
    {"trigger",       GLO_TRIGGER},
    {"additional",    GLO_ADDITIONAL},
    {"polygon",       GLO_POLYGON},
    {"poi",           GLO_POI},
    {"vehicle",       GLO_VEHICLE},
    {"person",        GLO_PERSON},
    {"container",     GLO_CONTAINER},
    {"undefined",     GLO_MAX}
};


StringBijection<GUIGlObjectType> GUIGlObject::TypeNames(GUIGlObjectTypeNamesInitializer, GLO_MAX);
const GUIGlID GUIGlObject::INVALID_ID = 0;

// ===========================================================================
// method definitions
// ===========================================================================

GUIGlObject::GUIGlObject(GUIGlObjectType type, const std::string& microsimID) :
    myGLObjectType(type),
    myMicrosimID(microsimID),
    myPrefix(TypeNames.getString(type)) {
    myFullName = createFullName();
    myGlID = GUIGlObjectStorage::gIDStorage.registerObject(this, myFullName);
}


GUIGlObject::GUIGlObject(const std::string& prefix, GUIGlObjectType type, const std::string& microsimID) :
    myGLObjectType(type),
    myMicrosimID(microsimID),
    myPrefix(prefix) {
    myFullName = createFullName();
    myGlID = GUIGlObjectStorage::gIDStorage.registerObject(this, myFullName);
}


GUIGlObject::~GUIGlObject() {
    for (std::set<GUIParameterTableWindow*>::iterator i = myParamWindows.begin(); i != myParamWindows.end(); ++i) {
        (*i)->removeObject(this);
    }
    GUIGlObjectStorage::gIDStorage.remove(getGlID());
}


const std::string&
GUIGlObject::getFullName() const {
    return myFullName;
}


const std::string&
GUIGlObject::getParentName() const {
    return StringUtils::emptyString;
}


GUIGlID
GUIGlObject::getGlID() const {
    return myGlID;
}


GUIParameterTableWindow*
GUIGlObject::getTypeParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    UNUSED_PARAMETER(&app);
    UNUSED_PARAMETER(&parent);
    return 0;
}


const std::string&
GUIGlObject::getMicrosimID() const {
    return myMicrosimID;
}


void
GUIGlObject::setMicrosimID(const std::string& newID) {
    myMicrosimID = newID;
    myFullName = createFullName();
}


GUIGlObjectType
GUIGlObject::getType() const {
    return myGLObjectType;
}


void
GUIGlObject::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    UNUSED_PARAMETER(&s);
    UNUSED_PARAMETER(parent);
}

#ifdef HAVE_OSG

osg::Node*
GUIGlObject::getNode() const {
    return myOSGNode;
}


void
GUIGlObject::setNode(osg::Node* node) {
    myOSGNode = node;
}

#endif

void
GUIGlObject::buildPopupHeader(GUIGLObjectPopupMenu* ret, GUIMainWindow& app,
                              bool addSeparator) {
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildCenterPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator) {
    new FXMenuCommand(ret, "Center", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildNameCopyPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator) {
    new FXMenuCommand(ret, "Copy name to clipboard", 0, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, "Copy typed name to clipboard", 0, ret, MID_COPY_TYPED_NAME);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildSelectionPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator) {
    if (gSelected.isSelected(getType(), getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected", GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected", GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildShowParamsPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator) {
    new FXMenuCommand(ret, "Show Parameter", GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildShowTypeParamsPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator) {
    new FXMenuCommand(ret, "Show Type Parameter", GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWTYPEPARS);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildPositionCopyEntry(GUIGLObjectPopupMenu* ret, bool addSeparator) {
    new FXMenuCommand(ret, "Copy cursor position to clipboard", 0, ret, MID_COPY_CURSOR_POSITION);
    if (GeoConvHelper::getFinal().usingGeoProjection()) {
        new FXMenuCommand(ret, "Copy cursor geo-position to clipboard", 0, ret, MID_COPY_CURSOR_GEOPOSITION);
    }
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildShowManipulatorPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator) {
    new FXMenuCommand(ret, "Open Manipulator...", GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::addParameterTable(GUIParameterTableWindow* t) {
    myParamWindows.insert(t);
}


void
GUIGlObject::removeParameterTable(GUIParameterTableWindow* t) {
    std::set<GUIParameterTableWindow*>::iterator i = myParamWindows.find(t);
    if (i != myParamWindows.end()) {
        myParamWindows.erase(i);
    }
}


void
GUIGlObject::setPrefix(const std::string& prefix) {
    myPrefix = prefix;
    myFullName = createFullName();
}


void
GUIGlObject::buildShapePopupOptions(GUIMainWindow& app, GUIGLObjectPopupMenu* ret, const std::string& type) {
    assert(ret);
    // build header (<tag>:<ID>
    buildPopupHeader(ret, app, false);
    // build center
    buildCenterPopupEntry(ret);
    // build copy name
    buildNameCopyPopupEntry(ret);
    // build select/unselect
    buildSelectionPopupEntry(ret);
    // build show parameters
    buildShowParamsPopupEntry(ret, false);
    // build copy cursor position to clipboard
    buildPositionCopyEntry(ret, false);
    // only show type if isn't empty
    if (type != "") {
        new FXMenuCommand(ret, ("type: " + type + "").c_str(), 0, 0, 0);
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildAdditionalsPopupOptions(GUIMainWindow& app, GUIGLObjectPopupMenu* ret, const std::string& type) {
    assert(ret);
    // build header (<tag>:<ID>
    buildPopupHeader(ret, app, false);
    // build center
    buildCenterPopupEntry(ret);
    // build copy name
    buildNameCopyPopupEntry(ret);
    // build select/unselect
    buildSelectionPopupEntry(ret);
    // build show parameters
    buildShowParamsPopupEntry(ret, false);
    // build copy cursor position to clipboard
    buildPositionCopyEntry(ret, false);
    // only show type if isn't empty
    if (type != "") {
        new FXMenuCommand(ret, ("type: " + type + "").c_str(), 0, 0, 0);
        new FXMenuSeparator(ret);
    }
}


std::string
GUIGlObject::createFullName() const {
    return myPrefix + ":" + getMicrosimID();
}


void
GUIGlObject::drawName(const Position& pos, const double scale, const GUIVisualizationTextSettings& settings, const double angle) const {
    if (settings.show) {
        GLHelper::drawText(getMicrosimID(), pos, GLO_MAX, settings.size / scale, settings.color, angle);
    }
}

/****************************************************************************/

