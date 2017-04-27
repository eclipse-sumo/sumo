/****************************************************************************/
/// @file    GNEInspectorFrame.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying network-element attributes (i.e. lane speed)
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

#include <iostream>
#include <cassert>
#include <cmath>
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

#include "GNEAdditional.h"
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
#include "GNEDialog_AllowDisallow.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_COPY_TEMPLATE,          GNEInspectorFrame::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_SET_TEMPLATE,           GNEInspectorFrame::onCmdSetTemplate),
    FXMAPFUNC(SEL_UPDATE,               MID_GNE_COPY_TEMPLATE,          GNEInspectorFrame::onUpdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_SET_BLOCKING,           GNEInspectorFrame::onCmdSetBlocking),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECT_GOBACK,         GNEInspectorFrame::onCmdGoBack),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_CHILDS,                 GNEInspectorFrame::onCmdShowChildMenu),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTFRAME_CENTER,    GNEInspectorFrame::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTFRAME_INSPECT,   GNEInspectorFrame::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTFRAME_DELETE,    GNEInspectorFrame::onCmdDeleteItem),
};


FXDEFMAP(GNEInspectorFrame::AttributeInput) AttrInputMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,         GNEInspectorFrame::AttributeInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_OPEN_ATTRIBUTE_EDITOR, GNEInspectorFrame::AttributeInput::onCmdOpenAllowDisallowEditor)
};

// Object implementation
FXIMPLEMENT(GNEInspectorFrame, FXVerticalFrame, GNEInspectorFrameMap, ARRAYNUMBER(GNEInspectorFrameMap))
FXIMPLEMENT(GNEInspectorFrame::AttributeInput, FXHorizontalFrame, AttrInputMap, ARRAYNUMBER(AttrInputMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEInspectorFrame::GNEInspectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Inspector"),
    myEdgeTemplate(NULL),
    myAdditional(NULL),
    myPreviousElementInspect(NULL),
    myPreviousElementDelete(NULL) {

    // Create back button
    myBackButton = new FXButton(myHeaderLeftFrame, "", GUIIconSubSys::getIcon(ICON_NETEDITARROW), this, MID_GNE_INSPECT_GOBACK, GUIDesignButtonIconRectangular);
    myHeaderLeftFrame->hide();
    myBackButton->hide();

    // Create groupBox for attributes
    myGroupBoxForAttributes = new FXGroupBox(myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame);
    myGroupBoxForAttributes->hide();

    // Create AttributeInput
    for (int i = 0; i < (int)GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myVectorOfAttributeInputs.push_back(new AttributeInput(myGroupBoxForAttributes, this));
    }

    // Create groupBox for Netedit parameters
    myGroupBoxForEditor = new FXGroupBox(myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame);
    myGroupBoxForEditor->hide();

    // Create check blocked label and button
    FXHorizontalFrame* blockMovementHorizontalFrame = new FXHorizontalFrame(myGroupBoxForEditor, GUIDesignAuxiliarHorizontalFrame);
    myCheckBlockedLabel = new FXLabel(blockMovementHorizontalFrame, "block movement", 0, GUIDesignLabelAttribute);
    myCheckBlocked = new FXCheckButton(blockMovementHorizontalFrame, "", this, MID_GNE_SET_BLOCKING, GUIDesignCheckButtonAttribute);
    myCheckBlocked->hide();

    // Create groupbox and tree list
    myGroupBoxForTreeList = new FXGroupBox(myContentFrame, "Childs", GUIDesignGroupBoxFrame);
    myTreelist = new FXTreeList(myGroupBoxForTreeList, this, MID_GNE_CHILDS, GUIDesignTreeListFrame);
    myGroupBoxForTreeList->hide();

    // Create groupBox for templates
    myGroupBoxForTemplates = new FXGroupBox(myContentFrame, "Templates", GUIDesignGroupBoxFrame);
    myGroupBoxForTemplates->hide();

    // Create copy template button
    myCopyTemplateButton = new FXButton(myGroupBoxForTemplates, "", 0, this, MID_GNE_COPY_TEMPLATE, GUIDesignButton);
    myCopyTemplateButton->hide();

    // Create set template button
    mySetTemplateButton = new FXButton(myGroupBoxForTemplates, "Set as Template\t\t", 0, this, MID_GNE_SET_TEMPLATE, GUIDesignButton);
    mySetTemplateButton->hide();
}


GNEInspectorFrame::~GNEInspectorFrame() {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::~GNEInspectorFrame");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
}


void
GNEInspectorFrame::show() {
    inspectElement(NULL);
    GNEFrame::show();
}


void
GNEInspectorFrame::inspectElement(GNEAttributeCarrier* AC) {
    // Use the implementation of inspect for multiple AttributeCarriers to avoid repetition of code
    std::vector<GNEAttributeCarrier*> itemToInspect;
    if (AC != NULL) {
        itemToInspect.push_back(AC);
    }
    inspectMultisection(itemToInspect);
}


void
GNEInspectorFrame::inspectMultisection(const std::vector<GNEAttributeCarrier*>& ACs) {
    // hide back button
    myHeaderLeftFrame->hide();
    myBackButton->hide();
    // Assing ACs to myACs
    myACs = ACs;
    // Hide all elements
    myGroupBoxForAttributes->hide();
    myGroupBoxForTemplates->hide();
    myCopyTemplateButton->hide();
    mySetTemplateButton->hide();
    myGroupBoxForEditor->hide();
    myGroupBoxForEditor->hide();
    myCheckBlocked->hide();
    myGroupBoxForTreeList->hide();
    // If vector of attribute Carriers contain data
    if (myACs.size() > 0) {
        // Set header
        std::string headerString;
        if (myACs.size() > 1) {
            headerString = "Selection: " + toString(myACs.size()) + " " + toString(myACs.front()->getTag()) + "s";
        } else {
            if (dynamic_cast<GNENetElement*>(myACs.front())) {
                headerString = "Net: " + toString(myACs.front()->getTag());
            } else if (dynamic_cast<GNEAdditional*>(myACs.front())) {
                headerString = "Additional: " + toString(myACs.front()->getTag());
            }
        }
        // Set headerString into header label
        getFrameHeaderLabel()->setText(headerString.c_str());

        //Show myGroupBoxForAttributes
        myGroupBoxForAttributes->show();

        // Hide all AttributeInput
        for (std::vector<GNEInspectorFrame::AttributeInput*>::iterator i = myVectorOfAttributeInputs.begin(); i != myVectorOfAttributeInputs.end(); i++) {
            (*i)->hideAttribute();
        }

        // Gets tag and attributes of element
        SumoXMLTag tag = myACs.front()->getTag();
        const std::vector<SumoXMLAttr>& attrs = myACs.front()->getAttrs();

        // Declare iterator over AttrInput
        std::vector<GNEInspectorFrame::AttributeInput*>::iterator itAttrs = myVectorOfAttributeInputs.begin();

        //  check if current AC is a Junction without TLSs (needed to hidde TLS options)
        bool disableTLSinJunctions = (dynamic_cast<GNEJunction*>(myACs.front()) && (dynamic_cast<GNEJunction*>(myACs.front())->getNBNode()->getControllingTLS().empty()));

        // Iterate over attributes
        for (std::vector<SumoXMLAttr>::const_iterator it = attrs.begin(); it != attrs.end(); it++) {
            if (myACs.size() > 1 && GNEAttributeCarrier::isUnique(tag, *it)) {
                // disable editing for some attributes in case of multi-selection
                // even displaying is problematic because of string rendering restrictions
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (std::vector<GNEAttributeCarrier*>::const_iterator it_ac = myACs.begin(); it_ac != myACs.end(); it_ac++) {
                occuringValues.insert((*it_ac)->getAttribute(*it));
            }
            // get current value
            std::ostringstream oss;
            for (std::set<std::string>::iterator it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            // Show attribute
            if (((disableTLSinJunctions == true) && (tag == SUMO_TAG_JUNCTION) && ((*it == SUMO_ATTR_TLTYPE) || (*it == SUMO_ATTR_TLID))) == false) {
                (*itAttrs)->showAttribute(myACs.front()->getTag(), (*it), oss.str());
            }
            // update attribute iterator
            itAttrs++;
        }

        // If attributes correspond to an Additional
        if (dynamic_cast<GNEAdditional*>(myACs.front())) {
            // Get pointer to additional
            myAdditional = dynamic_cast<GNEAdditional*>(myACs.front());
            // Show check blocked if additional is movable
            if (myAdditional->isAdditionalMovable()) {
                myCheckBlocked->setCheck(myAdditional->isAdditionalBlocked());
                if (myAdditional->isAdditionalBlocked()) {
                    myCheckBlocked->setText("true");
                } else {
                    myCheckBlocked->setText("false");
                }
                myCheckBlocked->show();
                myGroupBoxForEditor->show();
            }
        }

        // if we inspect a single Attribute carrier vector, show their childs
        if (myACs.size() == 1) {
            showAttributeCarrierChilds();
        }

        // If attributes correspond to an Edge
        if (dynamic_cast<GNEEdge*>(myACs.front())) {
            // show groupBox for templates
            myGroupBoxForTemplates->show();
            // show "Copy Template" (caption supplied via onUpdate)
            myCopyTemplateButton->show();
            // show "Set As Template"
            if (myACs.size() == 1) {
                mySetTemplateButton->show();
            }
        }
    } else {
        getFrameHeaderLabel()->setText("No Object selected");
        myContentFrame->recalc();
    }
}


void
GNEInspectorFrame::inspectChild(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement) {
    // Show back button if myPreviousElementInspect was defined
    myPreviousElementInspect = previousElement;
    if (myPreviousElementInspect != NULL) {
        // disable myPreviousElementDelete to avoid inconsistences
        myPreviousElementDelete = NULL;
        inspectElement(AC);
        myHeaderLeftFrame->show();
        myBackButton->show();
    }
}


void GNEInspectorFrame::inspectFromDeleteFrame(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement, bool previousElementWasMarked) {
    myPreviousElementDelete = previousElement;
    myPreviousElementDeleteWasMarked = previousElementWasMarked;
    // Show back button if myPreviousElementDelete is valid
    if (myPreviousElementDelete != NULL) {
        // disable myPreviousElementInspect to avoid inconsistences
        myPreviousElementInspect = NULL;
        inspectElement(AC);
        myHeaderLeftFrame->show();
        myBackButton->show();
    }
}


GNEEdge*
GNEInspectorFrame::getEdgeTemplate() const {
    return myEdgeTemplate;
}


void
GNEInspectorFrame::setEdgeTemplate(GNEEdge* tpl) {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::setEdgeTemplate");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
    myEdgeTemplate = tpl;
    myEdgeTemplate->incRef("GNEInspectorFrame::setEdgeTemplate");
}


long
GNEInspectorFrame::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    for (std::vector<GNEAttributeCarrier*>::iterator it = myACs.begin(); it != myACs.end(); it++) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(*it);
        assert(edge);
        edge->copyTemplate(myEdgeTemplate, myViewNet->getUndoList());
        inspectMultisection(myACs);
    }
    return 1;
}


long
GNEInspectorFrame::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    assert(myACs.size() == 1);
    GNEEdge* edge = dynamic_cast<GNEEdge*>(myACs.front());
    assert(edge);
    setEdgeTemplate(edge);
    return 1;
}


long
GNEInspectorFrame::onUpdCopyTemplate(FXObject* sender, FXSelector, void*) {
    FXString caption;
    if (myEdgeTemplate) {
        caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "'").c_str();
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    } else {
        caption = "No Template Set";
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    }
    sender->handle(this, FXSEL(SEL_COMMAND, FXLabel::ID_SETSTRINGVALUE), (void*)&caption);
    return 1;
}


long
GNEInspectorFrame::onCmdSetBlocking(FXObject*, FXSelector, void*) {
    if (myAdditional) {
        if (myCheckBlocked->getCheck() == 1) {
            myAdditional->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "true", getViewNet()->getUndoList());
            myCheckBlocked->setText("true");
        } else {
            myAdditional->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "false", getViewNet()->getUndoList());
            myCheckBlocked->setText("false");
        }
    }
    return 1;
}


long
GNEInspectorFrame::onCmdGoBack(FXObject*, FXSelector, void*) {
    // Inspect previous element or go back to Delete Frame
    if (myPreviousElementInspect) {
        inspectElement(myPreviousElementInspect);
        myPreviousElementInspect = NULL;
    } else if (myPreviousElementDelete != NULL) {
        myViewNet->getViewParent()->getDeleteFrame()->showAttributeCarrierChilds(myPreviousElementDelete);
        if (myPreviousElementDeleteWasMarked) {
            myViewNet->getViewParent()->getDeleteFrame()->markAttributeCarrier(myPreviousElementDelete);
        }
        myPreviousElementDelete = NULL;
        // Hide inspect frame and show delete frame
        hide();
        myViewNet->getViewParent()->getDeleteFrame()->show();
    }
    return 1;
}


const std::vector<GNEAttributeCarrier*>&
GNEInspectorFrame::getACs() const {
    return myACs;
}


long
GNEInspectorFrame::onCmdShowChildMenu(FXObject*, FXSelector, void* data) {
    // Obtain event
    FXEvent* e = (FXEvent*) data;
    FXTreeItem* item = myTreelist->getItemAt(e->win_x, e->win_y);
    // Check if there are an item in the position and create pop-up menu
    if (item && (myTreeItemsWithoutAC.find(item) == myTreeItemsWithoutAC.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[myTreelist->getItemAt(e->win_x, e->win_y)]);
    }
    return 1;
}


long
GNEInspectorFrame::onCmdCenterItem(FXObject*, FXSelector, void*) {
    if (dynamic_cast<GNENetElement*>(myRightClickedAC)) {
        myViewNet->centerTo(dynamic_cast<GNENetElement*>(myRightClickedAC)->getGlID(), false);
    } else if (dynamic_cast<GNEAdditional*>(myRightClickedAC)) {
        myViewNet->centerTo(dynamic_cast<GNEAdditional*>(myRightClickedAC)->getGlID(), false);
    } else if (dynamic_cast<GNEPOI*>(myRightClickedAC)) {
        myViewNet->centerTo(dynamic_cast<GNEPOI*>(myRightClickedAC)->getGlID(), false);
    } else if (dynamic_cast<GNEPoly*>(myRightClickedAC)) {
        myViewNet->centerTo(dynamic_cast<GNEPoly*>(myRightClickedAC)->getGlID(), false);
    }
    myViewNet->update();
    return 1;
}


long
GNEInspectorFrame::onCmdInspectItem(FXObject*, FXSelector, void*) {
    assert(myACs.size() == 1);
    inspectChild(myRightClickedAC, myACs.front());
    return 1;
}


long
GNEInspectorFrame::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    // Remove Attribute Carrier
    myViewNet->getViewParent()->getDeleteFrame()->show();
    myViewNet->getViewParent()->getDeleteFrame()->removeAttributeCarrier(myRightClickedAC);
    myViewNet->getViewParent()->getDeleteFrame()->hide();
    // show again childs of attribute carrier
    showAttributeCarrierChilds();
    return 1;
}


void
GNEInspectorFrame::createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac) {
    // create FXMenuPane
    FXMenuPane* pane = new FXMenuPane(myTreelist);
    // set current clicked AC
    myRightClickedAC = ac;
    // set name
    new MFXMenuHeader(pane, myViewNet->getViewParent()->getApp()->getBoldFont(), (toString(myRightClickedAC->getTag()) + ": " + myRightClickedAC->getID()).c_str(), myRightClickedAC->getIcon());
    new FXMenuSeparator(pane);
    // Fill FXMenuCommand
    new FXMenuCommand(pane, "Center", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_GNE_INSPECTFRAME_CENTER);
    new FXMenuCommand(pane, "Inspect", GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_INSPECTFRAME_INSPECT);
    new FXMenuCommand(pane, "Delete", GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_INSPECTFRAME_DELETE);
    // Center in the mouse position and create pane
    pane->setX(X);
    pane->setY(Y);
    pane->create();
    pane->show();
}


void
GNEInspectorFrame::showAttributeCarrierChilds() {
    // Only show attributes of ONE attribute carrier
    assert(myACs.size() == 1);
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsWithoutAC.clear();
    myGroupBoxForTreeList->show();
    // Switch gl type of ac
    switch (dynamic_cast<GUIGlObject*>(myACs.front())->getType()) {
        case GLO_JUNCTION: {
            // insert junction root
            GNEJunction* junction = dynamic_cast<GNEJunction*>(myACs.front());
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
            GNEEdge* edge = dynamic_cast<GNEEdge*>(myACs.front());
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
                GNEAdditional* additional = edge->getAdditionalChilds().at(i);
                FXTreeItem* additionalItem = myTreelist->insertItem(0, edgeItem, (toString(additional->getTag()) + " " + toString(i)).c_str(), additional->getIcon(), additional->getIcon());
                myTreeItemToACMap[additionalItem] = additional;
                additionalItem->setExpanded(true);
            }
            break;
        }
        case GLO_LANE: {
            // insert lane root
            GNELane* lane = dynamic_cast<GNELane*>(myACs.front());
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
            GNEPOI* POI = dynamic_cast<GNEPOI*>(myACs.front());
            FXTreeItem* POIItem = myTreelist->insertItem(0, 0, toString(POI->getTag()).c_str(), POI->getIcon(), POI->getIcon());
            myTreeItemToACMap[POIItem] = POI;
            POIItem->setExpanded(true);
            break;
        }
        case GLO_POLYGON: {
            // insert polygon root
            GNEPoly* polygon = dynamic_cast<GNEPoly*>(myACs.front());
            FXTreeItem* polygonItem = myTreelist->insertItem(0, 0, toString(polygon->getTag()).c_str(), polygon->getIcon(), polygon->getIcon());
            myTreeItemToACMap[polygonItem] = polygon;
            polygonItem->setExpanded(true);
            break;
        }
        case GLO_CROSSING: {
            // insert crossing root
            GNECrossing* crossing = dynamic_cast<GNECrossing*>(myACs.front());
            FXTreeItem* crossingItem = myTreelist->insertItem(0, 0, toString(crossing->getTag()).c_str(), crossing->getIcon(), crossing->getIcon());
            myTreeItemToACMap[crossingItem] = crossing;
            crossingItem->setExpanded(true);
            break;
        }
        case GLO_ADDITIONAL: {
            // insert additional root
            GNEAdditional* additional = dynamic_cast<GNEAdditional*>(myACs.front());
            FXTreeItem* additionalItem = myTreelist->insertItem(0, 0, toString(additional->getTag()).c_str(), additional->getIcon(), additional->getIcon());
            myTreeItemToACMap[additionalItem] = additional;
            additionalItem->setExpanded(true);
            break;
        }
        case GLO_CONNECTION: {
            // insert connection root
            GNEConnection* connection = dynamic_cast<GNEConnection*>(myACs.front());
            FXTreeItem* connectionItem = myTreelist->insertItem(0, 0, toString(connection->getTag()).c_str(), connection->getIcon(), connection->getIcon());
            myTreeItemToACMap[connectionItem] = connection;
            connectionItem->setExpanded(true);
            break;
        }
        default: {
            myGroupBoxForTreeList->hide();
            break;
        }
    }
}

// ===========================================================================
// AttributeInput method definitions
// ===========================================================================

GNEInspectorFrame::AttributeInput::AttributeInput(FXComposite* parent, GNEInspectorFrame* inspectorFrameParent) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myInspectorFrameParent(inspectorFrameParent),
    myTag(SUMO_TAG_NOTHING),
    myAttr(SUMO_ATTR_NOTHING) {
    // Create and hide ButtonCombinableChoices
    myButtonCombinableChoices = new FXButton(this, "AttributeButton", 0, this, MID_GNE_OPEN_ATTRIBUTE_EDITOR, GUIDesignButtonAttribute);
    myButtonCombinableChoices->hide();
    // Create and hide label
    myLabel = new FXLabel(this, "attributeLabel", 0, GUIDesignLabelAttribute);
    myLabel->hide();
    // Create and hide textField for int attributes
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldInt);
    myTextFieldInt->hide();
    // Create and hide textField for real/time attributes
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTextFieldReal->hide();
    // Create and hide textField for string attributes
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextFieldStrings->hide();
    // Create and hide ComboBox
    myChoicesCombo = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myChoicesCombo->hide();
    // Create and hide checkButton
    myBoolCheckButton = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBoolCheckButton->hide();
}


void
GNEInspectorFrame::AttributeInput::showAttribute(SumoXMLTag tag, SumoXMLAttr attr, const std::string& value) {
    // Set actual Tag and attribute
    myTag = tag;
    myAttr = attr;
    // Show attribute Label
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    // Set field depending of the type of value
    if (GNEAttributeCarrier::isBool(myTag, myAttr)) {
        // this is an special case for inspection of multiple attribute carriers with bools
        std::vector<bool> boolValues = GNEAttributeCarrier::parse<std::vector<bool> >(value);
        // set value of checkbox
        if (boolValues.size() == 1) {
            myBoolCheckButton->setCheck(boolValues.front());
        } else {
            int sum = 0;
            for (std::vector<bool>::iterator i = boolValues.begin(); i != boolValues.end(); i++) {
                sum += (int)(*i);
            }
            // only set true if all checkbox are true
            if ((sum == 0) || (sum != (int)boolValues.size())) {
                myBoolCheckButton->setCheck(false);
            } else {
                myBoolCheckButton->setCheck(true);
            }
        }
        // set text
        if (myBoolCheckButton->getCheck()) {
            myBoolCheckButton->setText("true");
        } else {
            myBoolCheckButton->setText("false");
        }
        myBoolCheckButton->show();
    } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string> choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable coices
        if (choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            // hide label
            myLabel->hide();
            // Show button combinable choices
            myButtonCombinableChoices->setText(toString(myAttr).c_str());
            myButtonCombinableChoices->show();
            // Show string with the values
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
        } else {
            // fill comboBox
            myChoicesCombo->clearItems();
            for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
                myChoicesCombo->appendItem(it->c_str());
            }
            myChoicesCombo->setNumVisible((int)choices.size());
            myChoicesCombo->setCurrentItem(myChoicesCombo->findItem(value.c_str()));
            myChoicesCombo->show();
        }
    } else if (GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr)) {
        // show TextField for real/time values
        myTextFieldReal->setText(value.c_str());
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->show();
    } else if (GNEAttributeCarrier::isInt(myTag, myAttr)) {
        // Show textField for int attributes
        myTextFieldInt->setText(value.c_str());
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->show();
    } else {
        // In any other case (String, list, etc.), show value as String
        myTextFieldStrings->setText(value.c_str());
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->show();
    }
    // Show AttributeInput
    show();
}


void
GNEInspectorFrame::AttributeInput::hideAttribute() {
    // Hide all elements
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myChoicesCombo->hide();
    myBoolCheckButton->hide();
    myButtonCombinableChoices->hide();
    // hide AttributeInput
    hide();
}


SumoXMLTag
GNEInspectorFrame::AttributeInput::getTag() const {
    return myTag;
}


SumoXMLAttr
GNEInspectorFrame::AttributeInput::getAttr() const {
    return myAttr;
}


long
GNEInspectorFrame::AttributeInput::onCmdOpenAllowDisallowEditor(FXObject*, FXSelector, void*) {
    // in XML either allow or disallow can be defined for convenience
    // In the input dialog, only the positive-list (allow) is defined even when
    // clicking on 'disallow'
    std::string allowed = myTextFieldStrings->getText().text();
    std::string disallowed = "";
    if (myAttr == SUMO_ATTR_DISALLOW) {
        std::swap(allowed, disallowed);
    }
    // remove special word "(combined!)"
    std::string::size_type i = allowed.find(" (combined!)");
    if (i != std::string::npos) {
        allowed.erase(i, 12);
    }
    SVCPermissions permissions;
    if (allowed == "all") {
        permissions = SVCAll;
    } else {
        permissions = parseVehicleClasses(allowed, disallowed);
    }
    // use expanded form
    allowed = getVehicleClassNames(permissions, true);
    GNEDialog_AllowDisallow(getApp(), &allowed).execute();
    if (myAttr == SUMO_ATTR_DISALLOW) {
        // invert again
        std::string disallowed = getVehicleClassNames(SVCAll & ~parseVehicleClasses(allowed));
        myTextFieldStrings->setText((disallowed).c_str());
    } else {
        myTextFieldStrings->setText((allowed).c_str());
    }
    onCmdSetAttribute(0, 0, 0);
    return 1;
}


long
GNEInspectorFrame::AttributeInput::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // First, obtain the string value of the new attribute depending of their type
    if (GNEAttributeCarrier::isBool(myTag, myAttr)) {
        // Set true o false depending of the checBox
        if (myBoolCheckButton->getCheck()) {
            myBoolCheckButton->setText("true");
            newVal = "true";
        } else {
            myBoolCheckButton->setText("false");
            newVal = "false";
        }
    } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable coices
        if (choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            // Get value obtained using AttributeEditor
            newVal = myTextFieldStrings->getText().text();
        } else {
            // Get value of ComboBox
            newVal = myChoicesCombo->getText().text();
        }
    } else if (GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr)) {
        // Check if default value of attribute must be set
        if (myTextFieldReal->getText().empty() && GNEAttributeCarrier::hasDefaultValue(myTag, myAttr)) {
            newVal = GNEAttributeCarrier::getDefaultValue<std::string>(myTag, myAttr);
            myTextFieldReal->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldReal
            newVal = myTextFieldReal->getText().text();
        }
    } else if (GNEAttributeCarrier::isInt(myTag, myAttr)) {
        // Check if default value of attribute must be set
        if (myTextFieldInt->getText().empty() && GNEAttributeCarrier::hasDefaultValue(myTag, myAttr)) {
            newVal = GNEAttributeCarrier::getDefaultValue<std::string>(myTag, myAttr);
            myTextFieldInt->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldInt
            newVal = myTextFieldInt->getText().text();
        }
    } else if (GNEAttributeCarrier::isString(myTag, myAttr)) {
        // Check if default value of attribute must be set
        if (myTextFieldStrings->getText().empty() && GNEAttributeCarrier::hasDefaultValue(myTag, myAttr)) {
            newVal = GNEAttributeCarrier::getDefaultValue<std::string>(myTag, myAttr);
            myTextFieldStrings->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldStrings
            newVal = myTextFieldStrings->getText().text();
        }
    }

    // Check if attribute must be changed
    if (myInspectorFrameParent->getACs().front()->isValid(myAttr, newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myInspectorFrameParent->getACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        // Set new value of attribute in all selected ACs
        for (std::vector<GNEAttributeCarrier*>::const_iterator it_ac = myInspectorFrameParent->getACs().begin(); it_ac != myInspectorFrameParent->getACs().end(); it_ac++) {
            (*it_ac)->setAttribute(myAttr, newVal, myInspectorFrameParent->getViewNet()->getUndoList());
        }
        // finish change multiple attributes
        if (myInspectorFrameParent->getACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_end();
        }
        // If previously value of TextField was red, change color to black
        if ((GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr)) && myTextFieldStrings != 0) {
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myTextFieldReal->killFocus();
        } else if (GNEAttributeCarrier::isInt(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myTextFieldInt->killFocus();
        } else if (GNEAttributeCarrier::isString(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
        }
    } else {
        // IF value of TextField isn't valid, change color to Red depending of type
        if ((GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr)) && myTextFieldStrings != 0) {
            myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        } else if (GNEAttributeCarrier::isInt(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        } else if (GNEAttributeCarrier::isString(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
        }

        // Write Warning in console if we're in testing mode
        if (myInspectorFrameParent->getViewNet()->isTestingModeEnabled() == true) {
            WRITE_WARNING("Value '" + newVal + "' for attribute " + toString(myAttr) + " of " + toString(myTag) + " isn't valid");
        }
    }
    // Update view net
    myInspectorFrameParent->getViewNet()->update();
    return 1;
}


void
GNEInspectorFrame::AttributeInput::show() {
    FXHorizontalFrame::show();
}


void
GNEInspectorFrame::AttributeInput::hide() {
    FXHorizontalFrame::hide();
}

/****************************************************************************/
