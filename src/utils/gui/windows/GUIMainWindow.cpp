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
/// @file    GUIMainWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
///
//
/****************************************************************************/
#include <config.h>

#include <string>
#include <algorithm>
#include <utils/foxtools/fxheader.h>
// fx3d includes windows.h so we need to guard against macro pollution
#ifdef WIN32
#define NOMINMAX
#endif
#include <fx3d.h>
#ifdef WIN32
#undef NOMINMAX
#endif
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/foxtools/MFXStaticToolTip.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/options/OptionsCont.h>
#include "GUIMainWindow.h"
#include "GUIGlChildWindow.h"


// ===========================================================================
// static member definitions
// ===========================================================================
GUIMainWindow* GUIMainWindow::myInstance = nullptr;

// ===========================================================================
// member method definitions
// ===========================================================================
GUIMainWindow::GUIMainWindow(FXApp* app) :
    FXMainWindow(app, "sumo-gui main window", nullptr, nullptr, DECOR_ALL, 20, 20, 600, 400),
    myAmFullScreen(false),
    myTrackerLock(true),
    myGLVisual(new FXGLVisual(app, VISUAL_DOUBLEBUFFER)),
    myAmGaming(false),
    myListInternal(false),
    myListParking(true),
    myListTeleporting(false) {
    // build static tooltips
    myStaticTooltipMenu = new MFXStaticToolTip(app);
    myStaticTooltipView = new MFXStaticToolTip(app);
    // build bold font
    FXFontDesc fdesc;
    app->getNormalFont()->getFontDesc(fdesc);
    fdesc.weight = FXFont::Bold;
    myBoldFont = new FXFont(app, fdesc);
    // https://en.wikipedia.org/wiki/Noto_fonts should be widely available
    myFallbackFont = new FXFont(app, "Noto Sans CJK JP");
    // build docks
    myTopDock = new FXDockSite(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X);
    myBottomDock = new FXDockSite(this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X);
    myLeftDock = new FXDockSite(this, LAYOUT_SIDE_LEFT | LAYOUT_FILL_Y);
    myRightDock = new FXDockSite(this, LAYOUT_SIDE_RIGHT | LAYOUT_FILL_Y);
    // avoid instance Windows twice
    if (myInstance != nullptr) {
        throw ProcessError("MainWindow initialized twice");
    }
    myInstance = this;
    //myGLVisual->setStencilSize(8); // enable stencil buffer
}


GUIMainWindow::~GUIMainWindow() {
    delete myStaticTooltipMenu;
    delete myStaticTooltipView;
    delete myBoldFont;
    delete myFallbackFont;
    delete myTopDock;
    delete myBottomDock;
    delete myLeftDock;
    delete myRightDock;
    myInstance = nullptr;
}



void
GUIMainWindow::addGLChild(GUIGlChildWindow* child) {
    myGLWindows.push_back(child);
}


void
GUIMainWindow::removeGLChild(GUIGlChildWindow* child) {
    std::vector<GUIGlChildWindow*>::iterator i = std::find(myGLWindows.begin(), myGLWindows.end(), child);
    if (i != myGLWindows.end()) {
        myGLWindows.erase(i);
    }
}


void
GUIMainWindow::addChild(FXMainWindow* child) {
    myTrackerLock.lock();
    myTrackerWindows.push_back(child);
    myTrackerLock.unlock();
}


void
GUIMainWindow::removeChild(FXMainWindow* child) {
    myTrackerLock.lock();
    std::vector<FXMainWindow*>::iterator i = std::find(myTrackerWindows.begin(), myTrackerWindows.end(), child);
    myTrackerWindows.erase(i);
    myTrackerLock.unlock();
}


FXDockSite*
GUIMainWindow::getTopDock() {
    return myTopDock;
}


std::vector<std::string>
GUIMainWindow::getViewIDs() const {
    std::vector<std::string> ret;
    for (GUIGlChildWindow* const window : myGLWindows) {
        ret.push_back(window->getTitle().text());
    }
    return ret;
}


GUIGlChildWindow*
GUIMainWindow::getViewByID(const std::string& id) const {
    for (GUIGlChildWindow* const window : myGLWindows) {
        if (std::string(window->getTitle().text()) == id) {
            return window;
        }
    }
    return nullptr;
}


void
GUIMainWindow::removeViewByID(const std::string& id) {
    for (GUIGlChildWindow* const window : myGLWindows) {
        if (std::string(window->getTitle().text()) == id) {
            window->close();
            removeGLChild(window);
            return;
        }
    }
}


FXFont*
GUIMainWindow::getBoldFont() {
    return myBoldFont;
}

FXFont*
GUIMainWindow::getFallbackFont() {
    return myFallbackFont;
}

const std::vector<GUIGlChildWindow*>&
GUIMainWindow::getViews() const {
    return myGLWindows;
}


void
GUIMainWindow::updateChildren(int msg) {
    // inform views
    myMDIClient->forallWindows(this, FXSEL(SEL_COMMAND, msg), nullptr);
    // inform other windows
    myTrackerLock.lock();
    for (int i = 0; i < (int)myTrackerWindows.size(); i++) {
        myTrackerWindows[i]->handle(this, FXSEL(SEL_COMMAND, msg), nullptr);
    }
    myTrackerLock.unlock();
}


FXGLVisual*
GUIMainWindow::getGLVisual() const {
    return myGLVisual;
}


MFXStaticToolTip*
GUIMainWindow::getStaticTooltipMenu() const {
    return myStaticTooltipMenu;
}


MFXStaticToolTip*
GUIMainWindow::getStaticTooltipView() const {
    return myStaticTooltipView;
}


FXLabel*
GUIMainWindow::getCartesianLabel() {
    return myCartesianCoordinate;
}


FXLabel*
GUIMainWindow::getGeoLabel() {
    return myGeoCoordinate;
}


FXLabel*
GUIMainWindow::getTestLabel() {
    return myTestCoordinate;
}


FXHorizontalFrame*
GUIMainWindow::getTestFrame() {
    return myTestFrame;
}


bool
GUIMainWindow::isGaming() const {
    return myAmGaming;
}


bool
GUIMainWindow::listInternal() const {
    return myListInternal;
}


bool
GUIMainWindow::listParking() const {
    return myListParking;
}


bool
GUIMainWindow::listTeleporting() const {
    return myListTeleporting;
}


GUIMainWindow*
GUIMainWindow::getInstance() {
    if (myInstance != nullptr) {
        return myInstance;
    }
    throw ProcessError("A GUIMainWindow instance was not yet constructed.");
}


GUISUMOAbstractView*
GUIMainWindow::getActiveView() const {
    GUIGlChildWindow* w = dynamic_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    if (w != nullptr) {
        return w->getView();
    }
    return nullptr;
}


void
GUIMainWindow::setWindowSizeAndPos() {
    int windowWidth = getApp()->reg().readIntEntry("SETTINGS", "width", 600);
    int windowHeight = getApp()->reg().readIntEntry("SETTINGS", "height", 400);
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.isSet("window-size")) {
        std::vector<std::string> windowSize = oc.getStringVector("window-size");
        if (windowSize.size() != 2) {
            WRITE_ERROR(TL("option window-size requires INT,INT"));
        } else {
            try {
                windowWidth = StringUtils::toInt(windowSize[0]);
                windowHeight = StringUtils::toInt(windowSize[1]);
            } catch (NumberFormatException& e) {
                WRITE_ERROR("option window-size requires INT,INT " + toString(e.what()));
            }
        }
    }
    if (oc.isSet("window-size") || getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 0 || oc.isSet("window-pos")) {
        // when restoring previous pos, make sure the window fits fully onto the current screen
        int x = MAX2(0, MIN2(getApp()->reg().readIntEntry("SETTINGS", "x", 150), getApp()->getRootWindow()->getWidth() - windowWidth));
        int y = MAX2(50, MIN2(getApp()->reg().readIntEntry("SETTINGS", "y", 150), getApp()->getRootWindow()->getHeight() - windowHeight));
        if (oc.isSet("window-pos")) {
            std::vector<std::string> windowPos = oc.getStringVector("window-pos");
            if (windowPos.size() != 2) {
                WRITE_ERROR(TL("option window-pos requires INT,INT"));
            } else {
                try {
                    x = StringUtils::toInt(windowPos[0]);
                    y = StringUtils::toInt(windowPos[1]);
                } catch (NumberFormatException& e) {
                    WRITE_ERROR("option window-pos requires INT,INT " + toString(e.what()));
                }
            }
        }
        move(x, y);
        resize(windowWidth, windowHeight);
    }
}

void
GUIMainWindow::storeWindowSizeAndPos() {
    if (!myAmFullScreen) {
        getApp()->reg().writeIntEntry("SETTINGS", "x", getX());
        getApp()->reg().writeIntEntry("SETTINGS", "y", getY());
        getApp()->reg().writeIntEntry("SETTINGS", "width", getWidth());
        getApp()->reg().writeIntEntry("SETTINGS", "height", getHeight());
    }
}


void
GUIMainWindow::buildLanguageMenu(FXMenuBar* menuBar) {
    myLanguageMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(menuBar, TL("Langua&ge"), nullptr, myLanguageMenu);

    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "English", "", TL("Change language to english. (en)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_EN), this, MID_LANGUAGE_EN);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "Deutsch", "", TL("Change language to german. (de)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_DE), this, MID_LANGUAGE_DE);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "Español", "", TL("Change language to spanish. (es)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_ES), this, MID_LANGUAGE_ES);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "Français", "", TL("Change language to french. (fr)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_FR), this, MID_LANGUAGE_FR);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "Italiano", "", TL("Change language to italian. (it)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_IT), this, MID_LANGUAGE_IT);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "简体中文", "", TL("简体中文 (zh)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_ZH), this, MID_LANGUAGE_ZH);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "繁體中文", "", TL("繁體中文 (zh-Hant)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_ZHT), this, MID_LANGUAGE_ZHT);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "Türkçe", "", TL("Change language to turkish. (tr)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_TR), this, MID_LANGUAGE_TR);
    GUIDesigns::buildFXMenuCommandShortcut(myLanguageMenu, "Magyar", "", TL("Change language to hungarian. (hu)"),
                                           GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_HU), this, MID_LANGUAGE_HU);
}


long
GUIMainWindow::onCmdChangeLanguage(FXObject*, FXSelector sel, void*) {
    // set language
    std::string langID;
    std::string lang;
    // continue depending of called button
    switch (FXSELID(sel)) {
        case MID_LANGUAGE_DE:
            langID = "de";
            lang = TL("german");
            break;
        case MID_LANGUAGE_ES:
            langID = "es";
            lang = TL("spanish");
            break;
        case MID_LANGUAGE_FR:
            langID = "fr";
            lang = TL("french");
            break;
        case MID_LANGUAGE_IT:
            langID = "it";
            lang = TL("italian");
            break;
        case MID_LANGUAGE_ZH:
            langID = "zh";
            lang = TL("chinese");
            break;
        case MID_LANGUAGE_ZHT:
            langID = "zh-Hant";
            lang = TL("chinese simplified");
            break;
        case MID_LANGUAGE_TR:
            langID = "tr";
            lang = TL("turkish");
            break;
        case MID_LANGUAGE_HU:
            langID = "hu";
            lang = TL("hungarian");
            break;
        default:
            langID = "C";
            lang = TL("english");
            break;
    }
    // check if change language
    if (langID != gLanguage) {
        // update language
        gLanguage = langID;
        // show info
        WRITE_MESSAGE(TL("Language changed to ") + lang);
        // show dialog
        const std::string header = TL("Restart needed");
        const std::string body = TL("Changing display language needs restart to take effect.") + std::string("\n") +
#ifdef DEBUG
#ifdef WIN32
                                 TL("For the Debug build you might also need to set the LANG environment variable.") + std::string("\n") +
#endif
#endif
                                 TL("Under development. You can help to improve the translation at:") + std::string("\n") +
                                 "https://hosted.weblate.org/projects/eclipse-sumo/";
        FXMessageBox::information(getApp(), MBOX_OK, header.c_str(), "%s", body.c_str());
        // update language in registry (common for sumo and netedit)
        std::string appKey = getApp()->reg().getAppKey().text();
        if (appKey == "SUMO GUI") {
            // registry is written again later so we have to modify the "life" version
            getApp()->reg().writeStringEntry("gui", "language", langID.c_str());
        } else {
            FXRegistry reg("SUMO GUI", "sumo-gui");
            reg.read();
            reg.writeStringEntry("gui", "language", langID.c_str());
            reg.write();
        }
    }
    return 1;
}


long
GUIMainWindow::onUpdChangeLanguage(FXObject* obj, FXSelector, void*) {
    // get language menu command
    FXMenuCommand* menuCommand = dynamic_cast<FXMenuCommand*>(obj);
    if (menuCommand) {
        // check if change color
        if ((gLanguage == "C") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_EN))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "de") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_DE))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "es") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_ES))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "fr") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_FR))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "it") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_IT))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "zh") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_ZH))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "zh-Hant") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_ZHT))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "tr") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_TR))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else if ((gLanguage == "hu") && (menuCommand->getIcon() == GUIIconSubSys::getIcon(GUIIcon::LANGUAGE_HU))) {
            menuCommand->setTextColor(FXRGB(0, 0, 255));
        } else {
            menuCommand->setTextColor(FXRGB(0, 0, 0));
        }
    }
    return 1;
}


/****************************************************************************/
