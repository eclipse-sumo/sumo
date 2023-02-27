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
/// @file    GUIGlChildWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
///
//
/****************************************************************************/
#include <config.h>

#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/foxtools/MFXMenuButtonTooltip.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXComboBox.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIMainWindow.h>

#include "GUIGlChildWindow.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GUIGlChildWindow) GUIGlChildWindowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTERVIEW,                   GUIGlChildWindow::onCmdRecenterView),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_I_EDITVIEWPORT,     GUIGlChildWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS_VIEW,              GUIGlChildWindow::onCmdShowToolTipsView),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS_MENU,              GUIGlChildWindow::onCmdShowToolTipsMenu),
    FXMAPFUNC(SEL_COMMAND,  MID_ZOOM_STYLE,                     GUIGlChildWindow::onCmdZoomStyle),
    FXMAPFUNC(SEL_COMMAND,  MID_COLOURSCHEMECHANGE,             GUIGlChildWindow::onCmdChangeColorScheme),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F9_EDIT_VIEWSCHEME,      GUIGlChildWindow::onCmdEditViewScheme),
};

FXIMPLEMENT(GUIGlChildWindow, FXMDIChild, GUIGlChildWindowMap, ARRAYNUMBER(GUIGlChildWindowMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GUIGlChildWindow::GUIGlChildWindow(FXMDIClient* p, GUIMainWindow* GUIMainWindowParent, FXMDIMenu* mdimenu,
                                   const FXString& name, FXMenuBar* gripNavigationToolbar, FXIcon* ic, FXuint opts, FXint x, FXint y, FXint w, FXint h) :
    FXMDIChild(p, name, ic, mdimenu, opts, x, y, w, h),
    myGUIMainWindowParent(GUIMainWindowParent),
    myGripNavigationToolbar(gripNavigationToolbar),
    myStaticNavigationToolBar(nullptr),
    myView(nullptr) {
    // Make MDI Window Menu
    setTracking();
    // create a vertical frame to add elements
    myChildWindowContentFrame = new FXVerticalFrame(this, GUIDesignChildWindowContentFrame);
    // if menuBarGripElements isn't NULL, use it to create a grip navigation elements. In other cas, create a static FXToolbar
    if (myGripNavigationToolbar == nullptr) {
        myStaticNavigationToolBar = new FXToolBar(myChildWindowContentFrame, GUIDesignToolbarMenuBar);
    }
    // build the tool bars
    buildNavigationToolBar(); // always there (recenter)
    buildColoringToolBar(); // always there (coloring)
    buildScreenshotToolBar(); // always there (screenshot)
}


GUIGlChildWindow::~GUIGlChildWindow() {
    delete myLocatorPopup;
    // only delete static navigation bar if it was created
    if (myStaticNavigationToolBar) {
        delete myStaticNavigationToolBar;
    }
}


void
GUIGlChildWindow::create() {
    FXMDIChild::create();
    // only create static navigation bar if it was created
    if (myStaticNavigationToolBar) {
        myStaticNavigationToolBar->create();
    } else {
        myGripNavigationToolbar->create();
    }
    myLocatorPopup->create();
    myView->create();
}


GUISUMOAbstractView*
GUIGlChildWindow::getView() const {
    return myView;
}


GUIMainWindow*
GUIGlChildWindow::getGUIMainWindowParent() {
    return myGUIMainWindowParent;
}


void
GUIGlChildWindow::buildNavigationToolBar() {
    // build the view settings
    // recenter view
    new MFXButtonTooltip(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
                         myGUIMainWindowParent->getStaticTooltipMenu(), "\tRecenter View\tRecenter view to the simulated area.",
                         GUIIconSubSys::getIcon(GUIIcon::RECENTERVIEW), this, MID_RECENTERVIEW, GUIDesignButtonToolbar);
    // add viewport button
    new MFXButtonTooltip(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
                         myGUIMainWindowParent->getStaticTooltipMenu(), "\tEdit Viewport\tOpens a menu which lets you edit the viewport. (Ctrl+I)",
                         GUIIconSubSys::getIcon(GUIIcon::EDITVIEWPORT), this, MID_HOTKEY_CTRL_I_EDITVIEWPORT, GUIDesignButtonToolbar);
    // toggle button for zooming style
    myZoomStyle = new MFXCheckableButton(false, myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
                                         myGUIMainWindowParent->getStaticTooltipMenu(), "\tToggle Zooming Style\tToggles whether zooming is based at cursor position or at the center of the view.",
                                         GUIIconSubSys::getIcon(GUIIcon::ZOOMSTYLE), this, MID_ZOOM_STYLE, GUIDesignMFXCheckableButtonSquare);
    myZoomStyle->setChecked(getApp()->reg().readIntEntry("gui", "zoomAtCenter", 0) != 1);
    // build the locator popup
    myLocatorPopup = new FXPopup(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar, POPUP_VERTICAL);
    // build locator button
    myLocatorButton = new MFXMenuButtonTooltip(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
            myGUIMainWindowParent->getStaticTooltipMenu(), "\tLocate Structures\tLocate structures within the network.",
            GUIIconSubSys::getIcon(GUIIcon::LOCATE), myLocatorPopup, nullptr, GUIDesignButtonToolbarLocator);
    // add toggle button for tooltips in view on/off (by default unchecked)
    myShowToolTipsView = new MFXCheckableButton(false, myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
            myGUIMainWindowParent->getStaticTooltipMenu(), "\tToggle View Tooltips\tToggles whether tooltips in the view shall be shown.",
            GUIIconSubSys::getIcon(GUIIcon::SHOWTOOLTIPS_VIEW), this, MID_SHOWTOOLTIPS_VIEW, GUIDesignMFXCheckableButtonSquare);
    myGUIMainWindowParent->getStaticTooltipView()->enableStaticToolTip(false);
    myGUIMainWindowParent->getStaticTooltipView()->enableStaticToolTip(false);
    // add toggle button for tooltips in menu on/off (by default checked)
    myShowToolTipsMenu = new MFXCheckableButton(false, myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
            myGUIMainWindowParent->getStaticTooltipMenu(), "\tToggle Menu Tooltips\tToggles whether tooltips in the menu shall be shown.",
            GUIIconSubSys::getIcon(GUIIcon::SHOWTOOLTIPS_MENU), this, MID_SHOWTOOLTIPS_MENU, GUIDesignMFXCheckableButtonSquare);
    myShowToolTipsMenu->setChecked(getApp()->reg().readIntEntry("gui", "menuToolTips", 0) != 1);
    myGUIMainWindowParent->getStaticTooltipMenu()->enableStaticToolTip(myShowToolTipsMenu->amChecked());
}


void
GUIGlChildWindow::buildColoringToolBar() {
    // Create Vertical separator
    new FXVerticalSeparator(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar, GUIDesignVerticalSeparator);
    // build coloring tools
    myColoringSchemes = new MFXComboBox(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
                                        GUIDesignComboBoxNCol, this, MID_COLOURSCHEMECHANGE, GUIDesignComboBoxStatic);
    // editor
    new MFXButtonTooltip(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
                         myGUIMainWindowParent->getStaticTooltipMenu(), "\tEdit Coloring Schemes\tOpens a menu which lets you edit the coloring schemes. (F9)",
                         GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL), this, MID_HOTKEY_F9_EDIT_VIEWSCHEME, GUIDesignButtonToolbar);
}


void
GUIGlChildWindow::buildScreenshotToolBar() {
    // Create Vertical separator
    new FXVerticalSeparator(myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar, GUIDesignVerticalSeparator);
    // snapshot
    new MFXCheckableButton(false, myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar,
                           myGUIMainWindowParent->getStaticTooltipMenu(), "\tMake Snapshot\tMakes a snapshot of the view.",
                           GUIIconSubSys::getIcon(GUIIcon::CAMERA), this, MID_MAKESNAPSHOT, GUIDesignButtonToolbar);
}


FXGLCanvas*
GUIGlChildWindow::getBuildGLCanvas() const {
    return myView;
}


FXToolBar*
GUIGlChildWindow::getNavigationToolBar(GUISUMOAbstractView&) {
    return myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar;
}


FXPopup*
GUIGlChildWindow::getLocatorPopup() {
    return myLocatorPopup;
}


FXComboBox*
GUIGlChildWindow::getColoringSchemesCombo() {
    return myColoringSchemes;
}


long
GUIGlChildWindow::onCmdRecenterView(FXObject*, FXSelector, void*) {
    myView->recenterView();
    myView->update();
    return 1;
}


long
GUIGlChildWindow::onCmdEditViewport(FXObject*, FXSelector, void*) {
    myView->showViewportEditor();
    return 1;
}


long
GUIGlChildWindow::onCmdEditViewScheme(FXObject*, FXSelector, void*) {
    myView->showViewschemeEditor();
    return 1;
}


long
GUIGlChildWindow::onCmdShowToolTipsView(FXObject*, FXSelector, void*) {
    // toggle check
    myShowToolTipsView->setChecked(!myShowToolTipsView->amChecked());
    // enable/disable static tooltip
    myGUIMainWindowParent->getStaticTooltipView()->enableStaticToolTip(myShowToolTipsView->amChecked());
    update();
    return 1;
}


long
GUIGlChildWindow::onCmdShowToolTipsMenu(FXObject*, FXSelector, void*) {
    // toggle check
    myShowToolTipsMenu->setChecked(!myShowToolTipsMenu->amChecked());
    // enable/disable static tooltip
    myGUIMainWindowParent->getStaticTooltipMenu()->enableStaticToolTip(myShowToolTipsMenu->amChecked());
    // save in registry
    getApp()->reg().writeIntEntry("gui", "menuToolTips", myShowToolTipsMenu->amChecked() ? 0 : 1);
    update();
    return 1;
}


long
GUIGlChildWindow::onCmdZoomStyle(FXObject*, FXSelector, void*) {
    myZoomStyle->setChecked(!myZoomStyle->amChecked());
    getApp()->reg().writeIntEntry("gui", "zoomAtCenter", myZoomStyle->amChecked() ? 0 : 1);
    update();
    myView->update();
    return 1;
}


long
GUIGlChildWindow::onCmdChangeColorScheme(FXObject*, FXSelector, void* ptr) {
    myView->setColorScheme((char*) ptr);
    return 1;
}


void
GUIGlChildWindow::setView(GUIGlID id) {
    // this is used by the locator widget. zooming to bounding box
    myView->centerTo(id, true, -1);
    myView->update();
}


bool
GUIGlChildWindow::isSelected(GUIGlObject* o) const {
    return gSelected.isSelected(o->getType(), o->getGlID());
}

/****************************************************************************/
