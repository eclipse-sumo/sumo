/********************************************************************************
*                                                                               *
*                  U n d o / R e d o - a b l e   C o m m a n d                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2006 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXUndoList2.cpp,v 1.57 2006/01/22 17:58:50 fox Exp $                      *
********************************************************************************/

#include "FXUndoList2.h"


#define NOMARK 2147483647       // No mark is set


/*******************************************************************************/

// Object implementation
FXIMPLEMENT_ABSTRACT(FXCommand2,FXObject,NULL,0)


// Default implementation of undo name is just "Undo"
FXString FXCommand2::undoName() const { return "Undo"; }


// Default implementation of redo name is just "Redo"
FXString FXCommand2::redoName() const { return "Redo"; }


// Allow merging is false by default
bool FXCommand2::canMerge() const { return false; }


// Don't merge by default
bool FXCommand2::mergeWith(FXCommand2*){ return false; }


// Default returns size of undo record itself
FXuint FXCommand2::size() const { return sizeof(FXCommand2); }


/*******************************************************************************/

// Object implementation
FXIMPLEMENT(FXCommandGroup2,FXCommand2,NULL,0)


// Undoing a command group undoes each sub command
void FXCommandGroup2::undo(){
  register FXCommand2 *command;
  while(undolist){
    command=undolist;
    undolist=undolist->next;
    command->undo();
    command->next=redolist;
    redolist=command;
    }
  }


// Undoing a command group undoes each sub command
void FXCommandGroup2::redo(){
  register FXCommand2 *command;
  while(redolist){
    command=redolist;
    redolist=redolist->next;
    command->redo();
    command->next=undolist;
    undolist=command;
    }
  }


// Return the size of the information in the undo command group.
FXuint FXCommandGroup2::size() const {
  register FXuint result=sizeof(FXCommandGroup2);
  register FXCommand2 *command;
  for(command=undolist; command; command=command->next){
    result+=command->size();
    }
  for(command=redolist; command; command=command->next){
    result+=command->size();
    }
  return result;
  }


// Destrying the command group destroys the subcommands
FXCommandGroup2::~FXCommandGroup2(){
  register FXCommand2 *command;
  while(redolist){
    command=redolist;
    redolist=redolist->next;
    delete command;
    }
  while(undolist){
    command=undolist;
    undolist=undolist->next;
    delete command;
    }
  delete group;
  }


/*******************************************************************************/

// Map
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
FXIMPLEMENT(FXUndoList2,FXCommandGroup2,FXUndoList2Map,ARRAYNUMBER(FXUndoList2Map))



// Make new empty undo list
FXUndoList2::FXUndoList2(){
  working=false;
  }

// Cut the redo list; can no longer revert to marked
// state if mark is inside the redo list.
void FXUndoList2::cut(){
  register FXCommand2 *command;
  while(redolist){
    command=redolist;
    redolist=redolist->next;
    delete command;
    }
  redolist=NULL;
  }


// Add new command, executing if desired
void FXUndoList2::add(FXCommand2* command,bool doit,bool merge){
  register FXCommandGroup2* g=this;
  register FXuint size=0;

  // Must pass a command
  if(!command){ fxerror("FXCommandGroup2::add: NULL command argument.\n"); }

  // Adding undo while in the middle of doing something!
  if(working){ fxerror("FXCommandGroup2::add: already working on undo or redo.\n"); }

  working=true;

  // Cut redo list
  cut();

  // Execute command
  if(doit) command->redo();

  // Hunt for end of group chain
  while(g->group){ g=g->group; }

  // Old size of previous record
  if(g->undolist) size=g->undolist->size();

  // Try to merge commands when desired and possible
  if(merge && g->undolist && (group!=NULL) && command->canMerge() && g->undolist->mergeWith(command)){


    // Delete incoming command that was merged
    delete command;
    }

  // Append new command to undo list
  else{

    // Append incoming command
    command->next=g->undolist;
    g->undolist=command;

    }

  working=false;
  }


// Begin a new undo command group
void FXUndoList2::begin(FXCommandGroup2 *command){
  register FXCommandGroup2* g=this;

  // Must pass a command group
  if(!command){ fxerror("FXCommandGroup2::begin: NULL command argument.\n"); }

  // Calling begin while in the middle of doing something!
  if(working){ fxerror("FXCommandGroup2::begin: already working on undo or redo.\n"); }

  // Cut redo list
  cut();

  // Hunt for end of group chain
  while(g->group){ g=g->group; }

  // Add to end
  g->group=command;
  }


// End undo command group
void FXUndoList2::end(){
  register FXCommandGroup2 *command;
  register FXCommandGroup2 *g=this;

  // Must have called begin
  if(!g->group){ ("FXCommandGroup2::end: no matching call to begin.\n"); }

  // Calling end while in the middle of doing something!
  if(working){ fxerror("FXCommandGroup2::end: already working on undo or redo.\n"); }

  // Hunt for one above end of group chain
  while(g->group->group){ g=g->group; }

  // Unlink from group chain
  command=g->group;
  g->group=NULL;

  // Add to group if non-empty
  if(!command->empty()){

    // Append new command to undo list
    command->next=g->undolist;
    g->undolist=command;

    }

  // Or delete if empty
  else{

    // Delete bottom group
    delete command;
    }
  }


// Abort undo command group
void FXUndoList2::abort(){
  register FXCommandGroup2 *g=this;

  // Must be called after begin
  if(!g->group){ fxerror("FXCommandGroup2::abort: no matching call to begin.\n"); }

  // Calling abort while in the middle of doing something!
  if(working){ fxerror("FXCommandGroup2::abort: already working on undo or redo.\n"); }

  // Hunt for one above end of group chain
  while(g->group->group){ g=g->group; }

  // Delete bottom group
  delete g->group;

  // New end of chain
  g->group=NULL;
  }


// Undo last command
void FXUndoList2::undo(){
  register FXCommand2 *command;
  if(group){ fxerror("FXCommandGroup2::undo: cannot call undo inside begin-end block.\n"); }
  if(undolist){
    working=true;
    command=undolist;                   // Remove from undolist BEFORE undo
    undolist=undolist->next;
    command->undo();
    command->next=redolist;             // Hang into redolist AFTER undo
    redolist=command;
    working=false;
    }
  }


// Redo next command
void FXUndoList2::redo(){
  register FXCommand2 *command;
  if(group){ fxerror("FXCommandGroup2::redo: cannot call undo inside begin-end block.\n"); }
  if(redolist){
    working=true;
    command=redolist;                   // Remove from redolist BEFORE redo
    redolist=redolist->next;
    command->redo();
    command->next=undolist;             // Hang into undolist AFTER redo
    undolist=command;
    working=false;
    }
  }


// Undo all commands
void FXUndoList2::undoAll(){
  while(canUndo()) undo();
  }


// Redo all commands
void FXUndoList2::redoAll(){
  while(canRedo()) redo();
  }


// Can we undo more commands
bool FXUndoList2::canUndo() const {
  return undolist!=NULL;
  }


// Can we redo more commands
bool FXUndoList2::canRedo() const {
  return redolist!=NULL;
  }


// Return name of the first undo command available, if any
FXString FXUndoList2::undoName() const {
  if(undolist) return undolist->undoName();
  return FXString::null;
  }


// Return name of the first redo command available, if any
FXString FXUndoList2::redoName() const {
  if(redolist) return redolist->redoName();
  return FXString::null;
  }


// Clear list
void FXUndoList2::clear(){
  register FXCommand2 *command;
  while(redolist){
    command=redolist;
    redolist=redolist->next;
    delete command;
    }
  while(undolist){
    command=undolist;
    undolist=undolist->next;
    delete command;
    }
  delete group;
  redolist=NULL;
  undolist=NULL;
  group=NULL;
  }


// Clear undo list
long FXUndoList2::onCmdClear(FXObject*,FXSelector,void*){
  clear();
  return 1;
  }


// Update Clear undo list
long FXUndoList2::onUpdClear(FXObject* sender,FXSelector,void*){
  sender->handle(this,(canUndo()||canRedo())?FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE):FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
  return 1;
  }


// Undo last command
long FXUndoList2::onCmdUndo(FXObject*,FXSelector,void*){
  undo();
  return 1;
  }


// Undo all commands
long FXUndoList2::onCmdUndoAll(FXObject*,FXSelector,void*){
  undoAll();
  return 1;
  }


// Update undo last command
long FXUndoList2::onUpdUndo(FXObject* sender,FXSelector,void*){
  sender->handle(this,canUndo()?FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE):FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
  return 1;
  }


// Redo last command
long FXUndoList2::onCmdRedo(FXObject*,FXSelector,void*){
  redo();
  return 1;
  }


// Redo all commands
long FXUndoList2::onCmdRedoAll(FXObject*,FXSelector,void*){
  redoAll();
  return 1;
  }


// Update redo last command
long FXUndoList2::onUpdRedo(FXObject* sender,FXSelector,void*){
  sender->handle(this,canRedo()?FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE):FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
  return 1;
  }