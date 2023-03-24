/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

FXDEFMAP(GNEMoveFrame::ShiftEdgeSelectedGeometry) ShiftEdgeGeometryMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ShiftEdgeSelectedGeometry::onCmdChangeShiftValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ShiftEdgeSelectedGeometry::onCmdShiftEdgeGeometry),
};

FXDEFMAP(GNEMoveFrame::ShiftShapeGeometry) ShiftShapeGeometryMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ShiftShapeGeometry::onCmdChangeShiftValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ShiftShapeGeometry::onCmdShiftShapeGeometry),
};


// Object implementation
FXIMPLEMENT(GNEMoveFrame::ChangeZInSelection,           MFXGroupBoxModule, ChangeZInSelectionMap,  ARRAYNUMBER(ChangeZInSelectionMap))
FXIMPLEMENT(GNEMoveFrame::ShiftEdgeSelectedGeometry,    MFXGroupBoxModule, ShiftEdgeGeometryMap,   ARRAYNUMBER(ShiftEdgeGeometryMap))
FXIMPLEMENT(GNEMoveFrame::ShiftShapeGeometry,           MFXGroupBoxModule, ShiftShapeGeometryMap,  ARRAYNUMBER(ShiftShapeGeometryMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMoveFrame::CommonModeOptions - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::CommonModeOptions::CommonModeOptions(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Common move options")) {
    // Create checkbox for enable/disable allow change lanes
    myAllowChangeLanes = new FXCheckButton(getCollapsableFrame(), TL("Allow change lanes"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myAllowChangeLanes->setCheck(FALSE);
    // Create checkbox for enable/disable merge geometry points
    myMergeGeometryPoints = new FXCheckButton(getCollapsableFrame(), TL("Merge geometry points"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myMergeGeometryPoints->setCheck(TRUE);
}


GNEMoveFrame::CommonModeOptions::~CommonModeOptions() {}


bool
GNEMoveFrame::CommonModeOptions::getAllowChangeLane() const {
    return (myAllowChangeLanes->getCheck() == TRUE);
}


bool
GNEMoveFrame::CommonModeOptions::getMergeGeometryPoints() const {
    return (myMergeGeometryPoints->getCheck() == TRUE);
}

// ---------------------------------------------------------------------------
// GNEMoveFrame::NetworkModeOptions - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::NetworkModeOptions::NetworkModeOptions(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Network move options")),
    myMoveFrameParent(moveFrameParent) {
    // Create checkbox for enable/disable move whole polygons
    myMoveWholePolygons = new FXCheckButton(getCollapsableFrame(), TL("Move whole polygons"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myMoveWholePolygons->setCheck(FALSE);
}


GNEMoveFrame::NetworkModeOptions::~NetworkModeOptions() {}


void
GNEMoveFrame::NetworkModeOptions::showNetworkModeOptions() {
    recalc();
    show();
}


void
GNEMoveFrame::NetworkModeOptions::hideNetworkModeOptions() {
    hide();
}


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
// GNEMoveFrame::DemandModeOptions - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::DemandModeOptions::DemandModeOptions(GNEMoveFrame* moveFrameParent) :
    MFXGroupBoxModule(moveFrameParent, TL("Demand move options")),
    myMoveFrameParent(moveFrameParent) {
    // Create checkbox for enable/disable move whole polygons
    myLeaveStopPersonsConnected = new FXCheckButton(getCollapsableFrame(), TL("Leave stopPersons connected"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myLeaveStopPersonsConnected->setCheck(FALSE);
}


GNEMoveFrame::DemandModeOptions::~DemandModeOptions() {}


void
GNEMoveFrame::DemandModeOptions::showDemandModeOptions() {
    recalc();
    show();
}


void
GNEMoveFrame::DemandModeOptions::hideDemandModeOptions() {
    hide();
}


bool
GNEMoveFrame::DemandModeOptions::getLeaveStopPersonsConnected() const {
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
    new FXLabel(myZValueFrame, "Shift value", 0, GUIDesignLabelThickedFixed(100));
    myShiftValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myShiftValueTextField->setText("0");
    // create apply button
    myApplyZValue = new FXButton(getCollapsableFrame(), (TL("Apply shift value") + std::string("\t\t") + TL("Shift edge geometry orthogonally to driving direction for all selected edges")).c_str(),
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
    new FXLabel(myZValueFrame, "Z value", 0, GUIDesignLabelThickedFixed(100));
    myZValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myZValueTextField->setText("0");
    // Create all options buttons
    myAbsoluteValue = new FXRadioButton(getCollapsableFrame(), (TL("Absolute value") + std::string("\t\t") + TL("Set Z value as absolute")).c_str(),
                                        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myRelativeValue = new FXRadioButton(getCollapsableFrame(), (TL("Relative value") + std::string("\t\t") + TL("Set Z value as relative")).c_str(),
                                        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create apply button
    myApplyButton = new FXButton(getCollapsableFrame(), (TL("Apply Z value") + std::string("\t\t") + TL("Apply Z value to all selected junctions")).c_str(),
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
        // declare minimum and maximun
        double selectionMinimum = 0;
        double selectionMaximun = 0;
        // set first values
        if (selectedJunctions.size() > 0) {
            selectionMinimum = selectedJunctions.front()->getNBNode()->getPosition().z();
            selectionMaximun = selectedJunctions.front()->getNBNode()->getPosition().z();
        } else {
            selectionMinimum = selectedEdges.front()->getNBEdge()->getGeometry().front().z();
            selectionMaximun = selectedEdges.front()->getNBEdge()->getGeometry().front().z();
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
            if (z > selectionMaximun) {
                selectionMaximun = z;
            }
            // update average
            selectionAverage += z;
            // update numPoints
            numPoints++;
        }
        // iterate over edges
        for (const auto& edge : selectedEdges) {
            // get innnen geometry
            const PositionVector innenGeometry = edge->getNBEdge()->getInnerGeometry();
            // iterate over innenGeometry
            for (const auto& geometryPoint : innenGeometry) {
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
    new FXButton(this,
                 (TL("Shift shape geometry") + std::string("\t\t") + TL("Shift shape geometry orthogonally to driving direction for all selected shapes")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), this, MID_GNE_APPLY, GUIDesignButton);
}


GNEMoveFrame::ShiftShapeGeometry::~ShiftShapeGeometry() {}


void
GNEMoveFrame::ShiftShapeGeometry::showShiftShapeGeometry() {
    // show modul
    show();
}


void
GNEMoveFrame::ShiftShapeGeometry::hideShiftShapeGeometry() {
    // hide modul
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
        if (shape->getTagProperty().getTag() == SUMO_TAG_POLY) {
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
        if (POI->getTagProperty().hasAttribute(SUMO_ATTR_POSITION)) {
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
    std::ostringstream information;
    // add label for shift+click
    information
            << TL("-Click over edge to") << "\n"
            << TL(" create or edit") << "\n"
            << TL(" geometry point.") << "\n"
            << TL("-Shift+click over edge") << "\n"
            << TL(" to edit start or end") << "\n"
            << TL(" geometry point.");
    // create label
    new FXLabel(getCollapsableFrame(), information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // create source label
    FXLabel* sourceLabel = new FXLabel(getCollapsableFrame(), TL("-Move geometry point"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    sourceLabel->setBackColor(MFXUtils::getFXColor(RGBColor::ORANGE));
    // create target label
    FXLabel* targetLabel = new FXLabel(getCollapsableFrame(), TL("-Merge geometry point"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    targetLabel->setBackColor(MFXUtils::getFXColor(RGBColor::CYAN));
}


GNEMoveFrame::Information::~Information() {}

// ---------------------------------------------------------------------------
// GNEMoveFrame - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::GNEMoveFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Move") {
    // create common mode options
    myCommonModeOptions = new CommonModeOptions(this);
    // create network mode options
    myNetworkModeOptions = new NetworkModeOptions(this);
    // create demand mode options
    myDemandModeOptions = new DemandModeOptions(this);
    // create shift edge geometry modul
    myShiftEdgeSelectedGeometry = new ShiftEdgeSelectedGeometry(this);
    // create change z selection
    myChangeZInSelection = new ChangeZInSelection(this);
    // create information label
    myInformation = new Information(this);
    // create shift shape geometry modul
    myShiftShapeGeometry = new ShiftShapeGeometry(this);
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
    // show network options frames
    if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        myNetworkModeOptions->showNetworkModeOptions();
    } else {
        myNetworkModeOptions->hideNetworkModeOptions();
    }
    // show demand options frames
    if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        myDemandModeOptions->showDemandModeOptions();
    } else {
        myDemandModeOptions->hideDemandModeOptions();
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


GNEMoveFrame::CommonModeOptions*
GNEMoveFrame::getCommonModeOptions() const {
    return myCommonModeOptions;
}


GNEMoveFrame::NetworkModeOptions*
GNEMoveFrame::getNetworkModeOptions() const {
    return myNetworkModeOptions;
}


GNEMoveFrame::DemandModeOptions*
GNEMoveFrame::getDemandModeOptions() const {
    return myDemandModeOptions;
}

/****************************************************************************/
