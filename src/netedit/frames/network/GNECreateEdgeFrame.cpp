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
/// @file    GNECreateEdgeFrame.cpp
/// @author  Mirko Barthauer (Technische Universitaet Braunschweig)
/// @date    May 2018
///
// The Widget for editing connection prohibits
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/frames/common/GNEInspectorFrame.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNECreateEdgeFrame) GNECreateEdgeFrameMap[] = {
    /** currently unused **/
    FXMAPFUNC(SEL_COMMAND, MID_CANCEL, GNECreateEdgeFrame::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_OK, GNECreateEdgeFrame::onCmdOK)
    /** **/
};

// Object implementation
FXIMPLEMENT(GNECreateEdgeFrame, FXVerticalFrame, GNECreateEdgeFrameMap, ARRAYNUMBER(GNECreateEdgeFrameMap))

// ===========================================================================
// static members
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================

GNECreateEdgeFrame::GNECreateEdgeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Create Edge"),
    myCreateEdgeSource(nullptr) {
}


GNECreateEdgeFrame::~GNECreateEdgeFrame() {}


void
GNECreateEdgeFrame::processClick(const Position& clickedPosition, GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor,
                                 GNEViewNetHelper::ObjectsUnderCursor& objectsUnderGrippedCursor, const bool oppositeEdge, const bool chainEdge) {
    // obtain junction depending of gridEnabled
    GNEJunction* junction = nullptr;
    if (objectsUnderCursor.getJunctionFront()) {
        junction = objectsUnderCursor.getJunctionFront();
    } else if (objectsUnderGrippedCursor.getJunctionFront()) {
        junction = objectsUnderGrippedCursor.getJunctionFront();
    }
    // begin undo list
    if (!myViewNet->getUndoList()->hasCommandGroup()) {
        myViewNet->getUndoList()->p_begin("create new " + toString(SUMO_TAG_EDGE));
    }
    // if we didn't clicked over another junction, then create a new
    if (junction == nullptr) {
        junction = myViewNet->getNet()->createJunction(myViewNet->snapToActiveGrid(clickedPosition), myViewNet->getUndoList());
    }
    // now check if we have to create a new edge
    if (myCreateEdgeSource == nullptr) {
        myCreateEdgeSource = junction;
        myCreateEdgeSource->markAsCreateEdgeSource();
        update();
    } else {
        // make sure that junctions source and destiny are different
        if (myCreateEdgeSource != junction) {
            // may fail to prevent double edges
            GNEEdge* newEdge = myViewNet->getNet()->createEdge(myCreateEdgeSource, junction,
                               myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList());
            // check if edge was sucesfully created
            if (newEdge) {
                // create another edge, if create opposite edge is enabled
                if (oppositeEdge) {
                    myViewNet->getNet()->createEdge(junction, myCreateEdgeSource, myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(),
                                                    myViewNet->getUndoList(), "-" + newEdge->getNBEdge()->getID());
                }
                // edge created, then unmark as create edge source
                myCreateEdgeSource->unMarkAsCreateEdgeSource();
                // end undo list
                if (myViewNet->getUndoList()->hasCommandGroup()) {
                    myViewNet->getUndoList()->p_end();
                } else {
                    std::cout << "edge created without an open CommandGroup )-:\n";
                }
                // if we're creating edges in chain mode, mark junction as junction edge source
                if (chainEdge) {
                    myCreateEdgeSource = junction;
                    myCreateEdgeSource->markAsCreateEdgeSource();
                    myViewNet->getUndoList()->p_begin("create new " + toString(SUMO_TAG_EDGE));
                } else {
                    myCreateEdgeSource = nullptr;
                }
            } else {
                myViewNet->setStatusBarText("An " + toString(SUMO_TAG_EDGE) + " with the same geometry already exists!");
            }
        } else {
            myViewNet->setStatusBarText("Start- and endpoint for an " + toString(SUMO_TAG_EDGE) + " must be distinct!");
        }
        update();
    }
}


void GNECreateEdgeFrame::abortEdgeCreation() {
    // if myCreateEdgeSource exist, unmark ist as create edge source
    if (myCreateEdgeSource != nullptr) {
        // remove current created edge source
        myCreateEdgeSource->unMarkAsCreateEdgeSource();
        myCreateEdgeSource = nullptr;
    }
}


void
GNECreateEdgeFrame::show() {
    GNEFrame::show();
}


void
GNECreateEdgeFrame::hide() {
    GNEFrame::hide();
}


long
GNECreateEdgeFrame::onCmdCancel(FXObject*, FXSelector, void*) {
    /** unused **/
    return 1;
}


long
GNECreateEdgeFrame::onCmdOK(FXObject*, FXSelector, void*) {
    /** unused **/
    return 1;
}


/****************************************************************************/
