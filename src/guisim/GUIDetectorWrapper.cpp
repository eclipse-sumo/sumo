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
/// @file    GUIDetectorWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
///
// The base class for detector wrapper
/****************************************************************************/
#include <config.h>

#include "GUIDetectorWrapper.h"
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDetectorWrapper::PopupMenu) GUIDetectorWrapperPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_VIRTUAL_DETECTOR, GUIDetectorWrapper::PopupMenu::onCmdSetOverride),
};

// Object implementation
FXIMPLEMENT(GUIDetectorWrapper::PopupMenu, GUIGLObjectPopupMenu, GUIDetectorWrapperPopupMenuMap, ARRAYNUMBER(GUIDetectorWrapperPopupMenuMap))

// ===========================================================================
// member method definitions
// ===========================================================================
GUIDetectorWrapper::GUIDetectorWrapper(GUIGlObjectType type, const std::string& id, FXIcon* icon) :
    GUIGlObject_AbstractAdd(type, id, icon),
    mySupportsOverride(false)
{}


GUIDetectorWrapper::~GUIDetectorWrapper() {}


GUIGLObjectPopupMenu*
GUIDetectorWrapper::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new PopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    if (mySupportsOverride) {
        new FXMenuSeparator(ret);
        if (haveOverride()) {
            GUIDesigns::buildFXMenuCommand(ret, "Reset override", nullptr, ret, MID_VIRTUAL_DETECTOR);
        } else {
            GUIDesigns::buildFXMenuCommand(ret, "Override detection", nullptr, ret, MID_VIRTUAL_DETECTOR);
        }
    }
    return ret;
}


double
GUIDetectorWrapper::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


/* -------------------------------------------------------------------------
 * GUIDetectorWrapper::PopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIDetectorWrapper::PopupMenu::PopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o) :
    GUIGLObjectPopupMenu(app, parent, o) {
}


long
GUIDetectorWrapper::PopupMenu::onCmdSetOverride(FXObject*, FXSelector, void*) {
    dynamic_cast<GUIDetectorWrapper*>(myObject)->toggleOverride();
    myParent->update();
    return 1;
}


/****************************************************************************/
