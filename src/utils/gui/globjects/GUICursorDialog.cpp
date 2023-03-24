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
/// @file    GUICursorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2022
///
// Dialog for edit element under cursor
/****************************************************************************/
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include <netedit/GNEUndoList.h>

#include "GUICursorDialog.h"


#define NUM_VISIBLE_ITEMS 10

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GUICursorDialog) GUICursorDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_SETFRONTELEMENT,   GUICursorDialog::onCmdSetFrontElement),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_DELETEELEMENT,     GUICursorDialog::onCmdDeleteElement),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_SELECTELEMENT,     GUICursorDialog::onCmdSelectElement),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_PROPERTIES,        GUICursorDialog::onCmdOpenPropertiesPopUp),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_MOVEUP,            GUICursorDialog::onCmdMoveListUp),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_MOVEDOWN,          GUICursorDialog::onCmdMoveListDown),
    FXMAPFUNC(SEL_COMMAND,  MID_CURSORDIALOG_FRONT,             GUICursorDialog::onCmdProcessFront),
    FXMAPFUNC(SEL_COMMAND,  FXWindow::ID_UNPOST,                GUICursorDialog::onCmdUnpost),
};

// Object implementation
FXIMPLEMENT(GUICursorDialog, GUIGLObjectPopupMenu, GUICursorDialogMap, ARRAYNUMBER(GUICursorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GUICursorDialog::GUICursorDialog(GUIGLObjectPopupMenu::PopupType type, GUISUMOAbstractView* view, const std::vector<GUIGlObject*>& objects) :
    GUIGLObjectPopupMenu(view->getMainWindow(), view, type),
    myType(type),
    myView(view) {
    // continue depending of properties
    if (type == GUIGLObjectPopupMenu::PopupType::PROPERTIES) {
        buildDialogElements(view, "Overlapped objects", GUIIcon::MODEINSPECT, MID_CURSORDIALOG_PROPERTIES, objects);
    } else if (type == GUIGLObjectPopupMenu::PopupType::DELETE_ELEMENT) {
        buildDialogElements(view, "Delete element", GUIIcon::MODEDELETE, MID_CURSORDIALOG_DELETEELEMENT, objects);
    } else if (type == GUIGLObjectPopupMenu::PopupType::SELECT_ELEMENT) {
        buildDialogElements(view, "Select element", GUIIcon::MODESELECT, MID_CURSORDIALOG_SELECTELEMENT, objects);
    } else if (type == GUIGLObjectPopupMenu::PopupType::FRONT_ELEMENT) {
        buildDialogElements(view, "Mark front element", GUIIcon::FRONTELEMENT, MID_CURSORDIALOG_SETFRONTELEMENT, objects);
    }
}


GUICursorDialog::~GUICursorDialog() {
    // delete all menu commands
    for (const auto& GLObject : myMenuCommandGLObjects) {
        delete GLObject.first;
    }
}


long
GUICursorDialog::onCmdSetFrontElement(FXObject* obj, FXSelector, void*) {
    // search element in myGLObjects
    for (const auto& GLObject : myMenuCommandGLObjects) {
        if (GLObject.first == obj) {
            GLObject.second->markAsFrontElement();
        }
    }
    // destroy popup
    myView->destroyPopup();
    return 1;
}


long
GUICursorDialog::onCmdDeleteElement(FXObject* obj, FXSelector, void*) {
    // search element in myGLObjects
    for (const auto& GLObject : myMenuCommandGLObjects) {
        if (GLObject.first == obj) {
            GLObject.second->deleteGLObject();
        }
    }
    // destroy popup
    myView->destroyPopup();
    return 1;
}


long
GUICursorDialog::onCmdSelectElement(FXObject* obj, FXSelector, void*) {
    // search element in myGLObjects
    for (const auto& GLObject : myMenuCommandGLObjects) {
        if (GLObject.first == obj) {
            GLObject.second->selectGLObject();
        }
    }
    // destroy popup
    myView->destroyPopup();
    return 1;
}


long
GUICursorDialog::onCmdOpenPropertiesPopUp(FXObject* obj, FXSelector, void*) {
    // search element in myGLObjects
    for (const auto& GLObject : myMenuCommandGLObjects) {
        if (GLObject.first == obj) {
            myView->replacePopup(GLObject.second->getPopUpMenu(*myView->getMainWindow(), *myView));
            return 1;
        }
    }
    return 0;
}


long
GUICursorDialog::onCmdMoveListUp(FXObject*, FXSelector, void*) {
    myListIndex -= NUM_VISIBLE_ITEMS;
    updateList();
    show();
    return 0;
}


long
GUICursorDialog::onCmdMoveListDown(FXObject*, FXSelector, void*) {
    myListIndex += NUM_VISIBLE_ITEMS;
    updateList();
    show();
    return 0;
}


long
GUICursorDialog::onCmdProcessFront(FXObject*, FXSelector, void*) {
    if (myMenuCommandGLObjects.size() > 0) {
        // continue depending of properties
        if (myType == GUIGLObjectPopupMenu::PopupType::DELETE_ELEMENT) {
            myMenuCommandGLObjects.front().second->deleteGLObject();
        } else if (myType == GUIGLObjectPopupMenu::PopupType::SELECT_ELEMENT) {
            myMenuCommandGLObjects.front().second->selectGLObject();
        } else if (myType == GUIGLObjectPopupMenu::PopupType::FRONT_ELEMENT) {
            myMenuCommandGLObjects.front().second->markAsFrontElement();
        }
    }
    return 0;
}


long
GUICursorDialog::onCmdUnpost(FXObject* obj, FXSelector, void* ptr) {
    // ignore move up, down and header
    if ((obj == myMoveUpMenuCommand) || (obj == myMoveDownMenuCommand) || (obj == myMenuHeader)) {
        return 1;
    }
    if (grabowner) {
        grabowner->handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), ptr);
    } else {
        popdown();
        if (grabbed()) {
            ungrab();
        }
    }
    return 1;
}


void
GUICursorDialog::updateList() {
    // first hide all menu commands
    for (const auto& GLObject : myMenuCommandGLObjects) {
        GLObject.first->hide();
    }
    // check if disable menu command up
    if (myListIndex == 0) {
        myMoveUpMenuCommand->disable();
    } else {
        myMoveUpMenuCommand->enable();
    }
    // show menu commands depending of myListIndex
    if ((myListIndex + NUM_VISIBLE_ITEMS) > (int)myMenuCommandGLObjects.size()) {
        for (int i = (int)myMenuCommandGLObjects.size() - NUM_VISIBLE_ITEMS; i < (int)myMenuCommandGLObjects.size(); i++) {
            myMenuCommandGLObjects.at(i).first->show();
        }
        myMoveDownMenuCommand->disable();
    } else {
        for (int i = myListIndex; i < (myListIndex + NUM_VISIBLE_ITEMS); i++) {
            myMenuCommandGLObjects.at(i).first->show();
        }
        myMoveDownMenuCommand->enable();
    }
    // recalc popup
    recalc();
}


void
GUICursorDialog::buildDialogElements(GUISUMOAbstractView* view, const FXString text, GUIIcon icon, FXSelector sel, const std::vector<GUIGlObject*>& objects) {
    // create header
    myMenuHeader = new MFXMenuHeader(this, view->getMainWindow()->getBoldFont(), text, GUIIconSubSys::getIcon(icon), nullptr, 0);
    new FXMenuSeparator(this);
    // check if create move up menu command
    if (objects.size() > NUM_VISIBLE_ITEMS) {
        myMoveUpMenuCommand = GUIDesigns::buildFXMenuCommand(this, "Previous", GUIIconSubSys::getIcon(GUIIcon::ARROW_UP), this, MID_CURSORDIALOG_MOVEUP);
        new FXMenuSeparator(this);
    }
    // create a menu command for every object
    for (const auto& GLObject : objects) {
        myMenuCommandGLObjects.push_back(std::make_pair(GUIDesigns::buildFXMenuCommand(this, GLObject->getMicrosimID(), GLObject->getGLIcon(), this, sel), GLObject));
    }
    // check if create move down menu command
    if (objects.size() > NUM_VISIBLE_ITEMS) {
        new FXMenuSeparator(this);
        myMoveDownMenuCommand = GUIDesigns::buildFXMenuCommand(this, "Next", GUIIconSubSys::getIcon(GUIIcon::ARROW_DOWN), this, MID_CURSORDIALOG_MOVEDOWN);
        updateList();
    }
}

/****************************************************************************/
