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

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>


#include "GNEFrame.h"
#include "GNEViewParent.h"
#include "GNEViewNet.h"
#include "GNEAttributeCarrier.h"
#include "GNEInspectorFrame.h"
#include "GNEPolygonFrame.h"
#include "GNEDeleteFrame.h"


#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNEAdditional.h"
#include "GNECrossing.h"
#include "GNEPOI.h"
#include "GNEPoly.h"
#include "GNEPOILane.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrame::NeteditAttributes) GNEFrameNeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_BLOCKMOVEMENT,  GNEFrame::NeteditAttributes::onCmdSetBlockMovement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_BLOCKING_SHAPE,             GNEFrame::NeteditAttributes::onCmdSetBlockShape),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_CLOSE,             GNEFrame::NeteditAttributes::onCmdsetClosingShape),
};

FXDEFMAP(GNEFrame::DrawingMode) GNEFrameDrawingModeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_STARTDRAWING,      GNEFrame::DrawingMode::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_STOPDRAWING,       GNEFrame::DrawingMode::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_ABORTDRAWING,      GNEFrame::DrawingMode::onCmdAbortDrawing),
};

FXDEFMAP(GNEFrame::ACHierarchy) GNEFrameACHierarchyMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_CENTER,          GNEFrame::ACHierarchy::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_INSPECT,         GNEFrame::ACHierarchy::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_DELETE,          GNEFrame::ACHierarchy::onCmdDeleteItem),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_DELETEFRAME_CHILDS,             GNEFrame::ACHierarchy::onCmdShowChildMenu),

};

// Object implementation
FXIMPLEMENT(GNEFrame::NeteditAttributes,    FXGroupBox,     GNEFrameNeteditAttributesMap,   ARRAYNUMBER(GNEFrameNeteditAttributesMap))
FXIMPLEMENT(GNEFrame::DrawingMode,          FXGroupBox,     GNEFrameDrawingModeMap,         ARRAYNUMBER(GNEFrameDrawingModeMap))
FXIMPLEMENT(GNEFrame::ACHierarchy,          FXGroupBox,     GNEFrameACHierarchyMap,         ARRAYNUMBER(GNEFrameACHierarchyMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrame::NeteditAttributes - methods
// ---------------------------------------------------------------------------

GNEFrame::NeteditAttributes::NeteditAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame) {
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockMovementLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_ADDITIONALFRAME_BLOCKMOVEMENT, GUIDesignCheckButtonAttribute);
    myBlockMovementCheckButton->setCheck(false);
    // Create Frame for block shape label and checkBox (By default disabled)
    myBlockShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockShapeLabel = new FXLabel(myBlockShapeFrame, "block shape", 0, GUIDesignLabelAttribute);
    myBlockShapeCheckButton = new FXCheckButton(myBlockShapeFrame, "false", this, MID_GNE_SET_BLOCKING_SHAPE, GUIDesignCheckButtonAttribute);
    // Create Frame for block close polygon and checkBox (By default disabled)
    myClosePolygonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myClosePolygonLabel = new FXLabel(myClosePolygonFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myClosePolygonCheckButton = new FXCheckButton(myClosePolygonFrame, "false", this, MID_GNE_POLYGONFRAME_CLOSE, GUIDesignCheckButtonAttribute);
    myBlockShapeCheckButton->setCheck(false);
}


GNEFrame::NeteditAttributes::~NeteditAttributes() {}


void
GNEFrame::NeteditAttributes::showNeteditAttributes(bool shapeEditing) {
    // show block and closing sahpe depending of shapeEditing
    if (shapeEditing) {
        myBlockShapeFrame->show();
        myClosePolygonFrame->show();
    } else {
        myBlockShapeFrame->hide();
        myClosePolygonFrame->hide();
    }
    FXGroupBox::show();
}


void
GNEFrame::NeteditAttributes::hideNeteditAttributes() {
    FXGroupBox::hide();
}


bool
GNEFrame::NeteditAttributes::isBlockMovementEnabled() const {
    return myBlockMovementCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEFrame::NeteditAttributes::isBlockShapeEnabled() const {
    return myBlockShapeCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEFrame::NeteditAttributes::isCloseShapeEnabled() const {
    return myClosePolygonCheckButton->getCheck() == 1 ? true : false;
}


long
GNEFrame::NeteditAttributes::onCmdSetBlockMovement(FXObject*, FXSelector, void*) {
    if (myBlockMovementCheckButton->getCheck()) {
        myBlockMovementCheckButton->setText("true");
    } else {
        myBlockMovementCheckButton->setText("false");
    }
    return 1;
}


long
GNEFrame::NeteditAttributes::onCmdSetBlockShape(FXObject*, FXSelector, void*) {
    if (myBlockShapeCheckButton->getCheck()) {
        myBlockShapeCheckButton->setText("true");
    } else {
        myBlockShapeCheckButton->setText("false");
    }
    return 1;
}


long
GNEFrame::NeteditAttributes::onCmdsetClosingShape(FXObject*, FXSelector, void*) {
    if (myClosePolygonCheckButton->getCheck()) {
        myClosePolygonCheckButton->setText("true");
    } else {
        myClosePolygonCheckButton->setText("false");
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::DrawingMode - methods
// ---------------------------------------------------------------------------

GNEFrame::DrawingMode::DrawingMode(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Drawing", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(this, "Start drawing", 0, this, MID_GNE_POLYGONFRAME_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(this, "Stop drawing", 0, this, MID_GNE_POLYGONFRAME_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(this, "Abort drawing", 0, this, MID_GNE_POLYGONFRAME_ABORTDRAWING, GUIDesignButton);

    // create information label
    std::ostringstream information;
    information
            << "- 'Start drawing' or ENTER\n"
            << "  draws polygon boundary.\n"
            << "- 'Stop drawing' or ENTER\n"
            << "  creates polygon.\n"
            << "- 'Abort drawing' or ESC\n"
            << "  removes drawed polygon.";
    myInformationLabel = new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEFrame::DrawingMode::~DrawingMode() {}


void GNEFrame::DrawingMode::showDrawingMode() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEFrame::DrawingMode::hideDrawingMode() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrame::DrawingMode::startDrawing() {
    // Only start drawing if DrawingMode modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrame::DrawingMode::stopDrawing() {
    // check if shape has to be closed
    if (myFrameParent->getNeteditAttributes()->isCloseShapeEnabled()) {
        myTemporalShapeShape.closePolygon();
    }
    // try to build polygon
    /**
    NOTE: This solution using dynamic_cast is provisional, and has to be changed
          for task #1112.
    **/
    if (dynamic_cast<GNEPolygonFrame*>(myFrameParent)->buildPoly(myTemporalShapeShape)) {
        // clear created points
        myTemporalShapeShape.clear();
        myFrameParent->getViewNet()->update();
        // change buttons
        myStartDrawingButton->enable();
        myStopDrawingButton->disable();
        myAbortDrawingButton->disable();
    } else {
        // abort drawing if polygon cannot be created
        abortDrawing();
    }
}


void
GNEFrame::DrawingMode::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->getViewNet()->update();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrame::DrawingMode::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrame::DrawingMode::removeLastPoint() {
    if (myStopDrawingButton->isEnabled()) {
        if (myTemporalShapeShape.size() > 0) {
            myTemporalShapeShape.pop_back();
        }
    } else {
        throw ProcessError("Last point cannot be removed if drawing wasn't started");
    }
}


const PositionVector&
GNEFrame::DrawingMode::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrame::DrawingMode::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


long
GNEFrame::DrawingMode::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrame::DrawingMode::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrame::DrawingMode::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrame::ACHierarchy - methods
// ---------------------------------------------------------------------------

GNEFrame::ACHierarchy::ACHierarchy(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Childs", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    myTreelist = new FXTreeList(this, this, MID_GNE_DELETEFRAME_CHILDS, GUIDesignTreeListFrame);
    hide();
}


GNEFrame::ACHierarchy::~ACHierarchy() {}


void 
GNEFrame::ACHierarchy::showACHierarchy(GNEAttributeCarrier* AC) {
    myAC = AC;
    show();
}


void 
GNEFrame::ACHierarchy::hideACHierarchy() {
    hide();
}


long 
GNEFrame::ACHierarchy::onCmdShowChildMenu(FXObject*, FXSelector, void* eventData) {
    // Obtain event
    FXEvent* e = (FXEvent*)eventData;
    FXTreeItem* item = myTreelist->getItemAt(e->win_x, e->win_y);
    // Check if there are an item in the position and create pop-up menu
    if (item && (myTreeItemsWithoutAC.find(item) == myTreeItemsWithoutAC.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[myTreelist->getItemAt(e->win_x, e->win_y)]);
    }
    return 1;
}


long 
GNEFrame::ACHierarchy::onCmdCenterItem(FXObject*, FXSelector, void*) {
    if (myRightClickedAC != NULL) {
        myFrameParent->getViewNet()->centerTo(myRightClickedAC->getGUIGLObject()->getGlID(), false);
        myFrameParent->getViewNet()->update();
    }
    return 1;
}


long 
GNEFrame::ACHierarchy::onCmdInspectItem(FXObject*, FXSelector, void*) {
    if ((myAC != NULL) && (myRightClickedAC != NULL)) {
        myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectChild(myRightClickedAC, myAC);
    }
    return 1;
}


long 
GNEFrame::ACHierarchy::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    // Remove Attribute Carrier
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->show();
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->removeAttributeCarrier(myRightClickedAC);
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->hide();
    // show again childs of attribute carrier
    showAttributeCarrierChilds();
    return 1;
}


void
GNEFrame::ACHierarchy::createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac) {
    // create FXMenuPane
    FXMenuPane* pane = new FXMenuPane(myTreelist);
    // set current clicked AC
    myRightClickedAC = ac;
    // set name
    new MFXMenuHeader(pane, myFrameParent->getViewNet()->getViewParent()->getApp()->getBoldFont(), (toString(myRightClickedAC->getTag()) + ": " + myRightClickedAC->getID()).c_str(), myRightClickedAC->getIcon());
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


void
GNEFrame::ACHierarchy::showAttributeCarrierChilds() {

    /** NOTE: This function has to be simplified **/

    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsWithoutAC.clear();
    show();
    // Switch gl type of ac
    switch (myAC->getGUIGLObject()->getType()) {
        case GLO_JUNCTION: {
            // insert junction root
            GNEJunction* junction = dynamic_cast<GNEJunction*>(myAC);
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
            GNEEdge* edge = dynamic_cast<GNEEdge*>(myAC);
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
            GNELane* lane = dynamic_cast<GNELane*>(myAC);
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
            // check type of POI
            if (myAC->getTag() == SUMO_TAG_POI) {
                // insert POI root
                GNEPOI* POI = dynamic_cast<GNEPOI*>(myAC);
                FXTreeItem* POIItem = myTreelist->insertItem(0, 0, toString(POI->getTag()).c_str(), POI->getIcon(), POI->getIcon());
                myTreeItemToACMap[POIItem] = POI;
                POIItem->setExpanded(true);
                break;
            }
            else {
                // insert POILane root
                GNEPOILane* POILane = dynamic_cast<GNEPOILane*>(myAC);
                FXTreeItem* POILaneItem = myTreelist->insertItem(0, 0, toString(POILane->getTag()).c_str(), POILane->getIcon(), POILane->getIcon());
                myTreeItemToACMap[POILaneItem] = POILane;
                POILaneItem->setExpanded(true);
                break;
            }
        }
        case GLO_POLYGON: {
            // insert polygon root
            GNEPoly* polygon = dynamic_cast<GNEPoly*>(myAC);
            FXTreeItem* polygonItem = myTreelist->insertItem(0, 0, toString(polygon->getTag()).c_str(), polygon->getIcon(), polygon->getIcon());
            myTreeItemToACMap[polygonItem] = polygon;
            polygonItem->setExpanded(true);
            break;
        }
        case GLO_CROSSING: {
            // insert crossing root
            GNECrossing* crossing = dynamic_cast<GNECrossing*>(myAC);
            FXTreeItem* crossingItem = myTreelist->insertItem(0, 0, toString(crossing->getTag()).c_str(), crossing->getIcon(), crossing->getIcon());
            myTreeItemToACMap[crossingItem] = crossing;
            crossingItem->setExpanded(true);
            break;
        }
        case GLO_ADDITIONAL: {
            // insert additional root
            GNEAdditional* additional = dynamic_cast<GNEAdditional*>(myAC);
            FXTreeItem* additionalItem = myTreelist->insertItem(0, 0, toString(additional->getTag()).c_str(), additional->getIcon(), additional->getIcon());
            myTreeItemToACMap[additionalItem] = additional;
            additionalItem->setExpanded(true);
            break;
        }
        case GLO_CONNECTION: {
            // insert connection root
            GNEConnection* connection = dynamic_cast<GNEConnection*>(myAC);
            FXTreeItem* connectionItem = myTreelist->insertItem(0, 0, toString(connection->getTag()).c_str(), connection->getIcon(), connection->getIcon());
            myTreeItemToACMap[connectionItem] = connection;
            connectionItem->setExpanded(true);
            break;
        }
        default: {
            hide();
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// GNEFrame - methods
// ---------------------------------------------------------------------------

GNEFrame::GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel) :
    FXVerticalFrame(horizontalFrameParent, GUIDesignAuxiliarFrame),
    myViewNet(viewNet),
    myNeteditAttributes(NULL),
    myDrawingMode(NULL) {

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


GNEFrame::NeteditAttributes*
GNEFrame::getNeteditAttributes() const {
    if (myNeteditAttributes) {
        return myNeteditAttributes;
    } else {
        throw ProcessError("Netedit Attributes editor wasn't created");
    }
}


GNEFrame::DrawingMode*
GNEFrame::getDrawingMode() const {
    if (myDrawingMode) {
        return myDrawingMode;
    } else {
        throw ProcessError("Drawing Mode editor wasn't created");
    }
}

/****************************************************************************/
