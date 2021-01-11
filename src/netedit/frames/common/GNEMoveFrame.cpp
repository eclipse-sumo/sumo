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
/// @file    GNEMoveFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2020
///
// The Widget for move elements
/****************************************************************************/
#include <config.h>

#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMoveFrame::ChangeJunctionsZ) ChangeJunctionsZMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ChangeJunctionsZ::onCmdChangeZValue),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEMoveFrame::ChangeJunctionsZ::onCmdChangeZMode),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ChangeJunctionsZ::onCmdApplyZ),
};

// Object implementation
FXIMPLEMENT(GNEMoveFrame::ChangeJunctionsZ, FXGroupBox, ChangeJunctionsZMap, ARRAYNUMBER(ChangeJunctionsZMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMoveFrame::ChangeJunctionsZ - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ChangeJunctionsZ::ChangeJunctionsZ(GNEMoveFrame* moveFrameParent) :
    FXGroupBox(moveFrameParent->myContentFrame, "Change junctions Z", GUIDesignGroupBoxFrame),
    myMoveFrameParent(moveFrameParent) {
    // create horizontal frame
    FXHorizontalFrame* myZValueFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(myZValueFrame, "Z value", 0, GUIDesignLabelAttribute);
    myZValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myZValueTextField->setText("0");
    // Create all options buttons
    myAbsoluteValue = new FXRadioButton(this, "Absolute value\t\tSet Z value as absolute",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myRelativeValue = new FXRadioButton(this, "Relative value\t\tSet Z value as relative",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create apply button
    new FXButton(this,
        "Apply Z value\t\tApply Z value to all selected junctions",
        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_APPLY, GUIDesignButton);
    // set absolute value as default
    myAbsoluteValue->setCheck(true);
}


GNEMoveFrame::ChangeJunctionsZ::~ChangeJunctionsZ() {}


void 
GNEMoveFrame::ChangeJunctionsZ::showChangeJunctionsZ() {
    // show modul
    show();
}


void
GNEMoveFrame::ChangeJunctionsZ::hideChangeJunctionsZ() {
    // hide modul
    hide();
}


long 
GNEMoveFrame::ChangeJunctionsZ::onCmdChangeZValue(FXObject*, FXSelector, void*) {
    // nothing to do
    return 1;
}


long 
GNEMoveFrame::ChangeJunctionsZ::onCmdChangeZMode(FXObject* obj, FXSelector, void*) {
    if (obj == myAbsoluteValue) {
        myAbsoluteValue->setCheck(true);
        myRelativeValue->setCheck(false);
    } else {
        myAbsoluteValue->setCheck(false);
        myRelativeValue->setCheck(true);
    }
    return 1;
}


long
GNEMoveFrame::ChangeJunctionsZ::onCmdApplyZ(FXObject*, FXSelector, void*) {
    // get value
    const double zValue = GNEAttributeCarrier::parse<double>(myZValueTextField->getText().text());
    // get junctions
    const auto junctions = myMoveFrameParent->getViewNet()->getNet()->retrieveJunctions(true);
    // begin undo-redo 
    myMoveFrameParent->getViewNet()->getUndoList()->p_begin("Change junctions z values");
    // iterate over junctions
    for (const auto& junction : junctions) {
        if (junction->getNBNode()->hasCustomShape()) {
            // get junction position
            PositionVector junctionShape = junction->getNBNode()->getShape();
            // modify z Value depending of absolute/relative
            for (auto &shapePos : junctionShape) {
                if (myAbsoluteValue->getCheck() == TRUE) {
                    shapePos.setz(zValue);
                } else {
                    shapePos.add(Position(0, 0, zValue));
                }
            }
            // set new position again
            junction->setAttribute(SUMO_ATTR_SHAPE, toString(junctionShape), myMoveFrameParent->getViewNet()->getUndoList());
        }
        // get junction position
        Position junctionPos = junction->getNBNode()->getPosition();
        // modify z Value depending of absolute/relative
        if (myAbsoluteValue->getCheck() == TRUE) {
            junctionPos.setz(zValue);
        } else {
            junctionPos.add(Position(0, 0, zValue));
        }
        // set new position again
        junction->setAttribute(SUMO_ATTR_POSITION, toString(junctionPos), myMoveFrameParent->getViewNet()->getUndoList());
    }
    // end undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->p_end();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMoveFrame - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::GNEMoveFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Move"),
    myChangeJunctionsZ(nullptr) {
    // create change junctions z
    myChangeJunctionsZ = new ChangeJunctionsZ(this);
}


GNEMoveFrame::~GNEMoveFrame() {}


void
GNEMoveFrame::processClick(const Position& /*clickedPosition*/,
                           const GNEViewNetHelper::ObjectsUnderCursor& /*objectsUnderCursor*/,
                           const GNEViewNetHelper::ObjectsUnderCursor& /*objectsUnderGrippedCursor*/) {
    // currently unused
}


void
GNEMoveFrame::show() {
    // check if there are junctions selected
    if (myViewNet->getNet()->retrieveJunctions(true).size() > 0) {
        myChangeJunctionsZ->showChangeJunctionsZ();
    } else {
        myChangeJunctionsZ->hideChangeJunctionsZ();
    }
    // recalc and update
    recalc();
    update();
    // show
    GNEFrame::show();
}


void
GNEMoveFrame::hide() {
    // hide frame
    GNEFrame::hide();
}

/****************************************************************************/
