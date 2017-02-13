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
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/gui/div/GUIDesigns.h>
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
#include "GNEInspectorFrame.h"
#include "GNESelectorFrame.h"
#include "GNEConnectorFrame.h"
#include "GNETLSEditorFrame.h"
#include "GNEAdditionalFrame.h"
#include "GNECrossingFrame.h"
#include "GNEDeleteFrame.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEViewParent) GNEViewParentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,                      GNEViewParent::onCmdMakeSnapshot),
    //FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,                   GNEViewParent::onCmdAllowRotation),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION,                    GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,                        GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,                         GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  FXMDIChild::ID_MDI_MENUCLOSE,          GNEViewParent::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_SIZEOF_FRAMEAREAWIDTH_UPDATED, GNEViewParent::onCmdUpdateFrameAreaWidth),
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
    FXGLCanvas* share, GNENet* net, GNEUndoList* undoList,
    FXIcon* ic, FXuint opts,
    FXint x, FXint y, FXint w, FXint h):
    GUIGlChildWindow(p, parentWindow, mdimenu, name, ic, opts, x, y, w, h) {
    // Add child to parent
    myParent->addChild(this, false);

    // disable coloring and screenshot
    //for (int i=5; i < myNavigationToolBar->numChildren(); i++) {
    //    myNavigationToolBar->childAtIndex(i)->hide();
    //}

    // add undo/redo buttons
    new FXButton(myNavigationToolBar, "\tUndo\tUndo the last Change.", GUIIconSubSys::getIcon(ICON_UNDO), parentWindow->getUndoList(), FXUndoList::ID_UNDO, GUIDesignButtonToolbar);
    new FXButton(myNavigationToolBar, "\tRedo\tRedo the last Change.", GUIIconSubSys::getIcon(ICON_REDO), parentWindow->getUndoList(), FXUndoList::ID_REDO, GUIDesignButtonToolbar);

    // Create Vertical separator
    new FXVerticalSeparator(myNavigationToolBar, GUIDesignVerticalSeparator);

    // Create Frame Splitter
    myFramesSplitter = new FXSplitter(myContentFrame, this, MID_GNE_SIZEOF_FRAMEAREAWIDTH_UPDATED, GUIDesignSplitter | SPLITTER_HORIZONTAL);

    // Create frames Area
    myFramesArea = new FXHorizontalFrame(myFramesSplitter, GUIDesignFrameArea);

    // Set default width of frames area
    myFramesArea->setWidth(220);

    // Create view area
    myViewArea = new FXHorizontalFrame(myFramesSplitter, GUIDesignViewnArea);

    // Add the view to a temporary parent so that we can add items to myViewArea in the desired order
    FXComposite* tmp = new FXComposite(this);

    // Create view net
    GNEViewNet* viewNet = new GNEViewNet(tmp, myViewArea, *myParent, this, net, undoList, myParent->getGLVisual(), share, myNavigationToolBar);

    // Set pointer myView with the created view net
    myView = viewNet;

    // Create frames
    myGNEFrames[MID_GNE_MODE_INSPECT] = new GNEInspectorFrame(myFramesArea, viewNet);
    myGNEFrames[MID_GNE_MODE_SELECT] = new GNESelectorFrame(myFramesArea, viewNet);
    myGNEFrames[MID_GNE_MODE_CONNECT] = new GNEConnectorFrame(myFramesArea, viewNet);
    myGNEFrames[MID_GNE_MODE_TLS] = new GNETLSEditorFrame(myFramesArea, viewNet);
    myGNEFrames[MID_GNE_MODE_ADDITIONAL] = new GNEAdditionalFrame(myFramesArea, viewNet);
    myGNEFrames[MID_GNE_MODE_CROSSING] = new GNECrossingFrame(myFramesArea, viewNet);
    myGNEFrames[MID_GNE_MODE_DELETE] = new GNEDeleteFrame(myFramesArea, viewNet);

    // Update frame areas after creation
    onCmdUpdateFrameAreaWidth(0, 0, 0);

    // Hidde all Frames Area
    hideFramesArea();

    //  Buld view toolBars
    myView->buildViewToolBars(*this);

    // create windows
    GUIGlChildWindow::create();
}


GNEViewParent::~GNEViewParent() {
    // Remove child before remove
    myParent->removeChild(this);
}


void
GNEViewParent::hideAllFrames() {
    for (std::map<int, GNEFrame*>::iterator i = myGNEFrames.begin(); i != myGNEFrames.end(); i++) {
        i->second->hide();
    }
}

GNEInspectorFrame*
GNEViewParent::getInspectorFrame() const {
    return dynamic_cast<GNEInspectorFrame*>(myGNEFrames.at(MID_GNE_MODE_INSPECT));
}


GNESelectorFrame*
GNEViewParent::getSelectorFrame() const {
    return dynamic_cast<GNESelectorFrame*>(myGNEFrames.at(MID_GNE_MODE_SELECT));
}


GNEConnectorFrame*
GNEViewParent::getConnectorFrame() const {
    return dynamic_cast<GNEConnectorFrame*>(myGNEFrames.at(MID_GNE_MODE_CONNECT));
}


GNETLSEditorFrame*
GNEViewParent::getTLSEditorFrame() const {
    return dynamic_cast<GNETLSEditorFrame*>(myGNEFrames.at(MID_GNE_MODE_TLS));
}


GNEAdditionalFrame*
GNEViewParent::getAdditionalFrame() const {
    return dynamic_cast<GNEAdditionalFrame*>(myGNEFrames.at(MID_GNE_MODE_ADDITIONAL));
}


GNECrossingFrame*
GNEViewParent::getCrossingFrame() const {
    return dynamic_cast<GNECrossingFrame*>(myGNEFrames.at(MID_GNE_MODE_CROSSING));
}


GNEDeleteFrame*
GNEViewParent::getDeleteFrame() const {
    return dynamic_cast<GNEDeleteFrame*>(myGNEFrames.at(MID_GNE_MODE_DELETE));
}


void
GNEViewParent::showFramesArea() {
    bool showFlag = false;
    // Iterate over GNEFrames
    for (std::map<int, GNEFrame*>::iterator i = myGNEFrames.begin(); i != myGNEFrames.end(); i++) {
        // if at least one frame is shown, change showFlag
        if (i->second->shown() == true) {
            showFlag = true;
        }
    }
    // show and recalc framesArea if showFlag is enabled
    if (showFlag) {
        myFramesArea->recalc();
        myFramesArea->show();
    }
}


void
GNEViewParent::hideFramesArea() {
    bool hideFlag = true;
    // Iterate over frames
    for (std::map<int, GNEFrame*>::iterator i = myGNEFrames.begin(); i != myGNEFrames.end(); i++) {
        // if at least one frame is shown,  change hideflag
        if (i->second->shown() == true) {
            hideFlag = false;
        }
    }
    // hide and recalc frames Area if hideFlag is enabled
    if (hideFlag) {
        myFramesArea->hide();
        myFramesArea->recalc();
    }
}


GUIMainWindow*
GNEViewParent::getApp() const {
    return myParent;
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


long
GNEViewParent::onCmdUpdateFrameAreaWidth(FXObject*, FXSelector, void*) {
    for (std::map<int, GNEFrame*>::iterator i = myGNEFrames.begin(); i != myGNEFrames.end(); i++) {
        // update size of all GNEFrame
        i->second->setFrameWidth(myFramesArea->getWidth());
    }
    return 0;
}

/****************************************************************************/

