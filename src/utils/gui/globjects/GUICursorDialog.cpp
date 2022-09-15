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
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_PROPERTIES,        GUICursorDialog::onCmdOpenPropertiesPopUp),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_MOVEUP,            GUICursorDialog::onCmdMoveListUp),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_MOVEDOWN,          GUICursorDialog::onCmdMoveListDown)
};

// Object implementation
FXIMPLEMENT(GUICursorDialog, GUIGLObjectPopupMenu, GUICursorDialogMap, ARRAYNUMBER(GUICursorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GUICursorDialog::GUICursorDialog(CursorDialogType cursorDialogType, GUISUMOAbstractView* view, const std::vector<GUIGlObject*> &objects) :
    GUIGLObjectPopupMenu(view->getMainWindow(), view),
    myView(view) {
    // continue depending of properties
    if (cursorDialogType == CursorDialogType::PROPERTIES) {
        // create header
        new MFXMenuHeader(this, view->getMainWindow()->getBoldFont(), "Overlapped objects", GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), nullptr, 0);
        new FXMenuSeparator(this);
        // check if create move up menu command
        if (objects.size() > 5) {
            myMoveUpMenuCommand = GUIDesigns::buildFXMenuCommand(this, "Previous", GUIIconSubSys::getIcon(GUIIcon::ARROW_UP), this, MID_CURSORDIALOG_MOVEUP);
            new FXMenuSeparator(this);
        }
        // create a menu command for every object
        for (const auto &GLObject : objects) {
            myMenuCommandGLObjects.push_back(std::make_pair(GUIDesigns::buildFXMenuCommand(this, GLObject->getMicrosimID(), GLObject->getIcon(), this, MID_CURSORDIALOG_PROPERTIES), GLObject));
        }
    } else if (cursorDialogType == CursorDialogType::FRONT_ELEMENT) {
        // create header
        new MFXMenuHeader(this, view->getMainWindow()->getBoldFont(), "Mark front element", GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), nullptr, 0);
        new FXMenuSeparator(this);
        // check if create move up menu command
        if (objects.size() > 5) {
            myMoveUpMenuCommand = GUIDesigns::buildFXMenuCommand(this, "Previous", GUIIconSubSys::getIcon(GUIIcon::ARROW_UP), this, MID_CURSORDIALOG_MOVEUP);
            new FXMenuSeparator(this);
        }
        // create a menu command for every object
        for (const auto &GLObject : objects) {
            myMenuCommandGLObjects.push_back(std::make_pair(GUIDesigns::buildFXMenuCommand(this, GLObject->getMicrosimID(), GLObject->getIcon(), this, MID_CURSORDIALOG_SETFRONTELEMENT), GLObject));
        }
    }
    // check if create move down menu command
    if (objects.size() > 5) {
        new FXMenuSeparator(this);
        myMoveDownMenuCommand = GUIDesigns::buildFXMenuCommand(this, "Next", GUIIconSubSys::getIcon(GUIIcon::ARROW_DOWN), this, MID_CURSORDIALOG_MOVEDOWN);
        updateList();
    }
}


GUICursorDialog::~GUICursorDialog() {
    // delete all menu commands
    for (const auto &GLObject : myMenuCommandGLObjects) {
        delete GLObject.first;
    }
}


long
GUICursorDialog::onCmdSetFrontElement(FXObject* obj, FXSelector, void*) {
    // search element in myGLObjects
    for (const auto &GLObject : myMenuCommandGLObjects) {
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
    for (const auto &GLObject : myMenuCommandGLObjects) {
        if (GLObject.first == obj) {
            myView->replacePopup(GLObject.second->getPopUpMenu(*myView->getMainWindow(), *myView));
            return 1;
        }
    }
    return 0;
}


long
GUICursorDialog::onCmdMoveListUp(FXObject*, FXSelector, void*) {
    myListIndex -= 5;
    updateList();
    show();
    return 0;
}


long
GUICursorDialog::onCmdMoveListDown(FXObject*, FXSelector, void*) {
    myListIndex += 5;
    updateList();
    show();
    return 0;
}


void 
GUICursorDialog::updateList() {
    // first hide all menu commands
    for (const auto &GLObject : myMenuCommandGLObjects) {
        GLObject.first->hide();
    }
    // recalc popup
    recalc();
    // show menu commands depending of myListIndex
    for (int i = myListIndex; (i < (myListIndex + 5)) && (i < (int)myMenuCommandGLObjects.size()); i++) {
        myMenuCommandGLObjects.at(i).first->show();
    }
    // check if disable menu command up
    if (myListIndex == 0) {
        myMoveUpMenuCommand->disable();
    } else {
        myMoveUpMenuCommand->enable();
    }
    // check if disable menu command down
    if ((myListIndex + 5) > (int)myMenuCommandGLObjects.size()) {
        myMoveDownMenuCommand->disable();
    } else {
        myMoveDownMenuCommand->enable();
    }
    // recalc popup
    recalc();
}

/****************************************************************************/
