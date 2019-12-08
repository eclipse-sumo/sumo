/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDeleteFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2016
///
// The Widget for remove network-elements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/additionals/GNETAZ.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDeleteFrame.h"


// ---------------------------------------------------------------------------
// GNEDeleteFrame::DeleteOptions - methods
// ---------------------------------------------------------------------------

GNEDeleteFrame::DeleteOptions::DeleteOptions(GNEDeleteFrame* deleteFrameParent) :
    FXGroupBox(deleteFrameParent->myContentFrame, "Options", GUIDesignGroupBoxFrame) {

    // Create checkbox for enable/disable automatic deletion of additionals children (by default, enabled)
    myForceDeleteAdditionals = new FXCheckButton(this, "Force deletion of additionals", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myForceDeleteAdditionals->setCheck(TRUE);

    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myDeleteOnlyGeometryPoints = new FXCheckButton(this, "Delete only geometryPoints", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myDeleteOnlyGeometryPoints->setCheck(FALSE);

    // Create checkbox for enable/disable delete only geomtery point(by default, disabled)
    myProtectDemandElements = new FXCheckButton(this, "Protect demand elements", deleteFrameParent, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myProtectDemandElements->setCheck(TRUE);
}


GNEDeleteFrame::DeleteOptions::~DeleteOptions() {}


bool
GNEDeleteFrame::DeleteOptions::forceDeleteAdditionals() const {
    return (myForceDeleteAdditionals->getCheck() == TRUE);
}


bool
GNEDeleteFrame::DeleteOptions::deleteOnlyGeometryPoints() const {
    return (myDeleteOnlyGeometryPoints->getCheck() == TRUE);
}


bool
GNEDeleteFrame::DeleteOptions::protectDemandElements() const {
    return (myProtectDemandElements->getCheck() == TRUE);
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
        if (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
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
                // obtain number of subordinated elements
                auto subordinatedElements = SubordinatedElements(objectsUnderCursor.getJunctionFront());
                // Check if junction can be deleted
                if (myDeleteOptions->forceDeleteAdditionals() || ignoreOptions) {
                    myViewNet->getNet()->deleteJunction(objectsUnderCursor.getJunctionFront(), myViewNet->getUndoList());
                } else {
                    if (subordinatedElements.childAdditionals > 0) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                        std::string plural = subordinatedElements.childAdditionals > 1 ? "s" : "";
                        // Open warning DialogBox
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + objectsUnderCursor.getJunctionFront()->getTagStr()).c_str(), "%s",
                                              (objectsUnderCursor.getJunctionFront()->getTagStr() + " '" + objectsUnderCursor.getJunctionFront()->getID() + "' cannot be deleted because owns " +
                                               toString(subordinatedElements.childAdditionals) + " child additional" + plural + ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                    } else {
                        myViewNet->getNet()->deleteJunction(objectsUnderCursor.getJunctionFront(), myViewNet->getUndoList());
                    }
                }
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                // check if click was over a geometry point or over a shape's edge
                if (objectsUnderCursor.getEdgeFront()->getVertexIndex(clickedPosition, false, false) != -1) {
                    objectsUnderCursor.getEdgeFront()->deleteGeometryPoint(clickedPosition);
                } else {
                    // obtain number of subordinated elements
                    auto subordinatedElements = SubordinatedElements(objectsUnderCursor.getEdgeFront());
                    // Check if edge can be deleted
                    if ((myDeleteOptions->forceDeleteAdditionals() && !myDeleteOptions->protectDemandElements()) || ignoreOptions) {
                        // when deleting a single edge, keep all unaffected connections as they were
                        myViewNet->getNet()->deleteEdge(objectsUnderCursor.getEdgeFront(), myViewNet->getUndoList(), false);
                    } else {
                        // declare strings to save certain messages used in FXMessageBox to improve code legilibly
                        std::string tagstr = objectsUnderCursor.getEdgeFront()->getTagStr();
                        std::string id = objectsUnderCursor.getEdgeFront()->getID();
                        // check number of child additional
                        if ((subordinatedElements.childAdditionals > 0) && !myDeleteOptions->forceDeleteAdditionals()) {
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                            std::string plural = subordinatedElements.childAdditionals > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                                  (tagstr + " '" + id + "' cannot be deleted because owns " + toString(subordinatedElements.childAdditionals) + " additional" + plural +
                                                   ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                        } else if ((subordinatedElements.childAdditionals > 0) && !myDeleteOptions->forceDeleteAdditionals()) {
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                            std::string plural = subordinatedElements.parentAdditionals > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                                  (tagstr + " '" + id + "' cannot be deleted because is part of " + toString(subordinatedElements.parentAdditionals) + " additional" + plural +
                                                   ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                        } else if ((subordinatedElements.childDemandElements > 0) && myDeleteOptions->protectDemandElements()) {
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Opening FXMessageBox 'Unprotect demand elements'");
                            std::string plural = subordinatedElements.childDemandElements > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                                  (tagstr + " '" + id + "' cannot be deleted because owns " + toString(subordinatedElements.childDemandElements) + " demand element" + plural +
                                                   ".\n Uncheck 'Protect demand elements' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Closed FXMessageBox 'Unprotect demand elements' with 'OK'");
                        } else if ((subordinatedElements.parentDemandElements > 0) && myDeleteOptions->protectDemandElements()) {
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Opening FXMessageBox 'Unprotect demand elements'");
                            std::string plural = subordinatedElements.parentDemandElements > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                                  (tagstr + " '" + id + "' cannot be deleted because is part of " + toString(subordinatedElements.parentDemandElements) + " demand element" + plural +
                                                   ".\n Uncheck 'Protect demand elements' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Closed FXMessageBox 'Unprotect demand elements' with 'OK'");
                        } else {
                            // if all ok, then delete edge
                            myViewNet->getNet()->deleteEdge(objectsUnderCursor.getEdgeFront(), myViewNet->getUndoList(), false);
                        }
                    }
                }
            } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE) {
                // obtain number of subordinated elements
                auto subordinatedElements = SubordinatedElements(objectsUnderCursor.getLaneFront());
                // Check if lane can be deleted
                if ((myDeleteOptions->forceDeleteAdditionals() && !myDeleteOptions->protectDemandElements()) || ignoreOptions) {
                    // when deleting a single lane, keep all unaffected connections as they were
                    myViewNet->getNet()->deleteLane(objectsUnderCursor.getLaneFront(), myViewNet->getUndoList(), false);
                } else {
                    // declare strings to save certain messages used in FXMessageBox to improve code legilibly
                    std::string tagstr = objectsUnderCursor.getLaneFront()->getTagStr();
                    std::string id = objectsUnderCursor.getLaneFront()->getID();
                    // check number of child additional
                    if ((subordinatedElements.childAdditionals > 0) && !myDeleteOptions->forceDeleteAdditionals()) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                        std::string plural = subordinatedElements.childAdditionals > 1 ? "s" : "";
                        // Open warning DialogBox
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                              (tagstr + " '" + id + "' cannot be deleted because owns " + toString(subordinatedElements.childAdditionals) + " additional" + plural +
                                               ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                    } else if ((subordinatedElements.childAdditionals > 0) && !myDeleteOptions->forceDeleteAdditionals()) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                        std::string plural = subordinatedElements.parentAdditionals > 1 ? "s" : "";
                        // Open warning DialogBox
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                              (tagstr + " '" + id + "' cannot be deleted because is part of " + toString(subordinatedElements.parentAdditionals) + " additional" + plural +
                                               ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                    } else if ((subordinatedElements.childDemandElements > 0) && myDeleteOptions->protectDemandElements()) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox 'Unprotect demand elements'");
                        std::string plural = subordinatedElements.childDemandElements > 1 ? "s" : "";
                        // Open warning DialogBox
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                              (tagstr + " '" + id + "' cannot be deleted because owns " + toString(subordinatedElements.childDemandElements) + " demand element" + plural +
                                               ".\n Uncheck 'Protect demand elements' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox 'Unprotect demand elements' with 'OK'");
                    } else if ((subordinatedElements.parentDemandElements > 0) && myDeleteOptions->protectDemandElements()) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox 'Unprotect demand elements'");
                        std::string plural = subordinatedElements.parentDemandElements > 1 ? "s" : "";
                        // Open warning DialogBox
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + tagstr).c_str(), "%s",
                                              (tagstr + " '" + id + "' cannot be deleted because is part of " + toString(subordinatedElements.parentDemandElements) + " demand element" + plural +
                                               ".\n Uncheck 'Protect demand elements' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox 'Unprotect demand elements' with 'OK'");
                    } else {
                        // if all ok, then delete lane
                        myViewNet->getNet()->deleteLane(objectsUnderCursor.getLaneFront(), myViewNet->getUndoList(), false);
                    }
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
    parentAdditionals(0),
    childAdditionals(0),
    parentDemandElements(0),
    childDemandElements(0) {
    // obtain number of additional and demand elements parents and children of junction
    parentAdditionals = (int)junction->getParentAdditionals().size();
    childAdditionals = (int)junction->getChildAdditionals().size();
    parentDemandElements = (int)junction->getParentDemandElements().size();
    childDemandElements = (int)junction->getChildDemandElements().size();
    // add the number of subodinated elements of child edges
    for (const auto& i : junction->getGNEEdges()) {
        (*this) += SubordinatedElements(i);
    }
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEEdge* edge) :
    parentAdditionals(0),
    childAdditionals(0),
    parentDemandElements(0),
    childDemandElements(0) {
    // obtain number of additional and demand elements parents and children of junction
    parentAdditionals = (int)edge->getParentAdditionals().size();
    childAdditionals = (int)edge->getChildAdditionals().size();
    parentDemandElements = (int)edge->getParentDemandElements().size();
    childDemandElements = (int)edge->getChildDemandElements().size();
    // add the number of subodinated elements of child lanes
    for (const auto& i : edge->getLanes()) {
        (*this) += SubordinatedElements(i);
    }
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNELane* lane) :
    parentAdditionals(0),
    childAdditionals(0),
    parentDemandElements(0),
    childDemandElements(0) {
    // obtain number of additional and demand elements parents and children of junction
    parentAdditionals = (int)lane->getParentAdditionals().size();
    childAdditionals = (int)lane->getChildAdditionals().size();
    parentDemandElements = (int)lane->getParentDemandElements().size();
    childDemandElements = (int)lane->getChildDemandElements().size();
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEAdditional* /* additional */) :
    parentAdditionals(0),
    childAdditionals(0),
    parentDemandElements(0),
    childDemandElements(0) {
}


GNEDeleteFrame::SubordinatedElements::SubordinatedElements(const GNEDemandElement* /* demandElement */) :
    parentAdditionals(0),
    childAdditionals(0),
    parentDemandElements(0),
    childDemandElements(0) {
}


GNEDeleteFrame::SubordinatedElements&
GNEDeleteFrame::SubordinatedElements::operator+=(const SubordinatedElements& other) {
    parentAdditionals += other.parentAdditionals;
    childAdditionals += other.childAdditionals;
    parentDemandElements += other.parentDemandElements;
    childDemandElements += other.childDemandElements;
    return *this;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame - protected methods
// ---------------------------------------------------------------------------

bool
GNEDeleteFrame::ACsToDelete() const {
    // invert selection of elements depending of current supermode
    if (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
        // iterate over junctions
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().junctions) {
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
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().additionals) {
            // first check if additional is selectable
            if (GNEAttributeCarrier::getTagProperties(i.first).isSelectable()) {
                for (const auto& j : i.second) {
                    if (j.second->isAttributeCarrierSelected()) {
                        return true;
                    }
                }
            }
        }
        // check polygons
        for (const auto& i : myViewNet->getNet()->getPolygons()) {
            GNEShape* shape = dynamic_cast<GNEShape*>(i.second);
            if (shape->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check POIs
        for (const auto& i : myViewNet->getNet()->getPOIs()) {
            GNEShape* shape = dynamic_cast<GNEShape*>(i.second);
            if (shape->isAttributeCarrierSelected()) {
                return true;
            }
        }
    } else {
        // check routes
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTE)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check vehicles
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VEHICLE)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check trips
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_TRIP)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check flows
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_FLOW)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check route flows
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTEFLOW)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check lane stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_LANE)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check bus stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_BUSSTOP)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check container stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CONTAINERSTOP)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check chargingstation stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CHARGINGSTATION)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
        // check parkingarea stops
        for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_PARKINGAREA)) {
            if (i.second->isAttributeCarrierSelected()) {
                return true;
            }
        }
    }
    return false;
}

/****************************************************************************/
