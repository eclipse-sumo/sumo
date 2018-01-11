/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDeleteFrame.cpp
/// @author  Pablo Alvarez Lopex
/// @date    Dec 2016
/// @version $Id$
///
// The Widget for remove network-elements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include "GNEAdditionalFrame.h"
#include "GNEAttributeCarrier.h"
#include "GNEChange_Selection.h"
#include "GNEConnection.h"
#include "GNECrossing.h"
#include "GNEDeleteFrame.h"
#include "GNEEdge.h"
#include "GNEInspectorFrame.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEPOI.h"
#include "GNEPoly.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNERerouter.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDeleteFrame) GNEDeleteFrameMap[] = {
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_DELETEFRAME_CHILDS,     GNEDeleteFrame::onCmdShowChildMenu),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETEFRAME_CENTER,     GNEDeleteFrame::onCmdCenterChildItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETEFRAME_INSPECT,    GNEDeleteFrame::onCmdInspectChildItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETEFRAME_DELETE,     GNEDeleteFrame::onCmdDeleteChildItem),
};

// Object implementation
FXIMPLEMENT(GNEDeleteFrame, FXVerticalFrame, GNEDeleteFrameMap, ARRAYNUMBER(GNEDeleteFrameMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEDeleteFrame::GNEDeleteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Delete"),
    myMarkedAC(NULL) {
    // Create Groupbox for current element
    myGroupBoxCurrentElement = new FXGroupBox(myContentFrame, "Current element", GUIDesignGroupBoxFrame);
    myCurrentElementLabel = new FXLabel(myGroupBoxCurrentElement, "No item under cursor", 0, GUIDesignLabelFrameInformation);

    // Create Groupbox for current element
    myGroupBoxOptions = new FXGroupBox(myContentFrame, "Options", GUIDesignGroupBoxFrame);

    // Create checkbox for enabling/disabling automatic deletion of additionals childs (by default, enabled)
    myAutomaticallyDeleteAdditionalsCheckButton = new FXCheckButton(myGroupBoxOptions, "Force deletion of additionals", this, MID_GNE_DELETEFRAME_AUTODELETEADDITIONALS, GUIDesignCheckButtonAttribute);
    myAutomaticallyDeleteAdditionalsCheckButton->setCheck(true);

    // Create checkbox for enabling/disabling delete only geomtery point(by default, disabled)
    myDeleteOnlyGeometryPoints = new FXCheckButton(myGroupBoxOptions, "Delete only geometryPoints", this, MID_GNE_DELETEFRAME_ONLYGEOMETRYPOINTS, GUIDesignCheckButtonAttribute);
    myDeleteOnlyGeometryPoints->setCheck(false);

    // Create groupbox for tree list
    myGroupBoxTreeList = new FXGroupBox(myContentFrame, "Childs", GUIDesignGroupBoxFrame);
    myMarkedElementLabel = new FXLabel(myGroupBoxTreeList, "No item marked", 0, GUIDesignLabelFrameInformation);
    myTreelist = new FXTreeList(myGroupBoxTreeList, this, MID_GNE_DELETEFRAME_CHILDS, GUIDesignTreeListFrame);

    // Create groupbox for help
    myGroupBoxInformation = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    myInformationLabel = new FXLabel(myGroupBoxInformation, " - Left click to delete element.\n - Hold <CTRL> and left click\n   to mark element.\n - Left click in another element\n   dismark marked element.", 0, GUIDesignLabelFrameInformation);
}


GNEDeleteFrame::~GNEDeleteFrame() {}


void
GNEDeleteFrame::showChildsOfMarkedAttributeCarrier() {
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsWithoutAC.clear();
    // Switch gl type of ac
    if (myMarkedAC) {
        switch (myMarkedAC->getGUIGLObject()->getType()) {
            case GLO_JUNCTION: {
                // insert junction root
                GNEJunction* junction = dynamic_cast<GNEJunction*>(myMarkedAC);
                FXTreeItem* junctionItem = myTreelist->insertItem(0, 0, toString(junction->getTag()).c_str(), junction->getIcon(), junction->getIcon());
                myTreeItemToACMap[junctionItem] = junction;
                junctionItem->setExpanded(true);
                // insert edges
                for (int i = 0; i < (int)junction->getGNEEdges().size(); i++) {
                    GNEEdge* edge = junction->getGNEEdges().at(i);
                    FXTreeItem* edgeItem = myTreelist->insertItem(0, junctionItem, (toString(edge->getTag()) + " " + toString(i)).c_str(), edge->getIcon(), edge->getIcon());
                    myTreeItemToACMap[edgeItem] = edge;
                    edgeItem->setExpanded(true);
                    // insert lanes
                    for (int j = 0; j < (int)edge->getLanes().size(); j++) {
                        GNELane* lane = edge->getLanes().at(j);
                        FXTreeItem* laneItem = myTreelist->insertItem(0, edgeItem, (toString(lane->getTag()) + " " + toString(j)).c_str(), lane->getIcon(), lane->getIcon());
                        myTreeItemToACMap[laneItem] = lane;
                        laneItem->setExpanded(true);
                        // insert additionals of lanes
                        for (int k = 0; k < (int)lane->getAdditionalChilds().size(); k++) {
                            GNEAdditional* additional = lane->getAdditionalChilds().at(k);
                            FXTreeItem* additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(k)).c_str(), additional->getIcon(), additional->getIcon());
                            myTreeItemToACMap[additionalItem] = additional;
                            additionalItem->setExpanded(true);
                        }
                        // insert incoming connections of lanes (by default isn't expanded)
                        if (lane->getGNEIncomingConnections().size() > 0) {
                            FXTreeItem* incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", lane->getGNEIncomingConnections().front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                            myTreeItemsWithoutAC.insert(incomingConnections);
                            incomingConnections->setExpanded(false);
                            for (int k = 0; k < (int)lane->getGNEIncomingConnections().size(); k++) {
                                GNEConnection* connection = lane->getGNEIncomingConnections().at(k);
                                FXTreeItem* connectionItem = myTreelist->insertItem(0, incomingConnections, (toString(connection->getTag()) + " " + toString(k)).c_str(), connection->getIcon(), connection->getIcon());
                                myTreeItemToACMap[connectionItem] = connection;
                                connectionItem->setExpanded(true);
                            }
                        }
                        // insert outcoming connections of lanes (by default isn't expanded)
                        if (lane->getGNEOutcomingConnections().size() > 0) {
                            FXTreeItem* outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", lane->getGNEOutcomingConnections().front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                            myTreeItemsWithoutAC.insert(outgoingConnections);
                            outgoingConnections->setExpanded(false);
                            for (int k = 0; k < (int)lane->getGNEOutcomingConnections().size(); k++) {
                                GNEConnection* connection = lane->getGNEOutcomingConnections().at(k);
                                FXTreeItem* connectionItem = myTreelist->insertItem(0, outgoingConnections, (toString(connection->getTag()) + " " + toString(k)).c_str(), connection->getIcon(), connection->getIcon());
                                myTreeItemToACMap[connectionItem] = connection;
                                connectionItem->setExpanded(true);
                            }
                        }
                    }
                    // insert additionals of edge
                    for (int j = 0; j < (int)edge->getAdditionalChilds().size(); j++) {
                        GNEAdditional* additional = edge->getAdditionalChilds().at(j);
                        FXTreeItem* additionalItem = myTreelist->insertItem(0, edgeItem, (toString(additional->getTag()) + " " + toString(j)).c_str(), additional->getIcon(), additional->getIcon());
                        myTreeItemToACMap[additionalItem] = additional;
                        additionalItem->setExpanded(true);
                    }

                }
                // insert crossings
                for (int i = 0; i < (int)junction->getGNECrossings().size(); i++) {
                    GNECrossing* crossing = junction->getGNECrossings().at(i);
                    FXTreeItem* crossingItem = myTreelist->insertItem(0, junctionItem, (toString(crossing->getTag()) + " " + toString(i)).c_str(), crossing->getIcon(), crossing->getIcon());
                    myTreeItemToACMap[crossingItem] = crossing;
                    crossingItem->setExpanded(true);
                }
                break;
            }
            case GLO_EDGE: {
                // insert edge root
                GNEEdge* edge = dynamic_cast<GNEEdge*>(myMarkedAC);
                FXTreeItem* edgeItem = myTreelist->insertItem(0, 0, toString(edge->getTag()).c_str(), edge->getIcon(), edge->getIcon());
                myTreeItemToACMap[edgeItem] = edge;
                edgeItem->setExpanded(true);
                // insert lanes
                for (int i = 0; i < (int)edge->getLanes().size(); i++) {
                    GNELane* lane = edge->getLanes().at(i);
                    FXTreeItem* laneItem = myTreelist->insertItem(0, edgeItem, (toString(lane->getTag()) + " " + toString(i)).c_str(), lane->getIcon(), lane->getIcon());
                    myTreeItemToACMap[laneItem] = lane;
                    laneItem->setExpanded(true);
                    // insert additionals of lanes
                    for (int j = 0; j < (int)lane->getAdditionalChilds().size(); j++) {
                        GNEAdditional* additional = lane->getAdditionalChilds().at(j);
                        FXTreeItem* additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(j)).c_str(), additional->getIcon(), additional->getIcon());
                        myTreeItemToACMap[additionalItem] = additional;
                        additionalItem->setExpanded(true);
                    }
                    // insert incoming connections of lanes (by default isn't expanded)
                    if (lane->getGNEIncomingConnections().size() > 0) {
                        FXTreeItem* incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", lane->getGNEIncomingConnections().front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                        myTreeItemsWithoutAC.insert(incomingConnections);
                        incomingConnections->setExpanded(false);
                        for (int j = 0; j < (int)lane->getGNEIncomingConnections().size(); j++) {
                            GNEConnection* connection = lane->getGNEIncomingConnections().at(j);
                            FXTreeItem* connectionItem = myTreelist->insertItem(0, incomingConnections, (toString(connection->getTag()) + " " + toString(j)).c_str(), connection->getIcon(), connection->getIcon());
                            myTreeItemToACMap[connectionItem] = connection;
                            connectionItem->setExpanded(true);
                        }
                    }
                    // insert outcoming connections of lanes (by default isn't expanded)
                    if (lane->getGNEOutcomingConnections().size() > 0) {
                        FXTreeItem* outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", lane->getGNEOutcomingConnections().front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                        myTreeItemsWithoutAC.insert(outgoingConnections);
                        outgoingConnections->setExpanded(false);
                        for (int j = 0; j < (int)lane->getGNEOutcomingConnections().size(); j++) {
                            GNEConnection* connection = lane->getGNEOutcomingConnections().at(j);
                            FXTreeItem* connectionItem = myTreelist->insertItem(0, outgoingConnections, (toString(connection->getTag()) + " " + toString(j)).c_str(), connection->getIcon(), connection->getIcon());
                            myTreeItemToACMap[connectionItem] = connection;
                            connectionItem->setExpanded(true);
                        }
                    }
                }
                // insert additionals of edge
                for (int i = 0; i < (int)edge->getAdditionalChilds().size(); i++) {
                    GNEAdditional* additionalChild = edge->getAdditionalChilds().at(i);
                    FXTreeItem* additionalItem = myTreelist->insertItem(0, edgeItem, (toString(additionalChild->getTag()) + " " + toString(i)).c_str(), additionalChild->getIcon(), additionalChild->getIcon());
                    myTreeItemToACMap[additionalItem] = additionalChild;
                    additionalItem->setExpanded(true);
                }
                // add a extra section for additional parent in which this edge is part
                if (edge->getAdditionalParents().size() > 0) {
                    FXTreeItem* additionalParents = myTreelist->insertItem(0, edgeItem, "part of", 0, 0);
                    myTreeItemsWithoutAC.insert(additionalParents);
                    additionalParents->setExpanded(true);
                    // insert reroutes of edge
                    for (int i = 0; i < (int)edge->getAdditionalParents().size(); i++) {
                        GNEAdditional* additionalParent = edge->getAdditionalParents().at(i);
                        FXTreeItem* additionalItem = myTreelist->insertItem(0, additionalParents, (toString(additionalParent->getTag()) + " " + toString(i)).c_str(), additionalParent->getIcon(), additionalParent->getIcon());
                        myTreeItemToACMap[additionalItem] = additionalParent;
                        additionalItem->setExpanded(true);
                    }
                }
                break;
            }
            case GLO_LANE: {
                // insert lane root
                GNELane* lane = dynamic_cast<GNELane*>(myMarkedAC);
                FXTreeItem* laneItem = myTreelist->insertItem(0, 0, toString(lane->getTag()).c_str(), lane->getIcon(), lane->getIcon());
                myTreeItemToACMap[laneItem] = lane;
                laneItem->setExpanded(true);
                // insert additionals of lanes
                for (int i = 0; i < (int)lane->getAdditionalChilds().size(); i++) {
                    GNEAdditional* additional = lane->getAdditionalChilds().at(i);
                    FXTreeItem* additionalItem = myTreelist->insertItem(0, laneItem, (toString(additional->getTag()) + " " + toString(i)).c_str(), additional->getIcon(), additional->getIcon());
                    myTreeItemToACMap[additionalItem] = additional;
                    additionalItem->setExpanded(true);
                }
                // insert incoming connections of lanes (by default isn't expanded)
                if (lane->getGNEIncomingConnections().size() > 0) {
                    FXTreeItem* incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", lane->getGNEIncomingConnections().front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                    myTreeItemsWithoutAC.insert(incomingConnections);
                    incomingConnections->setExpanded(false);
                    for (int i = 0; i < (int)lane->getGNEIncomingConnections().size(); i++) {
                        GNEConnection* connection = lane->getGNEIncomingConnections().at(i);
                        FXTreeItem* connectionItem = myTreelist->insertItem(0, incomingConnections, (toString(connection->getTag()) + " " + toString(i)).c_str(), connection->getIcon(), connection->getIcon());
                        myTreeItemToACMap[connectionItem] = connection;
                        connectionItem->setExpanded(true);
                    }
                }
                // insert outcoming connections of lanes (by default isn't expanded)
                if (lane->getGNEOutcomingConnections().size() > 0) {
                    FXTreeItem* outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", lane->getGNEOutcomingConnections().front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                    myTreeItemsWithoutAC.insert(outgoingConnections);
                    outgoingConnections->setExpanded(false);
                    for (int i = 0; i < (int)lane->getGNEOutcomingConnections().size(); i++) {
                        GNEConnection* connection = lane->getGNEOutcomingConnections().at(i);
                        FXTreeItem* connectionItem = myTreelist->insertItem(0, outgoingConnections, (toString(connection->getTag()) + " " + toString(i)).c_str(), connection->getIcon(), connection->getIcon());
                        myTreeItemToACMap[connectionItem] = connection;
                        connectionItem->setExpanded(true);
                    }
                }
                break;
            }
            case GLO_POI: {
                // insert POI root
                GNEPOI* POI = dynamic_cast<GNEPOI*>(myMarkedAC);
                FXTreeItem* POIItem = myTreelist->insertItem(0, 0, toString(POI->getTag()).c_str(), POI->getIcon(), POI->getIcon());
                myTreeItemToACMap[POIItem] = POI;
                POIItem->setExpanded(true);
                break;
            }
            case GLO_POLYGON: {
                // insert polygon root
                GNEPoly* polygon = dynamic_cast<GNEPoly*>(myMarkedAC);
                FXTreeItem* polygonItem = myTreelist->insertItem(0, 0, toString(polygon->getTag()).c_str(), polygon->getIcon(), polygon->getIcon());
                myTreeItemToACMap[polygonItem] = polygon;
                polygonItem->setExpanded(true);
                break;
            }
            case GLO_CROSSING: {
                // insert crossing root
                GNECrossing* crossing = dynamic_cast<GNECrossing*>(myMarkedAC);
                FXTreeItem* crossingItem = myTreelist->insertItem(0, 0, toString(crossing->getTag()).c_str(), crossing->getIcon(), crossing->getIcon());
                myTreeItemToACMap[crossingItem] = crossing;
                crossingItem->setExpanded(true);
                break;
            }
            case GLO_ADDITIONAL: {
                // insert additional root
                GNEAdditional* additional = dynamic_cast<GNEAdditional*>(myMarkedAC);
                FXTreeItem* additionalItem = myTreelist->insertItem(0, 0, toString(additional->getTag()).c_str(), additional->getIcon(), additional->getIcon());
                myTreeItemToACMap[additionalItem] = additional;
                additionalItem->setExpanded(true);
                break;
            }
            case GLO_CONNECTION: {
                // insert connection root
                GNEConnection* connection = dynamic_cast<GNEConnection*>(myMarkedAC);
                FXTreeItem* connectionItem = myTreelist->insertItem(0, 0, toString(connection->getTag()).c_str(), connection->getIcon(), connection->getIcon());
                myTreeItemToACMap[connectionItem] = connection;
                connectionItem->setExpanded(true);
                break;
            }
            default: {
                break;
            }
        }
    }
}


void
GNEDeleteFrame::removeAttributeCarrier(GNEAttributeCarrier* ac) {
    // obtain clicked position
    Position clickedPosition = myViewNet->getPositionInformation();
    // To remove an attribute carrier deleteFrame must be visible
    if (shown() == false) {
        // Hide inspector frame and show delete frame
        myViewNet->getViewParent()->getInspectorFrame()->hide();
        show();
    } else if (myDeleteOnlyGeometryPoints->getCheck()) {
        // check type of of GL object
        switch (ac->getGUIGLObject()->getType()) {
            case GLO_EDGE: {
                GNEEdge* edge = dynamic_cast<GNEEdge*>(ac);
                if (edge->getVertexIndex(clickedPosition, false) != -1) {
                    edge->deleteGeometryPoint(clickedPosition);
                }
                break;
            }
            case GLO_POLYGON: {
                GNEPoly* polygon = dynamic_cast<GNEPoly*>(ac);
                if (polygon->getVertexIndex(clickedPosition, false) != -1) {
                    polygon->deleteGeometryPoint(clickedPosition);
                }
                break;
            }
            default: {
                break;
            }
        }
    } else {
        // check type of of GL object
        switch (ac->getGUIGLObject()->getType()) {
            case GLO_JUNCTION: {
                GNEJunction* junction = dynamic_cast<GNEJunction*>(ac);
                // obtain number of additionals of junction's childs
                int numberOfAdditionals = 0;
                for (auto i : junction->getGNEEdges()) {
                    numberOfAdditionals += (int)i->getAdditionalChilds().size();
                    for (auto j : i->getLanes()) {
                        UNUSED_PARAMETER(j);
                        numberOfAdditionals += (int)i->getAdditionalChilds().size();
                    }
                }
                // Check if junction can be deleted
                if (myAutomaticallyDeleteAdditionalsCheckButton->getCheck()) {
                    myViewNet->getNet()->deleteJunction(junction, myViewNet->getUndoList());
                } else {
                    if (numberOfAdditionals > 0) {
                        // write warning if netedit is running in testing mode
                        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                            WRITE_WARNING("Opening FXMessageBox 'Force deletion needed'");
                        }
                        std::string plural = numberOfAdditionals > 1 ? "s" : "";
                        // Open warning DialogBox
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(junction->getTag())).c_str(), "%s",
                                              (toString(junction->getTag()) + " '" + junction->getID() + "' cannot be deleted because owns " +
                                               toString(numberOfAdditionals) + " additional child" + plural + ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                            WRITE_WARNING("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                        }
                    } else {
                        myViewNet->getNet()->deleteJunction(junction, myViewNet->getUndoList());
                    }
                }
                break;
            }
            case GLO_EDGE: {
                GNEEdge* edge = dynamic_cast<GNEEdge*>(ac);
                // check if click was over a geometry point or over a shape's edge
                if (edge->getVertexIndex(clickedPosition, false) != -1) {
                    edge->deleteGeometryPoint(clickedPosition);
                } else {
                    int numberOfAdditionalChilds = (int)edge->getAdditionalChilds().size();
                    int numberOfAdditionalParents = (int)edge->getAdditionalParents().size();
                    // Iterate over lanes and obtain total number of additional childs
                    for (auto i : edge->getLanes()) {
                        numberOfAdditionalChilds += (int)i->getAdditionalChilds().size();
                    }
                    // Check if edge can be deleted
                    if (myAutomaticallyDeleteAdditionalsCheckButton->getCheck()) {
                        // when deleting a single edge, keep all unaffected connections as they were
                        myViewNet->getNet()->deleteEdge(edge, myViewNet->getUndoList(), false);
                    } else {
                        if (numberOfAdditionalChilds > 0) {
                            // write warning if netedit is running in testing mode
                            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                                WRITE_WARNING("Opening FXMessageBox 'Force deletion needed'");
                            }
                            std::string plural = numberOfAdditionalChilds > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(edge->getTag())).c_str(), "%s",
                                                  (toString(edge->getTag()) + " '" + edge->getID() + "' cannot be deleted because owns " +
                                                   toString(numberOfAdditionalChilds) + " additional" + plural + ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                                WRITE_WARNING("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                            }
                        } else if (numberOfAdditionalParents > 0) {
                            // write warning if netedit is running in testing mode
                            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                                WRITE_WARNING("Opening FXMessageBox 'Force deletion needed'");
                            }
                            std::string plural = numberOfAdditionalParents > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(edge->getTag())).c_str(), "%s",
                                                  (toString(edge->getTag()) + " '" + edge->getID() + "' cannot be deleted because is part of " +
                                                   toString(numberOfAdditionalParents) + " additional" + plural + ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                                WRITE_WARNING("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                            }
                        } else {
                            // when deleting a single edge, keep all unaffected connections as they were
                            myViewNet->getNet()->deleteEdge(edge, myViewNet->getUndoList(), false);
                        }
                    }
                }
                break;
            }
            case GLO_LANE: {
                GNELane* lane = dynamic_cast<GNELane*>(ac);
                // Check if lane can be deleted
                if (myAutomaticallyDeleteAdditionalsCheckButton->getCheck()) {
                    // when deleting a single lane, keep all unaffected connections as they were
                    myViewNet->getNet()->deleteLane(lane, myViewNet->getUndoList(), false);
                } else {
                    if (lane->getAdditionalChilds().size() == 0) {
                        // when deleting a single lane, keep all unaffected connections as they were
                        myViewNet->getNet()->deleteLane(lane, myViewNet->getUndoList(), false);
                    } else {
                        // write warning if netedit is running in testing mode
                        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                            WRITE_WARNING("Opening FXMessageBox 'Force deletion needed'");
                        }
                        // open warning box
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(lane->getTag())).c_str(), "%s",
                                              (toString(lane->getTag()) + " '" + lane->getID() + "' cannot be deleted because it has " +
                                               toString(lane->getAdditionalChilds().size()) + " additional childs.\n Check 'Force deletion of Additionals' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                            WRITE_WARNING("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                        }
                    }
                }
                break;
            }
            case GLO_POLYGON:
            case GLO_POI: {
                myViewNet->getNet()->deleteShape(dynamic_cast<GNEShape*>(ac), myViewNet->getUndoList());
                break;
            }
            case GLO_CROSSING: {
                myViewNet->getNet()->deleteCrossing(dynamic_cast<GNECrossing*>(ac), myViewNet->getUndoList());
                break;
            }
            case GLO_ADDITIONAL: {
                myViewNet->getViewParent()->getAdditionalFrame()->removeAdditional(dynamic_cast<GNEAdditional*>(ac));
                break;
            }
            case GLO_CONNECTION: {
                myViewNet->getNet()->deleteConnection(dynamic_cast<GNEConnection*>(ac), myViewNet->getUndoList());
                break;
            }
            default: {
                break;
            }
        }
    }
    // update view to show changes
    myViewNet->update();
}


void
GNEDeleteFrame::markAttributeCarrier(GNEAttributeCarrier* ac) {
    // Check if under cursor there are an Attribute Carrier
    if (ac != NULL) {
        myMarkedElementLabel->setText(("  " + toString(ac->getTag()) + " '" + ac->getID() + "'").c_str());
        myMarkedElementLabel->setIcon(ac->getIcon());
    } else {
        myMarkedElementLabel->setText("No item marked");
        myMarkedElementLabel->setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    }
    // mark ac
    myMarkedAC = ac;
}


void
GNEDeleteFrame::updateCurrentLabel(GNEAttributeCarrier* ac) {
    // change label
    if (ac != NULL) {
        myCurrentElementLabel->setText((toString(ac->getTag()) + " '" + ac->getID() + "'").c_str());
        myCurrentElementLabel->setIcon(ac->getIcon());
    } else {
        myCurrentElementLabel->setText("No item under cursor");
        myCurrentElementLabel->setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    }
}


GNEAttributeCarrier*
GNEDeleteFrame::getMarkedAttributeCarrier() const {
    return myMarkedAC;
}


long
GNEDeleteFrame::onCmdShowChildMenu(FXObject*, FXSelector, void* eventData) {
    // Obtain event
    FXEvent* e = (FXEvent*) eventData;
    FXTreeItem* item = myTreelist->getItemAt(e->win_x, e->win_y);
    // Check if there are an item in the position and create pop-up menu
    if (item && (myTreeItemsWithoutAC.find(item) == myTreeItemsWithoutAC.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[myTreelist->getItemAt(e->win_x, e->win_y)]);
    }
    return 1;
}


long
GNEDeleteFrame::onCmdCenterChildItem(FXObject*, FXSelector, void*) {
    myViewNet->centerTo(myClickedAC->getGUIGLObject()->getGlID(), false);
    myViewNet->update();
    return 1;
}


long
GNEDeleteFrame::onCmdInspectChildItem(FXObject*, FXSelector, void*) {
    if (myMarkedAC != NULL) {
        myViewNet->getViewParent()->getInspectorFrame()->show();
        myViewNet->getViewParent()->getInspectorFrame()->inspectFromDeleteFrame(myClickedAC, myMarkedAC, true);
        // Hide delete frame and show inspector frame
        hide();
    }
    return 1;
}


long
GNEDeleteFrame::onCmdDeleteChildItem(FXObject*, FXSelector, void*) {
    removeAttributeCarrier(myClickedAC);
    return 1;
}


void
GNEDeleteFrame::createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac) {
    // create FXMenuPane
    FXMenuPane* pane = new FXMenuPane(myTreelist);
    // set current clicked AC
    myClickedAC = ac;
    // set name
    new MFXMenuHeader(pane, myViewNet->getViewParent()->getApp()->getBoldFont(), (toString(myClickedAC->getTag()) + ": " + myClickedAC->getID()).c_str(), myClickedAC->getIcon());
    new FXMenuSeparator(pane);
    // Fill FXMenuCommand
    new FXMenuCommand(pane, "Center", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_GNE_DELETEFRAME_CENTER);
    new FXMenuCommand(pane, "Inspect", GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_DELETEFRAME_INSPECT);
    new FXMenuCommand(pane, "Delete", GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_DELETEFRAME_DELETE);
    // Center in the mouse position and create pane
    pane->setX(X);
    pane->setY(Y);
    pane->create();
    pane->show();
}


/****************************************************************************/
