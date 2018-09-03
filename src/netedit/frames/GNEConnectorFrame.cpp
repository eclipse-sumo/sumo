/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2011-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEConnectorFrame.cpp
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying lane-to-lane connections
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNEInternalLane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>

#include "GNEInspectorFrame.h"
#include "GNEConnectorFrame.h"
#include "GNESelectorFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEConnectorFrame) GNEConnectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,                                 GNEConnectorFrame::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_OK,                                     GNEConnectorFrame::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,                          GNEConnectorFrame::onCmdClearSelectedConnections),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_RESET,                          GNEConnectorFrame::onCmdResetSelectedConnections),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CONNECTORFRAME_SELECTDEADENDS,      GNEConnectorFrame::onCmdSelectDeadEnds),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CONNECTORFRAME_SELECTDEADSTARTS,    GNEConnectorFrame::onCmdSelectDeadStarts),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CONNECTORFRAME_SELECTCONFLICTS,     GNEConnectorFrame::onCmdSelectConflicts),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CONNECTORFRAME_SELECTPASS,          GNEConnectorFrame::onCmdSelectPass),
};

// Object implementation
FXIMPLEMENT(GNEConnectorFrame, FXVerticalFrame, GNEConnectorFrameMap, ARRAYNUMBER(GNEConnectorFrameMap))

// ===========================================================================
// static members
// ===========================================================================
RGBColor GNEConnectorFrame::sourceColor;
RGBColor GNEConnectorFrame::potentialTargetColor;
RGBColor GNEConnectorFrame::targetColor;
RGBColor GNEConnectorFrame::targetPassColor;
RGBColor GNEConnectorFrame::conflictColor;

// ===========================================================================
// method definitions
// ===========================================================================
GNEConnectorFrame::GNEConnectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Edit Connections"),
    myCurrentLane(0) {
    // Create groupbox for lane information
    myGroupBoxDescription = new FXGroupBox(myContentFrame, "Lane", GUIDesignGroupBoxFrame);

    // Create label for lane description and update it
    myLaneDescriptionLabel = new FXLabel(myGroupBoxDescription, "", 0, GUIDesignLabelFrameInformation);
    updateDescription();

    // Create GroupBox for Buttons
    myGroupBoxModifications = new FXGroupBox(myContentFrame, "Modifications", GUIDesignGroupBoxFrame);

    // Create "Cancel" button
    myCancelButton = new FXButton(myGroupBoxModifications, "Cancel\t\tDiscard connection modifications (Esc)",
                                  GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_CANCEL, GUIDesignButton);
    // Create "OK" button
    mySaveButton = new FXButton(myGroupBoxModifications, "OK\t\tSave connection modifications (Enter)",
                                GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_OK, GUIDesignButton);

    // Create groupbox for operations
    myGroupBoxOperations = new FXGroupBox(myContentFrame, "Operations", GUIDesignGroupBoxFrame);

    // Create "Select Dead Ends" button
    mySelectDeadEndsButton = new FXButton(myGroupBoxOperations, "Select Dead Ends\t\tSelects all lanes that have no outgoing connection (clears previous selection)",
                                          0, this, MID_GNE_CONNECTORFRAME_SELECTDEADENDS, GUIDesignButton);
    // Create "Select Dead Starts" button
    mySelectDeadStartsButton = new FXButton(myGroupBoxOperations, "Select Dead Starts\t\tSelects all lanes that have no incoming connection (clears previous selection)",
                                            0, this, MID_GNE_CONNECTORFRAME_SELECTDEADSTARTS, GUIDesignButton);
    // Create "Select Conflicts" button
    mySelectConflictsButton = new FXButton(myGroupBoxOperations, "Select Conflicts\t\tSelects all lanes with more than one incoming connection from the same edge (clears previous selection)",
                                           0, this, MID_GNE_CONNECTORFRAME_SELECTCONFLICTS, GUIDesignButton);
    // Create "Select Edges which may always pass" button
    mySelectPassingButton = new FXButton(myGroupBoxOperations, "Select Passing\t\tSelects all lanes with a connection that has has the 'pass' attribute set",
                                         0, this, MID_GNE_CONNECTORFRAME_SELECTPASS, GUIDesignButton);
    // Create "Clear Selected" button
    myClearSelectedButton = new FXButton(myGroupBoxOperations, "Clear Selected\t\tClears all connections of all selected objects",
                                         0, this, MID_CHOOSEN_CLEAR, GUIDesignButton);
    // Create "Reset Selected" button
    myResetSelectedButton = new FXButton(myGroupBoxOperations, "Reset Selected\nJunctions\t\tRecomputes connections at all selected junctions",
                                         0, this, MID_CHOOSEN_RESET, GUIDesignButton);

    // Create groupbox for selection hints
    myGroupBoxSelection = new FXGroupBox(myContentFrame, "Selection", GUIDesignGroupBoxFrame);

    // Selection Hint
    myHoldShiftLabel = new FXLabel(myGroupBoxSelection, "Hold <SHIFT> while clicking\nto create unyielding\nconnections (pass=true).", 0, GUIDesignLabelFrameInformation);
    myHoldControlLabel = new FXLabel(myGroupBoxSelection, "Hold <CTRL> while clicking\nto create conflicting\nconnections (i.e. at zipper nodes\nor with incompatible permissions)", 0, GUIDesignLabelFrameInformation);

    // init colors here to avoid static order fiasco (https://isocpp.org/wiki/faq/ctors#static-init-order)
    sourceColor = RGBColor::CYAN;
    potentialTargetColor = RGBColor(0, 64, 0, 255);
    targetColor = RGBColor::GREEN;
    targetPassColor = RGBColor::MAGENTA;
    conflictColor = RGBColor::YELLOW;


    // create groupbox for legend
    myGroupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);

    // create source label
    mySourceLabel = new FXLabel(myGroupBoxLegend, "Source", 0, GUIDesignLabelLeft);
    mySourceLabel->setBackColor(MFXUtils::getFXColor(sourceColor));

    // create target label
    myTargetLabel = new FXLabel(myGroupBoxLegend, "Target", 0, GUIDesignLabelLeft);
    myTargetLabel->setBackColor(MFXUtils::getFXColor(targetColor));

    // create possible target label
    myPossibleTargetLabel = new FXLabel(myGroupBoxLegend, "Possible Target", 0, GUIDesignLabelLeft);
    myPossibleTargetLabel->setBackColor(MFXUtils::getFXColor(potentialTargetColor));

    // create target (pass) label
    myTargetPassLabel = new FXLabel(myGroupBoxLegend, "Target (pass)", 0, GUIDesignLabelLeft);
    myTargetPassLabel->setBackColor(MFXUtils::getFXColor(targetPassColor));

    // create conflict label
    myConflictLabel = new FXLabel(myGroupBoxLegend, "Conflict", 0, GUIDesignLabelLeft);
    myConflictLabel->setBackColor(MFXUtils::getFXColor(conflictColor));
}


GNEConnectorFrame::~GNEConnectorFrame() {}


void
GNEConnectorFrame::handleLaneClick(GNELane* lane, bool mayDefinitelyPass, bool allowConflict, bool toggle) {
    if (myCurrentLane == 0) {
        myCurrentLane = lane;
        myCurrentLane->setSpecialColor(&sourceColor);
        initTargets();
        myNumChanges = 0;
        myViewNet->getUndoList()->p_begin("modify " + toString(SUMO_TAG_CONNECTION) + "s");
    } else if (myPotentialTargets.count(lane)
               || (allowConflict && lane->getParentEdge().getGNEJunctionSource() == myCurrentLane->getParentEdge().getGNEJunctionDestiny())) {
        const int fromIndex = myCurrentLane->getIndex();
        GNEEdge& srcEdge = myCurrentLane->getParentEdge();
        GNEEdge& destEdge = lane->getParentEdge();
        std::vector<NBEdge::Connection> connections = srcEdge.getNBEdge()->getConnectionsFromLane(fromIndex);
        bool changed = false;
        LaneStatus status = getLaneStatus(connections, lane);
        if (status == CONFLICTED && allowConflict) {
            status = UNCONNECTED;
        }
        switch (status) {
            case UNCONNECTED:
                if (toggle) {
                    // create new connection
                    NBEdge::Connection newCon(fromIndex, destEdge.getNBEdge(), lane->getIndex(), mayDefinitelyPass);
                    // if the connection was previously deleted (by clicking the same lane twice), restore all values
                    for (NBEdge::Connection& c : myDeletedConnections) {
                        // fromLane must be the same, only check toLane
                        if (c.toEdge == destEdge.getNBEdge() && c.toLane == lane->getIndex()) {
                            newCon = c;
                        }
                    }
                    NBConnection newNBCon(srcEdge.getNBEdge(), fromIndex, destEdge.getNBEdge(), lane->getIndex(), newCon.tlLinkIndex);
                    myViewNet->getUndoList()->add(new GNEChange_Connection(&srcEdge, newCon, false, true), true);
                    lane->setSpecialColor(mayDefinitelyPass ? &targetPassColor : &targetColor);
                    srcEdge.getGNEJunctionDestiny()->invalidateTLS(myViewNet->getUndoList(), NBConnection::InvalidConnection, newNBCon);
                }
                break;
            case CONNECTED:
            case CONNECTED_PASS: {
                // remove connection
                GNEConnection* con = srcEdge.retrieveGNEConnection(fromIndex, destEdge.getNBEdge(), lane->getIndex());
                myDeletedConnections.push_back(con->getNBEdgeConnection());
                myViewNet->getNet()->deleteConnection(con, myViewNet->getUndoList());
                lane->setSpecialColor(&potentialTargetColor);
                changed = true;
                break;
            }
            case CONFLICTED:
                SVCPermissions fromPermissions = srcEdge.getNBEdge()->getPermissions(fromIndex);
                SVCPermissions toPermissions = destEdge.getNBEdge()->getPermissions(lane->getIndex());
                if ((fromPermissions & toPermissions) == SVC_PEDESTRIAN) {
                    myViewNet->setStatusBarText("Pedestrian connections are generated automatically");
                } else if ((fromPermissions & toPermissions) == 0) {
                    myViewNet->setStatusBarText("Incompatible vehicle class permissions");
                } else {
                    myViewNet->setStatusBarText("Another lane from the same edge already connects to that lane");
                }
                break;
        }
        if (changed) {
            myNumChanges += 1;
        }
    } else {
        myViewNet->setStatusBarText("Invalid target for " + toString(SUMO_TAG_CONNECTION));
    }
    updateDescription();
}


long
GNEConnectorFrame::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myCurrentLane != 0) {
        myViewNet->getUndoList()->p_abort();
        if (myNumChanges) {
            myViewNet->setStatusBarText("Changes reverted");
        }
        cleanup();
        myViewNet->update();
    }
    return 1;
}


long
GNEConnectorFrame::onCmdOK(FXObject*, FXSelector, void*) {
    if (myCurrentLane != 0) {
        myViewNet->getUndoList()->p_end();
        if (myNumChanges) {
            myViewNet->setStatusBarText("Changes accepted");
        }
        cleanup();
        myViewNet->update();
    }
    return 1;
}


long
GNEConnectorFrame::onCmdSelectDeadEnds(FXObject*, FXSelector, void*) {
    std::vector<GNEAttributeCarrier*> deadEnds;
    // every edge knows its outgoing connections so we can look at each edge in isolation
    const std::vector<GNEEdge*> edges = myViewNet->getNet()->retrieveEdges();
    for (auto i : edges) {
        for (auto j : i->getLanes()) {
            if (i->getNBEdge()->getConnectionsFromLane(j->getIndex()).size() == 0) {
                deadEnds.push_back(j);
            }
        }
    }
    myViewNet->getViewParent()->getSelectorFrame()->handleIDs(deadEnds, GNESelectorFrame::ModificationMode::SET_REPLACE);
    return 1;
}


long
GNEConnectorFrame::onCmdSelectDeadStarts(FXObject*, FXSelector, void*) {
    std::vector<GNEAttributeCarrier*> deadStarts;
    // every edge knows only its outgoing connections so we look at whole junctions
    const std::vector<GNEJunction*> junctions = myViewNet->getNet()->retrieveJunctions();
    for (auto i : junctions) {
        // first collect all outgoing lanes
        for (auto j : i->getNBNode()->getOutgoingEdges()) {
            GNEEdge* edge = myViewNet->getNet()->retrieveEdge(j->getID());
            for (auto k : edge->getLanes()) {
                deadStarts.push_back(k);
            }
        }
        // then remove all approached lanes
        for (auto j : i->getNBNode()->getIncomingEdges()) {
            GNEEdge* edge = myViewNet->getNet()->retrieveEdge(j->getID());
            for (auto k : edge->getNBEdge()->getConnections()) {
                deadStarts.push_back(myViewNet->getNet()->retrieveEdge(k.toEdge->getID())->getLanes()[k.toLane]);
            }
        }
    }
    myViewNet->getViewParent()->getSelectorFrame()->handleIDs(deadStarts, GNESelectorFrame::ModificationMode::SET_REPLACE);
    return 1;
}


long
GNEConnectorFrame::onCmdSelectConflicts(FXObject*, FXSelector, void*) {
    std::vector<GNEAttributeCarrier*> conflicts;
    // conflicts happen per edge so we can look at each edge in isolation
    const std::vector<GNEEdge*> edges = myViewNet->getNet()->retrieveEdges();
    for (auto i : edges) {
        const EdgeVector destinations = i->getNBEdge()->getConnectedEdges();
        for (auto j : destinations) {
            GNEEdge* dest = myViewNet->getNet()->retrieveEdge(j->getID());
            for (auto k : dest->getLanes()) {
                const bool isConflicted = count_if(i->getNBEdge()->getConnections().begin(), i->getNBEdge()->getConnections().end(),
                                                   NBEdge::connections_toedgelane_finder(j, (int)(k)->getIndex(), -1)) > 1;
                if (isConflicted) {
                    conflicts.push_back(k);
                }
            }
        }

    }
    myViewNet->getViewParent()->getSelectorFrame()->handleIDs(conflicts, GNESelectorFrame::ModificationMode::SET_REPLACE);
    return 1;
}


long
GNEConnectorFrame::onCmdSelectPass(FXObject*, FXSelector, void*) {
    std::vector<GNEAttributeCarrier*> pass;
    const std::vector<GNEEdge*> edges = myViewNet->getNet()->retrieveEdges();
    for (auto i : edges) {
        for (auto j : i->getNBEdge()->getConnections()) {
            if (j.mayDefinitelyPass) {
                pass.push_back(i->getLanes()[j.fromLane]);
            }
        }
    }
    myViewNet->getViewParent()->getSelectorFrame()->handleIDs(pass, GNESelectorFrame::ModificationMode::SET_REPLACE);
    return 1;
}


long
GNEConnectorFrame::onCmdClearSelectedConnections(FXObject*, FXSelector, void*) {
    onCmdCancel(0, 0, 0);
    myViewNet->getUndoList()->p_begin("clear connections from selected lanes, edges and " + toString(SUMO_TAG_JUNCTION) + "s");
    // clear junction's connection
    auto junctions = myViewNet->getNet()->retrieveJunctions(true);
    for (auto i : junctions) {
        i->setLogicValid(false, myViewNet->getUndoList()); // clear connections
        i->setLogicValid(false, myViewNet->getUndoList(), GNEAttributeCarrier::FEATURE_MODIFIED); // prevent re-guessing
    }
    // clear edge's connection
    auto edges = myViewNet->getNet()->retrieveEdges(true);
    for (auto i : edges) {
        for (auto j : i->getLanes()) {
            removeConnections(j);
        }
    }
    // clear lane's connection
    auto lanes = myViewNet->getNet()->retrieveLanes(true);
    for (auto i : lanes) {
        removeConnections(dynamic_cast<GNELane*>(i));
    }
    myViewNet->getUndoList()->p_end();
    return 1;
}


void
GNEConnectorFrame::removeConnections(GNELane* lane) {
    handleLaneClick(lane, false, false, true); // select as current lane
    for (auto i : myPotentialTargets) {
        handleLaneClick(i, false, false, false);  // deselect
    }
    onCmdOK(0, 0, 0); // save
}


long
GNEConnectorFrame::onCmdResetSelectedConnections(FXObject*, FXSelector, void*) {
    onCmdCancel(0, 0, 0);
    myViewNet->getUndoList()->p_begin("reset connections from selected lanes");
    auto junctions = myViewNet->getNet()->retrieveJunctions(true);
    for (auto i : junctions) {
        i->setLogicValid(false, myViewNet->getUndoList());
    }
    myViewNet->getUndoList()->p_end();
    return 1;
}


void
GNEConnectorFrame::updateDescription() const {
    if (myCurrentLane == 0) {
        myLaneDescriptionLabel->setText("No Lane Selected\n");
    } else {
        myLaneDescriptionLabel->setText(("Lane id: " + myCurrentLane->getMicrosimID() + "\nNumber of changes: " + toString(myNumChanges)).c_str());
    }
}


void
GNEConnectorFrame::initTargets() {
    // gather potential targets
    NBNode* nbn = myCurrentLane->getParentEdge().getGNEJunctionDestiny()->getNBNode();

    const EdgeVector& outgoing = nbn->getOutgoingEdges();
    for (EdgeVector::const_iterator it = outgoing.begin(); it != outgoing.end(); it++) {
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge((*it)->getID());
        const GNEEdge::LaneVector& lanes = edge->getLanes();
        for (GNEEdge::LaneVector::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
            myPotentialTargets.insert(*it_lane);
        }
    }
    // set color for existing connections
    const int fromIndex = myCurrentLane->getIndex();
    NBEdge* srcEdge = myCurrentLane->getParentEdge().getNBEdge();
    std::vector<NBEdge::Connection> connections = srcEdge->getConnectionsFromLane(fromIndex);
    for (std::set<GNELane*>::iterator it = myPotentialTargets.begin(); it != myPotentialTargets.end(); it++) {
        switch (getLaneStatus(connections, *it)) {
            case CONNECTED:
                (*it)->setSpecialColor(&targetColor);
                break;
            case CONNECTED_PASS:
                (*it)->setSpecialColor(&targetPassColor);
                break;
            case CONFLICTED:
                (*it)->setSpecialColor(&conflictColor);
                break;
            case UNCONNECTED:
                (*it)->setSpecialColor(&potentialTargetColor);
                break;
        }
    }
}


void
GNEConnectorFrame::cleanup() {
    // clean up
    for (std::set<GNELane*>::iterator it = myPotentialTargets.begin(); it != myPotentialTargets.end(); it++) {
        (*it)->setSpecialColor(0);
    }
    myPotentialTargets.clear();
    myNumChanges = 0;
    myCurrentLane->setSpecialColor(0);
    myCurrentLane = 0;
    myDeletedConnections.clear();
    updateDescription();
}


GNEConnectorFrame::LaneStatus
GNEConnectorFrame::getLaneStatus(const std::vector<NBEdge::Connection>& connections, GNELane* targetLane) {
    NBEdge* srcEdge = myCurrentLane->getParentEdge().getNBEdge();
    const int fromIndex = myCurrentLane->getIndex();
    NBEdge* destEdge = targetLane->getParentEdge().getNBEdge();
    const int toIndex = targetLane->getIndex();
    std::vector<NBEdge::Connection>::const_iterator con_it = find_if(
                connections.begin(), connections.end(),
                NBEdge::connections_finder(fromIndex, destEdge, toIndex));
    const bool isConnected = con_it != connections.end();
    if (isConnected) {
        if (con_it->mayDefinitelyPass) {
            return CONNECTED_PASS;
        } else {
            return CONNECTED;
        }
    } else if (srcEdge->hasConnectionTo(destEdge, toIndex)
               || (srcEdge->getPermissions(fromIndex) & destEdge->getPermissions(toIndex) & ~SVC_PEDESTRIAN) == 0) {
        return CONFLICTED;
    } else {
        return UNCONNECTED;
    }
}


/****************************************************************************/
