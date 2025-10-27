/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEElementTree.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for show hierarchical elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Children.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEElementTree.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEElementTree) HierarchicalElementTreeMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_CENTER,                     GNEElementTree::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECT,                    GNEElementTree::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETE,                     GNEElementTree::onCmdDeleteItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_ACHIERARCHY_MOVEUP,         GNEElementTree::onCmdMoveItemUp),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_ACHIERARCHY_MOVEDOWN,       GNEElementTree::onCmdMoveItemDown),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_ACHIERARCHY_SHOWCHILDMENU,  GNEElementTree::onCmdShowChildMenu)
};


// Object implementation
FXIMPLEMENT(GNEElementTree,    MFXGroupBoxModule,     HierarchicalElementTreeMap,     ARRAYNUMBER(HierarchicalElementTreeMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEElementTree::GNEElementTree(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Hierarchy")),
    myFrameParent(frameParent) {
    // Create tree list with fixed height
    myTreeListDynamic = new MFXTreeListDynamic(getCollapsableFrame(), this, MID_GNE_ACHIERARCHY_SHOWCHILDMENU, GUIDesignTreeListFixedHeight);
    hide();
}


GNEElementTree::~GNEElementTree() {}


void
GNEElementTree::showHierarchicalElementTree(GNEAttributeCarrier* AC) {
    myHierarchicalElement = AC;
    // show GNEElementTree and refresh GNEElementTree
    if (myHierarchicalElement) {
        // refresh GNEElementTree
        refreshHierarchicalElementTree();
        // show myTreeListDynamic
        myTreeListDynamic->show();
        // show module
        show();
    }
}


void
GNEElementTree::hideHierarchicalElementTree() {
    // set all pointers null
    myHierarchicalElement = nullptr;
    myClickedAC = nullptr;
    myClickedJunction = nullptr;
    myClickedEdge = nullptr;
    myClickedLane = nullptr;
    myClickedCrossing = nullptr;
    myClickedConnection = nullptr;
    myClickedAdditional = nullptr;
    myClickedTAZSourceSink = nullptr;
    myClickedDemandElement = nullptr;
    myClickedDataSet = nullptr;
    myClickedDataInterval = nullptr;
    myClickedGenericData = nullptr;
    // hide myTreeListDynamic
    myTreeListDynamic->hide();
    // hide module
    hide();
}


void
GNEElementTree::refreshHierarchicalElementTree() {
    // clear items
    myTreeListDynamic->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsConnections.clear();
    // show children of myHE
    if (myHierarchicalElement) {
        showHierarchicalElementChildren(myHierarchicalElement, showAttributeCarrierParents());
    }
}


void
GNEElementTree::removeCurrentEditedAttributeCarrier(const GNEAttributeCarrier* AC) {
    // simply check if AC is the same of myHE
    if (AC == myHierarchicalElement) {
        myHierarchicalElement = nullptr;
    }
}


long
GNEElementTree::onCmdShowChildMenu(FXObject*, FXSelector, void* eventData) {
    // obtain event
    FXEvent* e = (FXEvent*)eventData;
    // obtain FXTreeItem in the given position
    FXTreeItem* item = myTreeListDynamic->getItemAt(e->win_x, e->win_y);
    // open Pop-up if FXTreeItem has a Attribute Carrier vinculated
    if (item && (myTreeItemsConnections.find(item) == myTreeItemsConnections.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[item]);
    }
    return 1;
}


long
GNEElementTree::onCmdCenterItem(FXObject*, FXSelector, void*) {
    // Center item
    if (myClickedJunction) {
        myFrameParent->getViewNet()->centerTo(myClickedJunction->getGlID(), true, -1);
    } else if (myClickedEdge) {
        myFrameParent->getViewNet()->centerTo(myClickedEdge->getGlID(), true, -1);
    } else if (myClickedLane) {
        myFrameParent->getViewNet()->centerTo(myClickedLane->getGlID(), true, -1);
    } else if (myClickedCrossing) {
        myFrameParent->getViewNet()->centerTo(myClickedCrossing->getGlID(), true, -1);
    } else if (myClickedConnection) {
        myFrameParent->getViewNet()->centerTo(myClickedConnection->getGlID(), true, -1);
    } else if (myClickedAdditional) {
        myFrameParent->getViewNet()->centerTo(myClickedAdditional->getGlID(), true, -1);
    } else if (myClickedTAZSourceSink) {
        myFrameParent->getViewNet()->centerTo(myClickedTAZSourceSink->getParentEdges().front()->getGlID(), true, -1);
    } else if (myClickedDemandElement) {
        myFrameParent->getViewNet()->centerTo(myClickedDemandElement->getGlID(), true, -1);
    } else if (myClickedGenericData) {
        myFrameParent->getViewNet()->centerTo(myClickedGenericData->getGlID(), true, -1);
    }
    // update view after centering
    myFrameParent->getViewNet()->updateViewNet();
    return 1;
}


long
GNEElementTree::onCmdInspectItem(FXObject*, FXSelector, void*) {
    myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectElement(myClickedAC, myHierarchicalElement);
    return 1;
}


long
GNEElementTree::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    const auto& inspectedElements = myFrameParent->getViewNet()->getInspectedElements();
    // Remove Attribute Carrier
    if (myClickedJunction) {
        myFrameParent->getViewNet()->getNet()->deleteJunction(myClickedJunction, myFrameParent->getViewNet()->getUndoList());
    } else if (myClickedEdge) {
        myFrameParent->getViewNet()->getNet()->deleteEdge(myClickedEdge, myFrameParent->getViewNet()->getUndoList(), false);
    } else if (myClickedLane) {
        myFrameParent->getViewNet()->getNet()->deleteLane(myClickedLane, myFrameParent->getViewNet()->getUndoList(), false);
    } else if (myClickedCrossing) {
        myFrameParent->getViewNet()->getNet()->deleteCrossing(myClickedCrossing, myFrameParent->getViewNet()->getUndoList());
    } else if (myClickedConnection) {
        myFrameParent->getViewNet()->getNet()->deleteConnection(myClickedConnection, myFrameParent->getViewNet()->getUndoList());
    } else if (myClickedAdditional) {
        myFrameParent->getViewNet()->getNet()->deleteAdditional(myClickedAdditional, myFrameParent->getViewNet()->getUndoList());
    } else if (myClickedTAZSourceSink) {
        myFrameParent->getViewNet()->getNet()->deleteTAZSourceSink(myClickedTAZSourceSink, myFrameParent->getViewNet()->getUndoList());
    } else if (myClickedDemandElement) {
        // check that default VTypes aren't removed
        if ((myClickedDemandElement->getTagProperty()->getTag() == SUMO_TAG_VTYPE) && (GNEAttributeCarrier::parse<bool>(myClickedDemandElement->getAttribute(GNE_ATTR_DEFAULT_VTYPE)))) {
            WRITE_WARNINGF(TL("Default Vehicle Type '%' cannot be removed"), myClickedDemandElement->getAttribute(SUMO_ATTR_ID));
            return 1;
        } else if (myClickedDemandElement->getTagProperty()->isPlan()) {
            // we need to check if we're removing the last person plan of a person.
            auto planParent = myClickedDemandElement->getParentDemandElements().front();
            if (planParent->getChildDemandElements().size() == 1) {
                myFrameParent->getViewNet()->getNet()->deleteDemandElement(planParent, myFrameParent->getViewNet()->getUndoList());
            } else {
                myFrameParent->getViewNet()->getNet()->deleteDemandElement(myClickedDemandElement, myFrameParent->getViewNet()->getUndoList());
            }
        }
    } else if (myClickedDataSet) {
        myFrameParent->getViewNet()->getNet()->deleteDataSet(myClickedDataSet, myFrameParent->getViewNet()->getUndoList());
    } else if (myClickedDataInterval) {
        // check if we have to remove data Set
        if (myClickedDataInterval->getDataSetParent()->getDataIntervalChildren().size() == 1) {
            myFrameParent->getViewNet()->getNet()->deleteDataSet(myClickedDataInterval->getDataSetParent(), myFrameParent->getViewNet()->getUndoList());
        } else {
            myFrameParent->getViewNet()->getNet()->deleteDataInterval(myClickedDataInterval, myFrameParent->getViewNet()->getUndoList());
        }
    } else if (myClickedGenericData) {
        // check if we have to remove interval
        if (myClickedGenericData->getDataIntervalParent()->getGenericDataChildren().size() == 1) {
            // check if we have to remove data Set
            if (myClickedGenericData->getDataIntervalParent()->getDataSetParent()->getDataIntervalChildren().size() == 1) {
                myFrameParent->getViewNet()->getNet()->deleteDataSet(myClickedGenericData->getDataIntervalParent()->getDataSetParent(), myFrameParent->getViewNet()->getUndoList());
            } else {
                myFrameParent->getViewNet()->getNet()->deleteDataInterval(myClickedGenericData->getDataIntervalParent(), myFrameParent->getViewNet()->getUndoList());
            }
        } else {
            myFrameParent->getViewNet()->getNet()->deleteGenericData(myClickedGenericData, myFrameParent->getViewNet()->getUndoList());
        }
    }
    // refresh AC Hierarchy
    refreshHierarchicalElementTree();
    // check if inspector frame has to be shown again
    if (inspectedElements.isInspectingSingleElement()) {
        if (inspectedElements.getFirstAC() != myClickedAC) {
            myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectElement(inspectedElements.getFirstAC());
        } else {
            // inspect a nullptr element to reset inspector frame
            myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->clearInspection();
        }
    }
    return 1;
}


long
GNEElementTree::onCmdMoveItemUp(FXObject*, FXSelector, void*) {
    // currently only children of demand elements can be moved
    if (myClickedDemandElement) {
        myFrameParent->getViewNet()->getUndoList()->begin(myClickedDemandElement, ("moving up " + myClickedDemandElement->getTagStr()).c_str());
        // move element one position back
        myFrameParent->getViewNet()->getUndoList()->add(new GNEChange_Children(myClickedDemandElement->getParentDemandElements().at(0), myClickedDemandElement,
                GNEChange_Children::Operation::MOVE_BACK), true);
        myFrameParent->getViewNet()->getUndoList()->end();
    }
    // refresh after moving child
    refreshHierarchicalElementTree();
    return 1;
}


long
GNEElementTree::onCmdMoveItemDown(FXObject*, FXSelector, void*) {
    // currently only children of demand elements can be moved
    if (myClickedDemandElement) {
        myFrameParent->getViewNet()->getUndoList()->begin(myClickedDemandElement, ("moving down " + myClickedDemandElement->getTagStr()).c_str());
        // move element one position front
        myFrameParent->getViewNet()->getUndoList()->add(new GNEChange_Children(myClickedDemandElement->getParentDemandElements().at(0), myClickedDemandElement,
                GNEChange_Children::Operation::MOVE_FRONT), true);
        myFrameParent->getViewNet()->getUndoList()->end();
    }
    // refresh after moving child
    refreshHierarchicalElementTree();
    return 1;
}


void
GNEElementTree::createPopUpMenu(int X, int Y, GNEAttributeCarrier* clickedAC) {
    // get attributeCarriers
    const auto& attributeCarriers = myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // first check that AC exist
    if (clickedAC) {
        // set current clicked AC
        myClickedAC = clickedAC;
        // cast all elements
        myClickedJunction = attributeCarriers->retrieveJunction(clickedAC->getID(), false);
        myClickedEdge = attributeCarriers->retrieveEdge(clickedAC->getID(), false);
        myClickedLane = attributeCarriers->retrieveLane(clickedAC->getGUIGlObject(), false);
        myClickedCrossing = attributeCarriers->retrieveCrossing(clickedAC->getGUIGlObject(), false);
        myClickedConnection = attributeCarriers->retrieveConnection(clickedAC->getGUIGlObject(), false);
        myClickedAdditional = attributeCarriers->retrieveAdditional(clickedAC->getGUIGlObject(), false);
        myClickedTAZSourceSink = attributeCarriers->retrieveTAZSourceSink(clickedAC, false);
        myClickedDemandElement = attributeCarriers->retrieveDemandElement(clickedAC->getGUIGlObject(), false);
        myClickedDataSet = attributeCarriers->retrieveDataSet(clickedAC->getID(), false);
        myClickedDataInterval = attributeCarriers->retrieveDataInterval(clickedAC, false);
        myClickedGenericData = attributeCarriers->retrieveGenericData(clickedAC->getGUIGlObject(), false);
        // create FXMenuPane
        FXMenuPane* pane = new FXMenuPane(myTreeListDynamic->getFXWindow());
        // set item name and icon
        new MFXMenuHeader(pane, myFrameParent->getViewNet()->getViewParent()->getGUIMainWindow()->getBoldFont(), myClickedAC->getPopUpID().c_str(), myClickedAC->getACIcon());
        // add extra info for TAZ Source Sinks
        if (myClickedTAZSourceSink) {
            new FXMenuSeparator(pane);
            GUIDesigns::buildFXMenuCommand(pane, TLF("Edge: %", myClickedTAZSourceSink->getParentEdges().front()->getID()), nullptr, nullptr, 0);
        }
        // insert separator
        new FXMenuSeparator(pane);
        // create center menu command
        FXMenuCommand* centerMenuCommand = GUIDesigns::buildFXMenuCommand(pane, TL("Center"), GUIIconSubSys::getIcon(GUIIcon::RECENTERVIEW), this, MID_GNE_CENTER);
        // disable Centering for Vehicle Types, data sets and data intervals
        if (myClickedAC->getTagProperty()->isType() || myClickedAC->getTagProperty()->isTypeDist() || myClickedAC->getTagProperty()->isDistributionReference() ||
                (myClickedAC->getTagProperty()->getTag() == SUMO_TAG_DATASET) || (myClickedAC->getTagProperty()->getTag() == SUMO_TAG_DATAINTERVAL)) {
            centerMenuCommand->disable();
        }
        // create inspect and delete menu commands
        FXMenuCommand* inspectMenuCommand = GUIDesigns::buildFXMenuCommand(pane, TL("Inspect"), GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), this, MID_GNE_INSPECT);
        FXMenuCommand* deleteMenuCommand = GUIDesigns::buildFXMenuCommand(pane, TL("Delete"), GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE);
        // check if inspect and delete menu commands has to be disabled
        if (!isSupermodeValid(myClickedAC)) {
            inspectMenuCommand->disable();
            deleteMenuCommand->disable();
        }
        // now check if given AC support manually moving of their item up and down (Currently only for certain demand elements)
        /* if (myClickedDemandElement && myClickedAC->getTagProperty()->canBeSortedManually()) {
            // insert separator
            new FXMenuSeparator(pane);
            // create both moving menu commands
            FXMenuCommand* moveUpMenuCommand = GUIDesigns::buildFXMenuCommand(pane, "Move up", GUIIconSubSys::getIcon(GUIIcon::ARROW_UP), this, MID_GNE_ACHIERARCHY_MOVEUP);
            FXMenuCommand* moveDownMenuCommand = GUIDesigns::buildFXMenuCommand(pane, "Move down", GUIIconSubSys::getIcon(GUIIcon::ARROW_DOWN), this, MID_GNE_ACHIERARCHY_MOVEDOWN);
            // check if both commands has to be disabled
            if (myClickedDemandElement->getTagProperty()->isPlanStopPerson()) {
                moveUpMenuCommand->setText(TL("Move up (Stops cannot be moved)"));
                moveDownMenuCommand->setText(TL("Move down (Stops cannot be moved)"));
                moveUpMenuCommand->disable();
                moveDownMenuCommand->disable();
            } else {
                // check if moveUpMenuCommand has to be disabled
                if (myClickedDemandElement->getParentDemandElements().front()->getChildDemandElements().front() == myClickedDemandElement) {
                    moveUpMenuCommand->setText(TL("Move up (It's already the first element)"));
                    moveUpMenuCommand->disable();
                } else if (myClickedDemandElement->getParentDemandElements().front()->getPreviousChildDemandElement(myClickedDemandElement)->getTagProperty()->isPlanStopPerson()) {
                    moveUpMenuCommand->setText(TL("Move up (Previous element is a Stop)"));
                    moveUpMenuCommand->disable();
                }
                // check if moveDownMenuCommand has to be disabled
                if (myClickedDemandElement->getParentDemandElements().front()->getChildDemandElements().back() == myClickedDemandElement) {
                    moveDownMenuCommand->setText(TL("Move down (It's already the last element)"));
                    moveDownMenuCommand->disable();
                } else if (myClickedDemandElement->getParentDemandElements().front()->getNextChildDemandElement(myClickedDemandElement)->getTagProperty()->isPlanStopPerson()) {
                    moveDownMenuCommand->setText(TL("Move down (Next element is a Stop)"));
                    moveDownMenuCommand->disable();
                }
            }
        } */
        // Center in the mouse position and create pane
        pane->setX(X);
        pane->setY(Y);
        pane->create();
        pane->show();
    } else {
        // set all clicked elements to null
        myClickedAC = nullptr;
        myClickedJunction = nullptr;
        myClickedEdge = nullptr;
        myClickedLane = nullptr;
        myClickedCrossing = nullptr;
        myClickedConnection = nullptr;
        myClickedAdditional = nullptr;
        myClickedTAZSourceSink = nullptr;
        myClickedDemandElement = nullptr;
        myClickedDataSet = nullptr;
        myClickedDataInterval = nullptr;
        myClickedGenericData = nullptr;
    }
}


FXTreeItem*
GNEElementTree::showAttributeCarrierParents() {
    // get attributeCarriers
    const auto& attributeCarriers = myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // check tags
    if (myHierarchicalElement->getTagProperty()->isNetworkElement()) {
        // check demand element type
        switch (myHierarchicalElement->getTagProperty()->getTag()) {
            case SUMO_TAG_EDGE: {
                // obtain Edge
                GNEEdge* edge = attributeCarriers->retrieveEdge(myHierarchicalElement->getID(), false);
                if (edge == nullptr) {
                    return nullptr;
                } else {
                    // insert Junctions of edge in tree (Parallel because an edge has always two Junctions)
                    FXTreeItem* junctionSourceItem = myTreeListDynamic->appendItem(nullptr, (edge->getFromJunction()->getHierarchyName() + TL(" origin")).c_str(), edge->getFromJunction()->getACIcon());
                    FXTreeItem* junctionDestinationItem = myTreeListDynamic->appendItem(nullptr, (edge->getFromJunction()->getHierarchyName() + TL(" destination")).c_str(), edge->getFromJunction()->getACIcon());
                    junctionDestinationItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[junctionSourceItem] = edge->getFromJunction();
                    myTreeItemToACMap[junctionDestinationItem] = edge->getToJunction();
                    // return junction destination Item
                    return junctionDestinationItem;
                }
            }
            case SUMO_TAG_LANE: {
                // obtain lane
                GNELane* lane = attributeCarriers->retrieveLane(myHierarchicalElement->getID(), false);
                if (lane == nullptr) {
                    return nullptr;
                } else {
                    // obtain parent edge
                    GNEEdge* edge = attributeCarriers->retrieveEdge(lane->getParentEdge()->getID());
                    //insert Junctions of lane of edge in tree (Parallel because an edge has always two Junctions)
                    FXTreeItem* junctionSourceItem = myTreeListDynamic->appendItem(nullptr, (edge->getFromJunction()->getHierarchyName() + TL(" origin")).c_str(), edge->getFromJunction()->getACIcon());
                    FXTreeItem* junctionDestinationItem = myTreeListDynamic->appendItem(nullptr, (edge->getFromJunction()->getHierarchyName() + TL(" destination")).c_str(), edge->getFromJunction()->getACIcon());
                    junctionDestinationItem->setExpanded(true);
                    // Create edge item
                    FXTreeItem* edgeItem = myTreeListDynamic->appendItem(junctionDestinationItem, edge->getHierarchyName().c_str(), edge->getACIcon());
                    edgeItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[junctionSourceItem] = edge->getFromJunction();
                    myTreeItemToACMap[junctionDestinationItem] = edge->getToJunction();
                    myTreeItemToACMap[edgeItem] = edge;
                    // return edge item
                    return edgeItem;
                }
            }
            case SUMO_TAG_CROSSING: {
                // obtain crossing parent junction
                auto crossing = attributeCarriers->retrieveCrossing(myHierarchicalElement->getGUIGlObject(), false);
                if (crossing == nullptr) {
                    return nullptr;
                } else {
                    GNEJunction* junction = crossing->getParentJunctions().front();
                    // create junction item
                    FXTreeItem* junctionItem = myTreeListDynamic->appendItem(nullptr, junction->getHierarchyName().c_str(), junction->getACIcon());
                    junctionItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[junctionItem] = junction;
                    // return junction Item
                    return junctionItem;
                }
            }
            case SUMO_TAG_CONNECTION: {
                // obtain Connection
                GNEConnection* connection = attributeCarriers->retrieveConnection(myHierarchicalElement->getID(), false);
                if (connection == nullptr) {
                    return nullptr;
                } else {
                    // create edge from item
                    FXTreeItem* edgeFromItem = myTreeListDynamic->appendItem(nullptr, connection->getEdgeFrom()->getHierarchyName().c_str(), connection->getEdgeFrom()->getACIcon());
                    edgeFromItem->setExpanded(true);
                    // create edge to item
                    FXTreeItem* edgeToItem = myTreeListDynamic->appendItem(nullptr, connection->getEdgeTo()->getHierarchyName().c_str(), connection->getEdgeTo()->getACIcon());
                    edgeToItem->setExpanded(true);
                    // create connection item
                    FXTreeItem* connectionItem = myTreeListDynamic->appendItem(edgeToItem, connection->getHierarchyName().c_str(), connection->getACIcon());
                    connectionItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[edgeFromItem] = connection->getEdgeFrom();
                    myTreeItemToACMap[edgeToItem] = connection->getEdgeTo();
                    myTreeItemToACMap[connectionItem] = connection;
                    // return connection item
                    return connectionItem;
                }
            }
            default:
                break;
        }
    } else if (myHierarchicalElement->getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // Obtain POILane
        const auto* POILane = attributeCarriers->retrieveAdditional(myHierarchicalElement->getGUIGlObject(), false);
        if (POILane == nullptr) {
            return nullptr;
        } else {
            // obtain parent lane
            GNELane* lane = attributeCarriers->retrieveLane(POILane->getParentLanes().at(0)->getID());
            // obtain parent edge
            GNEEdge* edge = attributeCarriers->retrieveEdge(lane->getParentEdge()->getID());
            //insert Junctions of lane of edge in tree (Parallel because an edge has always two Junctions)
            FXTreeItem* junctionSourceItem = myTreeListDynamic->appendItem(nullptr, (edge->getFromJunction()->getHierarchyName() + TL(" origin")).c_str(), edge->getFromJunction()->getACIcon());
            FXTreeItem* junctionDestinationItem = myTreeListDynamic->appendItem(nullptr, (edge->getFromJunction()->getHierarchyName() + TL(" destination")).c_str(), edge->getFromJunction()->getACIcon());
            junctionDestinationItem->setExpanded(true);
            // Create edge item
            FXTreeItem* edgeItem = myTreeListDynamic->appendItem(junctionDestinationItem, edge->getHierarchyName().c_str(), edge->getACIcon());
            edgeItem->setExpanded(true);
            // Create lane item
            FXTreeItem* laneItem = myTreeListDynamic->appendItem(edgeItem, lane->getHierarchyName().c_str(), lane->getACIcon());
            laneItem->setExpanded(true);
            // Save items in myTreeItemToACMap
            myTreeItemToACMap[junctionSourceItem] = edge->getFromJunction();
            myTreeItemToACMap[junctionDestinationItem] = edge->getToJunction();
            myTreeItemToACMap[edgeItem] = edge;
            myTreeItemToACMap[laneItem] = lane;
            // return Lane item
            return laneItem;
        }
    } else if (myHierarchicalElement->getTagProperty()->isAdditionalElement()) {
        // Obtain Additional
        const GNEAdditional* additional = attributeCarriers->retrieveAdditional(myHierarchicalElement->getGUIGlObject(), false);
        if (additional == nullptr) {
            return nullptr;
        } else {
            // declare auxiliary FXTreeItem, due a demand element can have multiple "roots"
            FXTreeItem* root = nullptr;
            // check if there is demand elements parents
            if (additional->getParentAdditionals().size() > 0) {
                // check if we have more than one edge
                if (additional->getParentAdditionals().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentAdditionals().front());
                    // insert "spacer"
                    if (additional->getParentAdditionals().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentAdditionals().size() - 2) + TL(" additionals...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentAdditionals().back());
            }
            // check if there is parent demand elements
            if (additional->getParentDemandElements().size() > 0) {
                // check if we have more than one demand element
                if (additional->getParentDemandElements().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentDemandElements().front());
                    // insert "spacer"
                    if (additional->getParentDemandElements().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentDemandElements().size() - 2) + TL(" demand elements...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentDemandElements().back());
            }
            // check if there is parent edges
            if (additional->getParentEdges().size() > 0) {
                // check if we have more than one edge
                if (additional->getParentEdges().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentEdges().front());
                    // insert "spacer"
                    if (additional->getParentEdges().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentEdges().size() - 2) + TL(" edges...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentEdges().back());
            }
            // check if there is parent lanes
            if (additional->getParentLanes().size() > 0) {
                // check if we have more than one parent lane
                if (additional->getParentLanes().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentLanes().front());
                    // insert "spacer"
                    if (additional->getParentLanes().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentLanes().size() - 2) + TL(" lanes...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentLanes().back());
            }
            // return last inserted list item
            return root;
        }
    } else if (myHierarchicalElement->getTagProperty()->isTAZElement()) {
        // Obtain TAZElement
        const GNEAdditional* TAZElement = attributeCarriers->retrieveAdditional(myHierarchicalElement->getGUIGlObject(), false);
        if (TAZElement == nullptr) {
            return nullptr;
        } else {
            // declare auxiliary FXTreeItem, due a demand element can have multiple "roots"
            FXTreeItem* root = nullptr;
            // check if there is demand elements parents
            if (TAZElement->getParentAdditionals().size() > 0) {
                // check if we have more than one edge
                if (TAZElement->getParentAdditionals().size() > 1) {
                    // insert first item
                    addListItem(TAZElement->getParentAdditionals().front());
                    // insert "spacer"
                    if (TAZElement->getParentAdditionals().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)TAZElement->getParentAdditionals().size() - 2) + TL(" TAZElements...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(TAZElement->getParentAdditionals().back());
            }
            // check if there is parent demand elements
            if (TAZElement->getParentDemandElements().size() > 0) {
                // check if we have more than one demand element
                if (TAZElement->getParentDemandElements().size() > 1) {
                    // insert first item
                    addListItem(TAZElement->getParentDemandElements().front());
                    // insert "spacer"
                    if (TAZElement->getParentDemandElements().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)TAZElement->getParentDemandElements().size() - 2) + TL(" demand elements...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(TAZElement->getParentDemandElements().back());
            }
            // check if there is parent edges
            if (TAZElement->getParentEdges().size() > 0) {
                // check if we have more than one edge
                if (TAZElement->getParentEdges().size() > 1) {
                    // insert first item
                    addListItem(TAZElement->getParentEdges().front());
                    // insert "spacer"
                    if (TAZElement->getParentEdges().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)TAZElement->getParentEdges().size() - 2) + TL(" edges...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(TAZElement->getParentEdges().back());
            }
            // check if there is parent lanes
            if (TAZElement->getParentLanes().size() > 0) {
                // check if we have more than one parent lane
                if (TAZElement->getParentLanes().size() > 1) {
                    // insert first item
                    addListItem(TAZElement->getParentLanes().front());
                    // insert "spacer"
                    if (TAZElement->getParentLanes().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)TAZElement->getParentLanes().size() - 2) + TL(" lanes...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(TAZElement->getParentLanes().back());
            }
            // return last inserted list item
            return root;
        }
    } else if (myHierarchicalElement->getTagProperty()->isDemandElement()) {
        // Obtain DemandElement
        GNEDemandElement* demandElement = attributeCarriers->retrieveDemandElement(myHierarchicalElement->getGUIGlObject(), false);
        if (demandElement == nullptr) {
            return nullptr;
        } else {
            // declare auxiliar FXTreeItem, due a demand element can have multiple "roots"
            FXTreeItem* root = nullptr;
            // check if there are demand element parents
            if (demandElement->getParentAdditionals().size() > 0) {
                // check if we have more than one edge
                if (demandElement->getParentAdditionals().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentAdditionals().front());
                    // insert "spacer"
                    if (demandElement->getParentAdditionals().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentAdditionals().size() - 2) + TL(" additionals...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentAdditionals().back());
            }
            // check if there is parent demand elements
            if (demandElement->getParentDemandElements().size() > 0) {
                // check if we have more than one demand element
                if (demandElement->getParentDemandElements().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentDemandElements().front());
                    // insert "spacer"
                    if (demandElement->getParentDemandElements().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentDemandElements().size() - 2) + TL(" demand elements...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentDemandElements().back());
            }
            // check if there is parent edges
            if (demandElement->getParentEdges().size() > 0) {
                // check if we have more than one edge
                if (demandElement->getParentEdges().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentEdges().front());
                    // insert "spacer"
                    if (demandElement->getParentEdges().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentEdges().size() - 2) + TL(" edges...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentEdges().back());
            }
            // check if there is parent lanes
            if (demandElement->getParentLanes().size() > 0) {
                // check if we have more than one parent lane
                if (demandElement->getParentLanes().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentLanes().front());
                    // insert "spacer"
                    if (demandElement->getParentLanes().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentLanes().size() - 2) + TL(" lanes...")).c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentLanes().back());
            }
            // return last inserted list item
            return root;
        }
    } else if (myHierarchicalElement->getTagProperty()->isDataElement()) {
        // check if is a GNEDataInterval or a GNEGenericData
        if (myHierarchicalElement->getTagProperty()->getTag() == SUMO_TAG_DATASET) {
            return nullptr;
        } else if (myHierarchicalElement->getTagProperty()->getTag() == SUMO_TAG_DATAINTERVAL) {
            auto dataInterval = attributeCarriers->retrieveDataInterval(myHierarchicalElement, false);
            if (dataInterval == nullptr) {
                return nullptr;
            } else {
                return addListItem(dataInterval);
            }
        } else {
            // Obtain DataElement
            GNEGenericData* dataElement = dynamic_cast<GNEGenericData*>(myHierarchicalElement);
            if (dataElement == nullptr) {
                return nullptr;
            } else {
                // declare auxiliary FXTreeItem, due a data element can have multiple "roots"
                FXTreeItem* root = nullptr;
                // set dataset
                addListItem(dataElement->getDataIntervalParent()->getDataSetParent());
                // set data interval
                addListItem(dataElement->getDataIntervalParent());
                // check if there is data elements parents
                if (dataElement->getParentAdditionals().size() > 0) {
                    // check if we have more than one edge
                    if (dataElement->getParentAdditionals().size() > 1) {
                        // insert first item
                        addListItem(dataElement->getParentAdditionals().front());
                        // insert "spacer"
                        if (dataElement->getParentAdditionals().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentAdditionals().size() - 2) + TL(" additionals...")).c_str(), 0, false);
                        }
                    }
                    // return last inserted item
                    root = addListItem(dataElement->getParentAdditionals().back());
                }
                // check if there is parent demand elements
                if (dataElement->getParentDemandElements().size() > 0) {
                    // check if we have more than one demand element
                    if (dataElement->getParentDemandElements().size() > 1) {
                        // insert first item
                        addListItem(dataElement->getParentDemandElements().front());
                        // insert "spacer"
                        if (dataElement->getParentDemandElements().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentDemandElements().size() - 2) + TL(" demand elements...")).c_str(), 0, false);
                        }
                    }
                    // return last inserted item
                    root = addListItem(dataElement->getParentDemandElements().back());
                }
                // check if there is parent edges
                if (dataElement->getParentEdges().size() > 0) {
                    // check if we have more than one edge
                    if (dataElement->getParentEdges().size() > 1) {
                        // insert first ege
                        if (dataElement->getTagProperty()->getTag() == SUMO_TAG_EDGEREL) {
                            addListItem(dataElement->getParentEdges().front(), nullptr, "from ");
                        } else {
                            addListItem(dataElement->getParentEdges().front());
                        }
                        // insert "spacer"
                        if (dataElement->getParentEdges().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentEdges().size() - 2) + TL(" edges...")).c_str(), 0, false);
                        }
                    }
                    // insert last ege
                    if (dataElement->getTagProperty()->getTag() == SUMO_TAG_EDGEREL) {
                        addListItem(dataElement->getParentEdges().back(), nullptr, "to ");
                    } else {
                        addListItem(dataElement->getParentEdges().back());
                    }
                }
                // check if there is parent lanes
                if (dataElement->getParentLanes().size() > 0) {
                    // check if we have more than one parent lane
                    if (dataElement->getParentLanes().size() > 1) {
                        // insert first item
                        addListItem(dataElement->getParentLanes().front());
                        // insert "spacer"
                        if (dataElement->getParentLanes().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentLanes().size() - 2) + TL(" lanes...")).c_str(), 0, false);
                        }
                    }
                    // return last inserted item
                    root = addListItem(dataElement->getParentLanes().back());
                }
                // return last inserted list item
                return root;
            }
        }
    }
    // no parents
    return nullptr;
}


void
GNEElementTree::showHierarchicalElementChildren(GNEAttributeCarrier* hierarchicalElement, FXTreeItem* itemParent) {
    // get attributeCarriers
    const auto& attributeCarriers = myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // create item
    FXTreeItem* item = addListItem(hierarchicalElement, itemParent);
    // junctions
    for (const auto& junction : hierarchicalElement->getHierarchicalElement()->getChildJunctions()) {
        showHierarchicalElementChildren(junction, item);
    }
    // edges
    for (const auto& edge : hierarchicalElement->getHierarchicalElement()->getChildEdges()) {
        showHierarchicalElementChildren(edge, item);
    }
    // lanes
    for (const auto& lane : hierarchicalElement->getHierarchicalElement()->getChildLanes()) {
        showHierarchicalElementChildren(lane, item);
    }
    // crossings
    if (hierarchicalElement->getTagProperty()->getTag() == SUMO_TAG_JUNCTION) {
        // retrieve junction
        GNEJunction* junction = attributeCarriers->retrieveJunction(hierarchicalElement->getID(), false);
        if (junction) {
            // insert crossings
            for (const auto& crossing : junction->getGNECrossings()) {
                showHierarchicalElementChildren(crossing, item);
            }
        }
    }
    // connections
    if (hierarchicalElement->getTagProperty()->getTag() == SUMO_TAG_LANE) {
        // retrieve lane
        GNELane* lane = attributeCarriers->retrieveLane(hierarchicalElement->getID(), false);
        if (lane) {
            // insert incoming connections of lanes (by default isn't expanded)
            if (lane->getGNEIncomingConnections().size() > 0) {
                std::vector<GNEConnection*> incomingLaneConnections = lane->getGNEIncomingConnections();
                // insert intermediate list item
                FXTreeItem* incomingConnections = addListItem(item, TL("Incomings"), incomingLaneConnections.front()->getACIcon(), false);
                // insert incoming connections
                for (const auto& connection : incomingLaneConnections) {
                    showHierarchicalElementChildren(connection, incomingConnections);
                }
            }
            // insert outcoming connections of lanes (by default isn't expanded)
            if (lane->getGNEOutcomingConnections().size() > 0) {
                std::vector<GNEConnection*> outcomingLaneConnections = lane->getGNEOutcomingConnections();
                // insert intermediate list item
                FXTreeItem* outgoingConnections = addListItem(item, TL("Outgoing"), outcomingLaneConnections.front()->getACIcon(), false);
                // insert outcoming connections
                for (const auto& connection : outcomingLaneConnections) {
                    showHierarchicalElementChildren(connection, outgoingConnections);
                }
            }
        }
    }
    // additionals
    for (const auto& additional : hierarchicalElement->getHierarchicalElement()->getChildAdditionals()) {
        if (!additional->getTagProperty()->isSymbol()) {
            showHierarchicalElementChildren(additional, item);
        }
    }
    // additionals symbols
    for (const auto& additional : hierarchicalElement->getHierarchicalElement()->getChildAdditionals()) {
        if (additional->getTagProperty()->isSymbol()) {
            showHierarchicalElementChildren(additional, item);
        }
    }
    // TAZ SourceSinks (avoiding show a high number)
    if (hierarchicalElement->getHierarchicalElement()->getChildTAZSourceSinks().size() > 20) {
        addListItem(item, TLF("SourceSinks (%)", toString(hierarchicalElement->getHierarchicalElement()->getChildTAZSourceSinks().size())), GUIIconSubSys::getIcon(GUIIcon::TAZ), false);
    } else {
        // show source and sinks separated
        for (const auto& TAZSource : hierarchicalElement->getHierarchicalElement()->getChildTAZSourceSinks()) {
            if (TAZSource->getTagProperty()->getTag() == SUMO_TAG_TAZSOURCE) {
                showHierarchicalElementChildren(TAZSource, item);
            }
        }
        for (const auto& TAZSink : hierarchicalElement->getHierarchicalElement()->getChildTAZSourceSinks()) {
            if (TAZSink->getTagProperty()->getTag() == SUMO_TAG_TAZSINK) {
                showHierarchicalElementChildren(TAZSink, item);
            }
        }
    }
    // insert child demand elements
    for (const auto& demandElement : hierarchicalElement->getHierarchicalElement()->getChildDemandElements()) {
        showHierarchicalElementChildren(demandElement, item);
    }
    // insert child data elements
    if (hierarchicalElement->getHierarchicalElement()->getChildGenericDatas().size() > 0) {
        // insert intermediate list item
        FXTreeItem* dataElements = addListItem(item, TL("Data elements"), GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), false);
        for (const auto& genericDatas : hierarchicalElement->getHierarchicalElement()->getChildGenericDatas()) {
            showHierarchicalElementChildren(genericDatas, dataElements);
        }
    }
    // data sets
    if (hierarchicalElement->getTagProperty()->getTag() == SUMO_TAG_DATASET) {
        GNEDataSet* dataSet = attributeCarriers->retrieveDataSet(hierarchicalElement->getID(), false);
        if (dataSet) {
            // iterate over intervals
            for (const auto& interval : dataSet->getDataIntervalChildren()) {
                showHierarchicalElementChildren(interval.second, item);
            }
        }
    }
    // data interval
    if (hierarchicalElement->getTagProperty()->getTag() == SUMO_TAG_DATAINTERVAL) {
        auto dataInterval = attributeCarriers->retrieveDataInterval(hierarchicalElement, false);
        if (dataInterval) {
            // iterate over generic datas
            for (const auto& genericData : dataInterval->getGenericDataChildren()) {
                showHierarchicalElementChildren(genericData, item);
            }
        }
    } else if (hierarchicalElement->getHierarchicalElement()->getChildGenericDatas().size() > 0) {
        // insert intermediate list item
        FXTreeItem* dataElements = addListItem(item, TL("Data elements"), GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), false);
        for (const auto& genericDatas : hierarchicalElement->getHierarchicalElement()->getChildGenericDatas()) {
            showHierarchicalElementChildren(genericDatas, dataElements);
        }
    }
}


FXTreeItem*
GNEElementTree::addListItem(GNEAttributeCarrier* AC, FXTreeItem* itemParent, std::string prefix, std::string sufix) {
    if (AC) {
        // insert item in Tree list
        FXTreeItem* item = myTreeListDynamic->appendItem(itemParent, (prefix + AC->getHierarchyName() + sufix).c_str(), AC->getACIcon());
        // insert item in map
        myTreeItemToACMap[item] = AC;
        // by default item is expanded
        item->setExpanded(true);
        // return created FXTreeItem
        return item;
    } else {
        return nullptr;
    }
}


FXTreeItem*
GNEElementTree::addListItem(FXTreeItem* itemParent, const std::string& text, FXIcon* icon, bool expanded) {
    if (itemParent) {
        // insert item in Tree list
        FXTreeItem* item = myTreeListDynamic->appendItem(itemParent, text.c_str(), icon);
        // expand item depending of flag expanded
        item->setExpanded(expanded);
        // return created FXTreeItem
        return item;
    } else {
        return nullptr;
    }
}


bool
GNEElementTree::isSupermodeValid(const GNEAttributeCarrier* AC) const {
    const auto& editModes = myFrameParent->getViewNet()->getEditModes();
    const auto tagProperty = AC->getTagProperty();
    if (editModes.isCurrentSupermodeNetwork()) {
        if (tagProperty->isNetworkElement() || tagProperty->isAdditionalElement()) {
            return true;
        } else if ((tagProperty->getTag() == SUMO_TAG_TAZSOURCE) || (tagProperty->getTag() == SUMO_TAG_TAZSINK)) {
            return true;
        } else {
            return false;
        }
    } else if (editModes.isCurrentSupermodeDemand() &&
               tagProperty->isDemandElement()) {
        return true;
    } else if (editModes.isCurrentSupermodeData() &&
               (tagProperty->isDataElement() || tagProperty->isMeanData())) {
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
