/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEViewParent.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A single child window which contains a view of the edited network (adapted
// from GUISUMOViewParent)
// While we don't actually need MDI for netedit it is easier to adapt existing
// structures than to write everything from scratch.
/****************************************************************************/

#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/dialogs/GNEDialogACChooser.h>
#include <netedit/frames/common/GNEDeleteFrame.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <netedit/frames/data/GNEEdgeRelDataFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNEPersonTypeFrame.h>
#include <netedit/frames/demand/GNERouteFrame.h>
#include <netedit/frames/demand/GNEStopFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/demand/GNEVehicleTypeFrame.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>
#include <netedit/frames/network/GNEConnectorFrame.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/frames/network/GNECrossingFrame.h>
#include <netedit/frames/network/GNEPolygonFrame.h>
#include <netedit/frames/network/GNEProhibitionFrame.h>
#include <netedit/frames/network/GNETAZFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEApplicationWindow.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEViewParent.h"
#include "GNEUndoList.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEViewParent) GNEViewParentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,                       GNEViewParent::onCmdMakeSnapshot),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION,                     GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,                         GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,                      GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPERSON,                       GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEROUTE,                        GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATESTOP,                         GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,                          GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,                          GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,                          GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,                         GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  FXMDIChild::ID_MDI_MENUCLOSE,           GNEViewParent::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_VIEWPARENT_FRAMEAREAWIDTH,      GNEViewParent::onCmdUpdateFrameAreaWidth),
};

// Object implementation
FXIMPLEMENT(GNEViewParent, GUIGlChildWindow, GNEViewParentMap, ARRAYNUMBER(GNEViewParentMap))


// ===========================================================================
// member method definitions
// ===========================================================================

GNEViewParent::GNEViewParent(FXMDIClient* p, FXMDIMenu* mdimenu, const FXString& name, GNEApplicationWindow* parentWindow,
                             FXGLCanvas* share, GNENet* net, GNEUndoList* undoList, FXIcon* ic, FXuint opts, FXint x, FXint y, FXint w, FXint h) :
    GUIGlChildWindow(p, parentWindow, mdimenu, name, parentWindow->getToolbarsGrip().navigation, ic, opts, x, y, w, h),
    myGNEAppWindows(parentWindow) {
    // Add child to parent
    myParent->addGLChild(this);

    // Create Vertical separator
    new FXVerticalSeparator(myGripNavigationToolbar, GUIDesignVerticalSeparator);

    // Create undo/redo buttons
    myUndoButton = new FXButton(myGripNavigationToolbar, "\tUndo\tUndo the last Change.", GUIIconSubSys::getIcon(GUIIcon::UNDO), parentWindow, MID_HOTKEY_CTRL_Z_UNDO, GUIDesignButtonToolbar);
    myRedoButton = new FXButton(myGripNavigationToolbar, "\tRedo\tRedo the last Change.", GUIIconSubSys::getIcon(GUIIcon::REDO), parentWindow, MID_HOTKEY_CTRL_Y_REDO, GUIDesignButtonToolbar);

    // Create Frame Splitter
    myFramesSplitter = new FXSplitter(myContentFrame, this, MID_GNE_VIEWPARENT_FRAMEAREAWIDTH, GUIDesignSplitter | SPLITTER_HORIZONTAL);

    // Create frames Area
    myFramesArea = new FXHorizontalFrame(myFramesSplitter, GUIDesignFrameArea);

    // Set default width of frames area
    myFramesArea->setWidth(220);

    // Create view area
    myViewArea = new FXHorizontalFrame(myFramesSplitter, GUIDesignViewnArea);

    // Add the view to a temporary parent so that we can add items to myViewArea in the desired order
    FXComposite* tmp = new FXComposite(this);

    // Create view net
    GNEViewNet* viewNet = new GNEViewNet(tmp, myViewArea, *myParent, this, net, undoList, myParent->getGLVisual(), share);

    // show toolbar grips
    myGNEAppWindows->getToolbarsGrip().buildMenuToolbarsGrip();

    // Set pointer myView with the created view net
    myView = viewNet;

    // Create Network frames
    myFrames.inspectorFrame = new GNEInspectorFrame(myFramesArea, viewNet);
    myFrames.selectorFrame = new GNESelectorFrame(myFramesArea, viewNet);
    myFrames.connectorFrame = new GNEConnectorFrame(myFramesArea, viewNet);
    myFrames.prohibitionFrame = new GNEProhibitionFrame(myFramesArea, viewNet);
    myFrames.TLSEditorFrame = new GNETLSEditorFrame(myFramesArea, viewNet);
    myFrames.additionalFrame = new GNEAdditionalFrame(myFramesArea, viewNet);
    myFrames.crossingFrame = new GNECrossingFrame(myFramesArea, viewNet);
    myFrames.TAZFrame = new GNETAZFrame(myFramesArea, viewNet);
    myFrames.deleteFrame = new GNEDeleteFrame(myFramesArea, viewNet);
    myFrames.polygonFrame = new GNEPolygonFrame(myFramesArea, viewNet);
    myFrames.createEdgeFrame = new GNECreateEdgeFrame(myFramesArea, viewNet);

    // Create Demand frames
    myFrames.routeFrame = new GNERouteFrame(myFramesArea, viewNet);
    myFrames.vehicleFrame = new GNEVehicleFrame(myFramesArea, viewNet);
    myFrames.vehicleTypeFrame = new GNEVehicleTypeFrame(myFramesArea, viewNet);
    myFrames.stopFrame = new GNEStopFrame(myFramesArea, viewNet);
    myFrames.personTypeFrame = new GNEPersonTypeFrame(myFramesArea, viewNet);
    myFrames.personFrame = new GNEPersonFrame(myFramesArea, viewNet);
    myFrames.personPlanFrame = new GNEPersonPlanFrame(myFramesArea, viewNet);
    myFrames.edgeDataFrame = new GNEEdgeDataFrame(myFramesArea, viewNet);
    myFrames.edgeRelDataFrame = new GNEEdgeRelDataFrame(myFramesArea, viewNet);

    // Update frame areas after creation
    onCmdUpdateFrameAreaWidth(nullptr, 0, nullptr);

    // Hidde all Frames Area
    hideFramesArea();

    //  Buld view toolBars
    myView->buildViewToolBars(*this);

    // create windows
    GUIGlChildWindow::create();
}


GNEViewParent::~GNEViewParent() {
    // delete toolbar grips
    myGNEAppWindows->getToolbarsGrip().destroyParentToolbarsGrips();
    // Remove child before remove
    myParent->removeGLChild(this);
}


void
GNEViewParent::hideAllFrames() {
    myFrames.hideFrames();
}


GNEFrame*
GNEViewParent::getCurrentShownFrame() const {
    return myFrames.getCurrentShownFrame();
}


GNEInspectorFrame*
GNEViewParent::getInspectorFrame() const {
    return myFrames.inspectorFrame;
}


GNESelectorFrame*
GNEViewParent::getSelectorFrame() const {
    return myFrames.selectorFrame;
}


GNEConnectorFrame*
GNEViewParent::getConnectorFrame() const {
    return myFrames.connectorFrame;
}


GNETLSEditorFrame*
GNEViewParent::getTLSEditorFrame() const {
    return myFrames.TLSEditorFrame;
}


GNEAdditionalFrame*
GNEViewParent::getAdditionalFrame() const {
    return myFrames.additionalFrame;
}


GNECrossingFrame*
GNEViewParent::getCrossingFrame() const {
    return myFrames.crossingFrame;
}


GNETAZFrame*
GNEViewParent::getTAZFrame() const {
    return myFrames.TAZFrame;
}


GNEDeleteFrame*
GNEViewParent::getDeleteFrame() const {
    return myFrames.deleteFrame;
}


GNEPolygonFrame*
GNEViewParent::getPolygonFrame() const {
    return myFrames.polygonFrame;
}


GNEProhibitionFrame*
GNEViewParent::getProhibitionFrame() const {
    return myFrames.prohibitionFrame;
}


GNECreateEdgeFrame*
GNEViewParent::getCreateEdgeFrame() const {
    return myFrames.createEdgeFrame;
}


GNERouteFrame*
GNEViewParent::getRouteFrame() const {
    return myFrames.routeFrame;
}


GNEVehicleFrame*
GNEViewParent::getVehicleFrame() const {
    return myFrames.vehicleFrame;
}


GNEVehicleTypeFrame*
GNEViewParent::getVehicleTypeFrame() const {
    return myFrames.vehicleTypeFrame;
}


GNEStopFrame*
GNEViewParent::getStopFrame() const {
    return myFrames.stopFrame;
}


GNEPersonTypeFrame*
GNEViewParent::getPersonTypeFrame() const {
    return myFrames.personTypeFrame;
}


GNEPersonFrame*
GNEViewParent::getPersonFrame() const {
    return myFrames.personFrame;
}


GNEPersonPlanFrame*
GNEViewParent::getPersonPlanFrame() const {
    return myFrames.personPlanFrame;
}


GNEEdgeDataFrame*
GNEViewParent::getEdgeDataFrame() const {
    return myFrames.edgeDataFrame;
}


GNEEdgeRelDataFrame* 
GNEViewParent::getEdgeRelDataFrame() const {
    return myFrames.edgeRelDataFrame;
}


void
GNEViewParent::showFramesArea() {
    // show and recalc framesArea if at least there is a frame shown
    if (myFrames.isFrameShown()) {
        myFramesArea->recalc();
        myFramesArea->show();
    }
}


void
GNEViewParent::hideFramesArea() {
    // hide and recalc frames Area if all frames are hidden is enabled
    if (!myFrames.isFrameShown()) {
        myFramesArea->hide();
        myFramesArea->recalc();
    }
}


GUIMainWindow*
GNEViewParent::getGUIMainWindow() const {
    return myParent;
}


GNEApplicationWindow*
GNEViewParent::getGNEAppWindows() const {
    return myGNEAppWindows;
}


void
GNEViewParent::eraseACChooserDialog(GNEDialogACChooser* chooserDialog) {
    if (chooserDialog == nullptr) {
        throw ProcessError("ChooserDialog already deleted");
    } else if (chooserDialog == myACChoosers.ACChooserJunction) {
        myACChoosers.ACChooserJunction = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserEdges) {
        myACChoosers.ACChooserEdges = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserVehicles) {
        myACChoosers.ACChooserVehicles = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserPersons) {
        myACChoosers.ACChooserPersons = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserRoutes) {
        myACChoosers.ACChooserRoutes = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserStops) {
        myACChoosers.ACChooserStops = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserTLS) {
        myACChoosers.ACChooserTLS = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserAdditional) {
        myACChoosers.ACChooserAdditional = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserPOI) {
        myACChoosers.ACChooserPOI = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserPolygon) {
        myACChoosers.ACChooserPolygon = nullptr;
    } else if (chooserDialog == myACChoosers.ACChooserProhibition) {
        myACChoosers.ACChooserProhibition = nullptr;
    } else {
        throw ProcessError("Unregistered chooserDialog");
    }
}


void
GNEViewParent::updateUndoRedoButtons() {
    myGNEAppWindows->getUndoList()->p_onUpdUndo(myUndoButton, 0, nullptr);
    myGNEAppWindows->getUndoList()->p_onUpdRedo(myRedoButton, 0, nullptr);
}


long
GNEViewParent::onCmdMakeSnapshot(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Save Snapshot");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
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
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'error saving snapshot'");
        // open message box
        FXMessageBox::error(this, MBOX_OK, "Saving failed.", "%s", error.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'error saving snapshot' with 'OK'");
    }
    return 1;
}


long
GNEViewParent::onCmdClose(FXObject*, FXSelector /* sel */, void*) {
    myParent->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_W_CLOSESIMULATION), nullptr);
    return 1;
}


long
GNEViewParent::onCmdLocate(FXObject*, FXSelector sel, void*) {
    GNEViewNet* viewNet = dynamic_cast<GNEViewNet*>(myView);
    // check that viewNet exist
    if (viewNet) {
        // declare a vector in which save attribute carriers to locate
        std::vector<GNEAttributeCarrier*> ACsToLocate;
        switch (FXSELID(sel)) {
            case MID_LOCATEJUNCTION: {
                if (myACChoosers.ACChooserJunction) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserJunction->setFocus();
                } else {
                    // fill ACsToLocate with junctions
                    std::vector<GNEJunction*> junctions = viewNet->getNet()->retrieveJunctions();
                    ACsToLocate.reserve(junctions.size());
                    for (auto i : junctions) {
                        ACsToLocate.push_back(i);
                    }
                    myACChoosers.ACChooserJunction = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), "Junction Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATEEDGE: {
                if (myACChoosers.ACChooserEdges) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserEdges->setFocus();
                } else {
                    // fill ACsToLocate with edges
                    std::vector<GNEEdge*> edges = viewNet->getNet()->retrieveEdges();
                    ACsToLocate.reserve(edges.size());
                    for (auto i : edges) {
                        ACsToLocate.push_back(i);
                    }
                    myACChoosers.ACChooserEdges = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), "Edge Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATEVEHICLE: {
                if (myACChoosers.ACChooserVehicles) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserVehicles->setFocus();
                } else {
                    // reserve memory
                    ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VEHICLE).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_TRIP).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTEFLOW).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_FLOW).size());
                    // fill ACsToLocate with vehicles
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VEHICLE)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with vehicles
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_TRIP)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with routeFlows
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTEFLOW)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with routeFlowsFromTo
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_FLOW)) {
                        ACsToLocate.push_back(i.second);
                    }
                    myACChoosers.ACChooserVehicles = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), "Vehicle Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATEPERSON: {
                if (myACChoosers.ACChooserPersons) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserPersons->setFocus();
                } else {
                    // reserve memory
                    ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW).size());
                    // fill ACsToLocate with persons
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with personFlows
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW)) {
                        ACsToLocate.push_back(i.second);
                    }
                    myACChoosers.ACChooserPersons = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), "Person Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATEROUTE: {
                if (myACChoosers.ACChooserRoutes) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserRoutes->setFocus();
                } else {
                    // reserve memory
                    ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).size());
                    // fill ACsToLocate with routes
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
                        ACsToLocate.push_back(i.second);
                    }
                    myACChoosers.ACChooserRoutes = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEROUTE), "Route Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATESTOP: {
                if (myACChoosers.ACChooserStops) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserStops->setFocus();
                } else {
                    // reserve memory
                    ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_LANE).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_BUSSTOP).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CONTAINERSTOP).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CHARGINGSTATION).size() +
                                        viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_PARKINGAREA).size());
                    // fill ACsToLocate with stop over lanes
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_LANE)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with stop over busstops
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_BUSSTOP)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with stop over container stops
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CONTAINERSTOP)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with stop over charging stations
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CHARGINGSTATION)) {
                        ACsToLocate.push_back(i.second);
                    }
                    // fill ACsToLocate with stop over parking areas
                    for (const auto& i : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_PARKINGAREA)) {
                        ACsToLocate.push_back(i.second);
                    }
                    myACChoosers.ACChooserStops = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATESTOP), "Stop Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATETLS: {
                if (myACChoosers.ACChooserTLS) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserTLS->setFocus();
                } else {
                    // fill ACsToLocate with junctions that haven TLS
                    std::vector<GNEJunction*> junctions = viewNet->getNet()->retrieveJunctions();
                    ACsToLocate.reserve(junctions.size());
                    for (auto i : junctions) {
                        if (i->getNBNode()->getControllingTLS().size() > 0) {
                            ACsToLocate.push_back(i);
                        }
                    }
                    myACChoosers.ACChooserTLS = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), "TLS Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATEADD: {
                if (myACChoosers.ACChooserAdditional) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserAdditional->setFocus();
                } else {
                    // fill ACsToLocate with additionals
                    for (const auto& additionalTag : viewNet->getNet()->getAttributeCarriers()->getAdditionals()) {
                        for (const auto& additional : additionalTag.second) {
                            ACsToLocate.push_back(additional.second);
                        }
                    }
                    myACChoosers.ACChooserAdditional = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), "Additional Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATEPOI: {
                if (myACChoosers.ACChooserPOI) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserPOI->setFocus();
                } else {
                    // fill ACsToLocate with POIs
                    for (const auto& POI : viewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POI)) {
                        ACsToLocate.push_back(POI.second);
                    }
                    for (const auto& POILane : viewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POILANE)) {
                        ACsToLocate.push_back(POILane.second);
                    }
                    myACChoosers.ACChooserPOI = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), "POI Chooser", ACsToLocate);
                }
                break;
            }
            case MID_LOCATEPOLY: {
                if (myACChoosers.ACChooserPolygon) {
                    // set focus in the existent chooser dialog
                    myACChoosers.ACChooserPolygon->setFocus();
                } else {
                    // fill ACsToLocate with polys
                    for (const auto &polygon : viewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POLY)) {
                        ACsToLocate.push_back(polygon.second);
                    }
                    myACChoosers.ACChooserPolygon = new GNEDialogACChooser(this, GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), "Poly Chooser", ACsToLocate);
                }
                break;
            }
            default:
                throw ProcessError("Unknown Message ID in onCmdLocate");
        }
        // update locator popup
        myLocatorPopup->popdown();
        myLocatorButton->killFocus();
        myLocatorPopup->update();
    }
    return 1;
}


long
GNEViewParent::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    myView->onKeyPress(o, sel, eventData);
    return 0;
}


long
GNEViewParent::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    myView->onKeyRelease(o, sel, eventData);
    return 0;
}


long
GNEViewParent::onCmdUpdateFrameAreaWidth(FXObject*, FXSelector, void*) {
    // set width of FramesArea in all frames
    myFrames.setWidth(myFramesArea->getWidth());
    return 0;
}

// ---------------------------------------------------------------------------
// GNEViewParent::Frames - methods
// ---------------------------------------------------------------------------

GNEViewParent::Frames::Frames() :
    inspectorFrame(nullptr),
    selectorFrame(nullptr),
    connectorFrame(nullptr),
    TLSEditorFrame(nullptr),
    additionalFrame(nullptr),
    crossingFrame(nullptr),
    TAZFrame(nullptr),
    deleteFrame(nullptr),
    polygonFrame(nullptr),
    prohibitionFrame(nullptr),
    createEdgeFrame(nullptr),
    routeFrame(nullptr),
    vehicleFrame(nullptr),
    vehicleTypeFrame(nullptr),
    stopFrame(nullptr),
    personFrame(nullptr),
    personTypeFrame(nullptr),
    personPlanFrame(nullptr),
    edgeDataFrame(nullptr),
    edgeRelDataFrame(nullptr) {
}


void
GNEViewParent::Frames::hideFrames() {
    inspectorFrame->hide();
    selectorFrame->hide();
    connectorFrame->hide();
    TLSEditorFrame->hide();
    additionalFrame->hide();
    crossingFrame->hide();
    TAZFrame->hide();
    deleteFrame->hide();
    polygonFrame->hide();
    prohibitionFrame->hide();
    /** currently createEdgeFrame unused
    createEdgeFrame->hide();
    **/
    routeFrame->hide();
    vehicleFrame->hide();
    vehicleTypeFrame->hide();
    stopFrame->hide();
    personTypeFrame->hide();
    personFrame->hide();
    personPlanFrame->hide();
    edgeDataFrame->hide();
    edgeRelDataFrame->hide();
}


void
GNEViewParent::Frames::setWidth(int frameWidth) {
    // set width in all frames
    inspectorFrame->setFrameWidth(frameWidth);
    selectorFrame->setFrameWidth(frameWidth);
    connectorFrame->setFrameWidth(frameWidth);
    TLSEditorFrame->setFrameWidth(frameWidth);
    additionalFrame->setFrameWidth(frameWidth);
    crossingFrame->setFrameWidth(frameWidth);
    TAZFrame->setFrameWidth(frameWidth);
    deleteFrame->setFrameWidth(frameWidth);
    polygonFrame->setFrameWidth(frameWidth);
    prohibitionFrame->setFrameWidth(frameWidth);
    /** currently createEdgeFrame unused
    createEdgeFrame->setFrameWidth(frameWidth);
    **/
    routeFrame->setFrameWidth(frameWidth);
    vehicleFrame->setFrameWidth(frameWidth);
    vehicleTypeFrame->setFrameWidth(frameWidth);
    stopFrame->setFrameWidth(frameWidth);
    personTypeFrame->setFrameWidth(frameWidth);
    personFrame->setFrameWidth(frameWidth);
    personPlanFrame->setFrameWidth(frameWidth);
    edgeDataFrame->setFrameWidth(frameWidth);
    edgeRelDataFrame->setFrameWidth(frameWidth);
}


bool
GNEViewParent::Frames::isFrameShown() const {
    // check all frames
    if (inspectorFrame->shown()) {
        return true;
    } else if (selectorFrame->shown()) {
        return true;
    } else if (connectorFrame->shown()) {
        return true;
    } else if (TLSEditorFrame->shown()) {
        return true;
    } else if (additionalFrame->shown()) {
        return true;
    } else if (crossingFrame->shown()) {
        return true;
    } else if (TAZFrame->shown()) {
        return true;
    } else if (deleteFrame->shown()) {
        return true;
    } else if (polygonFrame->shown()) {
        return true;
    } else if (prohibitionFrame->shown()) {
        return true;
        /** currently createEdgeFrame unused
        } else if (createEdgeFrame->shown()) {
            return true;
        **/
    } else if (routeFrame->shown()) {
        return true;
    } else if (vehicleFrame->shown()) {
        return true;
    } else if (vehicleTypeFrame->shown()) {
        return true;
    } else if (stopFrame->shown()) {
        return true;
    } else if (personTypeFrame->shown()) {
        return true;
    } else if (personFrame->shown()) {
        return true;
    } else if (personPlanFrame->shown()) {
        return true;
    } else if (edgeDataFrame->shown()) {
        return true;
    } else if (edgeRelDataFrame->shown()) {
        return true;
    } else {
        return false;
    }
}


GNEFrame*
GNEViewParent::Frames::getCurrentShownFrame() const {
    // check all frames
    if (inspectorFrame->shown()) {
        return inspectorFrame;
    } else if (selectorFrame->shown()) {
        return selectorFrame;
    } else if (connectorFrame->shown()) {
        return connectorFrame;
    } else if (TLSEditorFrame->shown()) {
        return TLSEditorFrame;
    } else if (additionalFrame->shown()) {
        return additionalFrame;
    } else if (crossingFrame->shown()) {
        return crossingFrame;
    } else if (TAZFrame->shown()) {
        return TAZFrame;
    } else if (deleteFrame->shown()) {
        return deleteFrame;
    } else if (polygonFrame->shown()) {
        return polygonFrame;
    } else if (prohibitionFrame->shown()) {
        return prohibitionFrame;
        /** currently createEdgeFrame unused
        } else if (createEdgeFrame->shown()) {
            return true;
        **/
    } else if (routeFrame->shown()) {
        return routeFrame;
    } else if (vehicleFrame->shown()) {
        return vehicleFrame;
    } else if (vehicleTypeFrame->shown()) {
        return vehicleTypeFrame;
    } else if (personTypeFrame->shown()) {
        return personTypeFrame;
    } else if (stopFrame->shown()) {
        return stopFrame;
    } else if (personFrame->shown()) {
        return personFrame;
    } else if (personPlanFrame->shown()) {
        return personPlanFrame;
    } else if (edgeDataFrame->shown()) {
        return edgeDataFrame;
    } else if (edgeRelDataFrame->shown()) {
        return edgeRelDataFrame;
    } else {
        return nullptr;
    }
}

// ---------------------------------------------------------------------------
// GNEViewParent::ACChoosers - methods
// ---------------------------------------------------------------------------

GNEViewParent::ACChoosers::ACChoosers() :
    ACChooserJunction(nullptr),
    ACChooserEdges(nullptr),
    ACChooserVehicles(nullptr),
    ACChooserPersons(nullptr),
    ACChooserRoutes(nullptr),
    ACChooserStops(nullptr),
    ACChooserTLS(nullptr),
    ACChooserAdditional(nullptr),
    ACChooserPOI(nullptr),
    ACChooserPolygon(nullptr),
    ACChooserProhibition(nullptr) {
}


GNEViewParent::ACChoosers::~ACChoosers() {
    // remove all  dialogs if are active
    if (ACChooserJunction) {
        delete ACChooserJunction;
    }
    if (ACChooserEdges) {
        delete ACChooserEdges;
    }
    if (ACChooserRoutes) {
        delete ACChooserRoutes;
    }
    if (ACChooserStops) {
        delete ACChooserStops;
    }
    if (ACChooserVehicles) {
        delete ACChooserVehicles;
    }
    if (ACChooserPersons) {
        delete ACChooserPersons;
    }
    if (ACChooserTLS) {
        delete ACChooserTLS;
    }
    if (ACChooserAdditional) {
        delete ACChooserAdditional;
    }
    if (ACChooserPOI) {
        delete ACChooserPOI;
    }
    if (ACChooserPolygon) {
        delete ACChooserPolygon;
    }
    if (ACChooserProhibition) {
        delete ACChooserProhibition;
    }
}


/****************************************************************************/
