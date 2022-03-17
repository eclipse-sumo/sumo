/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrameModules.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for draw shapes
/****************************************************************************/
#include <config.h>

#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDrawingShape.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEM_DrawingShape) DrawingShapeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STARTDRAWING,   GNEM_DrawingShape::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STOPDRAWING,    GNEM_DrawingShape::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORTDRAWING,   GNEM_DrawingShape::onCmdAbortDrawing)
};

// Object implementation
FXIMPLEMENT(GNEM_DrawingShape,               FXGroupBoxModule,     DrawingShapeMap,                ARRAYNUMBER(DrawingShapeMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEM_DrawingShape::GNEM_DrawingShape(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Drawing"),
    myFrameParent(frameParent),
    myDeleteLastCreatedPoint(false) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(getCollapsableFrame(), "Start drawing", 0, this, MID_GNE_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(getCollapsableFrame(), "Stop drawing", 0, this, MID_GNE_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(getCollapsableFrame(), "Abort drawing", 0, this, MID_GNE_ABORTDRAWING, GUIDesignButton);
    // create information label
    std::ostringstream information;
    information
            << "- 'Start drawing' or ENTER\n"
            << "  to create shape.\n"
            << "- 'Stop drawing' or ENTER to\n"
            << "  finish shape creation.\n"
            << "- 'Abort drawing' or ESC to\n"
            << "  abort shape creation.\n"
            << "- 'Shift + Click' to remove\n"
            << "  last inserted point.";
    myInformationLabel = new FXLabel(getCollapsableFrame(), information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEM_DrawingShape::~GNEM_DrawingShape() {}


void GNEM_DrawingShape::showDrawingShape() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBoxModule
    FXGroupBoxModule::show();
}


void GNEM_DrawingShape::hideDrawingShape() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBoxModule
    FXGroupBoxModule::hide();
}


void
GNEM_DrawingShape::startDrawing() {
    // Only start drawing if GNEM_DrawingShape modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEM_DrawingShape::stopDrawing() {
    // try to build shape
    if (myFrameParent->shapeDrawed()) {
        // clear created points
        myTemporalShape.clear();
        // change buttons
        myStartDrawingButton->enable();
        myStopDrawingButton->disable();
        myAbortDrawingButton->disable();
    } else {
        // abort drawing if shape cannot be created
        abortDrawing();
    }
}


void
GNEM_DrawingShape::abortDrawing() {
    // clear created points
    myTemporalShape.clear();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEM_DrawingShape::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEM_DrawingShape::removeLastPoint() {
    if (myTemporalShape.size() > 1) {
        myTemporalShape.pop_back();
    }
}


const PositionVector&
GNEM_DrawingShape::getTemporalShape() const {
    return myTemporalShape;
}


bool
GNEM_DrawingShape::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEM_DrawingShape::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEM_DrawingShape::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEM_DrawingShape::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEM_DrawingShape::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEM_DrawingShape::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

/****************************************************************************/
