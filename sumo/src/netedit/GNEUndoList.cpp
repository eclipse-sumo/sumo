/****************************************************************************/
/// @file    GNEUndoList.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// FXUndoList is pretty dandy but some features are missing:
//   - we cannot find out wether we have currently begun an undo-group and
//     thus abort() is hard to use.
//   - onUpd-methods do not disable undo/redo while in an undo-group
//
// GNEUndoList inherits from FXUndoList and patches some methods. these are
// prefixed with p_
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/UtilExceptions.h>
#include "GNEUndoList.h"
#include "GNEChange_Attribute.h"
#include "GNEApplicationWindow.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEUndoList) GNEUndoListMap[] = {
    //FXMAPFUNC(SEL_COMMAND, FXUndoList::ID_REVERT,     FXUndoList::onCmdRevert),
    //FXMAPFUNC(SEL_COMMAND, FXUndoList::ID_UNDO,       FXUndoList::onCmdUndo),
    //FXMAPFUNC(SEL_COMMAND, FXUndoList::ID_REDO,       FXUndoList::onCmdRedo),
    //FXMAPFUNC(SEL_COMMAND, FXUndoList::ID_UNDO_ALL,   FXUndoList::onCmdUndoAll),
    //FXMAPFUNC(SEL_COMMAND, FXUndoList::ID_REDO_ALL,   FXUndoList::onCmdRedoAll),
    //
    //FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_UNDO_COUNT, FXUndoList::onUpdUndoCount),
    //FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_REDO_COUNT, FXUndoList::onUpdRedoCount),
    //FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_CLEAR,      FXUndoList::onUpdClear),
    //FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_REVERT,     FXUndoList::onUpdRevert),
    FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_UNDO_ALL,   GNEUndoList::p_onUpdUndo),
    FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_REDO_ALL,   GNEUndoList::p_onUpdRedo),
    FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_UNDO,       GNEUndoList::p_onUpdUndo),
    FXMAPFUNC(SEL_UPDATE,  FXUndoList::ID_REDO,       GNEUndoList::p_onUpdRedo)
};


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEUndoList, FXUndoList, GNEUndoListMap, ARRAYNUMBER(GNEUndoListMap))


// ===========================================================================
// member method definitions
// ===========================================================================

GNEUndoList::GNEUndoList(GNEApplicationWindow* parent) :
    FXUndoList(),
    myParent(parent) {
}


void
GNEUndoList::p_begin(const std::string& description) {
    myCommandGroups.push(new CommandGroup(description));
    begin(myCommandGroups.top());
}


void
GNEUndoList::p_end() {
    myCommandGroups.pop();
    end();
}


void
GNEUndoList::p_clear() {
    p_abort();
    clear();
}


void
GNEUndoList::p_abort() {
    while (hasCommandGroup()) {
        myCommandGroups.top()->undo();
        myCommandGroups.pop();
        abort();
    }
}


void
GNEUndoList::undo() {
    //std::cout << undoName().text() << "\n";
    FXUndoList::undo();
    myParent->updateControls();
}


void
GNEUndoList::redo() {
    //std::cout << redoName().text() << "\n";
    FXUndoList::redo();
    myParent->updateControls();
}


void
GNEUndoList::p_add(GNEChange_Attribute* cmd) {
    if (cmd->trueChange()) {
        add(cmd, true);
    } else {
        delete cmd;
    }
}

long
GNEUndoList::p_onUpdUndo(FXObject* sender, FXSelector, void*) {
    bool enable = canUndo() && !hasCommandGroup();
    sender->handle(this, enable ? FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE) : FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), 0);
    FXString caption = undoName();
    if (hasCommandGroup()) {
        caption = ("Cannot Undo in the middle of " + myCommandGroups.top()->getDescription()).c_str();
    } else if (!canUndo()) {
        caption = "Undo";
    }
    // only set caption on menu item
    if (dynamic_cast<FXMenuCommand*>(sender)) {
        sender->handle(this, FXSEL(SEL_COMMAND, FXMenuCaption::ID_SETSTRINGVALUE), (void*)&caption);
    }
    return 1;
}


long
GNEUndoList::p_onUpdRedo(FXObject* sender, FXSelector, void*) {
    bool enable = canRedo() && !hasCommandGroup();
    sender->handle(this, enable ? FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE) : FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), 0);
    FXString caption = redoName();
    if (hasCommandGroup()) {
        caption = ("Cannot Redo in the middle of " + myCommandGroups.top()->getDescription()).c_str();
    } else if (!canRedo()) {
        caption = "Redo";
    }
    // only set caption on menu item
    if (dynamic_cast<FXMenuCommand*>(sender)) {
        sender->handle(this, FXSEL(SEL_COMMAND, FXMenuCaption::ID_SETSTRINGVALUE), (void*)&caption);
    }
    return 1;
}


bool
GNEUndoList::hasCommandGroup() const {
    return myCommandGroups.size() != 0;
}


GNEUndoList::CommandGroup::CommandGroup(std::string description) :
    myDescription(description) {
}


const std::string&
GNEUndoList::CommandGroup::getDescription() {
    return myDescription;
}


FXString
GNEUndoList::CommandGroup::undoName() const {
    return ("Undo " + myDescription).c_str();
}


FXString
GNEUndoList::CommandGroup::redoName() const {
    return ("Redo " + myDescription).c_str();
}
