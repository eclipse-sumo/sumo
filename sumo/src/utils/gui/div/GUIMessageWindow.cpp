/****************************************************************************/
/// @file    GUIMessageWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 25 Nov 2003
/// @version $Id$
///
// A logging window for the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <utils/common/MsgHandler.h>
#include "GUIMessageWindow.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIMessageWindow::GUIMessageWindow(FXComposite* parent) :
    FXText(parent, 0, 0, 0, 0, 0, 0, 50),
    myStyles(0),
    myErrorRetriever(0),
    myMessageRetriever(0),
    myWarningRetriever(0) {
    setStyled(true);
    setEditable(false);
    myStyles = new FXHiliteStyle[4];
    // set separator style
    myStyles[0].normalForeColor = FXRGB(0x00, 0x00, 0x88);
    myStyles[0].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].selectBackColor = FXRGB(0x00, 0x00, 0x88);
    myStyles[0].hiliteForeColor = FXRGB(0x00, 0x00, 0x88);
    myStyles[0].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].style = 0;
    // set message text style
    myStyles[1].normalForeColor = FXRGB(0x00, 0x88, 0x00);
    myStyles[1].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].selectBackColor = FXRGB(0x00, 0x88, 0x00);
    myStyles[1].hiliteForeColor = FXRGB(0x00, 0x88, 0x00);
    myStyles[1].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].style = 0;
    // set error text style
    myStyles[2].normalForeColor = FXRGB(0x88, 0x00, 0x00);
    myStyles[2].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].selectBackColor = FXRGB(0x88, 0x00, 0x00);
    myStyles[2].hiliteForeColor = FXRGB(0x88, 0x00, 0x00);
    myStyles[2].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].style = 0;
    // set warning text style
    myStyles[3].normalForeColor = FXRGB(0xe6, 0x98, 0x00);
    myStyles[3].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].selectBackColor = FXRGB(0xe6, 0x98, 0x00);
    myStyles[3].hiliteForeColor = FXRGB(0xe6, 0x98, 0x00);
    myStyles[3].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].style = 0;
    //
    setHiliteStyles(myStyles);
}


GUIMessageWindow::~GUIMessageWindow() {
    delete[] myStyles;
    delete myMessageRetriever;
    delete myErrorRetriever;
    delete myWarningRetriever;
}


void
GUIMessageWindow::appendText(GUIEventType eType, const std::string& msg) {
    if (!isEnabled()) {
        show();
    }
    // build the styled message
    FXint style = 1;
    switch (eType) {
        case EVENT_ERROR_OCCURED:
            // color: red
            style = 2;
            break;
        case EVENT_WARNING_OCCURED:
            // color: yellow
            style = 3;
            break;
        case EVENT_MESSAGE_OCCURED:
            // color: green
            style = 1;
            break;
        default:
            assert(false);
    }
    // insert message to buffer
    FXText::appendStyledText(msg.c_str(), (FXint) msg.length(), style + 1, true);
    FXText::setCursorPos(getLength() - 1);
    FXText::setBottomLine(getLength() - 1);
    if (isEnabled()) {
        layout();
        update();
    }
}


void
GUIMessageWindow::addSeparator() {
    std::string msg = "----------------------------------------------------------------------------------------\n";
    FXText::appendStyledText(msg.c_str(), (FXint) msg.length(), 1, true);
    FXText::setCursorPos(getLength() - 1);
    FXText::setBottomLine(getLength() - 1);
    if (isEnabled()) {
        layout();
        update();
    }
}


void
GUIMessageWindow::clear() {
    if (getLength() == 0) {
        return;
    }
    FXText::removeText(0, getLength() - 1, true);
    if (isEnabled()) {
        layout();
        update();
    }
}


void
GUIMessageWindow::registerMsgHandlers() {
    if (myMessageRetriever == 0) {
        // initialize only if registration is requested
        myMessageRetriever = new MsgOutputDevice(this, EVENT_MESSAGE_OCCURED);
        myErrorRetriever = new MsgOutputDevice(this, EVENT_ERROR_OCCURED);
        myWarningRetriever = new MsgOutputDevice(this, EVENT_WARNING_OCCURED);
    }
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetriever);
}


void
GUIMessageWindow::unregisterMsgHandlers() {
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetriever);
}


/****************************************************************************/

