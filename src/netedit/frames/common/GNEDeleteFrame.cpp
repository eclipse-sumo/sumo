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
/// @file    GNEDeleteFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2016
///
// The Widget for remove network-elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDeleteFrame.h"


// ---------------------------------------------------------------------------
// GNEDeleteFrame::DeleteOptions - methods
// ---------------------------------------------------------------------------

GNEDeleteFrame::DeleteOptions::DeleteOptions(GNEDeleteFrame* deleteFrameParent) :
    FXGroupBox(deleteFrameParent->myContentFrame, "Options", GUIDesignGroupBoxFrame) {

    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myDeleteOnlyGeometryPoints = new FXCheckButton(this, "Delete only geometryPoints", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myDeleteOnlyGeometryPoints->setCheck(FALSE);

    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectAdditionals = new FXCheckButton(this, "Protect additional elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectAdditionals->setCheck(TRUE);

    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectShapes = new FXCheckButton(this, "Protect shape elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectShapes->setCheck(TRUE);

    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectDemandElements = new FXCheckButton(this, "Protect demand elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectDemandElements->setCheck(TRUE);

    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectGenericDatas = new FXCheckButton(this, "Protect data elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectGenericDatas->setCheck(TRUE);
}


GNEDeleteFrame::DeleteOptions::~DeleteOptions() {}


bool
GNEDeleteFrame::DeleteOptions::deleteOnlyGeometryPoints() const {
    return (myDeleteOnlyGeometryPoints->getCheck() == TRUE);
}


bool 
GNEDeleteFrame::DeleteOptions::protectAdditionals() const {
    return (myProtectAdditionals->getCheck() == TRUE);
}


bool 
GNEDeleteFrame::DeleteOptions::protectShapes() const {
    return (myProtectShapes->getCheck() == TRUE);
}


bool
GNEDeleteFrame::DeleteOptions::protectDemandElements() const {
    return (myProtectDemandElements->getCheck() == TRUE);
}


bool
GNEDeleteFrame::DeleteOptions::protectGenericDatas() const {
    return (myProtectGenericDatas->getCheck() == TRUE);
}

// ===========================================================================
// method definitions
// ===========================================================================

GNEDeleteFrame::GNEDeleteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Delete") {
    // create delete options modul
    myDeleteOptions = new DeleteOptions(this);

    // Create groupbox and tree list
    myAttributeCarrierHierarchy = new GNEFrameModuls::AttributeCarrierHierarchy(this);
}


GNEDeleteFrame::~GNEDeleteFrame() {}


void
GNEDeleteFrame::show() {
    if (myViewNet->getNet()->getSelectedAttributeCarriers(false).size() == 1) {
        myAttributeCarrierHierarchy->showAttributeCarrierHierarchy(*myViewNet->getNet()->getSelectedAttributeCarriers(false).begin());
    } else {
        myAttributeCarrierHierarchy->hideAttributeCarrierHierarchy();
    }
    GNEFrame::show();
}


void
GNEDeleteFrame::hide() {
    GNEFrame::hide();
}


void
GNEDeleteFrame::removeSelectedAttributeCarriers() {
    // first check if there is additional to remove
    if (ACsToDelete()) {
        // remove all selected attribute carrier susing the following parent-child sequence
        myViewNet->getUndoList()->p_begin("remove selected items");
        // disable update geometry
        myViewNet->getNet()->disableUpdateGeometry();
        // delete selected attribute carriers depending of current supermode
        if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            //junctions
            while (myViewNet->getNet()->retrieveJunctions(true).size() > 0) {
                myViewNet->getNet()->deleteJunction(myViewNet->getNet()->retrieveJunctions(true).front(), myViewNet->getUndoList());
            }
            // edges
            while (myViewNet->getNet()->retrieveEdges(true).size() > 0) {
                myViewNet->getNet()->deleteEdge(myViewNet->getNet()->retrieveEdges(true).front(), myViewNet->getUndoList(), false);
            }
            // lanes
            while (myViewNet->getNet()->retrieveLanes(true).size() > 0) {
                myViewNet->getNet()->deleteLane(myViewNet->getNet()->retrieveLanes(true).front(), myViewNet->getUndoList(), false);
            }
            // connections
            while (myViewNet->getNet()->retrieveConnections(true).size() > 0) {
                myViewNet->getNet()->deleteConnection(myViewNet->getNet()->retrieveConnections(true).front(), myViewNet->getUndoList());
            }
            // crossings
            while (myViewNet->getNet()->retrieveCrossings(true).size() > 0) {
                myViewNet->getNet()->deleteCrossing(myViewNet->getNet()->retrieveCrossings(true).front(), myViewNet->getUndoList());
            }
            // shapes
            while (myViewNet->getNet()->retrieveShapes(true).size() > 0) {
                myViewNet->getNet()->deleteShape(myViewNet->getNet()->retrieveShapes(true).front(), myViewNet->getUndoList());
            }
            // additionals
            while (myViewNet->getNet()->retrieveAdditionals(true).size() > 0) {
                myViewNet->getNet()->deleteAdditional(myViewNet->getNet()->retrieveAdditionals(true).front(), myViewNet->getUndoList());
            }
        } else {
            // demand elements
            while (myViewNet->getNet()->retrieveDemandElements(true).size() > 0) {
                myViewNet->getNet()->deleteDemandElement(myViewNet->getNet()->retrieveDemandElements(true).front(), myViewNet->getUndoList());
            }
        }
        // enable update geometry
        myViewNet->getNet()->enableUpdateGeometry();
        // finish deletion
        myViewNet->getUndoList()->p_end();
    }
}


void
GNEDeleteFrame::removeAttributeCarrier(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool ignoreOptions) {
    // first check if there is at leas an AC under cursor)
    if (objectsUnderCursor.getAttributeCarrierFront()) {
        // disable update geometry
        myViewNet->getNet()->disableUpdateGeometry();
        // obtain clicked position
        Position clickedPosition = myViewNet->getPositionInformation();
        // first check if we'll only delete a geometry point
        if (myDeleteOptions->deleteOnlyGeometryPoints() && !ignoreOptions) {
            // check type of of object under cursor object with geometry points
            if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                if (objectsUnderCursor.getEdgeFront()->getVertexIndex(clickedPosition, false, false) != -1) {
                    objectsUnderCursor.getEdgeFront()->deleteGeometryPoint(clickedPosition);
                }
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_POLY) {
                if (objectsUnderCursor.getPolyFront()->getVertexIndex(clickedPosition, false, false) != -1) {
                    objectsUnderCursor.getPolyFront()->deleteGeometryPoint(clickedPosition);
                }
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_TAZ) {
                if (objectsUnderCursor.getTAZFront()->getVertexIndex(clickedPosition, false, false) != -1) {
                    objectsUnderCursor.getTAZFront()->deleteGeometryPoint(clickedPosition);
                }
            }
        } else {
            // check type of of object under cursor object
            if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
                // Check if junction can be deleted
                if (ignoreOptions || SubordinatedElements(objectsUnderCursor.getJunctionFront()).checkElements(myDeleteOptions)) {
                    myViewNet->getNet()->deleteJunction(objectsUnderCursor.getJunctionFront(), myViewNet->getUndoList());
                }
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                // check if click was over a geometry point or over a shape's edge
                if (objectsUnderCursor.getEdgeFront()->getVertexIndex(clickedPosition, false, false) != -1) {
                    objectsUnderCursor.getEdgeFront()->deleteGeometryPoint(clickedPosition);
                } else if (ignoreOptions || SubordinatedElements(objectsUnderCursor.getEdgeFront()).checkElements(myDeleteOptions)) {
                    // if all ok, then delete edge
                    myViewNet->getNet()->deleteEdge(objectsUnderCursor.getEdgeFront(), myViewNet->getUndoList(), false);
                }
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE) {
                // Check if edge can be deleted
                if (ignoreOptions || SubordinatedElements(objectsUnderCursor.getLaneFront()).checkElements(myDeleteOptions)) {
                    // if all ok, then delete lane
                    myViewNet->getNet()->deleteLane(objectsUnderCursor.getLaneFront(), myViewNet->getUndoList(), false);
                }
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_CROSSING) {
                myViewNet->getNet()->deleteCrossing(objectsUnderCursor.getCrossingFront(), myViewNet->getUndoList());
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_CONNECTION) {
                myViewNet->getNet()->deleteConnection(objectsUnderCursor.getConnectionFront(), myViewNet->getUndoList());
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_TAZ) {
                myViewNet->getNet()->deleteAdditional(objectsUnderCursor.getTAZFront(), myViewNet->getUndoList());
            } else if (objectsUnderCursor.getAttributeCarrierFront() && (objectsUnderCursor.getAdditionalFront() == objectsUnderCursor.getAttributeCarrierFront())) {
                myViewNet->getNet()->deleteAdditional(objectsUnderCursor.getAdditionalFront(), myViewNet->getUndoList());
            } else if (objectsUnderCursor.getShapeFront() && (objectsUnderCursor.getShapeFront() == objectsUnderCursor.getAttributeCarrierFront())) {
                myViewNet->getNet()->deleteShape(objectsUnderCursor.getShapeFront(), myViewNet->getUndoList());
            } else if (objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront() == objectsUnderCursor.getAttributeCarrierFront())) {
                myViewNet->getNet()->deleteDemandElement(objectsUnderCursor.getDemandElementFront(), myViewNet->getUndoList());
            } else if (objectsUnderCursor.getGenericDataElementFront() && (objectsUnderCursor.getGenericDataElementFront() == objectsUnderCursor.getAttributeCarrierFront())) {
                myViewNet->getNet()->deleteGenericData(objectsUnderCursor.getGenericDataElementFront(), myViewNet->getUndoList());
            }
        }
        // enable update geometry
        myViewNet->getNet()->enableUpdateGeometry();
        // update view to show changes
        myViewNet->update();
    }
}


GNEDeleteFrame::DeleteOptions*
GNEDeleteFrame::getDeleteOptions() const {
    return myDeleteOptions;
}

// ---------------------------------------------------------------------------
// GNEDeleteFrame::SubordinatedElements - methods
// ---------------------------------------------------------------------------

GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEJunction* junction) :
    SubordinatedElements(junction, junction->getNet()->getViewNet(), junction, junction) {
    // add the number of subodinated elements of child edges
    for (const auto& edge : junction->getGNEEdges()) {
        add(this, edge);
    }
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEEdge* edge) :
    SubordinatedElements(edge, edge->getNet()->getViewNet(), edge, edge) {
    // add the number of subodinated elements of child lanes
    for (const auto& lane : edge->getLanes()) {
        add(this, lane);
    }
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNELane* lane) :
    SubordinatedElements(lane, lane->getNet()->getViewNet(), lane, lane) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEAdditional* additional) :
    SubordinatedElements(additional, additional->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEShape* shape) :
    SubordinatedElements(shape, shape->getNet()->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEDemandElement* demandElement) :
    SubordinatedElements(demandElement, demandElement->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEGenericData* genericData) :
    SubordinatedElements(genericData, genericData->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::~SubordinatedElements() {}


bool
GNEDeleteFrame::SubordinatedElements::checkElements(const DeleteOptions* deleteOptions) {
    // check every parent/child
    if ((additionalParents > 0) && deleteOptions->protectAdditionals()) {
        openWarningDialog("additional", additionalParents, false);
    } else if ((additionalChilds > 0) && deleteOptions->protectAdditionals()) {
        openWarningDialog("additional", additionalChilds, true);
    } else if ((shapeParents > 0) && deleteOptions->protectShapes()) {
        openWarningDialog("shape", shapeParents, false);
    } else if ((shapeChilds > 0) && deleteOptions->protectShapes()) {
        openWarningDialog("shape", shapeChilds, true);
    } else if ((demandElementParents > 0) && deleteOptions->protectDemandElements()) {
        openWarningDialog("demand", demandElementParents, false);
    } else if ((demandElementChilds > 0) && deleteOptions->protectDemandElements()) {
        openWarningDialog("demand", demandElementChilds, true);
    } else if ((genericDataParents > 0) && deleteOptions->protectGenericDatas()) {
        openWarningDialog("data", genericDataParents, false);
    } else if ((genericDataChilds > 0) && deleteOptions->protectGenericDatas()) {
        openWarningDialog("data", genericDataChilds, true);
    } else {
        // all checks ok, then return true, to remove element
        return true;
    }
    return false;
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEAttributeCarrier* attributeCarrier, GNEViewNet* viewNet) :
    myAttributeCarrier(attributeCarrier),
    myViewNet(viewNet),
    additionalParents(0),
    additionalChilds(0),
    shapeParents(0),
    shapeChilds(0),
    demandElementParents(0),
    demandElementChilds(0),
    genericDataParents(0),
    genericDataChilds(0) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEAttributeCarrier* attributeCarrier, GNEViewNet* viewNet,
    const GNEHierarchicalParentElements* hierarchicalParent,
    const GNEHierarchicalChildElements* hierarchicalChild) :
    myAttributeCarrier(attributeCarrier),
    myViewNet(viewNet),
    additionalParents(hierarchicalParent->getParentAdditionals().size()),
    additionalChilds(hierarchicalChild->getChildAdditionals().size()),
    shapeParents(hierarchicalParent->getParentShapes().size()),
    shapeChilds(hierarchicalChild->getChildShapes().size()),
    demandElementParents(hierarchicalParent->getParentDemandElements().size()),
    demandElementChilds(hierarchicalChild->getChildDemandElements().size()),
    genericDataParents(hierarchicalParent->getParentGenericDatas().size()),
    genericDataChilds(hierarchicalChild->getChildGenericDataElements().size()) {
    // add the number of subodinated elements of additionals, shapes, demand elements and generic datas
    for (const auto& additional : hierarchicalParent->getParentAdditionals()) {
        add(this, additional);
    }
    for (const auto& shape : hierarchicalParent->getParentShapes()) {
        add(this, shape);
    }
    for (const auto& demandElement : hierarchicalParent->getParentDemandElements()) {
        add(this, demandElement);
    }
    for (const auto& genericData : hierarchicalParent->getParentGenericDatas()) {
        add(this, genericData);
    }
    for (const auto& additional : hierarchicalChild->getChildAdditionals()) {
        add(this, additional);
    }
    for (const auto& shape : hierarchicalChild->getChildShapes()) {
        add(this, shape);
    }
    for (const auto& additional : hierarchicalChild->getChildDemandElements()) {
        add(this, additional);
    }
    for (const auto& genericData : hierarchicalChild->getChildGenericDataElements()) {
        add(this, genericData);
    }
}


void
GNEDeleteFrame::SubordinatedElements::add(SubordinatedElements* originalSE, const SubordinatedElements& newSE) {
    originalSE->additionalParents += newSE.additionalParents;
    originalSE->additionalChilds += newSE.additionalChilds;
    originalSE->shapeParents += newSE.shapeParents;
    originalSE->shapeChilds += newSE.shapeChilds;
    originalSE->demandElementParents += newSE.demandElementParents;
    originalSE->demandElementChilds += newSE.demandElementChilds;
    originalSE->genericDataParents += newSE.genericDataParents;
    originalSE->genericDataChilds += newSE.genericDataChilds;
}


void 
GNEDeleteFrame::SubordinatedElements::openWarningDialog(const std::string& type, const size_t number, const bool isChild) {
    // declare plural depending of "number"
    const std::string plural = (number > 1)? "s" : ""; 
    // declare header
    const std::string header = "Problem deleting " + myAttributeCarrier->getTagProperty().getTagStr() + " '" + myAttributeCarrier->getID() + "'";
    // declare message
    std::string message;
    // set message depending of isChild
    if (isChild) {
        message = myAttributeCarrier->getTagProperty().getTagStr() + " '" + myAttributeCarrier->getID() +
            "' cannot be deleted because has " + toString(number) + " " + type + " element" + plural + ".\n" + 
            "To delete it, uncheck 'protect " + type + " elements'.";
    } else {
        message = myAttributeCarrier->getTagProperty().getTagStr() + " '" + myAttributeCarrier->getID() +
            "' cannot be deleted because is part of " + toString(number) + " " + type + " element" + plural + ".\n" +
            "To delete it, uncheck 'protect " + type + " elements'.";
    }
    // write warning
    WRITE_DEBUG("Opened FXMessageBox " + header);
    // open message box
    FXMessageBox::warning(myViewNet->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Closed FXMessageBox " + header);
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame - protected methods
// ---------------------------------------------------------------------------

bool
GNEDeleteFrame::ACsToDelete() const {
    // invert selection of elements depending of current supermode
    if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        // iterate over junctions
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->junctions) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
            // due we iterate over all junctions, only it's neccesary iterate over incoming edges
            for (const auto& j : i.second->getGNEIncomingEdges()) {
                if (j->isAttributeCarrierSelected()) {
                    return true;
                }
                // check lanes
                for (auto k : j->getLanes()) {
                    if (k->isAttributeCarrierSelected()) {
                        return true;
                    }
                }
                // check connections
                for (const auto& k : j->getGNEConnections()) {
                    if (k->isAttributeCarrierSelected()) {
                        return true;
                    }
                }
            }
            // check crossings
            for (const auto& j : i.second->getGNECrossings()) {
                if (j->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
        // check additionals
        for (const auto& additionalTag : myViewNet->getNet()->getAttributeCarriers()->getAdditionals()) {
            for (const auto& additional : additionalTag.second) {
                if (additional.second->getTagProperty().isSelectable() && additional.second->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
        // check shapes
        for (const auto& shapeTag : myViewNet->getNet()->getAttributeCarriers()->getShapes()) {
            for (const auto& shape : shapeTag.second) {
                if (shape.second->getTagProperty().isSelectable() && shape.second->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
    } else {
        // check routes
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check vehicles
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VEHICLE)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check trips
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_TRIP)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check flows
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_FLOW)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check route flows
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTEFLOW)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check lane stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_LANE)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check bus stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_BUSSTOP)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check container stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CONTAINERSTOP)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check chargingstation stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_CHARGINGSTATION)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check parkingarea stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_STOP_PARKINGAREA)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
    }
    return false;
}


/****************************************************************************/
