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

#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include "GUIGlChildWindow.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
GUIGlChildWindow::GUIGlChildWindow(
    FXMDIClient* p,
    GUIMainWindow* parentWindow,
    FXMDIMenu* mdimenu, const FXString& name,
    FXIcon* ic,
    FXuint opts, FXint x, FXint y, FXint w, FXint h) :
    FXMDIChild(p, name, ic, mdimenu, opts, x, y, w, h),
    myView(0),
    myParent(parentWindow) {
    // Make MDI Window Menu
    setTracking();
    myContentFrame =
        new FXVerticalFrame(this,
                            FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y,
                            0, 0, 0, 0, 0, 0, 0, 0);
    // build the tool bar
    buildNavigationToolBar(); // always there (recenter)
    buildColoringToolBar(); // always there (coloring)
    buildScreenshotToolBar(); // always there (screen shot)
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


void
GUIGlChildWindow::buildNavigationToolBar() {
    myNavigationToolBar = new FXToolBar(myContentFrame, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED);
    // build the view settings
    // recenter view
    new FXButton(myNavigationToolBar,
                 "\tRecenter View\tRecenter view to the simulated area.",
                 GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_RECENTERVIEW,
                 ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
    // add viewport button
    new FXButton(myNavigationToolBar,
                 "\tEdit Viewport...\tOpens a menu which lets you edit the viewport.",
                 GUIIconSubSys::getIcon(ICON_EDITVIEWPORT), this, MID_EDITVIEWPORT,
                 ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
    // toggle button for zooming style
    MFXCheckableButton* zoomBut = new MFXCheckableButton(false, myNavigationToolBar,
            "\tToggles Zooming Style\tToggles whether zooming is based at cursor position or at the center of the view.",
            GUIIconSubSys::getIcon(ICON_ZOOMSTYLE), this, MID_ZOOM_STYLE,
            BUTTON_NORMAL | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT,
            0, 0, 23, 23);
    zoomBut->setChecked(getApp()->reg().readIntEntry("gui", "zoomAtCenter", 1) != 1);

    // build the locator popup
    myLocatorPopup = new FXPopup(myNavigationToolBar, POPUP_VERTICAL);
    myLocatorButton = new FXMenuButton(myNavigationToolBar, "\tLocate Structures\tLocate structures within the network.",
                                       GUIIconSubSys::getIcon(ICON_LOCATE), myLocatorPopup,
                                       MENUBUTTON_RIGHT | LAYOUT_TOP | BUTTON_TOOLBAR | FRAME_RAISED | FRAME_THICK);
    // add toggle button for tool-tips on/off
    new MFXCheckableButton(false, myNavigationToolBar,
                           "\tToggles Tool Tips\tToggles whether tool tips shall be shown.",
                           GUIIconSubSys::getIcon(ICON_SHOWTOOLTIPS), this, MID_SHOWTOOLTIPS,
                           BUTTON_NORMAL | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT,
                           0, 0, 23, 23);

}


void
GUIGlChildWindow::buildColoringToolBar() {
    // in fact, we currently are not able to build another bar
    //  we just add somthing to the navigation bar
//    myColoringToolBar = new FXToolBar(myContentFrame,LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myNavigationToolBar, NULL, 0, TOOLBARGRIP_SINGLE | FRAME_SUNKEN);

    // build coloring tools
    // combo
    myColoringSchemes =
        new FXComboBox(myNavigationToolBar, 12, this, MID_COLOURSCHEMECHANGE,
                       FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y);
    // editor
    new FXButton(myNavigationToolBar,
                 "\tEdit Coloring Schemes...\tOpens a menu which lets you edit the coloring schemes.",
                 GUIIconSubSys::getIcon(ICON_COLORWHEEL), this, MID_EDITVIEWSCHEME,
                 ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
}


void
GUIGlChildWindow::buildScreenshotToolBar() {
    // in fact, we currently are not able to build another bar
    //  we just add somthing to the navigation bar
//    myScreenshotToolBar = new FXToolBar(myContentFrame,LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myNavigationToolBar, NULL, 0, TOOLBARGRIP_SINGLE | FRAME_SUNKEN);
    // snapshot
    new FXButton(myNavigationToolBar,
                 "\tMake Snapshot\tMakes a snapshot of the view.",
                 GUIIconSubSys::getIcon(ICON_CAMERA), this, MID_MAKESNAPSHOT,
                 ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
}


FXGLCanvas*
GUIGlChildWindow::getBuildGLCanvas() const {
    return myView;
}


FXToolBar&
GUIGlChildWindow::getNavigationToolBar(GUISUMOAbstractView&) {
    return *myNavigationToolBar;
}


FXPopup*
GUIGlChildWindow::getLocatorPopup() {
    return myLocatorPopup;
}


FXComboBox&
GUIGlChildWindow::getColoringSchemesCombo() {
    return *myColoringSchemes;
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
    // this is used by the locator widget. zoom using fixed distance makes it easier to identify objects
    // (long streets in particular)
    myView->centerTo(id, true);
    myView->update();
}


bool
GUIGlChildWindow::isSelected(GUIGlObject* o) const {
    return gSelected.isSelected(o->getType(), o->getGlID());
}
/****************************************************************************/

