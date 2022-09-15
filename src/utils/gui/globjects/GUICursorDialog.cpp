/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GUICursorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2022
///
// Dialog for edit element under cursor
/****************************************************************************/
#include <config.h>

#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include <netedit/GNEUndoList.h>

#include "GUICursorDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GUICursorDialog) GUICursorDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_SETFRONTELEMENT,   GUICursorDialog::onCmdSetFrontElement),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_PROPERTIES,        GUICursorDialog::onCmdOpenPropertiesPopUp)
};

// Object implementation
FXIMPLEMENT(GUICursorDialog, GUIGLObjectPopupMenu, GUICursorDialogMap, ARRAYNUMBER(GUICursorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GUICursorDialog::GUICursorDialog(CursorDialogType cursorDialogType, GUISUMOAbstractView* view, const std::vector<GUIGlObject*> &objects) :
    GUIGLObjectPopupMenu(view->getMainWindow(), view),
    myView(view) {
    // continue depending of number of objects
    if (objects.empty()) {
        // create header
        new MFXMenuHeader(this, view->getMainWindow()->getBoldFont(), "No objects under cursor", nullptr, nullptr, 0);
    } else if (cursorDialogType == CursorDialogType::PROPERTIES) {
        // create header
        new MFXMenuHeader(this, view->getMainWindow()->getBoldFont(), "Properties", GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), nullptr, 0);
        new FXMenuSeparator(this);
        // create a menu command for every AC
        for (const auto &GLObject : objects) {
            auto menuCommand = GUIDesigns::buildFXMenuCommand(this, GLObject->getMicrosimID(), GLObject->getIcon(), this, MID_CURSORDIALOG_PROPERTIES);
            myGLObjects[menuCommand] = GLObject;
            // disable edge
            if (GLObject->getType() == GLO_EDGE) {
                menuCommand->disable();
            }
        }
    } else if (cursorDialogType == CursorDialogType::FRONT_ELEMENT) {
        // create header
        new MFXMenuHeader(this, view->getMainWindow()->getBoldFont(), "Mark front element", GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), nullptr, 0);
        new FXMenuSeparator(this);
        // create a menu command for every AC
        for (const auto &GLObject : objects) {
            myGLObjects[GUIDesigns::buildFXMenuCommand(this, GLObject->getMicrosimID(), GLObject->getIcon(), this, MID_CURSORDIALOG_SETFRONTELEMENT)] = GLObject;
        }
    }
}


GUICursorDialog::~GUICursorDialog() {
    for (const auto &GLObject : myGLObjects) {
        delete GLObject.first;
    }
}


long
GUICursorDialog::onCmdSetFrontElement(FXObject* obj, FXSelector, void*) {
    // search element in myGLObjects
    for (const auto &GLObject : myGLObjects) {
        if (GLObject.first == obj) {
            GLObject.second->markAsFrontElement();
        }
    }
    // destroy popup
    myView->destroyPopup();
    return 1;
}


long 
GUICursorDialog::onCmdOpenPropertiesPopUp(FXObject* obj, FXSelector, void*) {
    // search element in myGLObjects
    for (const auto &GLObject : myGLObjects) {
        if (GLObject.first == obj) {
            myView->replacePopup(GLObject.second->getPopUpMenu(*myView->getMainWindow(), *myView));
            return 1;
        }
    }
    return 0;
}


/****************************************************************************/
