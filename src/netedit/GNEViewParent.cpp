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
/// @file    GNEViewParent.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A single child window which contains a view of the edited network (adapted
// from GUISUMOViewParent)
// While we don't actually need MDI for netedit it is easier to adapt existing
// structures than to write everything from scratch.
/****************************************************************************/

#include <netedit/dialogs/GNEDialogACChooser.h>
#include <netedit/elements/network/GNEWalkingArea.h>
#include <netedit/frames/common/GNEDeleteFrame.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <netedit/frames/data/GNEEdgeRelDataFrame.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <netedit/frames/data/GNEMeanDataFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNERouteFrame.h>
#include <netedit/frames/demand/GNEStopFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/demand/GNETypeFrame.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>
#include <netedit/frames/network/GNEConnectorFrame.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/frames/network/GNECrossingFrame.h>
#include <netedit/frames/network/GNEShapeFrame.h>
#include <netedit/frames/network/GNEProhibitionFrame.h>
#include <netedit/frames/network/GNEWireFrame.h>
#include <netedit/frames/network/GNETAZFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/foxtools/MFXMenuButtonTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

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
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,      GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_E_LOCATEEDGE,          GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_W_LOCATEWALKINGAREA,   GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,       GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_P_LOCATEPERSON,        GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_R_LOCATEROUTE,         GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_S_LOCATESTOP,          GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_T_LOCATETLS,           GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,    GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_O_LOCATEPOI,           GNEViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_L_LOCATEPOLY,          GNEViewParent::onCmdLocate),
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
    myGUIMainWindowParent->addGLChild(this);

    // Create Vertical separator
    new FXVerticalSeparator(myGripNavigationToolbar, GUIDesignVerticalSeparator);

    // Create undo/redo buttons
    myUndoButton = new MFXButtonTooltip(myGripNavigationToolbar, myGNEAppWindows->getStaticTooltipMenu(),
                                        std::string("\t") + TL("Undo") + std::string("\t") + TL("Undo the last change. (Ctrl+Z)"),
                                        GUIIconSubSys::getIcon(GUIIcon::UNDO), parentWindow, MID_HOTKEY_CTRL_Z_UNDO, GUIDesignButtonToolbar);
    myRedoButton = new MFXButtonTooltip(myGripNavigationToolbar, myGNEAppWindows->getStaticTooltipMenu(),
                                        std::string("\t") + TL("Redo") + std::string("\t") + TL("Redo the last change. (Ctrl+Y)"),
                                        GUIIconSubSys::getIcon(GUIIcon::REDO), parentWindow, MID_HOTKEY_CTRL_Y_REDO, GUIDesignButtonToolbar);

    // Create Vertical separator
    new FXVerticalSeparator(myGripNavigationToolbar, GUIDesignVerticalSeparator);

    // create compute path manager button
    myComputePathManagerButton = new MFXButtonTooltip(myGripNavigationToolbar, myGNEAppWindows->getStaticTooltipMenu(),
            std::string("\t") + TL("Compute path manager") + std::string("\t") + TL("Compute path manager."),
            GUIIconSubSys::getIcon(GUIIcon::COMPUTEPATHMANAGER), parentWindow, MID_GNE_TOOLBAREDIT_COMPUTEPATHMANAGER, GUIDesignButtonToolbar);

    // Create Frame Splitter
    myFramesSplitter = new FXSplitter(myChildWindowContentFrame, this, MID_GNE_VIEWPARENT_FRAMEAREAWIDTH, GUIDesignSplitter | SPLITTER_HORIZONTAL);

    // Create frames Area (vertical frame)
    myFramesArea = new FXVerticalFrame(myFramesSplitter, GUIDesignFrameArea);

    // Create view area
    myViewArea = new FXHorizontalFrame(myFramesSplitter, GUIDesignViewnArea);

    // Add the view to a temporary parent so that we can add items to myViewArea in the desired order
    FXComposite* tmp = new FXComposite(this);

    // Create view net
    GNEViewNet* viewNet = new GNEViewNet(tmp, myViewArea, *myGUIMainWindowParent, this, net, undoList, myGUIMainWindowParent->getGLVisual(), share);

    // show toolbar grips
    myGNEAppWindows->getToolbarsGrip().buildMenuToolbarsGrip();

    // Set pointer myView with the created view net
    myView = viewNet;

    // build frames
    myCommonFrames.buildCommonFrames(this, viewNet);
    myNetworkFrames.buildNetworkFrames(this, viewNet);
    myDemandFrames.buildDemandFrames(this, viewNet);
    myDataFrames.buildDataFrames(this, viewNet);

    // set default frames area width
    myFramesArea->setWidth(GUIDesignFramesAreaDefaultWidth);

    // Update frame areas after creation
    onCmdUpdateFrameAreaWidth(nullptr, 0, nullptr);

    // Hide all Frames Area
    hideFramesArea();

    // Build view toolBars
    myView->buildViewToolBars(this);

    // create windows
    GUIGlChildWindow::create();
}


GNEViewParent::~GNEViewParent() {
    // delete toolbar grips
    myGNEAppWindows->getToolbarsGrip().destroyParentToolbarsGrips();
    // Remove child before remove
    myGUIMainWindowParent->removeGLChild(this);
}


void
GNEViewParent::hideAllFrames() {
    myCommonFrames.hideCommonFrames();
    myNetworkFrames.hideNetworkFrames();
    myDemandFrames.hideDemandFrames();
    myDataFrames.hideDataFrames();
}


GNEFrame*
GNEViewParent::getCurrentShownFrame() const {
    if (myCommonFrames.isCommonFrameShown()) {
        return myCommonFrames.getCurrentShownFrame();
    } else if (myNetworkFrames.isNetworkFrameShown()) {
        return myNetworkFrames.getCurrentShownFrame();
    } else if (myDemandFrames.isDemandFrameShown()) {
        return myDemandFrames.getCurrentShownFrame();
    } else if (myDataFrames.isDataFrameShown()) {
        return myDataFrames.getCurrentShownFrame();
    } else {
        return nullptr;
    }
}


GNEInspectorFrame*
GNEViewParent::getInspectorFrame() const {
    return myCommonFrames.inspectorFrame;
}


GNEDeleteFrame*
GNEViewParent::getDeleteFrame() const {
    return myCommonFrames.deleteFrame;
}


GNESelectorFrame*
GNEViewParent::getSelectorFrame() const {
    return myCommonFrames.selectorFrame;
}


GNEMoveFrame*
GNEViewParent::getMoveFrame() const {
    return myCommonFrames.moveFrame;
}


GNEConnectorFrame*
GNEViewParent::getConnectorFrame() const {
    return myNetworkFrames.connectorFrame;
}


GNETLSEditorFrame*
GNEViewParent::getTLSEditorFrame() const {
    return myNetworkFrames.TLSEditorFrame;
}


GNEAdditionalFrame*
GNEViewParent::getAdditionalFrame() const {
    return myNetworkFrames.additionalFrame;
}


GNECrossingFrame*
GNEViewParent::getCrossingFrame() const {
    return myNetworkFrames.crossingFrame;
}


GNETAZFrame*
GNEViewParent::getTAZFrame() const {
    return myNetworkFrames.TAZFrame;
}


GNEShapeFrame*
GNEViewParent::getShapeFrame() const {
    return myNetworkFrames.polygonFrame;
}


GNEProhibitionFrame*
GNEViewParent::getProhibitionFrame() const {
    return myNetworkFrames.prohibitionFrame;
}


GNEWireFrame*
GNEViewParent::getWireFrame() const {
    return myNetworkFrames.wireFrame;
}


GNECreateEdgeFrame*
GNEViewParent::getCreateEdgeFrame() const {
    return myNetworkFrames.createEdgeFrame;
}


GNERouteFrame*
GNEViewParent::getRouteFrame() const {
    return myDemandFrames.routeFrame;
}


GNEVehicleFrame*
GNEViewParent::getVehicleFrame() const {
    return myDemandFrames.vehicleFrame;
}


GNETypeFrame*
GNEViewParent::getTypeFrame() const {
    return myDemandFrames.vehicleTypeFrame;
}


GNEStopFrame*
GNEViewParent::getStopFrame() const {
    return myDemandFrames.stopFrame;
}


GNEPersonFrame*
GNEViewParent::getPersonFrame() const {
    return myDemandFrames.personFrame;
}


GNEPersonPlanFrame*
GNEViewParent::getPersonPlanFrame() const {
    return myDemandFrames.personPlanFrame;
}


GNEContainerFrame*
GNEViewParent::getContainerFrame() const {
    return myDemandFrames.containerFrame;
}


GNEContainerPlanFrame*
GNEViewParent::getContainerPlanFrame() const {
    return myDemandFrames.containerPlanFrame;
}


GNEEdgeDataFrame*
GNEViewParent::getEdgeDataFrame() const {
    return myDataFrames.edgeDataFrame;
}


GNEEdgeRelDataFrame*
GNEViewParent::getEdgeRelDataFrame() const {
    return myDataFrames.edgeRelDataFrame;
}


GNETAZRelDataFrame*
GNEViewParent::getTAZRelDataFrame() const {
    return myDataFrames.TAZRelDataFrame;
}


GNEMeanDataFrame*
GNEViewParent::getMeanDataFrame() const {
    return myDataFrames.meanDataFrame;
}


void
GNEViewParent::showFramesArea() {
    const bool frameShown = myCommonFrames.isCommonFrameShown() ||
                            myNetworkFrames.isNetworkFrameShown() ||
                            myDemandFrames.isDemandFrameShown() ||
                            myDataFrames.isDataFrameShown();
    // show and recalc framesArea if at least there is a frame shown
    if (frameShown) {
        myFramesArea->recalc();
        myFramesArea->show();
    }
}


void
GNEViewParent::hideFramesArea() {
    const bool frameShown = myCommonFrames.isCommonFrameShown() ||
                            myNetworkFrames.isNetworkFrameShown() ||
                            myDemandFrames.isDemandFrameShown() ||
                            myDataFrames.isDataFrameShown();
    // hide and recalc frames Area if all frames are hidden is enabled
    if (!frameShown) {
        myFramesArea->hide();
        myFramesArea->recalc();
    }
}


GUIMainWindow*
GNEViewParent::getGUIMainWindow() const {
    return myGUIMainWindowParent;
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
    } else if (chooserDialog == myACChoosers.ACChooserWalkingAreas) {
        myACChoosers.ACChooserWalkingAreas = nullptr;
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
    } else if (chooserDialog == myACChoosers.ACChooserWire) {
        myACChoosers.ACChooserWire = nullptr;
    } else {
        throw ProcessError("Unregistered chooserDialog");
    }
}


void
GNEViewParent::updateUndoRedoButtons() {
    myGNEAppWindows->getUndoList()->onUpdUndo(myUndoButton, 0, nullptr);
    myGNEAppWindows->getUndoList()->onUpdRedo(myRedoButton, 0, nullptr);
}


FXVerticalFrame*
GNEViewParent::getFramesArea() const {
    return myFramesArea;
}


int
GNEViewParent::getFrameAreaWidth() const {
    return myFramesArea->getWidth();
}


void
GNEViewParent::setFrameAreaWidth(const int frameAreaWith) {
    myFramesArea->setWidth(frameAreaWith);
    onCmdUpdateFrameAreaWidth(0, 0, 0);
}


long
GNEViewParent::onCmdMakeSnapshot(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, TL("Save Snapshot"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::CAMERA));
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
    if (file.find(".") == std::string::npos) {
        file.append(".png");
        WRITE_MESSAGE(TL("No file extension was specified - saving Snapshot as PNG."));
    }
    std::string error = myView->makeSnapshot(file);
    if (error != "") {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'error saving snapshot'");
        // open message box
        FXMessageBox::error(this, MBOX_OK, TL("Saving failed."), "%s", error.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'error saving snapshot' with 'OK'");
    } else {
        WRITE_MESSAGE(TL("Snapshot successfully saved!"));
    }
    return 1;
}


long
GNEViewParent::onCmdClose(FXObject*, FXSelector /* sel */, void*) {
    myGUIMainWindowParent->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_W_CLOSESIMULATION), nullptr);
    return 1;
}


long
GNEViewParent::onCmdLocate(FXObject*, FXSelector sel, void*) {
    GNEViewNet* viewNet = dynamic_cast<GNEViewNet*>(myView);
    // check that viewNet exist
    if (viewNet) {
        // declare a vector in which save attribute carriers to locate
        std::vector<GNEAttributeCarrier*> ACsToLocate;
        int messageId = FXSELID(sel);
        GNEDialogACChooser** chooserLoc = nullptr;
        std::string locateTitle;
        switch (messageId) {
            case MID_HOTKEY_SHIFT_J_LOCATEJUNCTION:
                chooserLoc = &myACChoosers.ACChooserJunction;
                locateTitle = TL("Junction Chooser");
                ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getJunctions().size());
                for (const auto& junction : viewNet->getNet()->getAttributeCarriers()->getJunctions()) {
                    ACsToLocate.push_back(junction.second);
                }
                break;
            case MID_HOTKEY_SHIFT_E_LOCATEEDGE:
                chooserLoc = &myACChoosers.ACChooserEdges;
                locateTitle = TL("Edge Chooser");
                ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getEdges().size());
                for (const auto& edge : viewNet->getNet()->getAttributeCarriers()->getEdges()) {
                    ACsToLocate.push_back(edge.second);
                }
                break;
            case MID_HOTKEY_SHIFT_W_LOCATEWALKINGAREA:
                chooserLoc = &myACChoosers.ACChooserWalkingAreas;
                locateTitle = TL("WalkingArea Chooser");
                ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getWalkingAreas().size());
                for (const auto& walkingArea : viewNet->getNet()->getAttributeCarriers()->getWalkingAreas()) {
                    ACsToLocate.push_back(walkingArea);
                }
                break;
            case MID_HOTKEY_SHIFT_V_LOCATEVEHICLE: {
                chooserLoc = &myACChoosers.ACChooserVehicles;
                locateTitle = TL("Vehicle Chooser");
                const auto demandElements = viewNet->getNet()->getAttributeCarriers()->getDemandElements();
                // reserve memory
                ACsToLocate.reserve(demandElements.at(SUMO_TAG_VEHICLE).size() +
                                    demandElements.at(SUMO_TAG_TRIP).size() +
                                    demandElements.at(GNE_TAG_VEHICLE_WITHROUTE).size() +
                                    demandElements.at(GNE_TAG_TRIP_JUNCTIONS).size() +
                                    demandElements.at(SUMO_TAG_FLOW).size() +
                                    demandElements.at(GNE_TAG_FLOW_ROUTE).size() +
                                    demandElements.at(GNE_TAG_FLOW_WITHROUTE).size() +
                                    demandElements.at(GNE_TAG_FLOW_JUNCTIONS).size());
                // fill ACsToLocate with vehicles,...
                for (const auto& vehicle : demandElements.at(SUMO_TAG_VEHICLE)) {
                    ACsToLocate.push_back(vehicle);
                }
                // ...trips,...
                for (const auto& trip : demandElements.at(SUMO_TAG_TRIP)) {
                    ACsToLocate.push_back(trip);
                }
                // ...vehicles with embedded routes,...
                for (const auto& trip : demandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
                    ACsToLocate.push_back(trip);
                }
                // ...trips over junctions,...
                for (const auto& trip : demandElements.at(GNE_TAG_TRIP_JUNCTIONS)) {
                    ACsToLocate.push_back(trip);
                }
                // ...flows,...
                for (const auto& flow : demandElements.at(SUMO_TAG_FLOW)) {
                    ACsToLocate.push_back(flow);
                }
                // ...flows over routes,...
                for (const auto& flowRoute : demandElements.at(GNE_TAG_FLOW_ROUTE)) {
                    ACsToLocate.push_back(flowRoute);
                }
                // ...flows with embedded routes...
                for (const auto& flowRoute : demandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
                    ACsToLocate.push_back(flowRoute);
                }
                // ... and flows over junctions.
                for (const auto& flowRoute : demandElements.at(GNE_TAG_FLOW_JUNCTIONS)) {
                    ACsToLocate.push_back(flowRoute);
                }
                break;
            }
            case MID_HOTKEY_SHIFT_P_LOCATEPERSON:
                chooserLoc = &myACChoosers.ACChooserPersons;
                locateTitle = TL("Person Chooser");
                ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON).size() +
                                    viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW).size());
                // fill ACsToLocate with persons
                for (const auto& person : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON)) {
                    ACsToLocate.push_back(person);
                }
                // fill ACsToLocate with personFlows
                for (const auto& personFlow : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW)) {
                    ACsToLocate.push_back(personFlow);
                }
                break;
            case MID_HOTKEY_SHIFT_R_LOCATEROUTE:
                chooserLoc = &myACChoosers.ACChooserRoutes;
                locateTitle = TL("Route Chooser");
                ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).size());
                for (const auto& route : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
                    ACsToLocate.push_back(route);
                }
                break;
            case MID_HOTKEY_SHIFT_S_LOCATESTOP: {
                chooserLoc = &myACChoosers.ACChooserStops;
                locateTitle = TL("Stop Chooser");
                // reserve memory
                ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_LANE).size() +
                                    viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_BUSSTOP).size() +
                                    viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CONTAINERSTOP).size() +
                                    viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CHARGINGSTATION).size() +
                                    viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_PARKINGAREA).size());
                // fill ACsToLocate with stop over lanes
                for (const auto& stopLane : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_LANE)) {
                    ACsToLocate.push_back(stopLane);
                }
                // fill ACsToLocate with stop over busstops
                for (const auto& stopBusStop : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_BUSSTOP)) {
                    ACsToLocate.push_back(stopBusStop);
                }
                // fill ACsToLocate with stop over container stops
                for (const auto& stopContainerStop : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CONTAINERSTOP)) {
                    ACsToLocate.push_back(stopContainerStop);
                }
                // fill ACsToLocate with stop over charging stations
                for (const auto& stopChargingStation : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CHARGINGSTATION)) {
                    ACsToLocate.push_back(stopChargingStation);
                }
                // fill ACsToLocate with stop over parking areas
                for (const auto& stopParkingArea : viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_PARKINGAREA)) {
                    ACsToLocate.push_back(stopParkingArea);
                }
                break;
            }
            case MID_HOTKEY_SHIFT_T_LOCATETLS:
                chooserLoc = &myACChoosers.ACChooserTLS;
                locateTitle = TL("TLS Chooser");
                // fill ACsToLocate with junctions that haven TLS
                ACsToLocate.reserve(viewNet->getNet()->getAttributeCarriers()->getJunctions().size());
                for (const auto& junction : viewNet->getNet()->getAttributeCarriers()->getJunctions()) {
                    if (junction.second->getNBNode()->getControllingTLS().size() > 0) {
                        ACsToLocate.push_back(junction.second);
                    }
                }
                break;
            case MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL:
                chooserLoc = &myACChoosers.ACChooserAdditional;
                locateTitle = TL("Additional Chooser");
                for (const auto& additionalTag : viewNet->getNet()->getAttributeCarriers()->getAdditionals()) {
                    if (additionalTag.first == SUMO_TAG_POI
                            || additionalTag.first == GNE_TAG_POILANE
                            || additionalTag.first == GNE_TAG_POIGEO
                            || additionalTag.first == SUMO_TAG_POLY) {
                        continue;
                    }
                    for (const auto& additional : additionalTag.second) {
                        ACsToLocate.push_back(additional);
                    }
                }
                break;
            case MID_HOTKEY_SHIFT_O_LOCATEPOI:
                chooserLoc = &myACChoosers.ACChooserPOI;
                locateTitle = TL("POI Chooser");
                // fill ACsToLocate with POIs
                for (const auto& POI : viewNet->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_POI)) {
                    ACsToLocate.push_back(POI);
                }
                for (const auto& POI : viewNet->getNet()->getAttributeCarriers()->getAdditionals().at(GNE_TAG_POILANE)) {
                    ACsToLocate.push_back(POI);
                }
                for (const auto& POI : viewNet->getNet()->getAttributeCarriers()->getAdditionals().at(GNE_TAG_POIGEO)) {
                    ACsToLocate.push_back(POI);
                }
                break;
            case MID_HOTKEY_SHIFT_L_LOCATEPOLY:
                chooserLoc = &myACChoosers.ACChooserPolygon;
                locateTitle = TL("Poly Chooser");
                // fill ACsToLocate with polys and TAZs (because share namespae)
                for (const auto& polygon : viewNet->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_POLY)) {
                    ACsToLocate.push_back(polygon);
                }
                for (const auto& taz : viewNet->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_TAZ)) {
                    ACsToLocate.push_back(taz);
                }
                break;
            default:
                throw ProcessError("Unknown Message ID in onCmdLocate");
        }
        if (*chooserLoc) {
            // restore focus in the existent chooser dialog
            GNEDialogACChooser* chooser = *chooserLoc;
            chooser->restore();
            chooser->setFocus();
            chooser->raise();
        } else {
            GNEDialogACChooser* chooser = new GNEDialogACChooser(this, messageId, GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), locateTitle, ACsToLocate);
            *chooserLoc = chooser;
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
    const int framesAreaWidth = myFramesArea->getWidth();
    // set width of FramesArea in all frames
    myCommonFrames.setCommonFramesWidth(framesAreaWidth);
    myNetworkFrames.setNetworkFramesWidth(framesAreaWidth);
    myDemandFrames.setDemandFramesWidth(framesAreaWidth);
    myDataFrames.setDataFramesWidth(framesAreaWidth);
    return 0;
}

// ---------------------------------------------------------------------------
// GNEViewParent::CommonFrames - methods
// ---------------------------------------------------------------------------

GNEViewParent::CommonFrames::CommonFrames() :
    inspectorFrame(nullptr),
    deleteFrame(nullptr),
    selectorFrame(nullptr),
    moveFrame(nullptr) {
}


void
GNEViewParent::CommonFrames::buildCommonFrames(GNEViewParent* viewParent, GNEViewNet* viewNet) {
    inspectorFrame = new GNEInspectorFrame(viewParent, viewNet);
    selectorFrame = new GNESelectorFrame(viewParent, viewNet);
    moveFrame = new GNEMoveFrame(viewParent, viewNet);
    deleteFrame = new GNEDeleteFrame(viewParent, viewNet);
}


void
GNEViewParent::CommonFrames::hideCommonFrames() {
    inspectorFrame->hide();
    deleteFrame->hide();
    selectorFrame->hide();
    moveFrame->hide();
}


void
GNEViewParent::CommonFrames::setCommonFramesWidth(int frameWidth) {
    // set width in all frames
    inspectorFrame->setFrameWidth(frameWidth);
    deleteFrame->setFrameWidth(frameWidth);
    selectorFrame->setFrameWidth(frameWidth);
    moveFrame->setFrameWidth(frameWidth);
}


bool
GNEViewParent::CommonFrames::isCommonFrameShown() const {
    // check all frames
    if (inspectorFrame->shown()) {
        return true;
    } else if (deleteFrame->shown()) {
        return true;
    } else if (selectorFrame->shown()) {
        return true;
    } else if (moveFrame->shown()) {
        return true;
    } else {
        return false;
    }
}


GNEFrame*
GNEViewParent::CommonFrames::getCurrentShownFrame() const {
    // check all frames
    if (inspectorFrame->shown()) {
        return inspectorFrame;
    } else if (deleteFrame->shown()) {
        return deleteFrame;
    } else if (selectorFrame->shown()) {
        return selectorFrame;
    } else if (moveFrame->shown()) {
        return moveFrame;
    } else {
        return nullptr;
    }
}

// ---------------------------------------------------------------------------
// GNEViewParent::NetworkFrames - methods
// ---------------------------------------------------------------------------

GNEViewParent::NetworkFrames::NetworkFrames() :
    connectorFrame(nullptr),
    TLSEditorFrame(nullptr),
    additionalFrame(nullptr),
    crossingFrame(nullptr),
    TAZFrame(nullptr),
    polygonFrame(nullptr),
    prohibitionFrame(nullptr),
    wireFrame(nullptr),
    createEdgeFrame(nullptr) {
}


void
GNEViewParent::NetworkFrames::buildNetworkFrames(GNEViewParent* viewParent, GNEViewNet* viewNet) {
    connectorFrame = new GNEConnectorFrame(viewParent, viewNet);
    prohibitionFrame = new GNEProhibitionFrame(viewParent, viewNet);
    wireFrame = new GNEWireFrame(viewParent, viewNet);
    TLSEditorFrame = new GNETLSEditorFrame(viewParent, viewNet);
    additionalFrame = new GNEAdditionalFrame(viewParent, viewNet);
    crossingFrame = new GNECrossingFrame(viewParent, viewNet);
    TAZFrame = new GNETAZFrame(viewParent, viewNet);
    polygonFrame = new GNEShapeFrame(viewParent, viewNet);
    createEdgeFrame = new GNECreateEdgeFrame(viewParent, viewNet);
}


void
GNEViewParent::NetworkFrames::hideNetworkFrames() {
    connectorFrame->hide();
    TLSEditorFrame->hide();
    additionalFrame->hide();
    crossingFrame->hide();
    TAZFrame->hide();
    polygonFrame->hide();
    prohibitionFrame->hide();
    wireFrame->hide();
    createEdgeFrame->hide();
}


void
GNEViewParent::NetworkFrames::setNetworkFramesWidth(int frameWidth) {
    // set width in all frames
    connectorFrame->setFrameWidth(frameWidth);
    TLSEditorFrame->setFrameWidth(frameWidth);
    additionalFrame->setFrameWidth(frameWidth);
    crossingFrame->setFrameWidth(frameWidth);
    TAZFrame->setFrameWidth(frameWidth);
    polygonFrame->setFrameWidth(frameWidth);
    prohibitionFrame->setFrameWidth(frameWidth);
    wireFrame->setFrameWidth(frameWidth);
    createEdgeFrame->setFrameWidth(frameWidth);
}


bool
GNEViewParent::NetworkFrames::isNetworkFrameShown() const {
    // check all frames
    if (connectorFrame->shown()) {
        return true;
    } else if (TLSEditorFrame->shown()) {
        return true;
    } else if (additionalFrame->shown()) {
        return true;
    } else if (crossingFrame->shown()) {
        return true;
    } else if (TAZFrame->shown()) {
        return true;
    } else if (polygonFrame->shown()) {
        return true;
    } else if (prohibitionFrame->shown()) {
        return true;
    } else if (wireFrame->shown()) {
        return true;
    } else if (createEdgeFrame->shown()) {
        return true;
    } else {
        return false;
    }
}


GNEFrame*
GNEViewParent::NetworkFrames::getCurrentShownFrame() const {
    // check all frames
    if (connectorFrame->shown()) {
        return connectorFrame;
    } else if (TLSEditorFrame->shown()) {
        return TLSEditorFrame;
    } else if (additionalFrame->shown()) {
        return additionalFrame;
    } else if (crossingFrame->shown()) {
        return crossingFrame;
    } else if (TAZFrame->shown()) {
        return TAZFrame;
    } else if (polygonFrame->shown()) {
        return polygonFrame;
    } else if (prohibitionFrame->shown()) {
        return prohibitionFrame;
    } else if (wireFrame->shown()) {
        return wireFrame;
    } else if (createEdgeFrame->shown()) {
        return createEdgeFrame;
    } else {
        return nullptr;
    }
}

// ---------------------------------------------------------------------------
// GNEViewParent::DemandFrames - methods
// ---------------------------------------------------------------------------

GNEViewParent::DemandFrames::DemandFrames() :
    routeFrame(nullptr),
    vehicleFrame(nullptr),
    vehicleTypeFrame(nullptr),
    stopFrame(nullptr),
    personFrame(nullptr),
    personPlanFrame(nullptr),
    containerFrame(nullptr),
    containerPlanFrame(nullptr) {
}


void
GNEViewParent::DemandFrames::buildDemandFrames(GNEViewParent* viewParent, GNEViewNet* viewNet) {
    routeFrame = new GNERouteFrame(viewParent, viewNet);
    vehicleFrame = new GNEVehicleFrame(viewParent, viewNet);
    vehicleTypeFrame = new GNETypeFrame(viewParent, viewNet);
    stopFrame = new GNEStopFrame(viewParent, viewNet);
    personFrame = new GNEPersonFrame(viewParent, viewNet);
    personPlanFrame = new GNEPersonPlanFrame(viewParent, viewNet);
    containerFrame = new GNEContainerFrame(viewParent, viewNet);
    containerPlanFrame = new GNEContainerPlanFrame(viewParent, viewNet);
}


void
GNEViewParent::DemandFrames::hideDemandFrames() {
    routeFrame->hide();
    vehicleFrame->hide();
    vehicleTypeFrame->hide();
    stopFrame->hide();
    personFrame->hide();
    personPlanFrame->hide();
    containerFrame->hide();
    containerPlanFrame->hide();
}


void
GNEViewParent::DemandFrames::setDemandFramesWidth(int frameWidth) {
    // set width in all frames
    routeFrame->setFrameWidth(frameWidth);
    vehicleFrame->setFrameWidth(frameWidth);
    vehicleTypeFrame->setFrameWidth(frameWidth);
    stopFrame->setFrameWidth(frameWidth);
    personFrame->setFrameWidth(frameWidth);
    personPlanFrame->setFrameWidth(frameWidth);
    containerFrame->setFrameWidth(frameWidth);
    containerPlanFrame->setFrameWidth(frameWidth);
}


bool
GNEViewParent::DemandFrames::isDemandFrameShown() const {
    // check all frames
    if (routeFrame->shown()) {
        return true;
    } else if (vehicleFrame->shown()) {
        return true;
    } else if (vehicleTypeFrame->shown()) {
        return true;
    } else if (stopFrame->shown()) {
        return true;
    } else if (personFrame->shown()) {
        return true;
    } else if (personPlanFrame->shown()) {
        return true;
    } else if (containerFrame->shown()) {
        return true;
    } else if (containerPlanFrame->shown()) {
        return true;
    } else {
        return false;
    }
}


GNEFrame*
GNEViewParent::DemandFrames::getCurrentShownFrame() const {
    // check all frames
    if (routeFrame->shown()) {
        return routeFrame;
    } else if (vehicleFrame->shown()) {
        return vehicleFrame;
    } else if (vehicleTypeFrame->shown()) {
        return vehicleTypeFrame;
    } else if (stopFrame->shown()) {
        return stopFrame;
    } else if (personFrame->shown()) {
        return personFrame;
    } else if (personPlanFrame->shown()) {
        return personPlanFrame;
    } else if (containerFrame->shown()) {
        return containerFrame;
    } else if (containerPlanFrame->shown()) {
        return containerPlanFrame;
    } else {
        return nullptr;
    }
}

// ---------------------------------------------------------------------------
// GNEViewParent::DataFrames - methods
// ---------------------------------------------------------------------------

GNEViewParent::DataFrames::DataFrames() {
}


void
GNEViewParent::DataFrames::buildDataFrames(GNEViewParent* viewParent, GNEViewNet* viewNet) {
    edgeDataFrame = new GNEEdgeDataFrame(viewParent, viewNet);
    edgeRelDataFrame = new GNEEdgeRelDataFrame(viewParent, viewNet);
    TAZRelDataFrame = new GNETAZRelDataFrame(viewParent, viewNet);
    meanDataFrame = new GNEMeanDataFrame(viewParent, viewNet);
}


void
GNEViewParent::DataFrames::hideDataFrames() {
    edgeDataFrame->hide();
    edgeRelDataFrame->hide();
    TAZRelDataFrame->hide();
    meanDataFrame->hide();
}


void
GNEViewParent::DataFrames::setDataFramesWidth(int frameWidth) {
    // set width in all frames
    edgeDataFrame->setFrameWidth(frameWidth);
    edgeRelDataFrame->setFrameWidth(frameWidth);
    TAZRelDataFrame->setFrameWidth(frameWidth);
    meanDataFrame->setFrameWidth(frameWidth);
}


bool
GNEViewParent::DataFrames::isDataFrameShown() const {
    // check all frames
    if (edgeDataFrame->shown()) {
        return true;
    } else if (edgeRelDataFrame->shown()) {
        return true;
    } else if (TAZRelDataFrame->shown()) {
        return true;
    } else if (meanDataFrame->shown()) {
        return true;
    } else {
        return false;
    }
}


GNEFrame*
GNEViewParent::DataFrames::getCurrentShownFrame() const {
    // check all frames
    if (edgeDataFrame->shown()) {
        return edgeDataFrame;
    } else if (edgeRelDataFrame->shown()) {
        return edgeRelDataFrame;
    } else if (TAZRelDataFrame->shown()) {
        return TAZRelDataFrame;
    } else if (meanDataFrame->shown()) {
        return meanDataFrame;
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
    ACChooserWalkingAreas(nullptr),
    ACChooserVehicles(nullptr),
    ACChooserPersons(nullptr),
    ACChooserRoutes(nullptr),
    ACChooserStops(nullptr),
    ACChooserTLS(nullptr),
    ACChooserAdditional(nullptr),
    ACChooserPOI(nullptr),
    ACChooserPolygon(nullptr),
    ACChooserProhibition(nullptr),
    ACChooserWire(nullptr) {
}


GNEViewParent::ACChoosers::~ACChoosers() {
    // remove all  dialogs if are active
    if (ACChooserJunction) {
        delete ACChooserJunction;
    }
    if (ACChooserEdges) {
        delete ACChooserEdges;
    }
    if (ACChooserWalkingAreas) {
        delete ACChooserWalkingAreas;
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
    if (ACChooserWire) {
        delete ACChooserWire;
    }
}


/****************************************************************************/
