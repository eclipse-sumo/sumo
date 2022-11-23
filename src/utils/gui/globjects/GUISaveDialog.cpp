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
/// @file    GUISaveDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Dialog for ask user about saving elements
/****************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXApp.h"
#include "FXIcon.h"
#include "FXGIFIcon.h"
#include "FXSeparator.h"
#include "FXLabel.h"
#include "FXButton.h"
#include "FXHorizontalFrame.h"
#include "FXVerticalFrame.h"
#include "FXMessageBox.h"
#include "icons.h"

/*
  Notes:
  - Want justify modes for the message box so the label can be aligned
    different ways.
  - Added MBOX_SAVE_CANCEL_DONTSAVE dialog from Sander.
*/

// Padding for message box buttons
#define HORZ_PAD 30
#define VERT_PAD 2

#define MBOX_BUTTON_MASK   (MBOX_OK|MBOX_OK_CANCEL|MBOX_YES_NO|MBOX_YES_NO_CANCEL|MBOX_QUIT_CANCEL|MBOX_QUIT_SAVE_CANCEL|MBOX_SAVE_CANCEL_DONTSAVE)

using namespace FX;

/*******************************************************************************/

namespace FX {

// Map
FXDEFMAP(FXMessageBox) FXMessageBoxMap[]={
  FXMAPFUNC(SEL_COMMAND,FXMessageBox::ID_CANCEL,FXMessageBox::onCmdCancel),
  FXMAPFUNCS(SEL_COMMAND,FXMessageBox::ID_CLICKED_YES,FXMessageBox::ID_CLICKED_SKIPALL,FXMessageBox::onCmdClicked),
  };



// Object implementation
FXIMPLEMENT(FXMessageBox,FXDialogBox,FXMessageBoxMap,ARRAYNUMBER(FXMessageBoxMap))


// Construct message box with given caption, icon, and message text
FXMessageBox::FXMessageBox(FXWindow* owner,const FXString& caption,const FXString& text,FXIcon* ic,FXuint opts,FXint x,FXint y):
  FXDialogBox(owner,caption,opts|DECOR_TITLE|DECOR_BORDER,x,y,0,0, 0,0,0,0, 4,4){
  initialize(text,ic,opts&MBOX_BUTTON_MASK);
  }


// Construct free floating message box with given caption, icon, and message text
FXMessageBox::FXMessageBox(FXApp* a,const FXString& caption,const FXString& text,FXIcon* ic,FXuint opts,FXint x,FXint y):
  FXDialogBox(a,caption,opts|DECOR_TITLE|DECOR_BORDER,x,y,0,0, 0,0,0,0, 4,4){
  initialize(text,ic,opts&MBOX_BUTTON_MASK);
  }


// Build contents
void FXMessageBox::initialize(const FXString& text,FXIcon* ic,FXuint whichbuttons){
  FXButton *initial;
  FXVerticalFrame* content=new FXVerticalFrame(this,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* info=new FXHorizontalFrame(content,LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,10,10,10,10);
  new FXLabel(info,FXString::null,ic,ICON_BEFORE_TEXT|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  new FXLabel(info,text,NULL,JUSTIFY_LEFT|ICON_BEFORE_TEXT|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  new FXHorizontalSeparator(content,SEPARATOR_GROOVE|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X);
  FXHorizontalFrame* buttons=new FXHorizontalFrame(content,LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0,10,10,5,5);
  if(whichbuttons==MBOX_OK){
    initial=new FXButton(buttons,tr("&OK"),NULL,this,ID_CLICKED_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  else if(whichbuttons==MBOX_OK_CANCEL){
    initial=new FXButton(buttons,tr("&OK"),NULL,this,ID_CLICKED_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("&Cancel"),NULL,this,ID_CLICKED_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  else if(whichbuttons==MBOX_YES_NO){
    initial=new FXButton(buttons,tr("&Yes"),NULL,this,ID_CLICKED_YES,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("&No"),NULL,this,ID_CLICKED_NO,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  else if(whichbuttons==MBOX_YES_NO_CANCEL){
    initial=new FXButton(buttons,tr("&Yes"),NULL,this,ID_CLICKED_YES,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("&No"),NULL,this,ID_CLICKED_NO,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("&Cancel"),NULL,this,ID_CLICKED_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  else if(whichbuttons==MBOX_QUIT_CANCEL){
    initial=new FXButton(buttons,tr("&Quit"),NULL,this,ID_CLICKED_QUIT,BUTTON_INITIAL|BUTTON_DEFAULT|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("&Cancel"),NULL,this,ID_CLICKED_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  else if(whichbuttons==MBOX_QUIT_SAVE_CANCEL){
    new FXButton(buttons,tr("&Quit"),NULL,this,ID_CLICKED_QUIT,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial=new FXButton(buttons,tr("&Save"),NULL,this,ID_CLICKED_SAVE,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("&Cancel"),NULL,this,ID_CLICKED_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  else if(whichbuttons==MBOX_SKIP_SKIPALL_CANCEL){
    initial=new FXButton(buttons,tr("&Skip"),NULL,this,ID_CLICKED_SKIP,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("Skip &All"),NULL,this,ID_CLICKED_SKIPALL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    new FXButton(buttons,tr("&Cancel"),NULL,this,ID_CLICKED_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  else if (whichbuttons==MBOX_SAVE_CANCEL_DONTSAVE){
    buttons->setPackingHints(PACK_NORMAL);
    new FXButton(buttons,tr("&Don't Save"),NULL,this,ID_CLICKED_NO,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_CENTER_X,0,0,0,0,15,15,VERT_PAD,VERT_PAD);
    FXHorizontalFrame *buttons3=new FXHorizontalFrame(buttons,LAYOUT_RIGHT|PACK_UNIFORM_WIDTH,0,0,0,0,0,0,0,0);
    new FXButton(buttons3,tr("&Cancel"),NULL,this,ID_CLICKED_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,15,15,VERT_PAD,VERT_PAD);
    initial=new FXButton(buttons3,tr("&Save"),NULL,this,ID_CLICKED_SAVE,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,15,15,VERT_PAD,VERT_PAD);
    initial->setFocus();
    }
  }


// Close dialog with a cancel
long FXMessageBox::onCmdClicked(FXObject*,FXSelector sel,void*){
  getApp()->stopModal(this,MBOX_CLICKED_YES+(FXSELID(sel)-ID_CLICKED_YES));
  hide();
  return 1;
  }


// Close dialog with a cancel
long FXMessageBox::onCmdCancel(FXObject* sender,FXSelector,void* ptr){
  return FXMessageBox::onCmdClicked(sender,FXSEL(SEL_COMMAND,ID_CLICKED_CANCEL),ptr);
  }


/*******************************************************************************/

// Show a modal error message
FXuint FXMessageBox::error(FXWindow* owner,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(owner->getApp(),erroricon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(owner,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_OWNER);
  }


// Show a modal error message, in free floating window
FXuint FXMessageBox::error(FXApp* app,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(app,erroricon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(app,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_SCREEN);
  }



// Show a modal warning message
FXuint FXMessageBox::warning(FXWindow* owner,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(owner->getApp(),warningicon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(owner,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_OWNER);
  }


// Show a modal warning message, in free floating window
FXuint FXMessageBox::warning(FXApp* app,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(app,warningicon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(app,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_SCREEN);
  }


// Show a modal question dialog
FXuint FXMessageBox::question(FXWindow* owner,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(owner->getApp(),questionicon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(owner,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_OWNER);
  }


// Show a modal question dialog, in free floating window
FXuint FXMessageBox::question(FXApp* app,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(app,questionicon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(app,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_SCREEN);
  }


// Show a modal information dialog
FXuint FXMessageBox::information(FXWindow* owner,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(owner->getApp(),infoicon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(owner,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_OWNER);
  }


// Show a modal information dialog, in free floating window
FXuint FXMessageBox::information(FXApp* app,FXuint opts,const char* caption,const char* message,...){
  FXGIFIcon icon(app,infoicon);
  va_list arguments;
  va_start(arguments,message);
  FXMessageBox box(app,caption,FXStringVFormat(message,arguments),&icon,opts|DECOR_TITLE|DECOR_BORDER);
  va_end(arguments);
  return box.execute(PLACEMENT_SCREEN);
  }
}

