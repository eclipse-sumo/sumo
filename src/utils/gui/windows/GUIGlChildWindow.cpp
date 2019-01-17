/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGlChildWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIMainWindow.h>

#include "GUIGlChildWindow.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIGlChildWindow) GUIGlChildWindowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTERVIEW,       GUIGlChildWindow::onCmdRecenterView),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITVIEWPORT,       GUIGlChildWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS,       GUIGlChildWindow::onCmdShowToolTips),
    FXMAPFUNC(SEL_COMMAND,  MID_ZOOM_STYLE,         GUIGlChildWindow::onCmdZoomStyle),
    FXMAPFUNC(SEL_COMMAND,  MID_COLOURSCHEMECHANGE, GUIGlChildWindow::onCmdChangeColorScheme),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITVIEWSCHEME,     GUIGlChildWindow::onCmdEditViewScheme),
};

FXIMPLEMENT(GUIGlChildWindow, FXMDIChild, GUIGlChildWindowMap, ARRAYNUMBER(GUIGlChildWindowMap))


// ===========================================================================
// member method definitions
// ===========================================================================
GUIGlChildWindow::GUIGlChildWindow(FXMDIClient* p, GUIMainWindow* parentWindow, FXMDIMenu* mdimenu, 
    const FXString& name, bool gripElements, FXIcon* ic, FXuint opts, FXint x, FXint y, FXint w, FXint h) :
    FXMDIChild(p, name, ic, mdimenu, opts, x, y, w, h),
    myView(nullptr),
    myParent(parentWindow) {
    // Make MDI Window Menu
    setTracking();
    // create a vertical frame to add elements
    myContentFrame = new FXVerticalFrame(this, GUIDesignFrameArea);
    // build navigation toolbar (for center, zoom, etc...)
    myNavigationToolBarDrag = new FXToolBarShell(parentWindow, GUIDesignToolBarShell3);
    myNavigationToolBar = new FXMenuBar(parentWindow, myNavigationToolBarDrag, LAYOUT_SIDE_TOP | FRAME_RAISED);
    // declare toolbar grip for menu bar
    new FXToolBarGrip(myNavigationToolBar, myNavigationToolBar, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build the tool bars
    buildNavigationToolBar(); // always there (recenter)
    buildColoringToolBar(); // always there (coloring)
    buildScreenshotToolBar(); // always there (screenshot)
    /*
    // Build navigation toolbar
    myNavigationToolBar = new FXToolBar(myContentFrame, GUIDesignBar);
    */
}


GUIGlChildWindow::~GUIGlChildWindow() {
    delete myLocatorPopup;
    delete myNavigationToolBar;
}


void
GUIGlChildWindow::create() {
    FXMDIChild::create();
    myNavigationToolBar->create();
    myLocatorPopup->create();
    myView->create();
}


GUISUMOAbstractView* 
GUIGlChildWindow::getView() const {
    return myView;
}


GUIMainWindow* 
GUIGlChildWindow::getParent() {
    return myParent;
}


void
GUIGlChildWindow::buildNavigationToolBar() {
    // build the view settings
    // recenter view
    new FXButton(myNavigationToolBar,
                 "\tRecenter View\tRecenter view to the simulated area.",
                 GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_RECENTERVIEW, GUIDesignButtonToolbar);
    // add viewport button
    new FXButton(myNavigationToolBar,
                 "\tEdit Viewport...\tOpens a menu which lets you edit the viewport.",
                 GUIIconSubSys::getIcon(ICON_EDITVIEWPORT), this, MID_EDITVIEWPORT, GUIDesignButtonToolbar);
    // toggle button for zooming style
    MFXCheckableButton* zoomBut = new MFXCheckableButton(false, myNavigationToolBar,
            "\tToggles Zooming Style\tToggles whether zooming is based at cursor position or at the center of the view.",
            GUIIconSubSys::getIcon(ICON_ZOOMSTYLE), this, MID_ZOOM_STYLE, GUIDesignButtonToolbarCheckable);
    zoomBut->setChecked(getApp()->reg().readIntEntry("gui", "zoomAtCenter", 1) != 1);

    // build the locator popup
    myLocatorPopup = new FXPopup(myNavigationToolBar, POPUP_VERTICAL);
    myLocatorButton = new FXMenuButton(myNavigationToolBar, "\tLocate Structures\tLocate structures within the network.",
                                       GUIIconSubSys::getIcon(ICON_LOCATE), myLocatorPopup,
                                       GUIDesignButtonToolbarLocator);
    // add toggle button for tool-tips on/off
    new MFXCheckableButton(false, myNavigationToolBar,
                           "\tToggles Tool Tips\tToggles whether tool tips shall be shown.",
                           GUIIconSubSys::getIcon(ICON_SHOWTOOLTIPS), this, MID_SHOWTOOLTIPS, GUIDesignButtonToolbarCheckable);
}


void
GUIGlChildWindow::buildColoringToolBar() {
    // Create Vertical separator
    new FXVerticalSeparator(myNavigationToolBar, GUIDesignVerticalSeparator);
    // build coloring tools
    // combo
    myColoringSchemes = new FXComboBox(myNavigationToolBar, GUIDesignComboBoxNCol, this, MID_COLOURSCHEMECHANGE, GUIDesignComboBoxStatic);
    // editor
    new FXButton(myNavigationToolBar,
                 "\tEdit Coloring Schemes...\tOpens a menu which lets you edit the coloring schemes.",
                 GUIIconSubSys::getIcon(ICON_COLORWHEEL), this, MID_EDITVIEWSCHEME, GUIDesignButtonToolbar);
}


void
GUIGlChildWindow::buildScreenshotToolBar() {
    // Create Vertical separator
    new FXVerticalSeparator(myNavigationToolBar, GUIDesignVerticalSeparator);
    // snapshot
    new MFXCheckableButton(false, myNavigationToolBar,
                           "\tMake Snapshot\tMakes a snapshot of the view.",
                           GUIIconSubSys::getIcon(ICON_CAMERA), this, MID_MAKESNAPSHOT, GUIDesignButtonToolbar);
}


FXGLCanvas*
GUIGlChildWindow::getBuildGLCanvas() const {
    return myView;
}


FXToolBar*
GUIGlChildWindow::getNavigationToolBar(GUISUMOAbstractView&) {
    return myNavigationToolBar;
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
GUIGlChildWindow::onCmdShowToolTips(FXObject* sender, FXSelector, void*) {
    MFXCheckableButton* button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    myView->showToolTips(button->amChecked());
    update();
    myView->update();
    return 1;
}


long
GUIGlChildWindow::onCmdZoomStyle(FXObject* sender, FXSelector, void*) {
    MFXCheckableButton* button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    getApp()->reg().writeIntEntry("gui", "zoomAtCenter",
                                  button->amChecked() ? 0 : 1);
    update();
    myView->update();
    return 1;
}


long
GUIGlChildWindow::onCmdChangeColorScheme(FXObject*, FXSelector , void* data) {
    myView->setColorScheme((char*) data);
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

