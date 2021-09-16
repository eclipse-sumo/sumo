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

// =========================================================================
// included modules
// =========================================================================

#include "FXUndoList2.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================

FXDEFMAP(FXUndoList2) FXUndoList2Map[]={
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
FXIMPLEMENT_ABSTRACT(FXCommand2, FXObject, nullptr, 0)
FXIMPLEMENT(FXCommandGroup2, FXCommand2, nullptr, 0)
FXIMPLEMENT(FXUndoList2, FXCommandGroup2, FXUndoList2Map, ARRAYNUMBER(FXUndoList2Map))


FXString 
FXCommand2::undoName() const { 
    return "Undo"; 
}


FXString 
FXCommand2::redoName() const { 
    return "Redo";
}


bool 
FXCommand2::canMerge() const { 
    return false; 
}


bool 
FXCommand2::mergeWith(FXCommand2*) {
    return false; 
}



FXuint 
FXCommand2::size() const { 
    return sizeof(FXCommand2); 
}


/*******************************************************************************/


void 
FXCommandGroup2::undo() {
    register FXCommand2 *command;
    while (undolist) {
        command=undolist;
        undolist=undolist->next;
        command->undo();
        command->next=redolist;
        redolist=command;
    }
}



void FXCommandGroup2::redo() {
    register FXCommand2 *command;
    while (redolist) {
        command=redolist;
        redolist=redolist->next;
        command->redo();
        command->next=undolist;
        undolist=command;
    }
}


FXuint FXCommandGroup2::size() const {
    register FXuint result=sizeof(FXCommandGroup2);
    register FXCommand2 *command;
    for (command=undolist; command; command=command->next) {
        result+=command->size();
    }
    for (command=redolist; command; command=command->next) {
        result+=command->size();
    }
    return result;
}


FXCommandGroup2::~FXCommandGroup2() {
    register FXCommand2 *command;
    while (redolist) {
        command=redolist;
        redolist=redolist->next;
        delete command;
    }
    while (undolist) {
        command=undolist;
        undolist=undolist->next;
        delete command;
    }
    delete group;
}


/*******************************************************************************/


FXUndoList2::FXUndoList2() {
    working=false;
}


void 
FXUndoList2::cut() {
    register FXCommand2 *command;
    while (redolist) {
        command=redolist;
        redolist=redolist->next;
        delete command;
    }
    redolist=nullptr;
}


void 
FXUndoList2::add(FXCommand2* command, bool doit, bool merge) {
    register FXCommandGroup2* g=this;
    register FXuint size=0;
    // Must pass a command
    if (!command) { 
        fxerror("FXCommandGroup2::add: nullptr command argument.\n");
    }
    // Adding undo while in the middle of doing something!
    if (working) { 
        fxerror("FXCommandGroup2::add: already working on undo or redo.\n"); 
    }
    working=true;
    // Cut redo list
    cut();
    // Execute command
    if (doit) {
        command->redo();
    }
    // Hunt for end of group chain
    while (g->group) { 
        g=g->group; 
    }
    // Old size of previous record
    if (g->undolist) {
        size=g->undolist->size();
    }
    // Try to merge commands when desired and possible
    if (merge && g->undolist && (group != nullptr) && command->canMerge() && g->undolist->mergeWith(command)) {
        // Delete incoming command that was merged
        delete command;
    } else {
        // Append incoming command
        command->next=g->undolist;
        g->undolist=command;

    }
    working=false;
}


void 
FXUndoList2::begin(FXCommandGroup2 *command) {
    register FXCommandGroup2* g=this;
    // Must pass a command group
    if (!command) {
        fxerror("FXCommandGroup2::begin: nullptr command argument.\n"); 
    }
    // Calling begin while in the middle of doing something!
    if (working) { 
        fxerror("FXCommandGroup2::begin: already working on undo or redo.\n"); 
    }
    // Cut redo list
    cut();
    // Hunt for end of group chain
    while (g->group) { 
        g=g->group;
    }
    // Add to end
    g->group=command;
}


void 
FXUndoList2::end() {
    register FXCommandGroup2 *command;
    register FXCommandGroup2 *g=this;
    // Must have called begin
    if (!g->group) { 
        fxerror("FXCommandGroup2::end: no matching call to begin.\n"); 
    }
    // Calling end while in the middle of doing something!
    if (working) {
        fxerror("FXCommandGroup2::end: already working on undo or redo.\n"); 
    }
    // Hunt for one above end of group chain
    while (g->group->group) {
        g=g->group;
    }
    // Unlink from group chain
    command=g->group;
    g->group=nullptr;
    // Add to group if non-empty
    if (!command->empty()) {
        // Append new command to undo list
        command->next=g->undolist;
        g->undolist=command;
    } else {
        // Delete bottom group
        delete command;
    }
}


void 
FXUndoList2::abort() {
    register FXCommandGroup2 *g=this;
    // Must be called after begin
    if (!g->group) { 
        fxerror("FXCommandGroup2::abort: no matching call to begin.\n");
    }
    // Calling abort while in the middle of doing something!
    if (working) { 
        fxerror("FXCommandGroup2::abort: already working on undo or redo.\n"); 
    }
    // Hunt for one above end of group chain
    while (g->group->group) {
        g=g->group; 
    }
    // Delete bottom group
    delete g->group;
    // New end of chain
    g->group=nullptr;
}


void 
FXUndoList2::undo() {
    register FXCommand2 *command;
    if (group) { 
        fxerror("FXCommandGroup2::undo: cannot call undo inside begin-end block.\n");
    }
    if (undolist) {
        working=true;
        command=undolist;  
        // Remove from undolist BEFORE undo
        undolist=undolist->next;
        command->undo();
        // Hang into redolist AFTER undo
        command->next=redolist;             
        redolist=command;
        working=false;
    }
}


void 
FXUndoList2::redo() {
    register FXCommand2 *command;
    if (group) { 
        fxerror("FXCommandGroup2::redo: cannot call undo inside begin-end block.\n"); 
    }
    if (redolist) {
        working=true;
        command=redolist;    
        // Remove from redolist BEFORE redo
        redolist=redolist->next;
        command->redo();
        // Hang into undolist AFTER redo
        command->next=undolist;             
        undolist=command;
        working=false;
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
    return undolist != nullptr;
}


bool 
FXUndoList2::canRedo() const {
    return redolist != nullptr;
}


FXString 
FXUndoList2::undoName() const {
    if (undolist) {
        return undolist->undoName();
    } else {
        return FXString::null;
    }
}


FXString 
FXUndoList2::redoName() const {
    if (redolist) {
        return redolist->redoName();
    } else {
        return FXString::null;
    }
}


void 
FXUndoList2::clear() {
    register FXCommand2 *command;
    while (redolist) {
        command=redolist;
        redolist=redolist->next;
        delete command;
    }
    while (undolist) {
        command=undolist;
        undolist=undolist->next;
        delete command;
    }
    delete group;
    redolist=nullptr;
    undolist=nullptr;
    group=nullptr;
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