/****************************************************************************/
/// @file    GNEViewParent.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A single child window which contains a view of the edited network (adapted
// from GUISUMOViewParent)
// While we don't actually need MDI for netedit it is easier to adapt existing
// structures than to write everything from scratch.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <vector>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_GLObjChooser.h>

#include "GNENet.h"
#include "GNEEdge.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNEUndoList.h"
#include "GNEApplicationWindow.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEViewParent) GNEViewParentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,             GNEViewParent::onCmdMakeSnapshot),
    //FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,          GNEViewParent::onCmdAllowRotation),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION,           GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,               GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,                GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  FXMDIChild::ID_MDI_MENUCLOSE, GNEViewParent::onCmdClose),
};

// Object implementation
FXIMPLEMENT(GNEViewParent, GUIGlChildWindow, GNEViewParentMap, ARRAYNUMBER(GNEViewParentMap))

// ===========================================================================
// member method definitions
// ===========================================================================
GNEViewParent::GNEViewParent(
    FXMDIClient* p, FXMDIMenu* mdimenu,
    const FXString& name,
    GNEApplicationWindow* parentWindow,
    FXGLCanvas* share, GNENet* net,
    FXIcon* ic, FXuint opts,
    FXint x, FXint y, FXint w, FXint h):
    GUIGlChildWindow(p, parentWindow, mdimenu, name, ic, opts, x, y, w, h) {
    myParent->addChild(this, false);

    // disable coloring and screenshot
    //for (int i=5; i < myNavigationToolBar->numChildren(); i++) {
    //    myNavigationToolBar->childAtIndex(i)->hide();
    //}

    // add undo/redo buttons
    new FXButton(myNavigationToolBar,
                 "\tUndo\tUndo the last Change.",
                 GUIIconSubSys::getIcon(ICON_UNDO), parentWindow->getUndoList(), FXUndoList::ID_UNDO,
                 ICON_BEFORE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
    new FXButton(myNavigationToolBar,
                 "\tRedo\tRedo the last Change.",
                 GUIIconSubSys::getIcon(ICON_REDO), parentWindow->getUndoList(), FXUndoList::ID_REDO,
                 ICON_BEFORE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
    new FXToolBarGrip(myNavigationToolBar, NULL, 0, TOOLBARGRIP_SINGLE | FRAME_SUNKEN);

    myViewArea = new FXHorizontalFrame(myContentFrame,
                                       FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y,
                                       0, 0, 0, 0, 0, 0, 0, 0);
    // we add the view to a temporary parent so that we can add items to
    // myViewArea in the desired order
    FXComposite* tmp = new FXComposite(this);
    myView = new GNEViewNet(tmp, myViewArea,
                            *myParent, this, net,
                            myParent->getGLVisual(), share,
                            myNavigationToolBar);

    myView->buildViewToolBars(*this);

    // create
    GUIGlChildWindow::create();
}


GNEViewParent::~GNEViewParent() {
    myParent->removeChild(this);
}


long
GNEViewParent::onCmdMakeSnapshot(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Save Snapshot");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("All Image Files (*.gif, *.bmp, *.xpm, *.pcx, *.ico, *.rgb, *.xbm, *.tga, *.png, *.jpg, *.jpeg, *.tif, *.tiff, *.ps, *.eps, *.pdf, *.svg, *.tex, *.pgf)\n"
                              "GIF Image (*.gif)\nBMP Image (*.bmp)\nXPM Image (*.xpm)\nPCX Image (*.pcx)\nICO Image (*.ico)\n"
                              "RGB Image (*.rgb)\nXBM Image (*.xbm)\nTARGA Image (*.tga)\nPNG Image  (*.png)\n"
                              "JPEG Image (*.jpg, *.jpeg)\nTIFF Image (*.tif, *.tiff)\n"
                              "Postscript (*.ps)\nEncapsulated Postscript (*.eps)\nPortable Document Format (*.pdf)\n"
                              "Scalable Vector Graphics (*.svg)\nLATEX text strings (*.tex)\nPortable LaTeX Graphics (*.pgf)\n"
                              "All Files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute() || !MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    gCurrentFolder = opendialog.getDirectory();
    std::string file = opendialog.getFilename().text();
    std::string error = myView->makeSnapshot(file);
    if (error != "") {
        FXMessageBox::error(this, MBOX_OK, "Saving failed.", "%s", error.c_str());
    }
    return 1;
}


long
GNEViewParent::onCmdClose(FXObject*, FXSelector /* sel */, void*) {
    myParent->handle(this, FXSEL(SEL_COMMAND, MID_CLOSE), 0);
    return 1;
}


long
GNEViewParent::onCmdLocate(FXObject*, FXSelector sel, void*) {
    GNEViewNet* view = dynamic_cast<GNEViewNet*>(myView);
    assert(view);
    GUIGlObjectType type;
    GUIIcon icon;
    std::string title;
    switch (FXSELID(sel)) {
        case MID_LOCATEJUNCTION:
            type = GLO_JUNCTION;
            icon = ICON_LOCATEJUNCTION;
            title = "Junction Chooser";
            break;
        case MID_LOCATEEDGE:
            type = GLO_EDGE;
            icon = ICON_LOCATEEDGE;
            title = "Edge Chooser";
            break;
        case MID_LOCATETLS:
            type = GLO_TLLOGIC;
            icon = ICON_LOCATETLS;
            title = "Traffic-Light-Junctions Chooser";
            break;
        default:
            throw ProcessError("Unknown Message ID in onCmdLocate");
    }
    std::set<GUIGlID> idSet = view->getNet()->getGlIDs(type);
    std::vector<GUIGlID> ids(idSet.begin(), idSet.end());
    myLocatorPopup->popdown();
    myLocatorButton->killFocus();
    myLocatorPopup->update();
    GUIDialog_GLObjChooser* chooser = new GUIDialog_GLObjChooser(
        this, GUIIconSubSys::getIcon(icon), title.c_str(), ids, GUIGlObjectStorage::gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


bool
GNEViewParent::isSelected(GUIGlObject* o) const {
    GUIGlObjectType type = o->getType();
    if (gSelected.isSelected(type, o->getGlID())) {
        return true;
    } else if (type == GLO_EDGE) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(o);
        assert(edge);
        const std::set<GUIGlID> laneIDs = edge->getLaneGlIDs();
        for (std::set<GUIGlID>::const_iterator it = laneIDs.begin(); it != laneIDs.end(); it++) {
            if (gSelected.isSelected(GLO_LANE, *it)) {
                return true;
            }
        }
        return false;
    } else {
        return false;
    }
}


long
GNEViewParent::onKeyPress(FXObject* o, FXSelector sel, void* data) {
    myView->onKeyPress(o, sel, data);
    return 0;
}


long
GNEViewParent::onKeyRelease(FXObject* o, FXSelector sel, void* data) {
    myView->onKeyRelease(o, sel, data);
    return 0;
}

/****************************************************************************/

