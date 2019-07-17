/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECreateEdgeFrame.cpp
/// @author  Mirko Barthauer (Technische Universitaet Braunschweig)
/// @date    May 2018
/// @version $Id$
///
// The Widget for editing connection prohibits
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>

#include "GNECreateEdgeFrame.h"
#include "GNEInspectorFrame.h"

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
GNECreateEdgeFrame::processClick(const Position& clickedPosition, GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool opossiteEdge, bool chainEdge) {
    if (!myViewNet->getUndoList()->hasCommandGroup()) {
        myViewNet->getUndoList()->p_begin("create new " + toString(SUMO_TAG_EDGE));
    }
    if (!objectsUnderCursor.getJunctionFront()) {
        objectsUnderCursor.setCreatedJunction(myViewNet->getNet()->createJunction(myViewNet->snapToActiveGrid(clickedPosition), myViewNet->getUndoList()));
    }
    if (myCreateEdgeSource == nullptr) {
        myCreateEdgeSource = objectsUnderCursor.getJunctionFront();
        myCreateEdgeSource->markAsCreateEdgeSource();
        update();
    } else {
        if (myCreateEdgeSource != objectsUnderCursor.getJunctionFront()) {
            // may fail to prevent double edges
            GNEEdge* newEdge = myViewNet->getNet()->createEdge(
                                   myCreateEdgeSource, objectsUnderCursor.getJunctionFront(), myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList());
            if (newEdge) {
                // create another edge, if create opposite edge is enabled
                if (opossiteEdge) {
                    myViewNet->getNet()->createEdge(objectsUnderCursor.getJunctionFront(), myCreateEdgeSource, myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList(), "-" + newEdge->getNBEdge()->getID());
                }
                myCreateEdgeSource->unMarkAsCreateEdgeSource();

                if (myViewNet->getUndoList()->hasCommandGroup()) {
                    myViewNet->getUndoList()->p_end();
                } else {
                    std::cout << "edge created without an open CommandGroup )-:\n";
                }
                if (chainEdge) {
                    myCreateEdgeSource = objectsUnderCursor.getJunctionFront();
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
