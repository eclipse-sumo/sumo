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
#include <config.h>

#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNENet.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNERerouter.h>

#include "GNEInspectorFrame.h"
#include "GNEDeleteFrame.h"
#include "GNEAdditionalFrame.h"


// ---------------------------------------------------------------------------
// GNEDeleteFrame::DeleteOptions - methods
// ---------------------------------------------------------------------------

GNEDeleteFrame::DeleteOptions::DeleteOptions(GNEDeleteFrame* deleteFrameParent) :
    FXGroupBox(deleteFrameParent->myContentFrame, "Options", GUIDesignGroupBoxFrame),
    myDeleteFrameParent(deleteFrameParent) {

    // Create checkbox for enabling/disabling automatic deletion of additionals childs (by default, enabled)
    myForceDeleteAdditionals = new FXCheckButton(this, "Force deletion of additionals", deleteFrameParent, MID_GNE_DELETEFRAME_AUTODELETEADDITIONALS, GUIDesignCheckButtonAttribute);
    myForceDeleteAdditionals->setCheck(TRUE);

    // Create checkbox for enabling/disabling delete only geomtery point(by default, disabled)
    myDeleteOnlyGeometryPoints = new FXCheckButton(this, "Delete only geometryPoints", deleteFrameParent, MID_GNE_DELETEFRAME_ONLYGEOMETRYPOINTS, GUIDesignCheckButtonAttribute);
    myDeleteOnlyGeometryPoints->setCheck(FALSE);
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

// ===========================================================================
// method definitions
// ===========================================================================

GNEDeleteFrame::GNEDeleteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Delete") {
    // create delete options modul
    myDeleteOptions = new DeleteOptions(this);

    // Create groupbox and tree list
    myACHierarchy = new GNEFrame::ACHierarchy(this);
}


GNEDeleteFrame::~GNEDeleteFrame() {}

void
GNEDeleteFrame::show() {
    if (myViewNet->getNet()->getSelectedAttributeCarriers().size() == 1) {
        myACHierarchy->showACHierarchy(*myViewNet->getNet()->getSelectedAttributeCarriers().begin());
    } else {
        myACHierarchy->hideACHierarchy();
    }
    GNEFrame::show();
}


void
GNEDeleteFrame::hide() {

    GNEFrame::hide();
}


void
GNEDeleteFrame::removeSelectedAttributeCarriers() {
    // remove all selected attribute carriers
    myViewNet->getUndoList()->p_begin("remove selected items");
    while (myViewNet->getNet()->getSelectedAttributeCarriers().size() > 0) {
        removeAttributeCarrier(*myViewNet->getNet()->getSelectedAttributeCarriers().begin(), true);
    }
    myViewNet->getUndoList()->p_end();
}


void
GNEDeleteFrame::removeAttributeCarrier(GNEAttributeCarrier* ac, bool ignoreOptions) {
    // obtain clicked position
    Position clickedPosition = myViewNet->getPositionInformation();
    if (myDeleteOptions->deleteOnlyGeometryPoints() && !ignoreOptions) {
        // check type of of GL object
        switch (ac->getTag()) {
            case SUMO_TAG_EDGE: {
                GNEEdge* edge = dynamic_cast<GNEEdge*>(ac);
                assert(edge);
                if (edge->getVertexIndex(clickedPosition, false) != -1) {
                    edge->deleteGeometryPoint(clickedPosition);
                }
                break;
            }
            case SUMO_TAG_POLY: {
                GNEPoly* polygon = dynamic_cast<GNEPoly*>(ac);
                assert(polygon);
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
        switch (ac->getTag()) {
            case SUMO_TAG_JUNCTION: {
                GNEJunction* junction = dynamic_cast<GNEJunction*>(ac);
                assert(junction);
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
                if (myDeleteOptions->forceDeleteAdditionals() || ignoreOptions) {
                    myViewNet->getNet()->deleteJunction(junction, myViewNet->getUndoList());
                } else {
                    if (numberOfAdditionals > 0) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                        std::string plural = numberOfAdditionals > 1 ? "s" : "";
                        // Open warning DialogBox
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(junction->getTag())).c_str(), "%s",
                                              (toString(junction->getTag()) + " '" + junction->getID() + "' cannot be deleted because owns " +
                                               toString(numberOfAdditionals) + " additional child" + plural + ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                    } else {
                        myViewNet->getNet()->deleteJunction(junction, myViewNet->getUndoList());
                    }
                }
                break;
            }
            case SUMO_TAG_EDGE: {
                GNEEdge* edge = dynamic_cast<GNEEdge*>(ac);
                assert(edge);
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
                    if (myDeleteOptions->forceDeleteAdditionals() || ignoreOptions) {
                        // when deleting a single edge, keep all unaffected connections as they were
                        myViewNet->getNet()->deleteEdge(edge, myViewNet->getUndoList(), false);
                    } else {
                        if (numberOfAdditionalChilds > 0) {
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                            std::string plural = numberOfAdditionalChilds > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(edge->getTag())).c_str(), "%s",
                                                  (toString(edge->getTag()) + " '" + edge->getID() + "' cannot be deleted because owns " +
                                                   toString(numberOfAdditionalChilds) + " additional" + plural + ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                        } else if (numberOfAdditionalParents > 0) {
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                            std::string plural = numberOfAdditionalParents > 1 ? "s" : "";
                            // Open warning DialogBox
                            FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(edge->getTag())).c_str(), "%s",
                                                  (toString(edge->getTag()) + " '" + edge->getID() + "' cannot be deleted because is part of " +
                                                   toString(numberOfAdditionalParents) + " additional" + plural + ".\n Check 'Force deletion of additionals' to force deletion.").c_str());
                            // write warning if netedit is running in testing mode
                            WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                        } else {
                            // when deleting a single edge, keep all unaffected connections as they were
                            myViewNet->getNet()->deleteEdge(edge, myViewNet->getUndoList(), false);
                        }
                    }
                }
                break;
            }
            case SUMO_TAG_LANE: {
                GNELane* lane = dynamic_cast<GNELane*>(ac);
                assert(lane);
                // Check if lane can be deleted
                if (myDeleteOptions->forceDeleteAdditionals() || ignoreOptions) {
                    // when deleting a single lane, keep all unaffected connections as they were
                    myViewNet->getNet()->deleteLane(lane, myViewNet->getUndoList(), false);
                } else {
                    if (lane->getAdditionalChilds().size() == 0) {
                        // when deleting a single lane, keep all unaffected connections as they were
                        myViewNet->getNet()->deleteLane(lane, myViewNet->getUndoList(), false);
                    } else {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox 'Force deletion needed'");
                        // open warning box
                        FXMessageBox::warning(getViewNet()->getApp(), MBOX_OK, ("Problem deleting " + toString(lane->getTag())).c_str(), "%s",
                                              (toString(lane->getTag()) + " '" + lane->getID() + "' cannot be deleted because it has " +
                                               toString(lane->getAdditionalChilds().size()) + " additional childs.\n Check 'Force deletion of Additionals' to force deletion.").c_str());
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox 'Force deletion needed' with 'OK'");
                    }
                }
                break;
            }
            case SUMO_TAG_CROSSING: {
                GNECrossing* crossing = dynamic_cast<GNECrossing*>(ac);
                assert(crossing);
                myViewNet->getNet()->deleteCrossing(crossing, myViewNet->getUndoList());
                break;
            }
            case SUMO_TAG_CONNECTION: {
                GNEConnection* connection = dynamic_cast<GNEConnection*>(ac);
                assert(connection);
                myViewNet->getNet()->deleteConnection(connection, myViewNet->getUndoList());
                break;
            }
            default: {
                // obtain tag property (only for improve code legibility)
                const auto& tagValue = GNEAttributeCarrier::getTagProperties(ac->getTag());
                if (tagValue.isAdditional()) {
                    GNEAdditional* additional = dynamic_cast<GNEAdditional*>(ac);
                    assert(additional);
                    myViewNet->getViewParent()->getAdditionalFrame()->removeAdditional(additional);
                } else if (tagValue.isShape()) {
                    GNEShape* shape = dynamic_cast<GNEShape*>(ac);
                    assert(shape);
                    myViewNet->getNet()->deleteShape(shape, myViewNet->getUndoList());
                }
                break;
            }
        }
    }
    // update view to show changes
    myViewNet->update();
}


GNEDeleteFrame::DeleteOptions*
GNEDeleteFrame::getDeleteOptions() const {
    return myDeleteOptions;
}

/****************************************************************************/
