/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/foxtools/MFXDynamicLabel.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMoveFrame::NetworkMoveOptions) NetworkMoveOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::NetworkMoveOptions::onCmdChangeOption)
};

FXDEFMAP(GNEMoveFrame::ChangeZInSelection) ChangeZInSelectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ChangeZInSelection::onCmdChangeZValue),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEMoveFrame::ChangeZInSelection::onCmdChangeZMode),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ChangeZInSelection::onCmdApplyZ),
};

FXDEFMAP(GNEMoveFrame::ShiftEdgeSelectedGeometry) ShiftEdgeGeometryMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ShiftEdgeSelectedGeometry::onCmdChangeShiftValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ShiftEdgeSelectedGeometry::onCmdShiftEdgeGeometry),
};

FXDEFMAP(GNEMoveFrame::ShiftShapeGeometry) ShiftShapeGeometryMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ShiftShapeGeometry::onCmdChangeShiftValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ShiftShapeGeometry::onCmdShiftShapeGeometry),
};

// Object implementation
FXIMPLEMENT(GNEMoveFrame::NetworkMoveOptions,           MFXGroupBoxModule, NetworkMoveOptionsMap,  ARRAYNUMBER(NetworkMoveOptionsMap))
FXIMPLEMENT(GNEMoveFrame::ChangeZInSelection,           MFXGroupBoxModule, ChangeZInSelectionMap,  ARRAYNUMBER(ChangeZInSelectionMap))
FXIMPLEMENT(GNEMoveFrame::ShiftEdgeSelectedGeometry,    MFXGroupBoxModule, ShiftEdgeGeometryMap,   ARRAYNUMBER(ShiftEdgeGeometryMap))
FXIMPLEMENT(GNEMoveFrame::ShiftShapeGeometry,           MFXGroupBoxModule, ShiftShapeGeometryMap,  ARRAYNUMBER(ShiftShapeGeometryMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMoveFrame::CommonMoveOptions - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::CommonMoveOptions::CommonMoveOptions(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Common move options")) {
    // Create checkbox for enable/disable allow change lanes
    myAllowChangeLanes = new FXCheckButton(getCollapsableFrame(), TL("Allow change lanes"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myAllowChangeLanes->setCheck(FALSE);
    // Create checkbox for enable/disable merge geometry points
    myMergeGeometryPoints = new FXCheckButton(getCollapsableFrame(), TL("Merge geometry points"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myMergeGeometryPoints->setCheck(TRUE);
}


GNEMoveFrame::CommonMoveOptions::~CommonMoveOptions() {}


bool
GNEMoveFrame::CommonMoveOptions::getAllowChangeLane() const {
    return (myAllowChangeLanes->getCheck() == TRUE);
}


bool
GNEMoveFrame::CommonMoveOptions::getMergeGeometryPoints() const {
    return (myMergeGeometryPoints->getCheck() == TRUE);
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::NetworkMoveOptions - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::NetworkMoveOptions::NetworkMoveOptions(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Network move options")),
    myMoveFrameParent(moveFrameParent) {
    // Create checkbox for enable/disable move whole polygons
    myMoveWholePolygons = new FXCheckButton(getCollapsableFrame(), TL("Move whole polygons"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myMoveWholePolygons->setCheck(FALSE);
    // Create checkbox for force draw end geometry points
    myForceDrawGeometryPoints = new FXCheckButton(getCollapsableFrame(), TL("Force draw geom. points"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myForceDrawGeometryPoints->setCheck(FALSE);
    // Create checkbox for force draw end geometry points
    myMoveOnlyJunctionCenter = new FXCheckButton(getCollapsableFrame(), TL("Move only junction center"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myMoveOnlyJunctionCenter->setCheck(FALSE);
}


GNEMoveFrame::NetworkMoveOptions::~NetworkMoveOptions() {}


void
GNEMoveFrame::NetworkMoveOptions::showNetworkMoveOptions() {
    recalc();
    show();
}


void
GNEMoveFrame::NetworkMoveOptions::hideNetworkMoveOptions() {
    hide();
}


bool
GNEMoveFrame::NetworkMoveOptions::getMoveWholePolygons() const {
    if (myMoveFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            (myMoveFrameParent->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE)) {
        return (myMoveWholePolygons->getCheck() == TRUE);
    } else {
        return false;
    }
}


bool
GNEMoveFrame::NetworkMoveOptions::getForceDrawGeometryPoints() const {
    if (myMoveFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            (myMoveFrameParent->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE)) {
        return (myForceDrawGeometryPoints->getCheck() == TRUE);
    } else {
        return false;
    }
}


bool
GNEMoveFrame::NetworkMoveOptions::getMoveOnlyJunctionCenter() const {
    if (myMoveFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            (myMoveFrameParent->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE)) {
        return (myMoveOnlyJunctionCenter->getCheck() == TRUE);
    } else {
        return false;
    }
}


long
GNEMoveFrame::NetworkMoveOptions::onCmdChangeOption(FXObject*, FXSelector, void*) {
    // just update viewNet
    myMoveFrameParent->getViewNet()->update();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::DemandMoveOptions - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::DemandMoveOptions::DemandMoveOptions(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Demand move options")),
    myMoveFrameParent(moveFrameParent) {
    // Create checkbox for enable/disable move whole polygons
    myLeaveStopPersonsConnected = new FXCheckButton(getCollapsableFrame(), TL("Leave stopPersons connected"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myLeaveStopPersonsConnected->setCheck(FALSE);
}


GNEMoveFrame::DemandMoveOptions::~DemandMoveOptions() {}


void
GNEMoveFrame::DemandMoveOptions::showDemandMoveOptions() {
    recalc();
    show();
}


void
GNEMoveFrame::DemandMoveOptions::hideDemandMoveOptions() {
    hide();
}


bool
GNEMoveFrame::DemandMoveOptions::getLeaveStopPersonsConnected() const {
    if (myMoveFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
            (myMoveFrameParent->getViewNet()->getEditModes().demandEditMode == DemandEditMode::DEMAND_MOVE)) {
        return (myLeaveStopPersonsConnected->getCheck() == TRUE);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::ShiftEdgeSelectedGeometry - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ShiftEdgeSelectedGeometry::ShiftEdgeSelectedGeometry(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Shift selected edges geometry")),
    myMoveFrameParent(moveFrameParent) {
    // create horizontal frame
    FXHorizontalFrame* myZValueFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(myZValueFrame, TL("Shift value"), 0, GUIDesignLabelThickedFixed(100));
    myShiftValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myShiftValueTextField->setText("0");
    // create apply button
    myApplyZValue = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Apply shift value"), "", TL("Shift edge geometry orthogonally to driving direction for all selected edges"),
                    GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), this, MID_GNE_APPLY, GUIDesignButton);
}


GNEMoveFrame::ShiftEdgeSelectedGeometry::~ShiftEdgeSelectedGeometry() {}


void
GNEMoveFrame::ShiftEdgeSelectedGeometry::enableShiftEdgeGeometry() {
    // enable elements
    myShiftValueTextField->enable();
    myApplyZValue->enable();
}


void
GNEMoveFrame::ShiftEdgeSelectedGeometry::disableShiftEdgeGeometry() {
    // enable elements
    myShiftValueTextField->disable();
    myApplyZValue->disable();
}


long
GNEMoveFrame::ShiftEdgeSelectedGeometry::onCmdChangeShiftValue(FXObject*, FXSelector, void*) {
    // just call onCmdShiftEdgeGeometry
    return onCmdShiftEdgeGeometry(nullptr, 0, nullptr);
}


long
GNEMoveFrame::ShiftEdgeSelectedGeometry::onCmdShiftEdgeGeometry(FXObject*, FXSelector, void*) {
    // get undo-list
    auto undoList = myMoveFrameParent->getViewNet()->getUndoList();
    // get value
    const double shiftValue = GNEAttributeCarrier::parse<double>(myShiftValueTextField->getText().text());
    // get selected edges
    const auto selectedEdges = myMoveFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedEdges();
    // begin undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::EDGE, "shift edge geometries");
    // iterate over edges
    for (const auto& edge : selectedEdges) {
        // get edge geometry
        PositionVector edgeShape = edge->getNBEdge()->getGeometry();
        // shift edge geometry
        edgeShape.move2side(shiftValue);
        // get first and last position
        const Position shapeStart = edgeShape.front();
        const Position shapeEnd = edgeShape.back();
        // set inner geometry
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
    myMoveFrameParent->getViewNet()->getUndoList()->end();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::ChangeZInSelection - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ChangeZInSelection::ChangeZInSelection(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Change Z in selection")),
    myMoveFrameParent(moveFrameParent) {
    // create horizontal frame
    FXHorizontalFrame* myZValueFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(myZValueFrame, TL("Z value"), 0, GUIDesignLabelThickedFixed(100));
    myZValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myZValueTextField->setText("0");
    // Create all options buttons
    myAbsoluteValue = GUIDesigns::buildFXRadioButton(getCollapsableFrame(), TL("Absolute value"), "", TL("Set Z value as absolute"),
                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myRelativeValue = GUIDesigns::buildFXRadioButton(getCollapsableFrame(), TL("Relative value"), "", TL("Set Z value as relative"),
                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create apply button
    myApplyButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Apply Z value"), "", TL("Apply Z value to all selected junctions"),
                    GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_APPLY, GUIDesignButton);
    // set absolute value as default
    myAbsoluteValue->setCheck(true);
    // set info label
    myInfoLabel = new FXLabel(getCollapsableFrame(), "", nullptr, GUIDesignLabelFrameInformation);
}


GNEMoveFrame::ChangeZInSelection::~ChangeZInSelection() {}


void
GNEMoveFrame::ChangeZInSelection::enableChangeZInSelection() {
    // enable elements
    myZValueTextField->enable();
    myAbsoluteValue->enable();
    myRelativeValue->enable();
    myApplyButton->enable();
    // update info label
    updateInfoLabel();
}


void
GNEMoveFrame::ChangeZInSelection::disableChangeZInSelection() {
    // disable elements
    myZValueTextField->disable();
    myAbsoluteValue->disable();
    myRelativeValue->disable();
    myApplyButton->disable();
}


long
GNEMoveFrame::ChangeZInSelection::onCmdChangeZValue(FXObject* /*obj*/, FXSelector /*sel*/, void*) {
    return 1;
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
    const auto selectedJunctions = myMoveFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedJunctions();
    // get selected edges
    const auto selectedEdges = myMoveFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedEdges();
    // begin undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::MODEMOVE, "change Z values in selection");
    // iterate over junctions
    for (const auto& junction : selectedJunctions) {
        if (junction->getNBNode()->hasCustomShape()) {
            // get junction position
            PositionVector junctionShape = junction->getNBNode()->getShape();
            // modify z Value depending of absolute/relative
            for (auto& shapePos : junctionShape) {
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
    for (const auto& edge : selectedEdges) {
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
                (shapeStart.distanceSquaredTo2D(edge->getFromJunction()->getNBNode()->getPosition()) < 2)) {
            edge->setAttribute(GNE_ATTR_SHAPE_START, toString(shapeStart), undoList);
        }
        if ((edge->getAttribute(GNE_ATTR_SHAPE_END).size() > 0) &&
                (shapeEnd.distanceSquaredTo2D(edge->getToJunction()->getNBNode()->getPosition()) < 2)) {
            edge->setAttribute(GNE_ATTR_SHAPE_END, toString(shapeEnd), undoList);
        }
    }
    // end undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->end();
    // update info label
    updateInfoLabel();
    return 1;
}


void
GNEMoveFrame::ChangeZInSelection::updateInfoLabel() {
    // get junctions
    const auto selectedJunctions = myMoveFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedJunctions();
    // get selected edges
    const auto selectedEdges = myMoveFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedEdges();
    // check if there is edges or junctions
    if ((selectedJunctions.size() > 0) || (selectedEdges.size() > 0)) {
        // declare minimum and maximum
        double selectionMinimum = 0;
        double selectionMaximum = 0;
        // set first values
        if (selectedJunctions.size() > 0) {
            selectionMinimum = selectedJunctions.front()->getNBNode()->getPosition().z();
            selectionMaximum = selectedJunctions.front()->getNBNode()->getPosition().z();
        } else {
            selectionMinimum = selectedEdges.front()->getNBEdge()->getGeometry().front().z();
            selectionMaximum = selectedEdges.front()->getNBEdge()->getGeometry().front().z();
        }
        // declare average
        double selectionAverage = 0;
        // declare numPoints
        int numPoints = 0;
        // iterate over junctions
        for (const auto& junction : selectedJunctions) {
            // get z
            const double z = junction->getNBNode()->getPosition().z();
            // check min
            if (z < selectionMinimum) {
                selectionMinimum = z;
            }
            // check max
            if (z > selectionMaximum) {
                selectionMaximum = z;
            }
            // update average
            selectionAverage += z;
            // update numPoints
            numPoints++;
        }
        // iterate over edges
        for (const auto& edge : selectedEdges) {
            // get inner geometry
            const PositionVector innerGeometry = edge->getNBEdge()->getInnerGeometry();
            // iterate over innerGeometry
            for (const auto& geometryPoint : innerGeometry) {
                // check min
                if (geometryPoint.z() < selectionMinimum) {
                    selectionMinimum = geometryPoint.z();
                }
                // check max
                if (geometryPoint.z() > selectionMaximum) {
                    selectionMaximum = geometryPoint.z();
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
                if (z > selectionMaximum) {
                    selectionMaximum = z;
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
                if (z > selectionMaximum) {
                    selectionMaximum = z;
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
            TL("- Num geometry points: ") + toString(numPoints) + "\n" +
            TL("- Selection minimum Z: ") + toString(selectionMinimum) + "\n" +
            TL("- Selection maximum Z: ") + toString(selectionMaximum) + "\n" +
            TL("- Selection average Z: ") + toString(selectionAverage);
        // update info label
        myInfoLabel->setText(labelStr.c_str());
    }
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::ShiftShapeGeometry - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ShiftShapeGeometry::ShiftShapeGeometry(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Shift shape geometry")),
    myMoveFrameParent(moveFrameParent) {
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameX = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(horizontalFrameX, "X value", 0, GUIDesignLabelThickedFixed(100));
    myShiftValueXTextField = new FXTextField(horizontalFrameX, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myShiftValueXTextField->setText("0");
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameY = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(horizontalFrameY, "Y value", 0, GUIDesignLabelThickedFixed(100));
    myShiftValueYTextField = new FXTextField(horizontalFrameY, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myShiftValueYTextField->setText("0");
    // create apply button
    GUIDesigns::buildFXButton(this,
                              TL("Shift shape geometry"), "", TL("Shift shape geometry orthogonally to driving direction for all selected shapes"),
                              GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), this, MID_GNE_APPLY, GUIDesignButton);
}


GNEMoveFrame::ShiftShapeGeometry::~ShiftShapeGeometry() {}


void
GNEMoveFrame::ShiftShapeGeometry::showShiftShapeGeometry() {
    // show module
    show();
}


void
GNEMoveFrame::ShiftShapeGeometry::hideShiftShapeGeometry() {
    // hide module
    hide();
}


long
GNEMoveFrame::ShiftShapeGeometry::onCmdChangeShiftValue(FXObject*, FXSelector, void*) {
    // just call onCmdShiftShapeGeometry
    return onCmdShiftShapeGeometry(nullptr, 0, nullptr);
}


long
GNEMoveFrame::ShiftShapeGeometry::onCmdShiftShapeGeometry(FXObject*, FXSelector, void*) {
    // get undo-list
    auto undoList = myMoveFrameParent->getViewNet()->getUndoList();
    // get values
    const double shiftValueX = GNEAttributeCarrier::parse<double>(myShiftValueXTextField->getText().text());
    const double shiftValueY = GNEAttributeCarrier::parse<double>(myShiftValueYTextField->getText().text());
    const Position shiftValue(shiftValueX, shiftValueY);
    // get selected polygons and POIs
    const auto selectedShapes = myMoveFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedShapes();
    std::vector<GNEAdditional*> polygons, POIs;
    for (const auto& shape : selectedShapes) {
        if (shape->getTagProperty()->getTag() == SUMO_TAG_POLY) {
            polygons.push_back(shape);
        } else {
            POIs.push_back(shape);
        }
    }
    // begin undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::POLY, "shift shape geometries");
    // iterate over shapes
    for (const auto& polygon : polygons) {
        // get shape geometry
        PositionVector shape = GNEAttributeCarrier::parse<PositionVector>(polygon->getAttribute(SUMO_ATTR_SHAPE));
        // shift shape geometry
        shape.add(shiftValue);
        // set new shape again
        polygon->setAttribute(SUMO_ATTR_SHAPE, toString(shape), undoList);
    }
    // iterate over POIs
    for (const auto& POI : POIs) {
        // currently only for POIs (not for POILanes or POIGEOs
        if (POI->getTagProperty()->hasAttribute(SUMO_ATTR_POSITION)) {
            // get shape geometry
            Position position = GNEAttributeCarrier::parse<Position>(POI->getAttribute(SUMO_ATTR_POSITION));
            // shift shape geometry
            position.add(shiftValue);
            // set new shape again
            POI->setAttribute(SUMO_ATTR_POSITION, toString(position), undoList);
        }
    }
    // end undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->end();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::Information - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::Information::Information(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Information")) {
    // create info label
    std::string info = std::string("- ") + TL("Click over edge to create or edit geometry point.") + std::string("\n") +
                       std::string("- ") + TL("Shift+click over edge to edit start or end geometry point.");
    new MFXDynamicLabel(getCollapsableFrame(), info.c_str(), 0, GUIDesignLabelFrameInformation);
}


GNEMoveFrame::Information::~Information() {}

// ---------------------------------------------------------------------------
// GNEMoveFrame - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::GNEMoveFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Move")) {
    // create common mode options
    myCommonMoveOptions = new CommonMoveOptions(this);
    // create network mode options
    myNetworkMoveOptions = new NetworkMoveOptions(this);
    // create demand mode options
    myDemandMoveOptions = new DemandMoveOptions(this);
    // create shift edge geometry module
    myShiftEdgeSelectedGeometry = new ShiftEdgeSelectedGeometry(this);
    // create change z selection
    myChangeZInSelection = new ChangeZInSelection(this);
    // create information label
    myInformation = new Information(this);
    // create shift shape geometry module
    myShiftShapeGeometry = new ShiftShapeGeometry(this);
}


GNEMoveFrame::~GNEMoveFrame() {}


void
GNEMoveFrame::processClick(const Position& /*clickedPosition*/,
                           const GNEViewNetHelper::ViewObjectsSelector& /*viewObjects*/,
                           const GNEViewNetHelper::ViewObjectsSelector& /*objectsUnderGrippedCursor*/) {
    // currently unused
}


void
GNEMoveFrame::show() {
    // show network options frames
    if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        myNetworkMoveOptions->showNetworkMoveOptions();
    } else {
        myNetworkMoveOptions->hideNetworkMoveOptions();
    }
    // show demand options frames
    if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        myDemandMoveOptions->showDemandMoveOptions();
    } else {
        myDemandMoveOptions->hideDemandMoveOptions();
    }
    // get selected junctions
    const auto selectedJunctions = myViewNet->getNet()->getAttributeCarriers()->getSelectedJunctions();
    // get selected edges
    const auto selectedEdges = myViewNet->getNet()->getAttributeCarriers()->getSelectedEdges();
    // check if there are junctions and edge selected
    if ((selectedJunctions.size() > 0) || (selectedEdges.size() > 0)) {
        myChangeZInSelection->enableChangeZInSelection();
    } else {
        myChangeZInSelection->disableChangeZInSelection();
    }
    // check if there are edges selected
    if (selectedEdges.size() > 0) {
        myShiftEdgeSelectedGeometry->enableShiftEdgeGeometry();
    } else {
        myShiftEdgeSelectedGeometry->disableShiftEdgeGeometry();
    }
    // check if there are shapes selected
    if (myViewNet->getNet()->getAttributeCarriers()->getSelectedShapes().size() > 0) {
        myShiftShapeGeometry->showShiftShapeGeometry();
    } else {
        myShiftShapeGeometry->hideShiftShapeGeometry();
    }
    // show
    GNEFrame::show();
    // recalc and update
    recalc();
    update();
}


void
GNEMoveFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


GNEMoveFrame::CommonMoveOptions*
GNEMoveFrame::getCommonMoveOptions() const {
    return myCommonMoveOptions;
}


GNEMoveFrame::NetworkMoveOptions*
GNEMoveFrame::getNetworkMoveOptions() const {
    return myNetworkMoveOptions;
}


GNEMoveFrame::DemandMoveOptions*
GNEMoveFrame::getDemandMoveOptions() const {
    return myDemandMoveOptions;
}

/****************************************************************************/
