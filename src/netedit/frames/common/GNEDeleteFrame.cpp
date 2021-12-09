/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDeleteFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDeleteFrame::DeleteOptions) DeleteOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDeleteFrame::DeleteOptions::onCmdSetOption),
};

// Object implementation
FXIMPLEMENT(GNEDeleteFrame::DeleteOptions, FXGroupBoxModule, DeleteOptionsMap, ARRAYNUMBER(DeleteOptionsMap))

// ---------------------------------------------------------------------------
// GNEDeleteFrame::DeleteOptions - methods
// ---------------------------------------------------------------------------

GNEDeleteFrame::DeleteOptions::DeleteOptions(GNEDeleteFrame* deleteFrameParent) :
    FXGroupBoxModule(deleteFrameParent->myContentFrame, "Options"),
    myDeleteFrameParent(deleteFrameParent) {
    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myDeleteOnlyGeometryPoints = new FXCheckButton(getCollapsableFrame(), "Delete geometry points", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myDeleteOnlyGeometryPoints->setCheck(FALSE);
}


GNEDeleteFrame::DeleteOptions::~DeleteOptions() {}


bool
GNEDeleteFrame::DeleteOptions::deleteOnlyGeometryPoints() const {
    return (myDeleteOnlyGeometryPoints->getCheck() == TRUE);
}


long
GNEDeleteFrame::DeleteOptions::onCmdSetOption(FXObject*, FXSelector, void*) {
    myDeleteFrameParent->getViewNet()->update();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEDeleteFrame::ProtectElements - methods
// ---------------------------------------------------------------------------

GNEDeleteFrame::ProtectElements::ProtectElements(GNEDeleteFrame* deleteFrameParent) :
    FXGroupBoxModule(deleteFrameParent->myContentFrame, "Protect Elements") {
    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectAdditionals = new FXCheckButton(getCollapsableFrame(), "Protect additional elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectAdditionals->setCheck(TRUE);
    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectTAZs = new FXCheckButton(getCollapsableFrame(), "Protect TAZ elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectTAZs->setCheck(TRUE);
    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectShapes = new FXCheckButton(getCollapsableFrame(), "Protect shape elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectShapes->setCheck(TRUE);
    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectDemandElements = new FXCheckButton(getCollapsableFrame(), "Protect demand elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectDemandElements->setCheck(TRUE);
    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectGenericDatas = new FXCheckButton(getCollapsableFrame(), "Protect data elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectGenericDatas->setCheck(TRUE);
}


GNEDeleteFrame::ProtectElements::~ProtectElements() {}


bool
GNEDeleteFrame::ProtectElements::protectAdditionals() const {
    return (myProtectAdditionals->getCheck() == TRUE);
}


bool
GNEDeleteFrame::ProtectElements::protectTAZs() const {
    return (myProtectTAZs->getCheck() == TRUE);
}


bool
GNEDeleteFrame::ProtectElements::protectShapes() const {
    return (myProtectShapes->getCheck() == TRUE);
}


bool
GNEDeleteFrame::ProtectElements::protectDemandElements() const {
    return (myProtectDemandElements->getCheck() == TRUE);
}


bool
GNEDeleteFrame::ProtectElements::protectGenericDatas() const {
    return (myProtectGenericDatas->getCheck() == TRUE);
}

// ===========================================================================
// method definitions
// ===========================================================================

GNEDeleteFrame::GNEDeleteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Delete") {
    // create delete options modul
    myDeleteOptions = new DeleteOptions(this);
    // create protect elements modul
    myProtectElements = new ProtectElements(this);
}


GNEDeleteFrame::~GNEDeleteFrame() {}


void
GNEDeleteFrame::show() {
    GNEFrame::show();
}


void
GNEDeleteFrame::hide() {
    GNEFrame::hide();
}


void
GNEDeleteFrame::removeSelectedAttributeCarriers() {
    // get attribute carriers
    const auto& attributeCarriers = myViewNet->getNet()->getAttributeCarriers();
    // first check if there is additional to remove
    if (selectedACsToDelete()) {
        // remove all selected attribute carrier susing the following parent-child sequence
        myViewNet->getUndoList()->begin(GUIIcon::MODEDELETE, "remove selected items");
        // disable update geometry
        myViewNet->getNet()->disableUpdateGeometry();
        // delete selected attribute carriers depending of current supermode
        if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            //junctions
            const auto selectedJunctions = attributeCarriers->getSelectedJunctions();
            for (const auto& selectedJunction : selectedJunctions) {
                myViewNet->getNet()->deleteJunction(selectedJunction, myViewNet->getUndoList());
            }
            // edges
            const auto selectedEdges = attributeCarriers->getSelectedEdges();
            for (const auto& selectedEdge : selectedEdges) {
                myViewNet->getNet()->deleteEdge(selectedEdge, myViewNet->getUndoList(), false);
            }
            // lanes
            const auto selectedLanes = attributeCarriers->getSelectedLanes();
            for (const auto& selectedLane : selectedLanes) {
                myViewNet->getNet()->deleteLane(selectedLane, myViewNet->getUndoList(), false);
            }
            // connections
            const auto selectedConnections = attributeCarriers->getSelectedConnections();
            for (const auto& selectedConnection : selectedConnections) {
                myViewNet->getNet()->deleteConnection(selectedConnection, myViewNet->getUndoList());
            }
            // crossings
            const auto selectedCrossings = attributeCarriers->getSelectedCrossings();
            for (const auto& selectedCrossing : selectedCrossings) {
                myViewNet->getNet()->deleteCrossing(selectedCrossing, myViewNet->getUndoList());
            }
            // shapes
            const auto selectedShapes = attributeCarriers->getSelectedShapes();
            for (const auto& selectedShape : selectedShapes) {
                myViewNet->getNet()->deleteShape(selectedShape, myViewNet->getUndoList());
            }
            // additionals
            while (attributeCarriers->getNumberOfSelectedAdditionals() > 0) {
                myViewNet->getNet()->deleteAdditional(attributeCarriers->getSelectedAdditionals().front(), myViewNet->getUndoList());
            }
        } else if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
            // demand elements
            while (attributeCarriers->getNumberOfSelectedDemandElements() > 0) {
                myViewNet->getNet()->deleteDemandElement(attributeCarriers->getSelectedDemandElements().front(), myViewNet->getUndoList());
            }
        } else if (myViewNet->getEditModes().isCurrentSupermodeData()) {
            // generic datas
            auto selectedGenericDatas = attributeCarriers->getSelectedGenericDatas();
            for (const auto& selectedGenericData : selectedGenericDatas) {
                myViewNet->getNet()->deleteGenericData(selectedGenericData, myViewNet->getUndoList());
            }
        }
        // enable update geometry
        myViewNet->getNet()->enableUpdateGeometry();
        // finish deletion
        myViewNet->getUndoList()->end();
    }
}


void
GNEDeleteFrame::removeAttributeCarrier(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool ignoreOptions) {
    // first check if there is at leas an AC under cursor)
    if (objectsUnderCursor.getAttributeCarrierFront()) {
        // disable update geometry
        myViewNet->getNet()->disableUpdateGeometry();
        // check type of of object under cursor object
        if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
            // Check if junction can be deleted
            if (ignoreOptions || SubordinatedElements(objectsUnderCursor.getJunctionFront()).checkElements(myProtectElements)) {
                myViewNet->getNet()->deleteJunction(objectsUnderCursor.getJunctionFront(), myViewNet->getUndoList());
            }
        } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
            if (ignoreOptions || SubordinatedElements(objectsUnderCursor.getEdgeFront()).checkElements(myProtectElements)) {
                // if all ok, then delete edge
                myViewNet->getNet()->deleteEdge(objectsUnderCursor.getEdgeFront(), myViewNet->getUndoList(), false);
            }
        } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE) {
            // Check if edge can be deleted
            if (ignoreOptions || SubordinatedElements(objectsUnderCursor.getLaneFront()).checkElements(myProtectElements)) {
                // if all ok, then delete lane
                myViewNet->getNet()->deleteLane(objectsUnderCursor.getLaneFront(), myViewNet->getUndoList(), false);
            }
        } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_CROSSING) {
            myViewNet->getNet()->deleteCrossing(objectsUnderCursor.getCrossingFront(), myViewNet->getUndoList());
        } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_CONNECTION) {
            myViewNet->getNet()->deleteConnection(objectsUnderCursor.getConnectionFront(), myViewNet->getUndoList());
        } else if (objectsUnderCursor.getAttributeCarrierFront() && (objectsUnderCursor.getAdditionalFront() == objectsUnderCursor.getAttributeCarrierFront())) {
            myViewNet->getNet()->deleteAdditional(objectsUnderCursor.getAdditionalFront(), myViewNet->getUndoList());
        } else if (objectsUnderCursor.getShapeFront() && (objectsUnderCursor.getShapeFront() == objectsUnderCursor.getAttributeCarrierFront())) {
            myViewNet->getNet()->deleteShape(objectsUnderCursor.getShapeFront(), myViewNet->getUndoList());
        } else if (objectsUnderCursor.getTAZElementFront() && (objectsUnderCursor.getTAZElementFront() == objectsUnderCursor.getAttributeCarrierFront())) {
            myViewNet->getNet()->deleteTAZElement(objectsUnderCursor.getTAZElementFront(), myViewNet->getUndoList());
        } else if (objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront() == objectsUnderCursor.getAttributeCarrierFront())) {
            // we need an special check for person plans
            if (objectsUnderCursor.getDemandElementFront()->getTagProperty().isPersonPlan()) {
                // get person plarent
                GNEDemandElement* personParent = objectsUnderCursor.getDemandElementFront()->getParentDemandElements().front();
                // if this is the last person plan element, remove person instead person plan
                if (personParent->getChildDemandElements().size() == 1) {
                    myViewNet->getNet()->deleteDemandElement(personParent, myViewNet->getUndoList());
                } else {
                    myViewNet->getNet()->deleteDemandElement(objectsUnderCursor.getDemandElementFront(), myViewNet->getUndoList());
                }
            } else {
                // just remove demand element
                myViewNet->getNet()->deleteDemandElement(objectsUnderCursor.getDemandElementFront(), myViewNet->getUndoList());
            }
        } else if (objectsUnderCursor.getGenericDataElementFront() && (objectsUnderCursor.getGenericDataElementFront() == objectsUnderCursor.getAttributeCarrierFront())) {
            myViewNet->getNet()->deleteGenericData(objectsUnderCursor.getGenericDataElementFront(), myViewNet->getUndoList());
        }
    }
    // enable update geometry
    myViewNet->getNet()->enableUpdateGeometry();
    // update view to show changes
    myViewNet->updateViewNet();
}


void
GNEDeleteFrame::removeGeometryPoint(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // get clicked position
    const Position clickedPosition = myViewNet->getPositionInformation();
    // check type of of object under cursor object with geometry points
    if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isNetworkElement()) {
        objectsUnderCursor.getNetworkElementFront()->removeGeometryPoint(clickedPosition, myViewNet->getUndoList());
    } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_POLY) {
        objectsUnderCursor.getPolyFront()->removeGeometryPoint(clickedPosition, myViewNet->getUndoList());
    } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        objectsUnderCursor.getTAZFront()->removeGeometryPoint(clickedPosition, myViewNet->getUndoList());
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
    SubordinatedElements(junction, junction->getNet()->getViewNet(), junction) {
    // add the number of subodinated elements of child edges
    for (const auto& edge : junction->getChildEdges()) {
        addValuesFromSubordinatedElements(this, edge);
    }
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEEdge* edge) :
    SubordinatedElements(edge, edge->getNet()->getViewNet(), edge) {
    // add the number of subodinated elements of child lanes
    for (const auto& lane : edge->getLanes()) {
        addValuesFromSubordinatedElements(this, lane);
    }
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNELane* lane) :
    SubordinatedElements(lane, lane->getNet()->getViewNet(), lane) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEAdditional* additional) :
    SubordinatedElements(additional, additional->getNet()->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEShape* shape) :
    SubordinatedElements(shape, shape->getNet()->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEDemandElement* demandElement) :
    SubordinatedElements(demandElement, demandElement->getNet()->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEGenericData* genericData) :
    SubordinatedElements(genericData, genericData->getNet()->getViewNet()) {
}


GNEDeleteFrame::SubordinatedElements::~SubordinatedElements() {}


bool
GNEDeleteFrame::SubordinatedElements::checkElements(const ProtectElements* protectElements) {
    // check every parent/child
    if ((myAdditionalParents > 0) && protectElements->protectAdditionals()) {
        openWarningDialog("additional", myAdditionalParents, false);
    } else if ((myAdditionalChilds > 0) && protectElements->protectAdditionals()) {
        openWarningDialog("additional", myAdditionalChilds, true);
    } else if ((myTAZParents > 0) && protectElements->protectTAZs()) {
        openWarningDialog("TAZ", myTAZParents, false);
    } else if ((myTAZChilds > 0) && protectElements->protectTAZs()) {
        openWarningDialog("TAZ", myTAZChilds, true);
    } else if ((myShapeParents > 0) && protectElements->protectShapes()) {
        openWarningDialog("shape", myShapeParents, false);
    } else if ((myShapeChilds > 0) && protectElements->protectShapes()) {
        openWarningDialog("shape", myShapeChilds, true);
    } else if ((myDemandElementParents > 0) && protectElements->protectDemandElements()) {
        openWarningDialog("demand", myDemandElementParents, false);
    } else if ((myDemandElementChilds > 0) && protectElements->protectDemandElements()) {
        openWarningDialog("demand", myDemandElementChilds, true);
    } else if ((myGenericDataParents > 0) && protectElements->protectGenericDatas()) {
        openWarningDialog("data", myGenericDataParents, false);
    } else if ((myGenericDataChilds > 0) && protectElements->protectGenericDatas()) {
        openWarningDialog("data", myGenericDataChilds, true);
    } else {
        // all checks ok, then return true, to remove element
        return true;
    }
    return false;
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEAttributeCarrier* attributeCarrier, GNEViewNet* viewNet) :
    myAttributeCarrier(attributeCarrier),
    myViewNet(viewNet),
    myAdditionalParents(0),
    myAdditionalChilds(0),
    myTAZParents(0),
    myTAZChilds(0),
    myShapeParents(0),
    myShapeChilds(0),
    myDemandElementParents(0),
    myDemandElementChilds(0),
    myGenericDataParents(0),
    myGenericDataChilds(0) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEAttributeCarrier* attributeCarrier, GNEViewNet* viewNet,
        const GNEHierarchicalElement* hierarchicalElement) :
    myAttributeCarrier(attributeCarrier),
    myViewNet(viewNet),
    myAdditionalParents(hierarchicalElement->getParentAdditionals().size()),
    myAdditionalChilds(hierarchicalElement->getChildAdditionals().size()),
    myTAZParents(hierarchicalElement->getParentTAZElements().size()),
    myTAZChilds(hierarchicalElement->getChildTAZElements().size()),
    myShapeParents(hierarchicalElement->getParentShapes().size()),
    myShapeChilds(hierarchicalElement->getChildShapes().size()),
    myDemandElementParents(hierarchicalElement->getParentDemandElements().size()),
    myDemandElementChilds(hierarchicalElement->getChildDemandElements().size()),
    myGenericDataParents(hierarchicalElement->getParentGenericDatas().size()),
    myGenericDataChilds(hierarchicalElement->getChildGenericDatas().size()) {
    // add the number of subodinated elements of additionals, shapes, demand elements and generic datas
    for (const auto& additional : hierarchicalElement->getParentAdditionals()) {
        addValuesFromSubordinatedElements(this, additional);
    }
    for (const auto& shape : hierarchicalElement->getParentShapes()) {
        addValuesFromSubordinatedElements(this, shape);
    }
    for (const auto& demandElement : hierarchicalElement->getParentDemandElements()) {
        addValuesFromSubordinatedElements(this, demandElement);
    }
    for (const auto& genericData : hierarchicalElement->getParentGenericDatas()) {
        addValuesFromSubordinatedElements(this, genericData);
    }
    for (const auto& additional : hierarchicalElement->getChildAdditionals()) {
        addValuesFromSubordinatedElements(this, additional);
    }
    for (const auto& shape : hierarchicalElement->getChildShapes()) {
        addValuesFromSubordinatedElements(this, shape);
    }
    for (const auto& additional : hierarchicalElement->getChildDemandElements()) {
        addValuesFromSubordinatedElements(this, additional);
    }
    for (const auto& genericData : hierarchicalElement->getChildGenericDatas()) {
        addValuesFromSubordinatedElements(this, genericData);
    }
}


void
GNEDeleteFrame::SubordinatedElements::addValuesFromSubordinatedElements(SubordinatedElements* originalSE, const SubordinatedElements& newSE) {
    originalSE->myAdditionalParents += newSE.myAdditionalParents;
    originalSE->myAdditionalChilds += newSE.myAdditionalChilds;
    originalSE->myTAZParents += newSE.myTAZParents;
    originalSE->myTAZChilds += newSE.myTAZChilds;
    originalSE->myShapeParents += newSE.myShapeParents;
    originalSE->myShapeChilds += newSE.myShapeChilds;
    originalSE->myDemandElementParents += newSE.myDemandElementParents;
    originalSE->myDemandElementChilds += newSE.myDemandElementChilds;
    originalSE->myGenericDataParents += newSE.myGenericDataParents;
    originalSE->myGenericDataChilds += newSE.myGenericDataChilds;
}


void
GNEDeleteFrame::SubordinatedElements::openWarningDialog(const std::string& type, const size_t number, const bool isChild) {
    // declare plural depending of "number"
    const std::string plural = (number > 1) ? "s" : "";
    // declare header
    const std::string header = "Problem deleting " + myAttributeCarrier->getTagProperty().getTagStr() + " '" + myAttributeCarrier->getID() + "'";
    // declare message
    std::string msg;
    // set message depending of isChild
    if (isChild) {
        msg = myAttributeCarrier->getTagProperty().getTagStr() + " '" + myAttributeCarrier->getID() +
              "' cannot be deleted because it has " + toString(number) + " " + type + " element" + plural + ".\n" +
              "To delete it, uncheck 'protect " + type + " elements'.";
    } else {
        msg = myAttributeCarrier->getTagProperty().getTagStr() + " '" + myAttributeCarrier->getID() +
              "' cannot be deleted because it is part of " + toString(number) + " " + type + " element" + plural + ".\n" +
              "To delete it, uncheck 'protect " + type + " elements'.";
    }
    // write warning
    WRITE_DEBUG("Opened FXMessageBox " + header);
    // open message box
    FXMessageBox::warning(myViewNet->getApp(), MBOX_OK, header.c_str(), "%s", msg.c_str());
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Closed FXMessageBox " + header);
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame - protected methods
// ---------------------------------------------------------------------------

bool
GNEDeleteFrame::selectedACsToDelete() const {
    // invert selection of elements depending of current supermode
    if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        // iterate over junctions
        for (const auto& junction : myViewNet->getNet()->getAttributeCarriers()->getJunctions()) {
            if (junction.second->isAttributeCarrierSelected()) {
                return true;
            }
            // due we iterate over all junctions, only it's neccesary iterate over incoming edges
            for (const auto& edge : junction.second->getGNEIncomingEdges()) {
                if (edge->isAttributeCarrierSelected()) {
                    return true;
                }
                // check lanes
                for (const auto& lane : edge->getLanes()) {
                    if (lane->isAttributeCarrierSelected()) {
                        return true;
                    }
                }
                // check connections
                for (const auto& connection : edge->getGNEConnections()) {
                    if (connection->isAttributeCarrierSelected()) {
                        return true;
                    }
                }
            }
            // check crossings
            for (const auto& crossing : junction.second->getGNECrossings()) {
                if (crossing->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
        // check shapes
        for (const auto& shapeTag : myViewNet->getNet()->getAttributeCarriers()->getShapes()) {
            for (const auto& shape : shapeTag.second) {
                if (shape->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
        // check additionals
        for (const auto& additionalTag : myViewNet->getNet()->getAttributeCarriers()->getAdditionals()) {
            for (const auto& additional : additionalTag.second) {
                if (additional->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
    } else if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        // check demand elements
        for (const auto& demandElementTag : myViewNet->getNet()->getAttributeCarriers()->getDemandElements()) {
            for (const auto& demandElement : demandElementTag.second) {
                if (demandElement->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
    } else if (myViewNet->getEditModes().isCurrentSupermodeData()) {
        // iterate over all generic datas
        for (const auto& genericDataTag : myViewNet->getNet()->getAttributeCarriers()->getGenericDatas()) {
            for (const auto& genericData : genericDataTag.second) {
                if (genericData->isAttributeCarrierSelected()) {
                    return true;
                }
            }
        }
    }
    return false;
}

/****************************************************************************/
