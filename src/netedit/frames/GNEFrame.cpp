/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/dialogs/GNEGenericParameterDialog.h>

#include "GNEFrame.h"
#include "GNEInspectorFrame.h"
#include "GNEPolygonFrame.h"
#include "GNEDeleteFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrame::ACHierarchy) GNEFrameACHierarchyMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_CENTER,      GNEFrame::ACHierarchy::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_INSPECT,     GNEFrame::ACHierarchy::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_DELETE,      GNEFrame::ACHierarchy::onCmdDeleteItem),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_DELETEFRAME_CHILDS,         GNEFrame::ACHierarchy::onCmdShowChildMenu),
};

FXDEFMAP(GNEFrame::GenericParametersEditor) GenericParametersEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrame::GenericParametersEditor::onCmdEditGenericParameter),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrame::GenericParametersEditor::onCmdSetGenericParameter),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                       GNEFrame::GenericParametersEditor::onCmdGenericParameterHelp),
};

// Object implementation
FXIMPLEMENT(GNEFrame::ACHierarchy,              FXGroupBox, GNEFrameACHierarchyMap, ARRAYNUMBER(GNEFrameACHierarchyMap))
FXIMPLEMENT(GNEFrame::GenericParametersEditor,  FXGroupBox, GenericParametersEditorMap, ARRAYNUMBER(GenericParametersEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrame::ACHierarchy - methods
// ---------------------------------------------------------------------------

GNEFrame::ACHierarchy::ACHierarchy(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Hierarchy", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myAC(nullptr) {
    // Create three list
    myTreelist = new FXTreeList(this, this, MID_GNE_DELETEFRAME_CHILDS, GUIDesignTreeListFrame);
    hide();
}


GNEFrame::ACHierarchy::~ACHierarchy() {}


void 
GNEFrame::ACHierarchy::showACHierarchy(GNEAttributeCarrier* AC) {
    myAC = AC;
    // show ACHierarchy and refresh ACHierarchy
    if(myAC) {
        show();
        refreshACHierarchy();
    }
}


void 
GNEFrame::ACHierarchy::hideACHierarchy() {
    myAC = nullptr;
    hide();
}


void 
GNEFrame::ACHierarchy::refreshACHierarchy() {
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsConnections.clear();
    // show ACChilds of myAC
    if(myAC) {
        showAttributeCarrierChilds(myAC, showAttributeCarrierParents());
    }
}


long 
GNEFrame::ACHierarchy::onCmdShowChildMenu(FXObject*, FXSelector, void* eventData) {
    // Obtain event
    FXEvent* e = (FXEvent*)eventData;
    // obtain FXTreeItem in the given position
    FXTreeItem* item = myTreelist->getItemAt(e->win_x, e->win_y);
    // open Pop-up if FXTreeItem has a Attribute Carrier vinculated
    if (item && (myTreeItemsConnections.find(item) == myTreeItemsConnections.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[item]);
    }
    return 1;
}


long 
GNEFrame::ACHierarchy::onCmdCenterItem(FXObject*, FXSelector, void*) {
    GUIGlObject *glObject = dynamic_cast<GUIGlObject*>(myRightClickedAC);
    if (glObject) {
        myFrameParent->getViewNet()->centerTo(glObject->getGlID(), false);
        myFrameParent->getViewNet()->update();
    }
    return 1;
}


long 
GNEFrame::ACHierarchy::onCmdInspectItem(FXObject*, FXSelector, void*) {
    if ((myAC != nullptr) && (myRightClickedAC != nullptr)) {
        myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectChild(myRightClickedAC, myAC);
    }
    return 1;
}


long 
GNEFrame::ACHierarchy::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    // check if Inspector frame was opened before removing
    const std::vector<GNEAttributeCarrier*> &currentInspectedACs= myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getInspectedACs();
    // Remove Attribute Carrier
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->removeAttributeCarrier(myRightClickedAC);
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->hide();
    // check if inspector frame has to be shown again
    if(currentInspectedACs.size() == 1) {
        if(currentInspectedACs.front() != myRightClickedAC) {
            myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectElement(currentInspectedACs.front());
        } else {
            myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectElement(nullptr);
        }
    }
    return 1;
}


void
GNEFrame::ACHierarchy::createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac) {
    // create FXMenuPane
    FXMenuPane* pane = new FXMenuPane(myTreelist);
    // set current clicked AC
    myRightClickedAC = ac;
    // set name
    new MFXMenuHeader(pane, myFrameParent->getViewNet()->getViewParent()->getGUIMainWindow()->getBoldFont(), myRightClickedAC->getPopUpID().c_str(), myRightClickedAC->getIcon());
    new FXMenuSeparator(pane);
    // Fill FXMenuCommand
    new FXMenuCommand(pane, "Center", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_GNE_INSPECTORFRAME_CENTER);
    new FXMenuCommand(pane, "Inspect", GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_INSPECTORFRAME_INSPECT);
    new FXMenuCommand(pane, "Delete", GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_INSPECTORFRAME_DELETE);
    // Center in the mouse position and create pane
    pane->setX(X);
    pane->setY(Y);
    pane->create();
    pane->show();
}


FXTreeItem* 
GNEFrame::ACHierarchy::showAttributeCarrierParents() {
    // Switch gl type of ac
    switch (myAC->getTag()) {
        case SUMO_TAG_EDGE: {
            // obtain Edge
            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(myAC->getID(), false);
            if(edge) {
                // insert Junctions of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                // return junction destiny Item
                return junctionDestinyItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_LANE: {
            // obtain lane
            GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(myAC->getID(), false);
            if(lane) {
                // obtain edge parent
                GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Create edge item
                FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                edgeItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                myTreeItemToACMap[edgeItem] = edge;
                // return edge item
                return edgeItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_POILANE: {
            // Obtain POILane
            GNEPOI* POILane = myFrameParent->getViewNet()->getNet()->retrievePOI(myAC->getID(), false);
            if(POILane) {
                // obtain lane parent
                GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(POILane->getLane()->getID());
                // obtain edge parent
                GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Create edge item
                FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                edgeItem->setExpanded(true);
                // Create lane item
                FXTreeItem* laneItem = myTreelist->insertItem(0, edgeItem, lane->getHierarchyName().c_str(), lane->getIcon(), lane->getIcon());
                laneItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                myTreeItemToACMap[edgeItem] = edge;
                myTreeItemToACMap[laneItem] = lane;
                // return Lane item
                return laneItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_CROSSING: {
            // obtain Crossing
            GNECrossing* crossing = myFrameParent->getViewNet()->getNet()->retrieveCrossing(myAC->getID(), false);
            if(crossing) {
                // obtain junction
                GNEJunction* junction = crossing->getParentJunction();
                // create junction item
                FXTreeItem* junctionItem = myTreelist->insertItem(nullptr, nullptr, junction->getHierarchyName().c_str(), junction->getIcon(), junction->getIcon());
                junctionItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionItem] = junction;
                // return junction Item
                return junctionItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_CONNECTION: {
            // obtain Connection
            GNEConnection* connection = myFrameParent->getViewNet()->getNet()->retrieveConnection(myAC->getID(), false);
            if(connection) {
                // create edge from item
                FXTreeItem* edgeFromItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeFrom()->getHierarchyName().c_str(), connection->getEdgeFrom()->getIcon(), connection->getEdgeFrom()->getIcon());
                edgeFromItem->setExpanded(true);
                // create edge to item
                FXTreeItem* edgeToItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeTo()->getHierarchyName().c_str(), connection->getEdgeTo()->getIcon(), connection->getEdgeTo()->getIcon());
                edgeToItem->setExpanded(true);
                // create connection item
                FXTreeItem* connectionItem = myTreelist->insertItem(0, edgeToItem, connection->getHierarchyName().c_str(), connection->getIcon(), connection->getIcon());
                connectionItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[edgeFromItem] = connection->getEdgeFrom();
                myTreeItemToACMap[edgeToItem] = connection->getEdgeTo();
                myTreeItemToACMap[connectionItem] = connection;
                // return connection item
                return connectionItem;
            } else {
                return nullptr;
            }
        }
        default: {
            // obtain tag property (only for improve code legibility)
            const auto &tagValue = GNEAttributeCarrier::getTagProperties(myAC->getTag());
            // check if is an additional, and in other case return nullptr
            if(tagValue.isAdditional()) {
                // Obtain Additional
                GNEAdditional* additional = myFrameParent->getViewNet()->getNet()->retrieveAdditional(myAC->getTag(), myAC->getID(), false);
                if(additional) {
                    // first check if additional has another additional as parent (to add it into root)
                    if (tagValue.hasParent()) {
                        GNEAdditional* additionalParent = myFrameParent->getViewNet()->getNet()->retrieveAdditional(tagValue.getParentTag(), additional->getAttribute(GNE_ATTR_PARENT));
                        // create additional parent item
                        FXTreeItem* additionalParentItem = myTreelist->insertItem(0, 0, additionalParent->getHierarchyName().c_str(), additionalParent->getIcon(), additionalParent->getIcon());
                        additionalParentItem->setExpanded(true);
                        // Save it in myTreeItemToACMap
                        myTreeItemToACMap[additionalParentItem] = additionalParent;
                    }
                    if(tagValue.hasAttribute(SUMO_ATTR_EDGE)) {
                        // obtain edge parent
                        GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(additional->getAttribute(SUMO_ATTR_EDGE));
                        //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                        FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                        FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                        junctionDestinyItem->setExpanded(true);
                        // Create edge item
                        FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                        edgeItem->setExpanded(true);
                        // Save items in myTreeItemToACMap
                        myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                        myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                        myTreeItemToACMap[edgeItem] = edge;
                        // return edge item
                        return edgeItem;
                    } else if (tagValue.hasAttribute(SUMO_ATTR_LANE)) {
                        // obtain lane parent
                        GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(additional->getAttribute(SUMO_ATTR_LANE));
                        // obtain edge parent
                        GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                        //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                        FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                        FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                        junctionDestinyItem->setExpanded(true);
                        // Create edge item
                        FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                        edgeItem->setExpanded(true);
                        // Create lane item
                        FXTreeItem* laneItem = myTreelist->insertItem(0, edgeItem, lane->getHierarchyName().c_str(), lane->getIcon(), lane->getIcon());
                        laneItem->setExpanded(true);
                        // Save items in myTreeItemToACMap
                        myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                        myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                        myTreeItemToACMap[edgeItem] = edge;
                        myTreeItemToACMap[laneItem] = lane;
                        // return lane item
                        return laneItem;
                    }
                }
            }
            return nullptr;
        }
    }
}


void
GNEFrame::ACHierarchy::showAttributeCarrierChilds(GNEAttributeCarrier *AC, FXTreeItem* itemParent) {
    // Switch gl type of ac
    switch (AC->getTag()) {
        case SUMO_TAG_JUNCTION: {
            // retrieve junction
            GNEJunction* junction = myFrameParent->getViewNet()->getNet()->retrieveJunction(AC->getID(), false);
            if(junction) {
                // insert junction item
                FXTreeItem* junctionItem = addACIntoList(AC, itemParent);
                // insert edges
                for (auto i : junction->getGNEEdges()) {
                    showAttributeCarrierChilds(i, junctionItem);
                }
                // insert crossings
                for (auto i : junction->getGNECrossings()) {
                    showAttributeCarrierChilds(i, junctionItem);
                }
            }
            break;
        }
        case SUMO_TAG_EDGE: {
            // retrieve edge
            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(AC->getID(), false);
            if(edge) {
                // insert edge item
                FXTreeItem* edgeItem = addACIntoList(AC, itemParent);
                // insert lanes
                for (int i = 0; i < (int)edge->getLanes().size(); i++) {
                    showAttributeCarrierChilds(edge->getLanes().at(i), edgeItem);
                }
                // insert additionals of edge
                for (auto i : edge->getAdditionalChilds()) {
                    showAttributeCarrierChilds(i, edgeItem);
                }
            }
            break;
        }
        case SUMO_TAG_LANE: {
            // retrieve lane
            GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(AC->getID(), false);
            if(lane) {
                // insert lane item
                FXTreeItem* laneItem = addACIntoList(AC, itemParent);
                // insert additionals of lanes
                for (auto i : lane->getAdditionalChilds()) {
                    showAttributeCarrierChilds(i, laneItem);
                }
                // insert incoming connections of lanes (by default isn't expanded)
                if (lane->getGNEIncomingConnections().size() > 0) {
                    std::vector<GNEConnection*> incomingLaneConnections = lane->getGNEIncomingConnections();
                    FXTreeItem* incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", incomingLaneConnections.front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                    myTreeItemsConnections.insert(incomingConnections);
                    incomingConnections->setExpanded(false);
                    // insert incoming connections
                    for (auto i : incomingLaneConnections) {
                        showAttributeCarrierChilds(i, incomingConnections);
                    }
                }
                // insert outcoming connections of lanes (by default isn't expanded)
                if (lane->getGNEOutcomingConnections().size() > 0) {
                    std::vector<GNEConnection*> outcomingLaneConnections = lane->getGNEOutcomingConnections();
                    FXTreeItem* outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", outcomingLaneConnections.front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                    myTreeItemsConnections.insert(outgoingConnections);
                    outgoingConnections->setExpanded(false);
                    // insert outcoming connections
                    for (auto i : outcomingLaneConnections) {
                        showAttributeCarrierChilds(i, outgoingConnections);
                    }
                }
            }
            break;
        }
        case SUMO_TAG_POI: 
        case SUMO_TAG_POLY:
        case SUMO_TAG_CROSSING:
        case SUMO_TAG_CONNECTION: {
            // insert connection item
            addACIntoList(AC, itemParent);
            break;
        }
        default: {
            // check if is an additional
            if(GNEAttributeCarrier::getTagProperties(AC->getTag()).isAdditional()) {
                // retrieve additional
                GNEAdditional *additional = myFrameParent->getViewNet()->getNet()->retrieveAdditional(AC->getTag(), AC->getID(), false);
                if(additional) {
                    // insert additional item
                    FXTreeItem* additionalItem = addACIntoList(AC, itemParent);
                    // insert additionals childs
                    for (auto i : additional->getAdditionalChilds()) {
                        showAttributeCarrierChilds(i, additionalItem);
                    }
                }
            }
            break;
        }
    }
}


FXTreeItem*
GNEFrame::ACHierarchy::addACIntoList(GNEAttributeCarrier *AC, FXTreeItem* itemParent) {
    FXTreeItem* item = myTreelist->insertItem(0, itemParent, AC->getHierarchyName().c_str(), AC->getIcon(), AC->getIcon());
    myTreeItemToACMap[item] = AC;
    item->setExpanded(true);
    return item;
}

// ---------------------------------------------------------------------------
// GNEFrame::GenericParametersEditor - methods
// ---------------------------------------------------------------------------

GNEFrame::GenericParametersEditor::GenericParametersEditor(GNEFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Generic parameters", GUIDesignGroupBoxFrame),
    myFrameParent(inspectorFrameParent) {
    // create textfield and buttons
    myGenericParameterField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEditGenericParameterButton = new FXButton(this, "Edit generic parameter", 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrame::GenericParametersEditor::~GenericParametersEditor() {}


void
GNEFrame::GenericParametersEditor::showGenericParametersEditor(GNEAttributeCarrier *AC) {
    assert(myAC);
    myAC = AC;
    show();
}


void
GNEFrame::GenericParametersEditor::hideGenericParametersEditor() {
    // hide groupbox
    hide();
}


void 
GNEFrame::GenericParametersEditor::refreshGenericParametersEditor() {
    ;
}


long 
GNEFrame::GenericParametersEditor::onCmdEditGenericParameter(FXObject*, FXSelector, void*) {

    return 1;
}


long 
GNEFrame::GenericParametersEditor::onCmdSetGenericParameter(FXObject*, FXSelector, void*) {

    return 1;
}


long 
GNEFrame::GenericParametersEditor::onCmdGenericParameterHelp(FXObject*, FXSelector, void*) {
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrame - methods
// ---------------------------------------------------------------------------

GNEFrame::GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel) :
    FXVerticalFrame(horizontalFrameParent, GUIDesignAuxiliarFrame),
    myViewNet(viewNet) {

    // Create font
    myFrameHeaderFont = new FXFont(getApp(), "Arial", 14, FXFont::Bold),

    // Create frame for header
    myHeaderFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create frame for left elements of header (By default unused)
    myHeaderLeftFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderLeftFrame->hide();

    // Create titel frame
    myFrameHeaderLabel = new FXLabel(myHeaderFrame, frameLabel.c_str(), 0, GUIDesignLabelFrameInformation);

    // Create frame for right elements of header (By default unused)
    myHeaderRightFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderRightFrame->hide();

    // Add separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);

    // Create frame for contents
    myScrollWindowsContents = new FXScrollWindow(this, GUIDesignContentsScrollWindow);

    // Create frame for contents
    myContentFrame = new FXVerticalFrame(myScrollWindowsContents, GUIDesignContentsFrame);

    // Set font of header
    myFrameHeaderLabel->setFont(myFrameHeaderFont);

    // Hide Frame
    FXVerticalFrame::hide();
}


GNEFrame::~GNEFrame() {
    delete myFrameHeaderFont;
}


void
GNEFrame::focusUpperElement() {
    myFrameHeaderLabel->setFocus();
}


void
GNEFrame::show() {
    // show scroll window
    FXVerticalFrame::show();
    // Show and update Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNEFrame::hide() {
    // hide scroll window
    FXVerticalFrame::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNEFrame::setFrameWidth(int newWidth) {
    setWidth(newWidth);
    myScrollWindowsContents->setWidth(newWidth);
}


GNEViewNet*
GNEFrame::getViewNet() const {
    return myViewNet;
}


FXLabel*
GNEFrame::getFrameHeaderLabel() const {
    return myFrameHeaderLabel;
}


FXFont*
GNEFrame::getFrameHeaderFont() const {
    return myFrameHeaderFont;
}


void
GNEFrame::openHelpAttributesDialog(SumoXMLTag elementTag) const {
    FXDialogBox *attributesHelpDialog = new FXDialogBox(myScrollWindowsContents, ("Parameters of " + toString(elementTag)).c_str(), GUIDesignDialogBoxResizable, 0, 0, 0, 0, 10, 10, 10, 38, 4, 4);
    // Create FXTable
    FXTable* myTable = new FXTable(attributesHelpDialog, attributesHelpDialog, MID_TABLE, GUIDesignTableNotEditable);
    attributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEINSPECT));
    const auto &attrs = GNEAttributeCarrier::getTagProperties(elementTag);
    int sizeColumnDescription = 0;
    int sizeColumnDefinitions = 0;
    myTable->setVisibleRows((FXint)(attrs.getNumberOfAttributes()));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(attrs.getNumberOfAttributes()), 3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Attribute");
    myTable->setColumnText(1, "Description");
    myTable->setColumnText(2, "Definition");
    myTable->getRowHeader()->setWidth(0);
    // Iterate over vector of additional parameters
    int itemIndex = 0;
    for (auto i : attrs) {
        // Set attribute 
        FXTableItem* attribute = new FXTableItem(toString(i.first).c_str());
        attribute->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(itemIndex, 0, attribute);
        // Set description of element
        FXTableItem* type = new FXTableItem("");
        type->setText(i.second.getDescription().c_str());
        sizeColumnDescription = MAX2(sizeColumnDescription, (int)i.second.getDescription().size());
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(itemIndex, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(i.second.getDefinition().c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(itemIndex, 2, definition);
        sizeColumnDefinitions = MAX2(sizeColumnDefinitions, (int)i.second.getDefinition().size());
        itemIndex++;
    }
    // set header
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, sizeColumnDescription * 7);
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, sizeColumnDefinitions * 6);
    // Create horizontal separator
    new FXHorizontalSeparator(attributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(attributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), attributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Opening HelpAttributes dialog for tag '" + toString(elementTag) + "' showing " + toString(attrs.getNumberOfAttributes()) + " attributes");
    }
    // create Dialog
    attributesHelpDialog->create();
    // show in the given position
    attributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(attributesHelpDialog);
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Closing HelpAttributes dialog for tag '" + toString(elementTag) + "'");
    }
}

/****************************************************************************/
