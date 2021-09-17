/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEUndoList.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2011
///
/****************************************************************************/
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/common/GNESelectorFrame.h>

#include "GNEApplicationWindow.h"
#include "GNEUndoList.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEUndoList) GNEUndoListMap[] = {
    FXMAPFUNC(SEL_COMMAND, GNEUndoList::ID_CLEAR,      GNEUndoList::onCmdClear), 
    FXMAPFUNC(SEL_UPDATE,  GNEUndoList::ID_CLEAR,      GNEUndoList::onUpdClear), 
    FXMAPFUNC(SEL_COMMAND, GNEUndoList::ID_UNDO,       GNEUndoList::onCmdUndo), 
    FXMAPFUNC(SEL_UPDATE,  GNEUndoList::ID_UNDO,       GNEUndoList::onUpdUndo), 
    FXMAPFUNC(SEL_COMMAND, GNEUndoList::ID_REDO,       GNEUndoList::onCmdRedo), 
    FXMAPFUNC(SEL_UPDATE,  GNEUndoList::ID_REDO,       GNEUndoList::onUpdRedo), 
    FXMAPFUNC(SEL_COMMAND, GNEUndoList::ID_UNDO_ALL,   GNEUndoList::onCmdUndoAll), 
    FXMAPFUNC(SEL_UPDATE,  GNEUndoList::ID_UNDO_ALL,   GNEUndoList::onUpdUndo), 
    FXMAPFUNC(SEL_COMMAND, GNEUndoList::ID_REDO_ALL,   GNEUndoList::onCmdRedoAll), 
    FXMAPFUNC(SEL_UPDATE,  GNEUndoList::ID_REDO_ALL,   GNEUndoList::onUpdRedo)
};



// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEUndoList, GNEChangeGroup, GNEUndoListMap, ARRAYNUMBER(GNEUndoListMap))


// ===========================================================================
// member method definitions
// ===========================================================================

GNEUndoList::GNEUndoList(GNEApplicationWindow* parent) :
    myWorking(false),
    myGNEApplicationWindowParent(parent) {
}


GNEUndoList::~GNEUndoList() {}


void
GNEUndoList::undo() {
    WRITE_DEBUG("Calling GNEUndoList::undo()");
    register GNEChange *change;
    if (group) { 
        throw ProcessError("GNEChangeGroup::undo: cannot call undo inside begin-end block");
    }
    if (undoList) {
        myWorking = true;
        change = undoList;  
        // Remove from undoList BEFORE undo
        undoList = undoList->next;
        change->undo();
        // Hang into redoList AFTER undo
        change->next = redoList;             
        redoList = change;
        myWorking = false;
    }
    // update specific controls
    myGNEApplicationWindowParent->updateControls();
}


void
GNEUndoList::redo() {
    WRITE_DEBUG("Calling GNEUndoList::redo()");
    register GNEChange *change;
    if (group) { 
        throw ProcessError("GNEChangeGroup::redo: cannot call undo inside begin-end block"); 
    }
    if (redoList) {
        myWorking = true;
        change = redoList;    
        // Remove from redoList BEFORE redo
        redoList = redoList->next;
        change->redo();
        // Hang into undoList AFTER redo
        change->next = undoList;             
        undoList = change;
        myWorking = false;
    }
    // update specific controls
    myGNEApplicationWindowParent->updateControls();
}


std::string 
GNEUndoList::undoName() const {
    if (undoList) {
        return undoList->undoName();
    } else {
        return "";
    }
}


std::string 
GNEUndoList::redoName() const {
    if (redoList) {
        return redoList->redoName();
    } else {
        return "";
    }
}


void
GNEUndoList::begin(const std::string& description) {
    myCommandGroups.push(new GNEChangeGroup(description));
    // get this reference
    register GNEChangeGroup* g = this;
    // Calling begin while in the middle of doing something!
    if (myWorking) { 
        throw ProcessError("GNEChangeGroup::begin: already working on undo or redo"); 
    }
    // Cut redo list
    cut();
    // Hunt for end of group chain
    while (g->group) { 
        g = g->group;
    }
    // Add to end
    g->group = myCommandGroups.top();
}


void
GNEUndoList::end() {
    myCommandGroups.pop();
    // check if net has to be updated
    if (myCommandGroups.empty() && myGNEApplicationWindowParent->getViewNet()) {
        myGNEApplicationWindowParent->getViewNet()->updateViewNet();
        // check if we have to update selector frame
        const auto &editModes = myGNEApplicationWindowParent->getViewNet()->getEditModes();
        if ((editModes.isCurrentSupermodeNetwork() && editModes.networkEditMode == NetworkEditMode::NETWORK_SELECT) ||
            (editModes.isCurrentSupermodeDemand() && editModes.demandEditMode == DemandEditMode::DEMAND_SELECT) ||
            (editModes.isCurrentSupermodeData() && editModes.dataEditMode == DataEditMode::DATA_SELECT)) {
            myGNEApplicationWindowParent->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionInformation()->updateInformationLabel();
        }
    }
    // continue with end
    register GNEChangeGroup *change;
    register GNEChangeGroup *g = this;
    // Must have called begin
    if (!g->group) { 
        throw ProcessError("GNEChangeGroup::end: no matching call to begin"); 
    }
    // Calling end while in the middle of doing something!
    if (myWorking) {
        throw ProcessError("GNEChangeGroup::end: already working on undo or redo"); 
    }
    // Hunt for one above end of group chain
    while (g->group->group) {
        g = g->group;
    }
    // Unlink from group chain
    change = g->group;
    g->group = nullptr;
    // Add to group if non-empty
    if (!change->empty()) {
        // Append new change to undo list
        change->next = g->undoList;
        g->undoList = change;
    } else {
        // Delete bottom group
        delete change;
    }
}


void
GNEUndoList::p_clear() {
    // disable updating of interval bar (check viewNet due #7252)
    if (myGNEApplicationWindowParent->getViewNet()) {
        myGNEApplicationWindowParent->getViewNet()->getIntervalBar().disableIntervalBarUpdate();
    }
    p_abort();
    clear();
    // enable updating of interval bar again (check viewNet due #7252)
    if (myGNEApplicationWindowParent->getViewNet()) {
        myGNEApplicationWindowParent->getViewNet()->getIntervalBar().enableIntervalBarUpdate();
    }
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
GNEUndoList::p_abortLastCommandGroup() {
    if (myCommandGroups.size() > 0) {
        myCommandGroups.top()->undo();
        myCommandGroups.pop();
        abort();
    }
}


void 
GNEUndoList::add(GNEChange* change, bool doit, bool merge) {
    register GNEChangeGroup* g = this;
    register FXuint size = 0;
    // Must pass a change
    if (!change) { 
        throw ProcessError("GNEChangeGroup::add: nullptr change argument");
    }
    // Adding undo while in the middle of doing something!
    if (myWorking) { 
        throw ProcessError("GNEChangeGroup::add: already working on undo or redo"); 
    }
    myWorking = true;
    // Cut redo list
    cut();
    // Execute change
    if (doit) {
        change->redo();
    }
    // Hunt for end of group chain
    while (g->group) { 
        g = g->group; 
    }
    // Old size of previous record
    if (g->undoList) {
        size = g->undoList->size();
    }
    // Try to merge commands when desired and possible
    if (merge && g->undoList && (group != nullptr) && change->canMerge() && g->undoList->mergeWith(change)) {
        // Delete incoming change that was merged
        delete change;
    } else {
        // Append incoming change
        change->next = g->undoList;
        g->undoList = change;
    }
    myWorking = false;
}


void
GNEUndoList::changeAttribute(GNEChange_Attribute* change) {
    if (change->trueChange()) {
        add(change, true);
    } else {
        delete change;
    }
}


int
GNEUndoList::currentCommandGroupSize() const {
    if (myCommandGroups.size() > 0) {
        return myCommandGroups.top()->size();
    } else {
        return 0;
    }
}


const GNEChange* 
GNEUndoList::getlastChange() const {
    if (myCommandGroups.empty()) {
        return nullptr;
    } else {
        const GNEChange* change = dynamic_cast<GNEChange*>(myCommandGroups.top());
        if (change) {
            return change;
        } else {
            return nullptr;
        }
    }
}


bool
GNEUndoList::hasCommandGroup() const {
    return myCommandGroups.size() != 0;
}



/*******************/




void 
GNEUndoList::cut() {
    register GNEChange *change;
    while (redoList) {
        change = redoList;
        redoList = redoList->next;
        delete change;
    }
    redoList = nullptr;
}


void 
GNEUndoList::abort() {
    register GNEChangeGroup *g = this;
    // Must be called after begin
    if (!g->group) { 
        throw ProcessError("GNEChangeGroup::abort: no matching call to begin");
    }
    // Calling abort while in the middle of doing something!
    if (myWorking) { 
        throw ProcessError("GNEChangeGroup::abort: already working on undo or redo"); 
    }
    // Hunt for one above end of group chain
    while (g->group->group) {
        g = g->group; 
    }
    // Delete bottom group
    delete g->group;
    // New end of chain
    g->group = nullptr;
}


void 
GNEUndoList::undoAll() {
    while (canUndo()) {
        undo();
    }
}


void
GNEUndoList::redoAll() {
    while (canRedo()) {
        redo();
    }
}


bool
GNEUndoList::canUndo() const {
    return undoList != nullptr;
}


bool 
GNEUndoList::canRedo() const {
    return redoList != nullptr;
}


bool 
GNEUndoList::busy() const { 
    return myWorking; 
}


GNEChange* 
GNEUndoList::current() const {
    return undoList;
}


void 
GNEUndoList::clear() {
    register GNEChange *change;
    while (redoList) {
        change = redoList;
        redoList = redoList->next;
        delete change;
    }
    while (undoList) {
        change = undoList;
        undoList = undoList->next;
        delete change;
    }
    delete group;
    redoList = nullptr;
    undoList = nullptr;
    group = nullptr;
}


long 
GNEUndoList::onCmdClear(FXObject*, FXSelector, void*) {
    clear();
    return 1;
}


long 
GNEUndoList::onUpdClear(FXObject* sender, FXSelector, void*) {
    sender->handle(this, (canUndo()||canRedo())?FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE):FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    return 1;
}


long
GNEUndoList::onCmdUndo(FXObject*, FXSelector, void*) {
    undo();
    return 1;
}


long 
GNEUndoList::onCmdUndoAll(FXObject*, FXSelector, void*) {
    undoAll();
    return 1;
}


long 
GNEUndoList::onUpdUndo(FXObject* sender, FXSelector, void*) {
    // first check if Undo Menu command or button has to be disabled
    const bool enable = canUndo() && !hasCommandGroup() && myGNEApplicationWindowParent->isUndoRedoEnabled().empty();
    // cast button (see #6209)
    const FXButton* button = dynamic_cast<FXButton*>(sender);
    // enable or disable depending of "enable" flag
    if (button) {
        // avoid unnnecesary enables/disables (due flickering)
        if (enable && !button->isEnabled()) {
            sender->handle(this, FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE), nullptr);
            button->update();
        } else if (!enable && button->isEnabled()) {
            sender->handle(this, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
            button->update();
        }
    } else {
        sender->handle(this, enable ? FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE) : FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    }
    // cast menu command
    FXMenuCommand* menuCommand = dynamic_cast<FXMenuCommand*>(sender);
    // only set caption on menu command item
    if (menuCommand) {
        // change caption of FXMenuCommand
        std::string caption = undoName();
        // set caption of FXmenuCommand edit/undo
        if (myGNEApplicationWindowParent->isUndoRedoEnabled().size() > 0) {
            caption = "Cannot Undo in the middle of " + myGNEApplicationWindowParent->isUndoRedoEnabled();
        } else if (hasCommandGroup()) {
            caption = "Cannot Undo in the middle of " + myCommandGroups.top()->getDescription();
        } else if (!canUndo()) {
            caption = "Undo";
        }
        menuCommand->handle(this, FXSEL(SEL_COMMAND, FXMenuCaption::ID_SETSTRINGVALUE), (void*)&caption);
        menuCommand->update();
    }
    return 1;
}


long
GNEUndoList::onCmdRedo(FXObject*, FXSelector, void*) {
    redo();
    return 1;
}


long
GNEUndoList::onCmdRedoAll(FXObject*, FXSelector, void*) {
    redoAll();
    return 1;
}


long 
GNEUndoList::onUpdRedo(FXObject* sender, FXSelector, void*) {
    // first check if Redo Menu command or button has to be disabled
    const bool enable = canRedo() && !hasCommandGroup() && myGNEApplicationWindowParent->isUndoRedoEnabled().empty();
    // cast button (see #6209)
    const FXButton* button = dynamic_cast<FXButton*>(sender);
    // enable or disable depending of "enable" flag
    if (button) {
        // avoid unnnecesary enables/disables (due flickering)
        if (enable && !button->isEnabled()) {
            sender->handle(this, FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE), nullptr);
            button->update();
        } else if (!enable && button->isEnabled()) {
            sender->handle(this, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
            button->update();
        }
    } else {
        sender->handle(this, enable ? FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE) : FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    }
    // cast menu command
    FXMenuCommand* menuCommand = dynamic_cast<FXMenuCommand*>(sender);
    // only set caption on menu command item
    if (menuCommand) {
        // change caption of FXMenuCommand
        std::string caption = redoName();
        // set caption of FXmenuCommand edit/undo
        if (myGNEApplicationWindowParent->isUndoRedoEnabled().size() > 0) {
            caption = "Cannot Redo in the middle of " + myGNEApplicationWindowParent->isUndoRedoEnabled();
        } else if (hasCommandGroup()) {
            caption = "Cannot Redo in the middle of " + myCommandGroups.top()->getDescription();
        } else if (!canRedo()) {
            caption = "Redo";
        }
        menuCommand->handle(this, FXSEL(SEL_COMMAND, FXMenuCaption::ID_SETSTRINGVALUE), (void*)&caption);
        menuCommand->update();
    }
    return 1;
}




/******************************/