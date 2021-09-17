/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    FXUndoList2.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2021
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <utils/common/UtilExceptions.h>

#include "FXUndoList2.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXUndoList2) FXUndoList2Map[] = {
    FXMAPFUNC(SEL_COMMAND, FXUndoList2::ID_CLEAR,      FXUndoList2::onCmdClear), 
    FXMAPFUNC(SEL_UPDATE,  FXUndoList2::ID_CLEAR,      FXUndoList2::onUpdClear), 
    FXMAPFUNC(SEL_COMMAND, FXUndoList2::ID_UNDO,       FXUndoList2::onCmdUndo), 
    FXMAPFUNC(SEL_UPDATE,  FXUndoList2::ID_UNDO,       FXUndoList2::onUpdUndo), 
    FXMAPFUNC(SEL_COMMAND, FXUndoList2::ID_REDO,       FXUndoList2::onCmdRedo), 
    FXMAPFUNC(SEL_UPDATE,  FXUndoList2::ID_REDO,       FXUndoList2::onUpdRedo), 
    FXMAPFUNC(SEL_COMMAND, FXUndoList2::ID_UNDO_ALL,   FXUndoList2::onCmdUndoAll), 
    FXMAPFUNC(SEL_UPDATE,  FXUndoList2::ID_UNDO_ALL,   FXUndoList2::onUpdUndo), 
    FXMAPFUNC(SEL_COMMAND, FXUndoList2::ID_REDO_ALL,   FXUndoList2::onCmdRedoAll), 
    FXMAPFUNC(SEL_UPDATE,  FXUndoList2::ID_REDO_ALL,   FXUndoList2::onUpdRedo), 
};


// Object implementation
FXIMPLEMENT(FXCommandGroup2, GNEChange, nullptr, 0)
FXIMPLEMENT(FXUndoList2, FXCommandGroup2, FXUndoList2Map, ARRAYNUMBER(FXUndoList2Map))

// ---------------------------------------------------------------------------
// FXCommandGroup2 - methods
// ---------------------------------------------------------------------------

FXCommandGroup2::FXCommandGroup2(const std::string &description) :
    undoList(nullptr), 
    redoList(nullptr), 
    group(nullptr),
    myDescription(description) {
}


const std::string&
FXCommandGroup2::getDescription() {
    return myDescription;
}


FXString
FXCommandGroup2::undoName() const {
    return ("Undo " + myDescription).c_str();
}


FXString
FXCommandGroup2::redoName() const {
    return ("Redo " + myDescription).c_str();
}


bool
FXCommandGroup2::empty() { 
    return (undoList == nullptr); 
}


void
FXCommandGroup2::undo() {
    register GNEChange *change;
    while (undoList) {
        change = undoList;
        undoList = undoList->next;
        change->undo();
        change->next = redoList;
        redoList = change;
    }
}


void FXCommandGroup2::redo() {
    register GNEChange *change;
    while (redoList) {
        change = redoList;
        redoList = redoList->next;
        change->redo();
        change->next = undoList;
        undoList = change;
    }
}


FXuint FXCommandGroup2::size() const {
    register FXuint result = sizeof(FXCommandGroup2);
    register GNEChange *change;
    for (change = undoList; change; change = change->next) {
        result += change->size();
    }
    for (change = redoList; change; change = change->next) {
        result += change->size();
    }
    return result;
}


FXCommandGroup2::~FXCommandGroup2() {
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
}


FXCommandGroup2::FXCommandGroup2() :
    undoList(nullptr), 
    redoList(nullptr), 
    group(nullptr) {
}

// ---------------------------------------------------------------------------
// FXUndoList2 - methods
// ---------------------------------------------------------------------------

FXUndoList2::FXUndoList2() {
    myWorking = false;
}


void 
FXUndoList2::cut() {
    register GNEChange *change;
    while (redoList) {
        change = redoList;
        redoList = redoList->next;
        delete change;
    }
    redoList = nullptr;
}


void 
FXUndoList2::add(GNEChange* change, bool doit, bool merge) {
    register FXCommandGroup2* g = this;
    register FXuint size = 0;
    // Must pass a change
    if (!change) { 
        throw ProcessError("FXCommandGroup2::add: nullptr change argument");
    }
    // Adding undo while in the middle of doing something!
    if (myWorking) { 
        throw ProcessError("FXCommandGroup2::add: already working on undo or redo"); 
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
FXUndoList2::begin(FXCommandGroup2 *change) {
    register FXCommandGroup2* g = this;
    // Must pass a change group
    if (!change) {
        throw ProcessError("FXCommandGroup2::begin: nullptr change argument"); 
    }
    // Calling begin while in the middle of doing something!
    if (myWorking) { 
        throw ProcessError("FXCommandGroup2::begin: already working on undo or redo"); 
    }
    // Cut redo list
    cut();
    // Hunt for end of group chain
    while (g->group) { 
        g = g->group;
    }
    // Add to end
    g->group = change;
}


void 
FXUndoList2::end() {
    register FXCommandGroup2 *change;
    register FXCommandGroup2 *g = this;
    // Must have called begin
    if (!g->group) { 
        throw ProcessError("FXCommandGroup2::end: no matching call to begin"); 
    }
    // Calling end while in the middle of doing something!
    if (myWorking) {
        throw ProcessError("FXCommandGroup2::end: already working on undo or redo"); 
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
FXUndoList2::abort() {
    register FXCommandGroup2 *g = this;
    // Must be called after begin
    if (!g->group) { 
        throw ProcessError("FXCommandGroup2::abort: no matching call to begin");
    }
    // Calling abort while in the middle of doing something!
    if (myWorking) { 
        throw ProcessError("FXCommandGroup2::abort: already working on undo or redo"); 
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
FXUndoList2::undo() {
    register GNEChange *change;
    if (group) { 
        throw ProcessError("FXCommandGroup2::undo: cannot call undo inside begin-end block");
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
}


void 
FXUndoList2::redo() {
    register GNEChange *change;
    if (group) { 
        throw ProcessError("FXCommandGroup2::redo: cannot call undo inside begin-end block"); 
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
}


void 
FXUndoList2::undoAll() {
    while (canUndo()) {
        undo();
    }
}


void
FXUndoList2::redoAll() {
    while (canRedo()) {
        redo();
    }
}


bool
FXUndoList2::canUndo() const {
    return undoList != nullptr;
}


bool 
FXUndoList2::canRedo() const {
    return redoList != nullptr;
}


bool 
FXUndoList2::busy() const { 
    return myWorking; 
}


GNEChange* 
FXUndoList2::current() const {
    return undoList;
}


FXString 
FXUndoList2::undoName() const {
    if (undoList) {
        return undoList->undoName();
    } else {
        return FXString::null;
    }
}


FXString 
FXUndoList2::redoName() const {
    if (redoList) {
        return redoList->redoName();
    } else {
        return FXString::null;
    }
}


void 
FXUndoList2::clear() {
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
FXUndoList2::onCmdClear(FXObject*, FXSelector, void*) {
    clear();
    return 1;
}


long 
FXUndoList2::onUpdClear(FXObject* sender, FXSelector, void*) {
    sender->handle(this, (canUndo()||canRedo())?FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE):FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    return 1;
}


long
FXUndoList2::onCmdUndo(FXObject*, FXSelector, void*) {
    undo();
    return 1;
}


long 
FXUndoList2::onCmdUndoAll(FXObject*, FXSelector, void*) {
    undoAll();
    return 1;
}


long 
FXUndoList2::onUpdUndo(FXObject* sender, FXSelector, void*) {
    sender->handle(this, canUndo()?FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE):FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    return 1;
}


long
FXUndoList2::onCmdRedo(FXObject*, FXSelector, void*) {
    redo();
    return 1;
}


long
FXUndoList2::onCmdRedoAll(FXObject*, FXSelector, void*) {
    redoAll();
    return 1;
}


long FXUndoList2::onUpdRedo(FXObject* sender, FXSelector, void*) {
    sender->handle(this, canRedo()?FXSEL(SEL_COMMAND, FXWindow::ID_ENABLE):FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), nullptr);
    return 1;
}