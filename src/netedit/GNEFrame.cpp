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
#include "GNENet.h"


#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNEAdditional.h"
#include "GNECrossing.h"
#include "GNEPOI.h"
#include "GNEPoly.h"

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
    FXGroupBox(frameParent->myContentFrame, "Hierarchy", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // Create three list
    myTreelist = new FXTreeList(this, this, MID_GNE_DELETEFRAME_CHILDS, GUIDesignTreeListFrame);
    hide();
}


GNEFrame::ACHierarchy::~ACHierarchy() {}


void 
GNEFrame::ACHierarchy::showACHierarchy(GNEAttributeCarrier* AC) {
    myAC = AC;
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsWithoutAC.clear();
    show();

    // show ACChilds
    showAttributeCarrierChilds(myAC, showAttributeCarrierParents(), 0);


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
    showAttributeCarrierChilds(NULL, NULL, 0);
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


FXTreeItem* 
GNEFrame::ACHierarchy::showAttributeCarrierParents() {
    // Switch gl type of ac
    switch (myAC->getGUIGLObject()->getType()) {
        case GLO_JUNCTION: {
            // Junctions don't have parents
            return NULL;
        }
        case GLO_EDGE: {
            // obtain Edge
            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(myAC->getID());
            // insert Junctions of edge in tree (Pararell because a edge has always two Junctions)
            FXTreeItem* junctionSourceItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
            FXTreeItem* junctionDestinyItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
            junctionDestinyItem->setExpanded(true);
            // Save items in myTreeItemToACMap
            myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
            myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
            // return junction destiny Item
            return junctionDestinyItem;
        }
        case GLO_LANE: {
            // obtain lane
            GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(myAC->getID());
            // obtain edge parent
            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
            //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
            FXTreeItem* junctionSourceItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
            FXTreeItem* junctionDestinyItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
            junctionDestinyItem->setExpanded(true);
            // Create edge item
            FXTreeItem* edgeItem = myTreelist->insertItem(NULL, junctionDestinyItem, toString(edge->getTag()).c_str(), edge->getIcon(), edge->getIcon());
            edgeItem->setExpanded(true);
            // Save items in myTreeItemToACMap
            myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
            myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
            myTreeItemToACMap[edgeItem] = edge;
            // return edge item
            return edgeItem;
        }
        case GLO_POI: {
            // check type of POI
            if (myAC->getTag() == SUMO_TAG_POI) {
                // POIs don't have lane parents
                return NULL;
            } else {
                // Obtain GNEPOI
                GNEPOI* GNEPOI = myFrameParent->getViewNet()->getNet()->retrievePOI(myAC->getID());
                // obtain lane parent
                GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(GNEPOI->getLane()->getID());
                // obtain edge parent
                GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Create edge item
                FXTreeItem* edgeItem = myTreelist->insertItem(NULL, junctionDestinyItem, toString(edge->getTag()).c_str(), edge->getIcon(), edge->getIcon());
                edgeItem->setExpanded(true);
                // Create lane item
                FXTreeItem* laneItem = myTreelist->insertItem(0, edgeItem, toString(lane->getTag()).c_str(), lane->getIcon(), lane->getIcon());
                laneItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                myTreeItemToACMap[edgeItem] = edge;
                myTreeItemToACMap[laneItem] = lane;
                // return Lane item
                return laneItem;
            }
        }
        case GLO_POLYGON: {
            // Polygons doesn't have parents
            return NULL;
        }
        case GLO_CROSSING: {
            // obtain Crossing
            GNECrossing* crossing = myFrameParent->getViewNet()->getNet()->retrieveCrossing(myAC->getID());
            // obtain junction
            GNEJunction* junction = crossing->getParentJunction();
            // create junction item
            FXTreeItem* junctionItem = myTreelist->insertItem(NULL, NULL, toString(junction->getTag()).c_str(), junction->getIcon(), junction->getIcon());
            junctionItem->setExpanded(true);
            // Save items in myTreeItemToACMap
            myTreeItemToACMap[junctionItem] = junction;
            // return junction Item
            return junctionItem;
        }
        case GLO_ADDITIONAL: {
            // Obtain Additional
            GNEAdditional* additional = myFrameParent->getViewNet()->getNet()->retrieveAdditional(myAC->getID());
            // first check if additional has another additional as parent (to add it into root)
             if (GNEAttributeCarrier::canHaveParent(additional->getTag())) {
                 GNEAdditional* additionalParent = myFrameParent->getViewNet()->getNet()->retrieveAdditional(additional->getAttribute(GNE_ATTR_PARENT));
                 // create additional parent item
                 FXTreeItem* additionalParentItem = myTreelist->insertItem(0, 0, toString(additionalParent->getTag()).c_str(), additionalParent->getIcon(), additionalParent->getIcon());
                 additionalParentItem->setExpanded(true);
                 // Save it in myTreeItemToACMap
                 myTreeItemToACMap[additionalParentItem] = additionalParent;
             }
            if(additional->hasAttribute(additional->getTag(), SUMO_ATTR_EDGE)) {
                // obtain edge parent
                GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(additional->getAttribute(SUMO_ATTR_EDGE));
                //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Create edge item
                FXTreeItem* edgeItem = myTreelist->insertItem(NULL, junctionDestinyItem, toString(edge->getTag()).c_str(), edge->getIcon(), edge->getIcon());
                edgeItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                myTreeItemToACMap[edgeItem] = edge;
                // return edge item
                return edgeItem;
            } else if (additional->hasAttribute(additional->getTag(), SUMO_ATTR_LANE)) {
                // obtain lane parent
                GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(additional->getAttribute(SUMO_ATTR_LANE));
                // obtain edge parent
                GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(NULL, NULL, (toString(edge->getGNEJunctionSource()->getTag()) + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Create edge item
                FXTreeItem* edgeItem = myTreelist->insertItem(NULL, junctionDestinyItem, toString(edge->getTag()).c_str(), edge->getIcon(), edge->getIcon());
                edgeItem->setExpanded(true);
                // Create lane item
                FXTreeItem* laneItem = myTreelist->insertItem(0, edgeItem, toString(lane->getTag()).c_str(), lane->getIcon(), lane->getIcon());
                laneItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                myTreeItemToACMap[edgeItem] = edge;
                myTreeItemToACMap[laneItem] = lane;
                // return lane item
                return laneItem;
            } else {
                // Certain additionals don't have parents
                return NULL;
            }
        }
        case GLO_CONNECTION: {
            // obtain Connection
            GNEConnection* connection = myFrameParent->getViewNet()->getNet()->retrieveConnection(myAC->getID());
            // create edge from item
            FXTreeItem* edgeFromItem = myTreelist->insertItem(NULL, NULL, toString(connection->getEdgeFrom()->getTag()).c_str(), connection->getEdgeFrom()->getIcon(), connection->getEdgeFrom()->getIcon());
            edgeFromItem->setExpanded(true);
            // create edge to item
            FXTreeItem* edgeToItem = myTreelist->insertItem(NULL, NULL, toString(connection->getEdgeTo()->getTag()).c_str(), connection->getEdgeTo()->getIcon(), connection->getEdgeTo()->getIcon());
            edgeToItem->setExpanded(true);
            // create connection item
            FXTreeItem* connectionItem = myTreelist->insertItem(0, edgeToItem, toString(connection->getTag()).c_str(), connection->getIcon(), connection->getIcon());
            connectionItem->setExpanded(true);
            // Save items in myTreeItemToACMap
            myTreeItemToACMap[edgeFromItem] = connection->getEdgeFrom();
            myTreeItemToACMap[edgeToItem] = connection->getEdgeTo();
            myTreeItemToACMap[connectionItem] = connection;
            // return connection item
            return connectionItem;
        }
        default: {
            throw ProcessError("Invalid GUIGLObject type");
        }
    }
}


void
GNEFrame::ACHierarchy::showAttributeCarrierChilds(GNEAttributeCarrier *AC, FXTreeItem* itemParent, int index) {
    // Switch gl type of ac
    switch (AC->getGUIGLObject()->getType()) {
        case GLO_JUNCTION: {
            // insert junction item
            FXTreeItem* junctionItem = addACIntoList(AC, itemParent, index);
            // retrieve junction
            GNEJunction* junction = myFrameParent->getViewNet()->getNet()->retrieveJunction(AC->getID());
            // insert edges
            for (int i = 0; i < (int)junction->getGNEEdges().size(); i++) {
                showAttributeCarrierChilds(junction->getGNEEdges().at(i), junctionItem, i);
            }
            // insert crossings
            for (int i = 0; i < (int)junction->getGNECrossings().size(); i++) {
                showAttributeCarrierChilds(junction->getGNECrossings().at(i), junctionItem, i);
            }
            break;
        }
        case GLO_EDGE: {
            // insert edge item
            FXTreeItem* edgeItem = addACIntoList(AC, itemParent, index);
            // retrieve edge
            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(AC->getID());
            // insert lanes
            for (int i = 0; i < (int)edge->getLanes().size(); i++) {
                showAttributeCarrierChilds(edge->getLanes().at(i), edgeItem, i);
            }
            // insert additionals of edge
            for (int i = 0; i < (int)edge->getAdditionalChilds().size(); i++) {
                showAttributeCarrierChilds(edge->getAdditionalChilds().at(i), edgeItem, i);
            }
            break;
        }
        case GLO_LANE: {
            // insert lane item
            FXTreeItem* laneItem = addACIntoList(AC, itemParent, index);
            // retrieve lane
            GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(AC->getID());
            // insert additionals of lanes
            for (int i = 0; i < (int)lane->getAdditionalChilds().size(); i++) {
                showAttributeCarrierChilds(lane->getAdditionalChilds().at(i), laneItem, i);
            }
            // insert incoming connections of lanes (by default isn't expanded)
            if (lane->getGNEIncomingConnections().size() > 0) {
                FXTreeItem* incomingConnections = myTreelist->insertItem(0, laneItem, "Incomings", lane->getGNEIncomingConnections().front()->getIcon(), lane->getGNEIncomingConnections().front()->getIcon());
                myTreeItemsWithoutAC.insert(incomingConnections);
                incomingConnections->setExpanded(false);
                // insert incoming connections
                for (int i = 0; i < (int)lane->getGNEIncomingConnections().size(); i++) {
                    showAttributeCarrierChilds(lane->getGNEIncomingConnections().at(i), incomingConnections, i);
                }
            }
            // insert outcoming connections of lanes (by default isn't expanded)
            if (lane->getGNEOutcomingConnections().size() > 0) {
                FXTreeItem* outgoingConnections = myTreelist->insertItem(0, laneItem, "Outcomings", lane->getGNEOutcomingConnections().front()->getIcon(), lane->getGNEOutcomingConnections().front()->getIcon());
                myTreeItemsWithoutAC.insert(outgoingConnections);
                outgoingConnections->setExpanded(false);
                // insert outcoming connections
                for (int i = 0; i < (int)lane->getGNEOutcomingConnections().size(); i++) {
                    showAttributeCarrierChilds(lane->getGNEOutcomingConnections().at(i), outgoingConnections, i);
                }
            }
            break;
        }
        case GLO_POI: {
            // check type of POI
            if (AC->getTag() == SUMO_TAG_POI) {
                // insert POI item
                addACIntoList(AC, itemParent, index);
            } else {
                // insert POILane item
                addACIntoList(AC, itemParent, index);
            }
            break;
        }
        case GLO_POLYGON: {
            // retrieve polygon
            addACIntoList(AC, itemParent, index);
            break;
        }
        case GLO_CROSSING: {
            // insert crossing item
            addACIntoList(AC, itemParent, index);
            break;
        }
        case GLO_ADDITIONAL: {
            // insert additional item
            addACIntoList(AC, itemParent, index);
            // retrieve additional
            myFrameParent->getViewNet()->getNet()->retrieveAdditional(AC->getID());
            break;
        }
        case GLO_CONNECTION: {
            // insert connection item
            addACIntoList(AC, itemParent, index);
            break;
        }
        default: {
            throw ProcessError("Invalid GUIGLObject type");
        }
    }
}


FXTreeItem*
GNEFrame::ACHierarchy::addACIntoList(GNEAttributeCarrier *AC, FXTreeItem* itemParent, int /* index */) {
    FXTreeItem* item = myTreelist->insertItem(0, itemParent, toString(AC->getTag()).c_str(), AC->getIcon(), AC->getIcon());
    myTreeItemToACMap[itemParent] = AC;
    item->setExpanded(true);
    return item;
}

// ---------------------------------------------------------------------------
// GNEFrame::HelpAttributes
// ---------------------------------------------------------------------------

GNEFrame::HelpAttributes::HelpAttributes(FXWindow* parent, SumoXMLTag tag) :
    FXDialogBox(parent, ("Parameters of " + toString(tag)).c_str(), GUIDesignDialogBoxResizable, 0, 0, 0, 0, 10, 10, 10, 38, 4, 4) {
    // Create FXTable
    FXTable* myTable = new FXTable(this, this, MID_TABLE, GUIDesignTableNotEditable);
    auto attrs = GNEAttributeCarrier::allowedAttributes(tag);
    myTable->setVisibleRows((FXint)(attrs.size()));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(attrs.size()), 4);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Attribute");
    myTable->setColumnText(1, "Type");
    myTable->setColumnText(2, "Restriction");
    myTable->setColumnText(3, "Definition");
    myTable->getRowHeader()->setWidth(0);
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, 90);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(2, 80);
    int maxSizeColumnDefinitions = 0;
    // Iterate over vector of additional parameters
    for (int i = 0; i < attrs.size(); i++) {
        // Set attribute 
        FXTableItem* attribute = new FXTableItem(toString(attrs.at(i).first).c_str());
        attribute->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 0, attribute);
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (attrs.at(i).first == SUMO_ATTR_SHAPE) {
            type->setText("list of positions");
        } else if (GNEAttributeCarrier::isInt(tag, attrs.at(i).first)) {
            type->setText("int");
        } else if (GNEAttributeCarrier::isFloat(tag, attrs.at(i).first)) {
            type->setText("float");
        } else if (GNEAttributeCarrier::isTime(tag, attrs.at(i).first)) {
            type->setText("time");
        } else if (GNEAttributeCarrier::isBool(tag, attrs.at(i).first)) {
            type->setText("bool");
        } else if (GNEAttributeCarrier::isColor(tag, attrs.at(i).first)) {
            type->setText("color");
        } else if (GNEAttributeCarrier::isString(tag, attrs.at(i).first)) {
            if (attrs.at(i).first == SUMO_ATTR_POSITION) {
                type->setText("position");
            } else {
                type->setText("string");
            }
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set restriction
        FXTableItem* restriction = new FXTableItem(GNEAttributeCarrier::getRestriction(tag, attrs.at(i).first).c_str());
        restriction->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 2, restriction);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(tag, attrs.at(i).first).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 3, definition);
        if ((int)GNEAttributeCarrier::getDefinition(tag, attrs.at(i).first).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(tag, attrs.at(i).first).size());
        }
    }
 
    // Set size of column
    header->setItemJustify(3, JUSTIFY_CENTER_X);
    header->setItemSize(3, maxSizeColumnDefinitions * 6);
    // Create horizontal separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // create Dialog
    create();
    // show in the given position
    show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(this);

    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Opening HelpAttributes dialog for tag '" + toString(tag) + "' showing " + toString(attrs.size()) + " attributes");
    }
}


GNEFrame::HelpAttributes::~HelpAttributes() {
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Closing HelpAttributes dialog");
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
