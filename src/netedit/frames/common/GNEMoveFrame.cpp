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

FXDEFMAP(GNEMoveFrame::ChangeZInSelection) ChangeZInSelectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ChangeZInSelection::onCmdChangeZValue),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEMoveFrame::ChangeZInSelection::onCmdChangeZMode),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ChangeZInSelection::onCmdApplyZ),
};

FXDEFMAP(GNEMoveFrame::ShiftEdgeGeometry) ShiftEdgeGeometryMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ShiftEdgeGeometry::onCmdChangeShiftValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ShiftEdgeGeometry::onCmdShiftEdgeGeometry),
};

// Object implementation
FXIMPLEMENT(GNEMoveFrame::ChangeZInSelection,   FXGroupBox, ChangeZInSelectionMap,  ARRAYNUMBER(ChangeZInSelectionMap))
FXIMPLEMENT(GNEMoveFrame::ShiftEdgeGeometry,    FXGroupBox, ShiftEdgeGeometryMap,   ARRAYNUMBER(ShiftEdgeGeometryMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMoveFrame::NetworkModeOptions - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::NetworkModeOptions::NetworkModeOptions(GNEMoveFrame* moveFrameParent) :
    FXGroupBox(moveFrameParent->myContentFrame, "Network move options", GUIDesignGroupBoxFrame),
    myMoveFrameParent(moveFrameParent) {
    // Create checkbox for enable/disable move whole polygons
    myMoveWholePolygons = new FXCheckButton(this, "Move whole polygons", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myMoveWholePolygons->setCheck(FALSE);
}


GNEMoveFrame::NetworkModeOptions::~NetworkModeOptions() {}


bool 
GNEMoveFrame::NetworkModeOptions::getMoveWholePolygons() const {
    if (myMoveFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
        (myMoveFrameParent->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE)) {
        return (myMoveWholePolygons->getCheck() == TRUE);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::ShiftEdgeGeometry - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ShiftEdgeGeometry::ShiftEdgeGeometry(GNEMoveFrame* moveFrameParent) :
    FXGroupBox(moveFrameParent->myContentFrame, "Shift edge geometry", GUIDesignGroupBoxFrame),
    myMoveFrameParent(moveFrameParent) {
    // create horizontal frame
    FXHorizontalFrame* myZValueFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(myZValueFrame, "Shift value", 0, GUIDesignLabelAttribute);
    myShiftValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myShiftValueTextField->setText("0");
    // create apply button
    new FXButton(this,
        "Shift edge geometry\t\tShift edge geometry orthogonally to driving direction for all selected edges",
        GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), this, MID_GNE_APPLY, GUIDesignButton);
}


GNEMoveFrame::ShiftEdgeGeometry::~ShiftEdgeGeometry() {}


void
GNEMoveFrame::ShiftEdgeGeometry::showShiftEdgeGeometry() {
    // show modul
    show();
}


void
GNEMoveFrame::ShiftEdgeGeometry::hideShiftEdgeGeometry() {
    // hide modul
    hide();
}


long
GNEMoveFrame::ShiftEdgeGeometry::onCmdChangeShiftValue(FXObject*, FXSelector, void*) {
    // just call onCmdShiftEdgeGeometry
    return onCmdShiftEdgeGeometry(nullptr, 0, nullptr);
}


long
GNEMoveFrame::ShiftEdgeGeometry::onCmdShiftEdgeGeometry(FXObject*, FXSelector, void*) {
    // get undo-list
    auto undoList = myMoveFrameParent->getViewNet()->getUndoList();
    // get value
    const double shiftValue = GNEAttributeCarrier::parse<double>(myShiftValueTextField->getText().text());
    // get selected edges
    const auto edges = myMoveFrameParent->getViewNet()->getNet()->retrieveEdges(true);
    // begin undo-redo 
    myMoveFrameParent->getViewNet()->getUndoList()->p_begin("shift edge geometries");
    // iterate over edges
    for (const auto& edge : edges) {
        // get edge geometry
        PositionVector edgeShape = edge->getNBEdge()->getGeometry();
        // shift edge geometry
        edgeShape.move2side(shiftValue);
        // get first and last position
        const Position shapeStart = edgeShape.front();
        const Position shapeEnd = edgeShape.back();
        // set innen geometry
        edgeShape.pop_front();
        edgeShape.pop_back();
        // set new shape again
        if (edgeShape.size() > 0) {
            edge->setAttribute(SUMO_ATTR_SHAPE, toString(edgeShape), undoList);
        }
        // set new start and end positions
        edge->setAttribute(GNE_ATTR_SHAPE_START, toString(shapeStart), undoList);
        edge->setAttribute(GNE_ATTR_SHAPE_END, toString(shapeEnd), undoList);
    }
    // end undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->p_end();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::ChangeZInSelection - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ChangeZInSelection::ChangeZInSelection(GNEMoveFrame* moveFrameParent) :
    FXGroupBox(moveFrameParent->myContentFrame, "Change Z in selection", GUIDesignGroupBoxFrame),
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
    // set info label
    myInfoLabel = new FXLabel(this, "", nullptr, GUIDesignLabelFrameInformation);
}


GNEMoveFrame::ChangeZInSelection::~ChangeZInSelection() {}


void 
GNEMoveFrame::ChangeZInSelection::showChangeZInSelection() {
    // update info label
    updateInfoLabel();
    // show modul
    show();
}


void
GNEMoveFrame::ChangeZInSelection::hideChangeZInSelection() {
    // hide modul
    hide();
}


long 
GNEMoveFrame::ChangeZInSelection::onCmdChangeZValue(FXObject*, FXSelector, void*) {
    // just call onCmdApplyZ
    return onCmdApplyZ(nullptr, 0, nullptr);
}


long 
GNEMoveFrame::ChangeZInSelection::onCmdChangeZMode(FXObject* obj, FXSelector, void*) {
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
GNEMoveFrame::ChangeZInSelection::onCmdApplyZ(FXObject*, FXSelector, void*) {
    // get undo-list
    auto undoList = myMoveFrameParent->getViewNet()->getUndoList();
    // get value
    const double zValue = GNEAttributeCarrier::parse<double>(myZValueTextField->getText().text());
    // get junctions
    const auto junctions = myMoveFrameParent->getViewNet()->getNet()->retrieveJunctions(true);
    // get selected edges
    const auto edges = myMoveFrameParent->getViewNet()->getNet()->retrieveEdges(true);
    // begin undo-redo 
    myMoveFrameParent->getViewNet()->getUndoList()->p_begin("change Z values in selection");
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
            junction->setAttribute(SUMO_ATTR_SHAPE, toString(junctionShape), undoList);
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
        junction->setAttribute(SUMO_ATTR_POSITION, toString(junctionPos), undoList);
    }
    // iterate over edges
    for (const auto& edge : edges) {
        // get edge geometry
        PositionVector edgeShape = edge->getNBEdge()->getInnerGeometry();
        // get first and last position
        Position shapeStart = edge->getNBEdge()->getGeometry().front();
        Position shapeEnd = edge->getNBEdge()->getGeometry().back();
        // modify z Value depending of absolute/relative
        for (auto& shapePos : edgeShape) {
            if (myAbsoluteValue->getCheck() == TRUE) {
                shapePos.setz(zValue);
            } else {
                shapePos.add(Position(0, 0, zValue));
            }
        }
        // modify begin an end positions
        if (myAbsoluteValue->getCheck() == TRUE) {
            shapeStart.setz(zValue);
            shapeEnd.setz(zValue);
        } else {
            shapeStart.add(Position(0, 0, zValue));
            shapeEnd.add(Position(0, 0, zValue));
        }
        // set new shape again
        if (edgeShape.size() > 0) {
            edge->setAttribute(SUMO_ATTR_SHAPE, toString(edgeShape), undoList);
        }
        // set new start and end positions
        if ((edge->getAttribute(GNE_ATTR_SHAPE_START).size() > 0) && 
            (shapeStart.distanceSquaredTo2D(edge->getParentJunctions().front()->getNBNode()->getPosition()) < 2)) {
            edge->setAttribute(GNE_ATTR_SHAPE_START, toString(shapeStart), undoList);
        }
        if ((edge->getAttribute(GNE_ATTR_SHAPE_END).size() > 0) && 
            (shapeEnd.distanceSquaredTo2D(edge->getParentJunctions().back()->getNBNode()->getPosition()) < 2)) {
            edge->setAttribute(GNE_ATTR_SHAPE_END, toString(shapeEnd), undoList);
        }
    }
    // end undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->p_end();
    // update info label
    updateInfoLabel();
    return 1;
}


void 
GNEMoveFrame::ChangeZInSelection::updateInfoLabel() {
    // get junctions
    const auto junctions = myMoveFrameParent->getViewNet()->getNet()->retrieveJunctions(true);
    // get selected edges
    const auto edges = myMoveFrameParent->getViewNet()->getNet()->retrieveEdges(true);
    // check if there is edges or junctions
    if ((junctions.size() > 0) || (edges.size() > 0)) {
        // declare minimum and maximun
        double selectionMinimum = 0;
        double selectionMaximun = 0;
        // set first values
        if (junctions.size() > 0) {
            selectionMinimum = junctions.front()->getNBNode()->getPosition().z();
            selectionMaximun = junctions.front()->getNBNode()->getPosition().z();
        } else {
            selectionMinimum = edges.front()->getNBEdge()->getGeometry().front().z();
            selectionMaximun = edges.front()->getNBEdge()->getGeometry().front().z();
        }
        // declare average
        double selectionAverage = 0;
        // declare numPoints
        int numPoints = 0;
        // iterate over junctions
        for (const auto& junction : junctions) {
            // get z
            const double z = junction->getNBNode()->getPosition().z();
            // check min
            if (z < selectionMinimum) {
                selectionMinimum = z;
            }
            // check max
            if (z > selectionMaximun) {
                selectionMaximun = z;
            }
            // update average
            selectionAverage += z;
            // update numPoints
            numPoints++;
        }
        // iterate over edges
        for (const auto& edge : edges) {
            // get innnen geometry
            const PositionVector innenGeometry = edge->getNBEdge()->getInnerGeometry();
            // iterate over innenGeometry
            for (const auto &geometryPoint : innenGeometry) {
                // check min
                if (geometryPoint.z() < selectionMinimum) {
                    selectionMinimum = geometryPoint.z();
                }
                // check max
                if (geometryPoint.z() > selectionMaximun) {
                    selectionMaximun = geometryPoint.z();
                }
                // update average
                selectionAverage += geometryPoint.z();
                // update numPoints
                numPoints++;
            }
            // check shape start
            if (edge->getAttribute(GNE_ATTR_SHAPE_START).size() > 0) {
                // get z
                const double z = edge->getNBEdge()->getGeometry().front().z();
                // check min
                if (z < selectionMinimum) {
                    selectionMinimum = z;
                }
                // check max
                if (z > selectionMaximun) {
                    selectionMaximun = z;
                }
                // update average
                selectionAverage += z;
                // update numPoints
                numPoints++;
            }
            // check shape end
            if (edge->getAttribute(GNE_ATTR_SHAPE_END).size() > 0) {
                // get z
                const double z = edge->getNBEdge()->getGeometry().back().z();
                // check min
                if (z < selectionMinimum) {
                    selectionMinimum = z;
                }
                // check max
                if (z > selectionMaximun) {
                    selectionMaximun = z;
                }
                // update average
                selectionAverage += z;
                // update numPoints
                numPoints++;
            }
        }
        // update average
        selectionAverage = (100 * selectionAverage) / (double)numPoints;
        // floor average
        selectionAverage = floor(selectionAverage);
        selectionAverage *= 0.01;
        // set label string
        const std::string labelStr = 
            "- Num geometry points: " + toString(numPoints) + "\n" +
            "- Selection minimum Z: " + toString(selectionMinimum) + "\n" +
            "- Selection maximum Z: " + toString(selectionMaximun) + "\n" +
            "- Selection average Z: " + toString(selectionAverage);
        // update info label
        myInfoLabel->setText(labelStr.c_str());
    }
}

// ---------------------------------------------------------------------------
// GNEMoveFrame - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::GNEMoveFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Move") {
    // create network mode options
    myNetworkModeOptions = new NetworkModeOptions(this);
    // create shift edge geometry modul
    myShiftEdgeGeometry = new ShiftEdgeGeometry(this);
    // create change z selection
    myChangeZInSelection = new ChangeZInSelection(this);
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
    // get selected junctions
    const auto junctions = myViewNet->getNet()->retrieveJunctions(true);
    // get selected edges
    const auto edges = myViewNet->getNet()->retrieveEdges(true);
    // check if there are junctions and edge selected
    if ((junctions.size() > 0) || (edges.size() > 0)) {
        myChangeZInSelection->showChangeZInSelection();
    } else {
        myChangeZInSelection->hideChangeZInSelection();
    }
    // check if there are edges selected
    if (edges.size() > 0) {
        myShiftEdgeGeometry->showShiftEdgeGeometry();
    } else {
        myShiftEdgeGeometry->hideShiftEdgeGeometry();
    }
    // show
    GNEFrame::show();
    // recalc and update
    update();
}


void
GNEMoveFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


GNEMoveFrame::NetworkModeOptions*
GNEMoveFrame::getNetworkModeOptions() const {
    return myNetworkModeOptions;
}

/****************************************************************************/
