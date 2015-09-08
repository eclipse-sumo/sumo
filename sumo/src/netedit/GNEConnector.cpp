/****************************************************************************/
/// @file    GNEConnector.cpp
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying lane-to-lane connections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include "GNEConnector.h"
#include "GNESelector.h"
#include "GNEViewNet.h"
#include "GNEChange_Connection.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNELane.h"
#include "GNEInternalLane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEConnector) GNEConnectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,                 GNEConnector::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_OK,                     GNEConnector::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_DEAD_ENDS,   GNEConnector::onCmdSelectDeadEnds),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_DEAD_STARTS, GNEConnector::onCmdSelectDeadStarts),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_CONFLICTS,   GNEConnector::onCmdSelectConflicts),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_PASS,        GNEConnector::onCmdSelectPass),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,          GNEConnector::onCmdClearSelectedConnections),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_RESET,          GNEConnector::onCmdResetSelectedConnections),
};

// Object implementation
FXIMPLEMENT(GNEConnector, FXScrollWindow, GNEConnectorMap, ARRAYNUMBER(GNEConnectorMap))

// ===========================================================================
// static members
// ===========================================================================
const int GNEConnector::WIDTH = 140;
const RGBColor GNEConnector::sourceColor(RGBColor::CYAN);
const RGBColor GNEConnector::potentialTargetColor(RGBColor(0, 64, 0, 255));
const RGBColor GNEConnector::targetColor(RGBColor::GREEN);
const RGBColor GNEConnector::targetPassColor(RGBColor::MAGENTA);
const RGBColor GNEConnector::conflictColor(RGBColor::YELLOW);

// ===========================================================================
// method definitions
// ===========================================================================
GNEConnector::GNEConnector(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList):
    FXScrollWindow(parent, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0),
    myHeaderFont(new FXFont(getApp(), "Arial", 11, FXFont::Bold)),
    myCurrentLane(0),
    myUndoList(undoList),
    myUpdateTarget(updateTarget) {
    // heading
    myContentFrame = new FXVerticalFrame(this, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0);
    FXLabel* heading = new FXLabel(myContentFrame, "Edit Connections", 0, JUSTIFY_LEFT);
    heading->setFont(myHeaderFont);
    myDescription = new FXLabel(myContentFrame, "", 0, JUSTIFY_LEFT);
    new FXHorizontalSeparator(myContentFrame, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
    updateDescription();

    // buttons
    // "Cancel"
    new FXButton(myContentFrame, "Cancel\t\tDiscard connection modifications (Esc)", 0, this, MID_CANCEL,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "OK"
    new FXButton(myContentFrame, "OK\t\tSave connection modifications (Enter)", 0, this, MID_OK,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    new FXHorizontalSeparator(myContentFrame, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
    // "Select Dead Ends"
    new FXButton(myContentFrame,
                 "Select Dead Ends\t\tSelects all lanes that have no outgoing connection (clears previous selection)",
                 0, this, MID_GNE_SELECT_DEAD_ENDS,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Select Dead Starts"
    new FXButton(myContentFrame,
                 "Select Dead Starts\t\tSelects all lanes that have no incoming connection (clears previous selection)",
                 0, this, MID_GNE_SELECT_DEAD_STARTS,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Select Conflicts"
    new FXButton(myContentFrame,
                 "Select Conflicts\t\tSelects all lanes with more than one incoming connection from the same edge (clears previous selection)",
                 0, this, MID_GNE_SELECT_CONFLICTS,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Select Edges which may always pass"
    new FXButton(myContentFrame,
                 "Select Passing\t\tSelects all lanes with a connection that has has the 'pass' attribute set",
                 0, this, MID_GNE_SELECT_PASS,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Clear Selected"
    new FXButton(myContentFrame,
                 "Clear Selected\t\tClears all connections of all selected objects",
                 0, this, MID_CHOOSEN_CLEAR,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Reset Selected"
    new FXButton(myContentFrame,
                 "Reset Selected\nJunctions\t\tRecomputes connections at all selected junctions",
                 0, this, MID_CHOOSEN_RESET,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(this, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    // Selection Hint
    new FXLabel(myContentFrame, "Hold <SHIFT> while\nclicking to create\nunyielding conn's.\n", 0, JUSTIFY_LEFT);
    // Legend
    new FXHorizontalSeparator(myContentFrame, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
    FXLabel* l;
    new FXLabel(myContentFrame, "Color Legend:", 0, JUSTIFY_LEFT);
    l = new FXLabel(myContentFrame, "Source", 0, JUSTIFY_LEFT);
    l->setBackColor(MFXUtils::getFXColor(sourceColor));
    l = new FXLabel(myContentFrame, "Target", 0, JUSTIFY_LEFT);
    l->setBackColor(MFXUtils::getFXColor(targetColor));
    l = new FXLabel(myContentFrame, "Possible Target", 0, JUSTIFY_LEFT);
    l->setBackColor(MFXUtils::getFXColor(potentialTargetColor));
    l = new FXLabel(myContentFrame, "Target (pass)", 0, JUSTIFY_LEFT);
    l->setBackColor(MFXUtils::getFXColor(targetPassColor));
    l = new FXLabel(myContentFrame, "Conflict", 0, JUSTIFY_LEFT);
    l->setBackColor(MFXUtils::getFXColor(conflictColor));
}


GNEConnector::~GNEConnector() {
    delete myHeaderFont;
}


void
GNEConnector::handleLaneClick(GNELane* lane, bool mayDefinitelyPass, bool toggle) {
    if (myCurrentLane == 0) {
        myCurrentLane = lane;
        myCurrentLane->setSpecialColor(&sourceColor);
        initTargets();
        buildIinternalLanes(lane->getParentEdge().getNBEdge()->getToNode());
        myNumChanges = 0;
        myUndoList->p_begin("modify connections");
    } else if (myPotentialTargets.count(lane)) {
        const unsigned int fromIndex = myCurrentLane->getIndex();
        GNEEdge& srcEdge = myCurrentLane->getParentEdge();
        GNEEdge& destEdge = lane->getParentEdge();
        const std::string& destEdgeID = destEdge.getMicrosimID();
        std::vector<NBEdge::Connection> connections = srcEdge.getNBEdge()->getConnectionsFromLane(fromIndex);
        bool changed = false;
        NBConnection deletedConnection = NBConnection::InvalidConnection;
        const LaneStatus status = getLaneStatus(connections, lane);
        switch (status) {
            case UNCONNECTED:
                if (toggle) {
                    myUndoList->add(new GNEChange_Connection(&srcEdge, fromIndex,
                                    destEdgeID, lane->getIndex(), mayDefinitelyPass, true), true);
                    lane->setSpecialColor(mayDefinitelyPass ? &targetPassColor : &targetColor);
                    changed = true;
                }
                break;
            case CONNECTED:
            case CONNECTED_PASS:
                myUndoList->add(new GNEChange_Connection(&srcEdge, fromIndex, destEdgeID, lane->getIndex(),
                                status == CONNECTED_PASS, false), true);
                lane->setSpecialColor(&potentialTargetColor);
                changed = true;
                deletedConnection = NBConnection(srcEdge.getNBEdge(), fromIndex,
                                                 destEdge.getNBEdge(), lane->getIndex(),
                                                 (int)getTLLLinkNumber(connections, lane));
                break;
            case CONFLICTED:
                myUpdateTarget->setStatusBarText("Another lane from the same edge already connects to that lane");
                break;
        }
        if (changed) {
            myNumChanges += 1;
            GNEJunction* affected = myUpdateTarget->getNet()->retrieveJunction(srcEdge.getDest()->getMicrosimID());
            affected->invalidateTLS(myUndoList, deletedConnection);
            buildIinternalLanes(myCurrentLane->getParentEdge().getNBEdge()->getToNode());
        }
    } else {
        myUpdateTarget->setStatusBarText("Invalid target for connection");
    }
    updateDescription();
}



long
GNEConnector::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myCurrentLane != 0) {
        myUndoList->p_abort();
        if (myNumChanges) {
            myUpdateTarget->setStatusBarText("Changes reverted");
        }
        cleanup();
        myUpdateTarget->update();
    }
    return 1;
}


long
GNEConnector::onCmdOK(FXObject*, FXSelector, void*) {
    if (myCurrentLane != 0) {
        myUndoList->p_end();
        if (myNumChanges) {
            myUpdateTarget->setStatusBarText("Changes accepted");
        }
        cleanup();
        myUpdateTarget->update();
    }
    return 1;
}


long
GNEConnector::onCmdSelectDeadEnds(FXObject*, FXSelector, void*) {
    std::vector<GUIGlID> selectIDs;
    // every edge knows its outgoing connections so we can look at each edge in isolation
    const std::vector<GNEEdge*> edges = myUpdateTarget->getNet()->retrieveEdges();
    for (std::vector<GNEEdge*>::const_iterator edge_it = edges.begin(); edge_it != edges.end(); edge_it++) {
        const GNEEdge::LaneVector& lanes = (*edge_it)->getLanes();
        for (GNEEdge::LaneVector::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
            if ((*edge_it)->getNBEdge()->getConnectionsFromLane((*it_lane)->getIndex()).size() == 0) {
                selectIDs.push_back((*it_lane)->getGlID());
            }
        }
    }
    myUpdateTarget->getSelector()->handleIDs(selectIDs, false, GNESelector::SET_REPLACE);
    return 1;
}


long
GNEConnector::onCmdSelectDeadStarts(FXObject*, FXSelector, void*) {
    GNENet* net = myUpdateTarget->getNet();
    std::set<GUIGlID> selectIDs;
    // every edge knows only its outgoing connections so we look at whole junctions
    const std::vector<GNEJunction*> junctions = net->retrieveJunctions();
    for (std::vector<GNEJunction*>::const_iterator junction_it = junctions.begin(); junction_it != junctions.end(); junction_it++) {
        // first collect all outgoing lanes
        const EdgeVector& outgoing = (*junction_it)->getNBNode()->getOutgoingEdges();
        for (EdgeVector::const_iterator it = outgoing.begin(); it != outgoing.end(); it++) {
            GNEEdge* edge = net->retrieveEdge((*it)->getID());
            const std::set<GUIGlID> laneIDs = edge->getLaneGlIDs();
            for (std::set<GUIGlID>::const_iterator lid_it = laneIDs.begin(); lid_it != laneIDs.end(); lid_it++) {
                selectIDs.insert(*lid_it);
            }
        }
        // then remove all approached lanes
        const EdgeVector& incoming = (*junction_it)->getNBNode()->getIncomingEdges();
        for (EdgeVector::const_iterator it = incoming.begin(); it != incoming.end(); it++) {
            GNEEdge* edge = net->retrieveEdge((*it)->getID());
            NBEdge* nbe = edge->getNBEdge();
            const std::vector<NBEdge::Connection>& connections = nbe->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator con_it = connections.begin(); con_it != connections.end(); con_it++) {
                GNEEdge* approachedEdge = net->retrieveEdge(con_it->toEdge->getID());
                GNELane* approachedLane = approachedEdge->getLanes()[con_it->toLane];
                selectIDs.erase(approachedLane->getGlID());
            }
        }
    }
    myUpdateTarget->getSelector()->handleIDs(
        std::vector<GUIGlID>(selectIDs.begin(), selectIDs.end()),
        false, GNESelector::SET_REPLACE);
    return 1;
}


long
GNEConnector::onCmdSelectConflicts(FXObject*, FXSelector, void*) {
    std::vector<GUIGlID> selectIDs;
    // conflicts happen per edge so we can look at each edge in isolation
    const std::vector<GNEEdge*> edges = myUpdateTarget->getNet()->retrieveEdges();
    for (std::vector<GNEEdge*>::const_iterator edge_it = edges.begin(); edge_it != edges.end(); edge_it++) {
        NBEdge* nbe = (*edge_it)->getNBEdge();
        const EdgeVector destinations = nbe->getConnectedEdges();
        const std::vector<NBEdge::Connection>& connections = nbe->getConnections();
        for (EdgeVector::const_iterator dest_it = destinations.begin(); dest_it != destinations.end(); dest_it++) {
            GNEEdge* dest = myUpdateTarget->getNet()->retrieveEdge((*dest_it)->getID());
            const GNEEdge::LaneVector& destLanes = dest->getLanes();
            for (GNEEdge::LaneVector::const_iterator it_lane = destLanes.begin(); it_lane != destLanes.end(); it_lane++) {
                const bool isConflicted = count_if(
                                              connections.begin(), connections.end(),
                                              NBEdge::connections_toedgelane_finder(*dest_it, (int)(*it_lane)->getIndex(), -1)) > 1;
                if (isConflicted) {
                    selectIDs.push_back((*it_lane)->getGlID());
                }
            }
        }

    }
    myUpdateTarget->getSelector()->handleIDs(selectIDs, false, GNESelector::SET_REPLACE);
    return 1;
}


long
GNEConnector::onCmdSelectPass(FXObject*, FXSelector, void*) {
    std::vector<GUIGlID> selectIDs;
    const std::vector<GNEEdge*> edges = myUpdateTarget->getNet()->retrieveEdges();
    for (std::vector<GNEEdge*>::const_iterator edge_it = edges.begin(); edge_it != edges.end(); edge_it++) {
        GNEEdge* edge = *edge_it;
        NBEdge* nbe = edge->getNBEdge();
        const std::vector<NBEdge::Connection>& connections = nbe->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator it = connections.begin(); it != connections.end(); ++it) {
            if (it->mayDefinitelyPass) {
                GNELane* lane = edge->getLanes()[it->fromLane];
                selectIDs.push_back(lane->getGlID());
            }
        }
    }
    myUpdateTarget->getSelector()->handleIDs(selectIDs, false, GNESelector::SET_REPLACE);
    return 1;
}


long
GNEConnector::onCmdClearSelectedConnections(FXObject*, FXSelector, void*) {
    onCmdCancel(0, 0, 0);
    myUndoList->p_begin("clear connections from selected lanes, edges and junctions");
    const std::set<GUIGlID> ids = gSelected.getSelected();
    for (std::set<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        GUIGlID id = *it;
        GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        if (object) {
            switch (object->getType()) {
                case GLO_JUNCTION: {
                    GNEJunction* junction = dynamic_cast<GNEJunction*>(object);
                    junction->setLogicValid(false, myUndoList); // clear connections
                    junction->setLogicValid(false, myUndoList, GNEAttributeCarrier::MODIFIED); // prevent re-guessing
                    break;
                }
                case GLO_EDGE: {
                    const GNEEdge::LaneVector& lanes = dynamic_cast<GNEEdge*>(object)->getLanes();
                    for (GNEEdge::LaneVector::const_iterator l_it = lanes.begin(); l_it != lanes.end(); ++l_it) {
                        removeConnections(*l_it);
                    }
                    break;
                }
                case GLO_LANE:
                    removeConnections(dynamic_cast<GNELane*>(object));
                    break;
                default:
                    break;
            }
        }
    }
    myUndoList->p_end();
    return 1;
}


void
GNEConnector::removeConnections(GNELane* lane) {
    handleLaneClick(lane); // select as current lane
    for (std::set<GNELane*>::iterator it = myPotentialTargets.begin(); it != myPotentialTargets.end(); it++) {
        handleLaneClick(*it, false, false);  // deselect
    }
    onCmdOK(0, 0, 0); // save
}


long
GNEConnector::onCmdResetSelectedConnections(FXObject*, FXSelector, void*) {
    onCmdCancel(0, 0, 0);
    myUndoList->p_begin("reset connections from selected lanes");
    const std::set<GUIGlID> nodeIDs = gSelected.getSelected(GLO_JUNCTION);
    for (std::set<GUIGlID>::const_iterator nid_it = nodeIDs.begin(); nid_it != nodeIDs.end(); nid_it++) {
        GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(*nid_it);
        if (object) {
            GNEJunction* junction = dynamic_cast<GNEJunction*>(object);
            if (junction) {
                junction->setLogicValid(false, myUndoList);
            } else {
                throw ProcessError("Wrong object type returned from gIDStorage");
            }
        }
    }
    myUndoList->p_end();
    return 1;
}


void
GNEConnector::updateDescription() const {
    if (myCurrentLane == 0) {
        myDescription->setText("No Lane Selected\n");
    } else {
        myDescription->setText((
                                   myCurrentLane->getMicrosimID() + "\n(" +
                                   toString(myNumChanges) + " changes)").c_str());
    }
}


void
GNEConnector::initTargets() {
    // gather potential targets
    NBNode* nbn = myCurrentLane->getParentEdge().getDest()->getNBNode();

    const EdgeVector& outgoing = nbn->getOutgoingEdges();
    for (EdgeVector::const_iterator it = outgoing.begin(); it != outgoing.end(); it++) {
        GNEEdge* edge = myUpdateTarget->getNet()->retrieveEdge((*it)->getID());
        const GNEEdge::LaneVector& lanes = edge->getLanes();
        for (GNEEdge::LaneVector::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
            myPotentialTargets.insert(*it_lane);
        }
    }
    // set color for existing connections
    const unsigned int fromIndex = myCurrentLane->getIndex();
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
GNEConnector::cleanup() {
    // clean up
    for (std::set<GNELane*>::iterator it = myPotentialTargets.begin(); it != myPotentialTargets.end(); it++) {
        (*it)->setSpecialColor(0);
    }
    myPotentialTargets.clear();
    myNumChanges = 0;
    myCurrentLane->setSpecialColor(0);
    myCurrentLane = 0;
    buildIinternalLanes(0); // only clears
    updateDescription();
}


GNEConnector::LaneStatus
GNEConnector::getLaneStatus(const std::vector<NBEdge::Connection>& connections, GNELane* targetLane) {
    NBEdge* srcEdge = myCurrentLane->getParentEdge().getNBEdge();
    const unsigned int fromIndex = myCurrentLane->getIndex();
    NBEdge* destEdge = targetLane->getParentEdge().getNBEdge();
    const unsigned int toIndex = targetLane->getIndex();
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
    } else if (srcEdge->hasConnectionTo(destEdge, toIndex)) {
        return CONFLICTED;
    } else {
        return UNCONNECTED;
    }
}


unsigned int
GNEConnector::getTLLLinkNumber(const std::vector<NBEdge::Connection>& connections, GNELane* targetLane) {
    const unsigned int fromIndex = myCurrentLane->getIndex();
    NBEdge* destEdge = targetLane->getParentEdge().getNBEdge();
    const unsigned int toIndex = targetLane->getIndex();
    std::vector<NBEdge::Connection>::const_iterator it = find_if(
                connections.begin(), connections.end(),
                NBEdge::connections_finder(fromIndex, destEdge, toIndex));
    assert(it != connections.end());
    return it->tlLinkNo;
}


void
GNEConnector::buildIinternalLanes(NBNode* node) {
    // clean up previous objects
    SUMORTree& rtree = myUpdateTarget->getNet()->getVisualisationSpeedUp();
    for (std::map<int, GNEInternalLane*>::iterator it = myInternalLanes.begin(); it != myInternalLanes.end(); it++) {
        rtree.removeAdditionalGLObject(it->second);
        delete it->second;
    }
    myInternalLanes.clear();
    if (node != 0) {
        const int NUM_POINTS = 5;
        SUMORTree& rtree = myUpdateTarget->getNet()->getVisualisationSpeedUp();
        std::string innerID = ":" + node->getID(); // see NWWriter_SUMO::writeInternalEdges
        
        int index = 0;
        const EdgeVector& incoming = node->getIncomingEdges();
        for (EdgeVector::const_iterator it_edg = incoming.begin(); it_edg != incoming.end(); it_edg++) {
            const std::vector<NBEdge::Connection>& conns = (*it_edg)->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator it_con = conns.begin(); it_con != conns.end(); ++it_con) {
                const PositionVector shape = node->computeInternalLaneShape(*it_edg, *it_con, NUM_POINTS);
                LinkState state = node->getLinkState(*it_edg, it_con->toEdge, it_con->fromLane, it_con->mayDefinitelyPass, it_con->tlID);
                GNEInternalLane* ilane = new GNEInternalLane(0, innerID + '_' + toString(index) , shape, -1, state);
                rtree.addAdditionalGLObject(ilane);
                myInternalLanes[index] = ilane;
                index++;
            }
        }
    }
}


/****************************************************************************/
