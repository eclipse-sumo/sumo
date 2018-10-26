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
#include <utils/common/StringTokenizer.h>
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
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/dialogs/GNEGenericParameterDialog.h>

#include "GNEFrame.h"
#include "GNEInspectorFrame.h"
#include "GNEFrame.h"
#include "GNEDeleteFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrame::ItemSelector) ItemSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEFrame::ItemSelector::onCmdSelectItem),
};

FXDEFMAP(GNEFrame::ACHierarchy) ACHierarchyMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_CENTER,      GNEFrame::ACHierarchy::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_INSPECT,     GNEFrame::ACHierarchy::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_DELETE,      GNEFrame::ACHierarchy::onCmdDeleteItem),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_DELETEFRAME_CHILDS,         GNEFrame::ACHierarchy::onCmdShowChildMenu),
};

FXDEFMAP(GNEFrame::GenericParametersEditor) GenericParametersEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrame::GenericParametersEditor::onCmdEditGenericParameter),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrame::GenericParametersEditor::onCmdSetGenericParameter),
};

FXDEFMAP(GNEFrame::DrawingShape) DrawingShapeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STARTDRAWING,   GNEFrame::DrawingShape::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STOPDRAWING,    GNEFrame::DrawingShape::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORTDRAWING,   GNEFrame::DrawingShape::onCmdAbortDrawing),
};

FXDEFMAP(GNEFrame::NeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEFrame::NeteditAttributes::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEFrame::NeteditAttributes::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNEFrame::ItemSelector,             FXGroupBox,     ItemSelectorMap,                ARRAYNUMBER(ItemSelectorMap))
FXIMPLEMENT(GNEFrame::ACHierarchy,              FXGroupBox,     ACHierarchyMap,                 ARRAYNUMBER(ACHierarchyMap))
FXIMPLEMENT(GNEFrame::GenericParametersEditor,  FXGroupBox,     GenericParametersEditorMap,     ARRAYNUMBER(GenericParametersEditorMap))
FXIMPLEMENT(GNEFrame::DrawingShape,             FXGroupBox,     DrawingShapeMap,                ARRAYNUMBER(DrawingShapeMap))
FXIMPLEMENT(GNEFrame::NeteditAttributes,        FXGroupBox,     NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrame::ItemSelector - methods
// ---------------------------------------------------------------------------

GNEFrame::ItemSelector::ItemSelector(GNEFrame* frameParent, GNEAttributeCarrier::TAGProperty type, bool onlyDrawables) :
    FXGroupBox(frameParent->myContentFrame, "Element", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myTagPropertyType(type),
    myOnlyDrawables(onlyDrawables),
    myCurrentType(SUMO_TAG_NOTHING) {
    // first check that property is valid
    switch (type)     {
        case GNEAttributeCarrier::TAGProperty::TAGPROPERTY_NETELEMENT:
            setText("NetElement element");
            break;
        case GNEAttributeCarrier::TAGProperty::TAGPROPERTY_ADDITIONAL:
            setText("Additional element");
            break;
        case GNEAttributeCarrier::TAGProperty::TAGPROPERTY_SHAPE:
            setText("Shape element");
            break;
        case GNEAttributeCarrier::TAGProperty::TAGPROPERTY_TAZ:
            setText("TAZ element");
            break;
        default:
            throw ProcessError("invalid tag property");

    }
    // Create FXListBox in myTypeMatchBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // Add options to myTypeMatchBox based on tag
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(myTagPropertyType, myOnlyDrawables);
    for (auto i : listOfTags) {
        myTypeMatchBox->appendItem(toString(i).c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // ItemSelector is always shown
    show();
}


GNEFrame::ItemSelector::~ItemSelector() {}


SumoXMLTag
GNEFrame::ItemSelector::getCurrentTypeTag() const {
    return myCurrentType;
}


void
GNEFrame::ItemSelector::setCurrentTypeTag(SumoXMLTag typeTag) {
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
        if (myTypeMatchBox->getItem(i).text() == toString(typeTag)) {
            myTypeMatchBox->setCurrentItem(i);
            // Set new current type
            myCurrentType = typeTag;
        }
    }
    // Check that typeTag type is valid
    if (myCurrentType != SUMO_TAG_NOTHING) {
        // show moduls if selected item is valid
        myFrameParent->enableModuls(GNEAttributeCarrier::getTagProperties(myCurrentType));
    } else {
        // hide all moduls if selected item isn't valid
        myFrameParent->disableModuls();
    }
}


long
GNEFrame::ItemSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(myTagPropertyType, myOnlyDrawables);
    for (auto i : listOfTags) {
        if (toString(i) == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentType = i;
            // show moduls if selected item is valid
            myFrameParent->enableModuls(GNEAttributeCarrier::getTagProperties(myCurrentType));
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in ItemSelector").text());
            return 1;
        }
    }
    // if additional name isn't correct, set SUMO_TAG_NOTHING as current type
    myCurrentType = SUMO_TAG_NOTHING;
    // hide all moduls if selected item isn't valid
    myFrameParent->disableModuls();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in ItemSelector");
    return 1;
}

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
    if (myAC) {
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
    if (myAC) {
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
    GUIGlObject* glObject = dynamic_cast<GUIGlObject*>(myRightClickedAC);
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
    const std::vector<GNEAttributeCarrier*>& currentInspectedACs = myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getInspectedACs();
    // Remove Attribute Carrier
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->removeAttributeCarrier(myRightClickedAC);
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->hide();
    // check if inspector frame has to be shown again
    if (currentInspectedACs.size() == 1) {
        if (currentInspectedACs.front() != myRightClickedAC) {
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
            if (edge) {
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
            if (lane) {
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
            if (POILane) {
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
                FXTreeItem* laneItem = myTreelist->insertItem(nullptr, edgeItem, lane->getHierarchyName().c_str(), lane->getIcon(), lane->getIcon());
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
            if (crossing) {
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
            if (connection) {
                // create edge from item
                FXTreeItem* edgeFromItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeFrom()->getHierarchyName().c_str(), connection->getEdgeFrom()->getIcon(), connection->getEdgeFrom()->getIcon());
                edgeFromItem->setExpanded(true);
                // create edge to item
                FXTreeItem* edgeToItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeTo()->getHierarchyName().c_str(), connection->getEdgeTo()->getIcon(), connection->getEdgeTo()->getIcon());
                edgeToItem->setExpanded(true);
                // create connection item
                FXTreeItem* connectionItem = myTreelist->insertItem(nullptr, edgeToItem, connection->getHierarchyName().c_str(), connection->getIcon(), connection->getIcon());
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
            const auto& tagValue = GNEAttributeCarrier::getTagProperties(myAC->getTag());
            // check if is an additional or a TAZ, and in other case return nullptr
            if (tagValue.isAdditional() || tagValue.isTAZ()) {
                // Obtain Additional
                GNEAdditional* additional = myFrameParent->getViewNet()->getNet()->retrieveAdditional(myAC->getTag(), myAC->getID(), false);
                if (additional) {
                    // first check if additional has another additional as parent (to add it into root)
                    if (tagValue.hasParent() && tagValue.getParentTag() != SUMO_TAG_LANE) {
                        GNEAdditional* additionalParent = myFrameParent->getViewNet()->getNet()->retrieveAdditional(tagValue.getParentTag(), additional->getAttribute(GNE_ATTR_PARENT));
                        // create additional parent item
                        FXTreeItem* additionalParentItem = myTreelist->insertItem(nullptr, nullptr, additionalParent->getHierarchyName().c_str(), additionalParent->getIcon(), additionalParent->getIcon());
                        additionalParentItem->setExpanded(true);
                        // Save it in myTreeItemToACMap
                        myTreeItemToACMap[additionalParentItem] = additionalParent;
                    }
                    if (tagValue.hasAttribute(SUMO_ATTR_EDGE)) {
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
                        FXTreeItem* laneItem = myTreelist->insertItem(nullptr, edgeItem, lane->getHierarchyName().c_str(), lane->getIcon(), lane->getIcon());
                        laneItem->setExpanded(true);
                        // Save items in myTreeItemToACMap
                        myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                        myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                        myTreeItemToACMap[edgeItem] = edge;
                        myTreeItemToACMap[laneItem] = lane;
                        // return lane item
                        return laneItem;
                    } else if (tagValue.hasAttribute(SUMO_ATTR_LANES)) {
                        // obtain lane parent
                        std::vector<GNELane*> lanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(myFrameParent->getViewNet()->getNet(), additional->getAttribute(SUMO_ATTR_LANES));
                        // obtain edge parent
                        GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lanes.front()->getParentEdge().getID());
                        //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                        FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                        FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                        junctionDestinyItem->setExpanded(true);
                        // Create edge item
                        FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                        edgeItem->setExpanded(true);
                        // Create lane item
                        FXTreeItem* laneItem = myTreelist->insertItem(nullptr, edgeItem, lanes.front()->getHierarchyName().c_str(), lanes.front()->getIcon(), lanes.front()->getIcon());
                        laneItem->setExpanded(true);
                        // Save items in myTreeItemToACMap
                        myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                        myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                        myTreeItemToACMap[edgeItem] = edge;
                        myTreeItemToACMap[laneItem] = lanes.front();
                        // return lane item
                        return laneItem;
                    }
                }
            }
        }
    }
    // there isn't parents
    return nullptr;
}


void
GNEFrame::ACHierarchy::showAttributeCarrierChilds(GNEAttributeCarrier* AC, FXTreeItem* itemParent) {
    // Switch gl type of ac
    switch (AC->getTag()) {
        case SUMO_TAG_JUNCTION: {
            // retrieve junction
            GNEJunction* junction = myFrameParent->getViewNet()->getNet()->retrieveJunction(AC->getID(), false);
            if (junction) {
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
            if (edge) {
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
            if (lane) {
                // insert lane item
                FXTreeItem* laneItem = addACIntoList(AC, itemParent);
                // insert additionals of lanes
                for (auto i : lane->getAdditionalChilds()) {
                    showAttributeCarrierChilds(i, laneItem);
                }
                // insert incoming connections of lanes (by default isn't expanded)
                if (lane->getGNEIncomingConnections().size() > 0) {
                    std::vector<GNEConnection*> incomingLaneConnections = lane->getGNEIncomingConnections();
                    FXTreeItem* incomingConnections = myTreelist->insertItem(nullptr, laneItem, "Incomings", incomingLaneConnections.front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
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
                    FXTreeItem* outgoingConnections = myTreelist->insertItem(nullptr, laneItem, "Outcomings", outcomingLaneConnections.front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
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
            // check if is an additional or TAZ
            if (GNEAttributeCarrier::getTagProperties(AC->getTag()).isAdditional() || GNEAttributeCarrier::getTagProperties(AC->getTag()).isTAZ()) {
                // retrieve additional
                GNEAdditional* additional = myFrameParent->getViewNet()->getNet()->retrieveAdditional(AC->getTag(), AC->getID(), false);
                if (additional) {
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
GNEFrame::ACHierarchy::addACIntoList(GNEAttributeCarrier* AC, FXTreeItem* itemParent) {
    FXTreeItem* item = myTreelist->insertItem(nullptr, itemParent, AC->getHierarchyName().c_str(), AC->getIcon(), AC->getIcon());
    myTreeItemToACMap[item] = AC;
    item->setExpanded(true);
    return item;
}

// ---------------------------------------------------------------------------
// GNEFrame::GenericParametersEditor - methods
// ---------------------------------------------------------------------------

GNEFrame::GenericParametersEditor::GenericParametersEditor(GNEFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Generic parameters", GUIDesignGroupBoxFrame),
    myFrameParent(inspectorFrameParent),
    myAC(nullptr),
    myGenericParameters(nullptr) {
    // create empty vector with generic parameters
    myGenericParameters = new std::vector<std::pair<std::string, std::string> >;
    // create textfield and buttons
    myTextFieldGenericParameter = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEditGenericParameterButton = new FXButton(this, "Edit generic parameter", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrame::GenericParametersEditor::~GenericParametersEditor() {
    delete myGenericParameters;
}


void
GNEFrame::GenericParametersEditor::showGenericParametersEditor(GNEAttributeCarrier* AC) {
    if (AC != nullptr) {
        myAC = AC;
        myACs.clear();
        // obtain a copy of generic parameters of AC
        if (myAC) {
            *myGenericParameters = myAC->getGenericParameters();
        }
        // refresh GenericParametersEditor
        refreshGenericParametersEditor();
        // show groupbox
        show();
    }
}


void
GNEFrame::GenericParametersEditor::showGenericParametersEditor(std::vector<GNEAttributeCarrier*> ACs) {
    if (ACs.size() > 0) {
        myAC = nullptr;
        myACs = ACs;
        // check if generic parameters are different
        bool differentsGenericParameters = false;
        std::string genericParameter = myACs.front()->getAttribute(GNE_ATTR_GENERIC);
        for (auto i : myACs) {
            if (genericParameter != i->getAttribute(GNE_ATTR_GENERIC)) {
                differentsGenericParameters = true;
            }
        }
        // set generic Parameters editor
        if (differentsGenericParameters) {
            myGenericParameters->clear();
        } else {
            *myGenericParameters = myACs.front()->getGenericParameters();
        }
        // refresh GenericParametersEditor
        refreshGenericParametersEditor();
        // show groupbox
        show();
    }
}


void
GNEFrame::GenericParametersEditor::hideGenericParametersEditor() {
    myAC = nullptr;
    // hide groupbox
    hide();
}


void
GNEFrame::GenericParametersEditor::refreshGenericParametersEditor() {
    // update text field depending of AC
    if (myAC) {
        myTextFieldGenericParameter->setText(getGenericParametersStr().c_str());
        myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
    } else if (myACs.size()) {
        // check if generic parameters of all inspected ACs are different
        std::string genericParameter = myACs.front()->getAttribute(GNE_ATTR_GENERIC);

        for (auto i : myACs) {
            if (genericParameter != i->getAttribute(GNE_ATTR_GENERIC)) {
                genericParameter = "different generic attributes";
            }
        }
        myTextFieldGenericParameter->setText(genericParameter.c_str());
        myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
    }
}


std::string
GNEFrame::GenericParametersEditor::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
        result += i->first + "=" + i->second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


long
GNEFrame::GenericParametersEditor::onCmdEditGenericParameter(FXObject*, FXSelector, void*) {
    // edit generic parameters using dialog
    if (GNEGenericParameterDialog(myFrameParent->getViewNet(), myGenericParameters).execute()) {
        // set values edited in Parameter dialog in Edited AC
        if (myAC) {
            myAC->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
        } else if (myACs.size() > 0) {
            myFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple generic attributes");
            for (auto i : myACs) {
                i->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
            }
            myFrameParent->getViewNet()->getUndoList()->p_end();
        }
        // Refresh parameter editor
        refreshGenericParametersEditor();
    }
    return 1;
}


long
GNEFrame::GenericParametersEditor::onCmdSetGenericParameter(FXObject*, FXSelector, void*) {
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer st(myTextFieldGenericParameter->getText().text(), "|", true);
    while (st.hasNext()) {
        parsedValues.push_back(st.next());
    }
    // first check if parsed generic parameters are valid
    for (auto i : parsedValues) {
        if (!GNEAttributeCarrier::isGenericParametersValid(i)) {
            WRITE_WARNING("Invalid format of Generic Parameter (" + i + ")");
            myTextFieldGenericParameter->setTextColor(FXRGB(255, 0, 0));
            return 1;
        }
    }
    // now check if there is duplicated parameters
    std::sort(parsedValues.begin(), parsedValues.end());
    for (auto i = parsedValues.begin(); i != parsedValues.end(); i++) {
        if (((i + 1) != parsedValues.end())) {
            std::vector<std::string> firstKey, secondKey;
            StringTokenizer stKey1(*i, "=", true);
            StringTokenizer stKey2(*(i + 1), "=", true);
            //parse both keys
            while (stKey1.hasNext()) {
                firstKey.push_back(stKey1.next());
            }
            while (stKey2.hasNext()) {
                secondKey.push_back(stKey2.next());
            }
            // compare both keys and stop if are equal
            if ((firstKey.size() != 2) || (secondKey.size() != 2) || (firstKey.front() == secondKey.front())) {
                WRITE_WARNING("Generic Parameters wit the same key aren't allowed (" + (*i) + "," + * (i + 1) + ")");
                myTextFieldGenericParameter->setTextColor(FXRGB(255, 0, 0));
                return 1;
            }
        }
    }
    // parsed generic parameters ok, then set text field black and continue
    myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
    myTextFieldGenericParameter->killFocus();
    // clear current existent generic parameters and set parsed generic parameters
    myGenericParameters->clear();
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            myGenericParameters->push_back(std::make_pair(parsedParameters.front(), parsedParameters.back()));
        }
    }
    // if we're editing generic attributes of an AttributeCarrier, set it
    if (myAC) {
        myAC->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
    } else if (myACs.size() > 0) {
        myFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple generic attributes");
        for (auto i : myACs) {
            i->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
        }
        myFrameParent->getViewNet()->getUndoList()->p_end();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::DrawingShape - methods
// ---------------------------------------------------------------------------

GNEFrame::DrawingShape::DrawingShape(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Drawing", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myDeleteLastCreatedPoint(false) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(this, "Start drawing", 0, this, MID_GNE_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(this, "Stop drawing", 0, this, MID_GNE_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(this, "Abort drawing", 0, this, MID_GNE_ABORTDRAWING, GUIDesignButton);

    // create information label
    std::ostringstream information;
    information
            << "- 'Start drawing' or ENTER\n"
            << "  draws shape boundary.\n"
            << "- 'Stop drawing' or ENTER\n"
            << "  creates shape.\n"
            << "- 'Shift + Click'\n"
            << "  removes last created point.\n"
            << "- 'Abort drawing' or ESC\n"
            << "  removes drawed shape.";
    myInformationLabel = new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEFrame::DrawingShape::~DrawingShape() {}


void GNEFrame::DrawingShape::showDrawingShape() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEFrame::DrawingShape::hideDrawingShape() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrame::DrawingShape::startDrawing() {
    // Only start drawing if DrawingShape modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrame::DrawingShape::stopDrawing() {
    // try to build shape
    if (myFrameParent->buildShape()) {
        // clear created points
        myTemporalShapeShape.clear();
        myFrameParent->getViewNet()->update();
        // change buttons
        myStartDrawingButton->enable();
        myStopDrawingButton->disable();
        myAbortDrawingButton->disable();
    } else {
        // abort drawing if shape cannot be created
        abortDrawing();
    }
}


void
GNEFrame::DrawingShape::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->getViewNet()->update();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrame::DrawingShape::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrame::DrawingShape::removeLastPoint() {

}


const PositionVector&
GNEFrame::DrawingShape::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrame::DrawingShape::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEFrame::DrawingShape::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEFrame::DrawingShape::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEFrame::DrawingShape::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrame::DrawingShape::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrame::DrawingShape::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEFrame::NeteditAttributes::NeteditAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT),
    myFrameParent(frameParent),
    myCurrentLengthValid(true) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");
    // Create Frame for Length Label and textField
    FXHorizontalFrame* lengthFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLengthLabel = new FXLabel(lengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelAttribute);
    myLengthTextField = new FXTextField(lengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockMovementLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBlockMovementCheckButton->setCheck(false);
    // Create Frame for block shape label and checkBox (By default disabled)
    FXHorizontalFrame* blockShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockShapeLabel = new FXLabel(blockShapeFrame, "block shape", 0, GUIDesignLabelAttribute);
    myBlockShapeCheckButton = new FXCheckButton(blockShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    // Create Frame for block close polygon and checkBox (By default disabled)
    FXHorizontalFrame *closePolygonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myClosePolygonLabel = new FXLabel(closePolygonFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myCloseShapeCheckButton = new FXCheckButton(closePolygonFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBlockShapeCheckButton->setCheck(false);
    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEFrame::NeteditAttributes::~NeteditAttributes() {}


void
GNEFrame::NeteditAttributes::showNeteditAttributesModul(const GNEAttributeCarrier::TagValues& tagValue) {
    // we assume that frame will not be show
    bool showFrame = false;
    // check if lenght text field has to be showed
    if(tagValue.canMaskStartEndPos()) {
        myLengthLabel->show();
        myLengthTextField->show();
        myReferencePointMatchBox->show();
        showFrame = true;
    } else {
        myLengthLabel->hide();
        myLengthTextField->hide();
        myReferencePointMatchBox->hide();
    }
    // check if block movement check button has to be show
    if (tagValue.canBlockMovement()) {
        myBlockMovementLabel->show();
        myBlockMovementCheckButton->show();
        showFrame = true;
    } else {
        myBlockMovementLabel->hide();
        myBlockMovementCheckButton->hide();
    }
    // check if block shape check button has to be show
    if (tagValue.canBlockShape()) {
        myBlockShapeLabel->show();
        myBlockShapeCheckButton->show();
        showFrame = true;
    } else {
        myBlockShapeLabel->hide();
        myBlockShapeCheckButton->hide();
    }
    // check if close shape check button has to be show
    if (tagValue.canCloseShape()) {
        myClosePolygonLabel->show();
        myCloseShapeCheckButton->show();
        showFrame = true;
    } else {
        myClosePolygonLabel->hide();
        myCloseShapeCheckButton->hide();
    }
    // if at least one element is show, show modul
    if(showFrame) {
        show();
    } else {
        hide();
    }
}


void
GNEFrame::NeteditAttributes::hideNeteditAttributesModul() {
    hide();
}


bool 
GNEFrame::NeteditAttributes::getNeteditAttributesAndValues(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane *lane) const {
    // check if we need to obtain a start and end position over an edge
    if(myReferencePointMatchBox->shown()) {
        // we need a valid lane to calculate position over lane
        if(lane == nullptr) {
            return false;
        } else if (myCurrentLengthValid) {
            // Obtain position of the mouse over lane (limited over grid)
            double mousePositionOverLane = lane->getShape().nearest_offset_to_point2D(myFrameParent->myViewNet->snapToActiveGrid(myFrameParent->myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
            // check if current reference point is valid
            if (myActualAdditionalReferencePoint == GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                std::string errorMessage = "Current selected reference point isn't valid";
                myFrameParent->myViewNet->setStatusBarText(errorMessage);
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(errorMessage);
                return false;
            } else {
                // obtain lenght
                double lenght = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
                // set start and end position
                valuesMap[SUMO_ATTR_STARTPOS] = toString(setStartPosition(mousePositionOverLane, lenght));
                valuesMap[SUMO_ATTR_ENDPOS] = toString(setEndPosition(mousePositionOverLane, lenght));
            }
        } else {
            return false;
        }
    }
    // Save block value if element can be blocked
    if (myBlockMovementCheckButton->shown()) {
        if(myBlockMovementCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "true";
        } else {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "false";
        }
    }
    // Save block shape value if shape's element can be blocked
    if (myBlockShapeCheckButton->shown()) {
        if(myBlockShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "true";
        } else {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "false";
        }
    }
    // Save close shape value if shape's element can be closed
    if (myCloseShapeCheckButton->shown()) {
        if(myCloseShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "true";
        } else {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "false";
        }
    }
    // all ok, then return true to continue creating element
    return true;
}


long
GNEFrame::NeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myBlockMovementCheckButton) {
        if (myBlockMovementCheckButton->getCheck()) {
            myBlockMovementCheckButton->setText("true");
        } else {
            myBlockMovementCheckButton->setText("false");
        }
    } else if (obj == myBlockShapeCheckButton) {
        if (myBlockShapeCheckButton->getCheck()) {
            myBlockShapeCheckButton->setText("true");
        } else {
            myBlockShapeCheckButton->setText("false");
        }
    } else if (obj == myCloseShapeCheckButton) {
        if (myCloseShapeCheckButton->getCheck()) {
            myCloseShapeCheckButton->setText("true");
        } else {
            myCloseShapeCheckButton->setText("false");
        }
    } else if (obj == myLengthTextField) {
        // change color of text field depending of the input length
        if (GNEAttributeCarrier::canParse<double>(myLengthTextField->getText().text()) &&
                GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text()) > 0) {
            myLengthTextField->setTextColor(FXRGB(0, 0, 0));
            myLengthTextField->killFocus();
            myCurrentLengthValid = true;
        } else {
            myLengthTextField->setTextColor(FXRGB(255, 0, 0));
            myCurrentLengthValid = false;
        }
        // Update aditional frame
        update();
    } else if (obj == myReferencePointMatchBox) {
        // Cast actual reference point type
        if (myReferencePointMatchBox->getText() == "reference left") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_LEFT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference right") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_RIGHT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference center") {
            myLengthTextField->enable();
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_CENTER;
            myLengthTextField->enable();
        } else {
            myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_INVALID;
            myLengthTextField->disable();
        }
    }

    return 1;
}


long
GNEFrame::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << "- Referece point: Mark the initial position of the additional element.\n"
            << "  Example: If you want to create a busStop with a length of 30 in the point 100 of the lane:\n"
            << "  - Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "  - Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "  - Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "- Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening NeteditAttributes help dialog");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing NeteditAttributes help dialog");
    return 1;
    /**********
    help from PolygonFrame
            << "- Block movement: If enabled, the created polygon element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Block shape: If enabled, the shape of created polygon element will be blocked. i.e. their geometry points\n"
            << "  cannot be edited be moved with the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Close shape: If enabled, the created polygon element will be closed. i.e. the last created geometry point\n"
            << "  will be connected with the first geometry point automatically. This option can be modified inspecting element.";
    
    ****************/
}


double
GNEFrame::NeteditAttributes::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const {
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNEFrame::NeteditAttributes::setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional)  const{
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

// ---------------------------------------------------------------------------
// GNEFrame - methods
// ---------------------------------------------------------------------------

GNEFrame::GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel) :
    FXVerticalFrame(horizontalFrameParent, GUIDesignAuxiliarFrame),
    myViewNet(viewNet),
    myEdgeCandidateColor(RGBColor(0, 64, 0, 255)),
    myEdgeCandidateSelectedColor(RGBColor::GREEN) {

    // Create font
    myFrameHeaderFont = new FXFont(getApp(), "Arial", 14, FXFont::Bold),

    // Create frame for header
    myHeaderFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create frame for left elements of header (By default unused)
    myHeaderLeftFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderLeftFrame->hide();

    // Create titel frame
    myFrameHeaderLabel = new FXLabel(myHeaderFrame, frameLabel.c_str(), nullptr, GUIDesignLabelFrameInformation);

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


bool 
GNEFrame::buildShape() {
    // this function has to be reimplemente in all child frames that needs to draw a polygon (for example, GNEPolygonFrame or GNETAZFrame)
    return false;
}


void 
GNEFrame::enableModuls(const GNEAttributeCarrier::TagValues&) {
    // this function has to be reimplemente in all child frames that uses a ItemSelector modul
}


void 
GNEFrame::disableModuls() {
    // this function has to be reimplemente in all child frames that uses a ItemSelector modul
}


void
GNEFrame::openHelpAttributesDialog(SumoXMLTag elementTag) const {
    FXDialogBox* attributesHelpDialog = new FXDialogBox(myScrollWindowsContents, ("Parameters of " + toString(elementTag)).c_str(), GUIDesignDialogBoxResizable, 0, 0, 0, 0, 10, 10, 10, 38, 4, 4);
    // Create FXTable
    FXTable* myTable = new FXTable(attributesHelpDialog, attributesHelpDialog, MID_TABLE, GUIDesignTableNotEditable);
    attributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEINSPECT));
    const auto& attrs = GNEAttributeCarrier::getTagProperties(elementTag);
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
    WRITE_DEBUG("Opening HelpAttributes dialog for tag '" + toString(elementTag) + "' showing " + toString(attrs.getNumberOfAttributes()) + " attributes");
    // create Dialog
    attributesHelpDialog->create();
    // show in the given position
    attributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(attributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing HelpAttributes dialog for tag '" + toString(elementTag) + "'");
}


const RGBColor&
GNEFrame::getEdgeCandidateColor() const {
    return myEdgeCandidateColor;
}


const RGBColor& 
GNEFrame::getEdgeCandidateSelectedColor() const {
    return myEdgeCandidateSelectedColor;
}

/****************************************************************************/
