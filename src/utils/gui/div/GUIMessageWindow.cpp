/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2003-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIMessageWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Tue, 25 Nov 2003
///
// A logging window for the gui
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <fxkeys.h>
#include "GUIMessageWindow.h"


// ===========================================================================
// static members
// ===========================================================================

bool GUIMessageWindow::myLocateLinks = true;
SUMOTime GUIMessageWindow::myBreakPointOffset = TIME2STEPS(-5);
FXHiliteStyle* GUIMessageWindow::myStyles = new FXHiliteStyle[8];

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GUIMessageWindow) GUIMessageWindowMap[] = {
    FXMAPFUNC(SEL_KEYPRESS, 0, GUIMessageWindow::onKeyPress),
};

FXIMPLEMENT_ABSTRACT(GUIMessageWindow, FXText, GUIMessageWindowMap, ARRAYNUMBER(GUIMessageWindowMap))

// ===========================================================================
// method definitions
// ===========================================================================
GUIMessageWindow::GUIMessageWindow(FXComposite* parent, GUIMainWindow* mainWindow) :
    FXText(parent, nullptr, 0, 0, 0, 0, 0, 50),
    myMainWindow(mainWindow),
    myErrorRetriever(nullptr),
    myMessageRetriever(nullptr),
    myWarningRetriever(nullptr) {
    setStyled(true);
    setEditable(false);
    // fill styles
    fillStyles();
    // set styles
    setHiliteStyles(myStyles);
}


GUIMessageWindow::~GUIMessageWindow() {
    delete[] myStyles;
    delete myMessageRetriever;
    delete myErrorRetriever;
    delete myWarningRetriever;
}


const GUIGlObject*
GUIMessageWindow::getActiveStringObject(const FXString& text, const FXint pos, const FXint lineS, const FXint lineE) const {
    const FXint idS = MAX2(text.rfind(" '", pos), text.rfind("='", pos));
    const FXint idE = text.find("'", pos);
    if (idS >= 0 && idE >= 0 && idS >= lineS && idE <= lineE) {
        FXint typeS = text.rfind(" ", idS - 1);
        if (typeS >= 0) {
            if (text.at(typeS + 1) == '(') {
                typeS++;
            }
            std::string type(text.mid(typeS + 1, idS - typeS - 1).lower().text());
            if (type == "tllogic") {
                type = "tlLogic"; // see GUIGlObject.cpp
            } else if (type == "busstop" || type == "trainstop") {
                type = "busStop";
            } else if (type == "containerstop") {
                type = "containerStop";
            } else if (type == "chargingstation") {
                type = "chargingStation";
            } else if (type == "overheadwiresegment") {
                type = "overheadWireSegment";
            } else if (type == "parkingarea") {
                type = "parkingArea";
            }
            const std::string id(text.mid(idS + 2, idE - idS - 2).text());
            const std::string typedID = type + ":" + id;
            const GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(typedID);
            //std::cout << " getActiveStringObject '" << typedID << "' o=" << (o == nullptr ? "NULL" : o->getMicrosimID()) << "\n";
            return o;
        }
    }
    return nullptr;
}

SUMOTime
GUIMessageWindow::getTimeString(const FXString& text, const FXint pos, const FXint /*lineS*/, const FXint /*lineE*/) const {
    const FXint end = text.find(" ", pos + 1);
    std::string time;
    if (end >= 0) {
        time = text.mid(pos, end - pos).text();
    } else {
        time = text.mid(pos, text.length() - pos).text();
        if (time.empty()) {
            return -1;
        }
        if (time.back() == '\n') {
            time.pop_back();
        }
        if (time.empty()) {
            return -1;
        }
        if (time.back() == '.') {
            time.pop_back();
        }
    }
    if (time.empty()) {
        return -1;
    }
    if (time.front() == ' ') {
        time = time.substr(1);
    }
    //std::cout << "text='" << text.text() << "' pos=" << pos << " time='" << time << "'\n";
    try {
        //std::cout << "  SUMOTime=" << string2time(time) << "\n";
        return string2time(time);
    } catch (...) {
        return -1;
    }
}


void
GUIMessageWindow::setCursorPos(FXint pos, FXbool notify) {
    FXText::setCursorPos(pos, notify);
    if (myLocateLinks) {
        GUIMainWindow* const main = GUIMainWindow::getInstance();
        std::vector<std::string> viewIDs = main->getViewIDs();
        if (viewIDs.empty()) {
            return;
        }
        GUIGlChildWindow* const child = main->getViewByID(viewIDs[0]);
        const FXString text = getText();
        const GUIGlObject* const glObj = getActiveStringObject(text, pos, lineStart(pos), lineEnd(pos));
        if (glObj != nullptr) {
            child->setView(glObj->getGlID());
            GUIGlObjectStorage::gIDStorage.unblockObject(glObj->getGlID());
            if (getApp()->getKeyState(KEY_Control_L)) {
                gSelected.toggleSelection(glObj->getGlID());
            }
        } else {
            const int lookback = MIN2(pos, 20);
            const int start = MAX2(lineStart(pos), pos - lookback);
            const FXString candidate = text.mid(start, lineEnd(pos) - start);
            FXint timePos = candidate.find(" time") + 6;
            SUMOTime t = -1;
            if (pos >= 0 && pos > start + timePos) {
                t = getTimeString(candidate, timePos, 0, candidate.length());
                if (t >= 0) {
                    t += myBreakPointOffset;
                    std::vector<SUMOTime> breakpoints = myMainWindow->retrieveBreakpoints();
                    if (std::find(breakpoints.begin(), breakpoints.end(), t) == breakpoints.end()) {
                        breakpoints.push_back(t);
                        std::sort(breakpoints.begin(), breakpoints.end());
                        myMainWindow->setBreakpoints(breakpoints);
                        myMainWindow->setStatusBarText("Set breakpoint at " + time2string(t));
                    }
                }
            }
        }
    }
}


void
GUIMessageWindow::appendMsg(GUIEventType eType, const std::string& msg) {
    if (!isEnabled()) {
        show();
    }
    // build the styled message
    FXint style = 1;
    switch (eType) {
        case GUIEventType::DEBUG_OCCURRED:
            // color: blue
            style = 0;
            break;
        case GUIEventType::GLDEBUG_OCCURRED:
            // color: fuchsia
            style = 7;
            break;
        case GUIEventType::ERROR_OCCURRED:
            // color: red
            style = 2;
            break;
        case GUIEventType::WARNING_OCCURRED:
            // color: yellow
            style = 3;
            break;
        case GUIEventType::MESSAGE_OCCURRED:
            // color: green
            style = 1;
            break;
        default:
            assert(false);
    }
    FXString text(msg.c_str());
    if (myLocateLinks) {
        FXint pos = text.find("'");
        while (pos >= 0) {
            const GUIGlObject* const glObj = getActiveStringObject(text, pos + 1, 0, text.length());
            if (glObj != nullptr) {
                GUIGlObjectStorage::gIDStorage.unblockObject(glObj->getGlID());
                FXString insText = text.left(pos + 1);
                FXText::appendStyledText(insText, style + 1);
                text.erase(0, pos + 1);
                pos = text.find("'");
                insText = text.left(pos);
                FXText::appendStyledText(insText, style + 4);
                text.erase(0, pos);
            }
            pos = text.find("'", pos + 1);
        }
        // find time links
        pos = text.find(" time");
        SUMOTime t = -1;
        if (pos >= 0) {
            t = getTimeString(text, pos + 6, 0, text.length());
        }
        if (t >= 0) {
            FXString insText = text.left(pos + 6);
            FXText::appendStyledText(insText, style + 1);
            text.erase(0, pos + 6);
            pos = text.find(" ");
            if (pos < 0) {
                pos = text.rfind(".");
            }
            insText = text.left(pos);
            FXText::appendStyledText(insText, style + 4);
            text.erase(0, pos);
        }
    }
    // insert rest of the message
    FXText::appendStyledText(text, style + 1, true);
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
    if (myMessageRetriever == nullptr) {
        // initialize only if registration is requested
        myMessageRetriever = new MsgOutputDevice(this, GUIEventType::MESSAGE_OCCURRED);
        myErrorRetriever = new MsgOutputDevice(this, GUIEventType::ERROR_OCCURRED);
        myDebugRetriever = new MsgOutputDevice(this, GUIEventType::DEBUG_OCCURRED);
        myGLDebugRetriever = new MsgOutputDevice(this, GUIEventType::GLDEBUG_OCCURRED);
        myWarningRetriever = new MsgOutputDevice(this, GUIEventType::WARNING_OCCURRED);
    }
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    MsgHandler::getDebugInstance()->addRetriever(myDebugRetriever);
    MsgHandler::getGLDebugInstance()->addRetriever(myGLDebugRetriever);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetriever);
}


void
GUIMessageWindow::unregisterMsgHandlers() {
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    MsgHandler::getDebugInstance()->removeRetriever(myDebugRetriever);
    MsgHandler::getGLDebugInstance()->removeRetriever(myGLDebugRetriever);
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetriever);
}


long
GUIMessageWindow::onKeyPress(FXObject* o, FXSelector sel, void* ptr) {
    FXEvent* e = (FXEvent*) ptr;
    // permit ctrl+a, ctrl+c
    if (e->state & CONTROLMASK) {
        return FXText::onKeyPress(o, sel, ptr);
    }
    return 0;
}


FXHiliteStyle*
GUIMessageWindow::getStyles() {
    return myStyles;
}


void
GUIMessageWindow::fillStyles() {
    const FXColor white   = FXRGB(0xff, 0xff, 0xff);
    const FXColor blue    = FXRGB(0x00, 0x00, 0x88);
    const FXColor green   = FXRGB(0x00, 0x88, 0x00);
    const FXColor red     = FXRGB(0x88, 0x00, 0x00);
    const FXColor yellow  = FXRGB(0xe6, 0x98, 0x00);
    const FXColor fuchsia = FXRGB(0x88, 0x00, 0x88);
    // set separator style
    myStyles[0].normalForeColor = blue;
    myStyles[0].normalBackColor = white;
    myStyles[0].selectForeColor = white;
    myStyles[0].selectBackColor = blue;
    myStyles[0].hiliteForeColor = blue;
    myStyles[0].hiliteBackColor = white;
    myStyles[0].activeBackColor = white;
    myStyles[0].style = 0;
    // set message text style
    myStyles[1] = myStyles[0];
    myStyles[1].normalForeColor = green;
    myStyles[1].selectBackColor = green;
    myStyles[1].hiliteForeColor = green;
    myStyles[4] = myStyles[1];
    myStyles[4].style = STYLE_UNDERLINE;
    // set error text style
    myStyles[2] = myStyles[0];
    myStyles[2].normalForeColor = red;
    myStyles[2].selectBackColor = red;
    myStyles[2].hiliteForeColor = red;
    myStyles[5] = myStyles[2];
    myStyles[5].style = STYLE_UNDERLINE;
    // set warning text style
    myStyles[3] = myStyles[0];
    myStyles[3].normalForeColor = yellow;
    myStyles[3].selectBackColor = yellow;
    myStyles[3].hiliteForeColor = yellow;
    myStyles[6] = myStyles[3];
    myStyles[6].style = STYLE_UNDERLINE;
    // set GLDebug text style
    myStyles[7] = myStyles[0];
    myStyles[7].normalForeColor = fuchsia;
    myStyles[7].selectBackColor = fuchsia;
    myStyles[7].hiliteForeColor = fuchsia;
}

/****************************************************************************/
