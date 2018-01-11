/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEFrame.h"
#include "GNEViewParent.h"
#include "GNEViewNet.h"
#include "GNEAttributeCarrier.h"
#include "GNEInspectorFrame.h"
#include "GNEPolygonFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrame::NeteditAttributes) GNEFrameNeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_BLOCKMOVEMENT,  GNEFrame::NeteditAttributes::onCmdSetBlockMovement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_BLOCKING_SHAPE,             GNEFrame::NeteditAttributes::onCmdSetBlockShape),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_CLOSE,             GNEFrame::NeteditAttributes::onCmdsetClosingShape),
};

FXDEFMAP(GNEFrame::GEOAttributes) GNEFrameGEOAttributes[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNEFRAME_GEOATTRIBUTE,              GNEFrame::GEOAttributes::onCmdSetGEOAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNEFRAME_USEGEO,                    GNEFrame::GEOAttributes::onCmdUseGEOParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                               GNEFrame::GEOAttributes::onCmdHelp),
};

FXDEFMAP(GNEFrame::DrawingMode) GNEFrameDrawingModeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_STARTDRAWING,      GNEFrame::DrawingMode::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_STOPDRAWING,       GNEFrame::DrawingMode::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_ABORTDRAWING,      GNEFrame::DrawingMode::onCmdAbortDrawing),
};

// Object implementation
FXIMPLEMENT(GNEFrame::NeteditAttributes,    FXGroupBox,     GNEFrameNeteditAttributesMap,   ARRAYNUMBER(GNEFrameNeteditAttributesMap))
FXIMPLEMENT(GNEFrame::GEOAttributes,        FXGroupBox,     GNEFrameGEOAttributes,          ARRAYNUMBER(GNEFrameGEOAttributes))
FXIMPLEMENT(GNEFrame::DrawingMode,          FXGroupBox,     GNEFrameDrawingModeMap,         ARRAYNUMBER(GNEFrameDrawingModeMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrame::NeteditAttributes - methods
// ---------------------------------------------------------------------------

GNEFrame::NeteditAttributes::NeteditAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame) {
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockMovementLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_ADDITIONALFRAME_BLOCKMOVEMENT, GUIDesignCheckButtonAttribute);
    myBlockMovementCheckButton->setCheck(false);
    // Create Frame for block shape label and checkBox (By default disabled)
    myBlockShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockShapeLabel = new FXLabel(myBlockShapeFrame, "block shape", 0, GUIDesignLabelAttribute);
    myBlockShapeCheckButton = new FXCheckButton(myBlockShapeFrame, "false", this, MID_GNE_SET_BLOCKING_SHAPE, GUIDesignCheckButtonAttribute);
    // Create Frame for block close polygon and checkBox (By default disabled)
    myClosePolygonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myClosePolygonLabel = new FXLabel(myClosePolygonFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myClosePolygonCheckButton = new FXCheckButton(myClosePolygonFrame, "false", this, MID_GNE_POLYGONFRAME_CLOSE, GUIDesignCheckButtonAttribute);
    myBlockShapeCheckButton->setCheck(false);
}


GNEFrame::NeteditAttributes::~NeteditAttributes() {}


void
GNEFrame::NeteditAttributes::showNeteditAttributes(bool shapeEditing) {
    // show block and closing sahpe depending of shapeEditing
    if (shapeEditing) {
        myBlockShapeFrame->show();
        myClosePolygonFrame->show();
    } else {
        myBlockShapeFrame->hide();
        myClosePolygonFrame->hide();
    }
    FXGroupBox::show();
}


void
GNEFrame::NeteditAttributes::hideNeteditAttributes() {
    FXGroupBox::hide();
}


bool
GNEFrame::NeteditAttributes::isBlockMovementEnabled() const {
    return myBlockMovementCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEFrame::NeteditAttributes::isBlockShapeEnabled() const {
    return myBlockShapeCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEFrame::NeteditAttributes::isCloseShapeEnabled() const {
    return myClosePolygonCheckButton->getCheck() == 1 ? true : false;
}


long
GNEFrame::NeteditAttributes::onCmdSetBlockMovement(FXObject*, FXSelector, void*) {
    if (myBlockMovementCheckButton->getCheck()) {
        myBlockMovementCheckButton->setText("true");
    } else {
        myBlockMovementCheckButton->setText("false");
    }
    return 1;
}


long
GNEFrame::NeteditAttributes::onCmdSetBlockShape(FXObject*, FXSelector, void*) {
    if (myBlockShapeCheckButton->getCheck()) {
        myBlockShapeCheckButton->setText("true");
    } else {
        myBlockShapeCheckButton->setText("false");
    }
    return 1;
}


long
GNEFrame::NeteditAttributes::onCmdsetClosingShape(FXObject*, FXSelector, void*) {
    if (myClosePolygonCheckButton->getCheck()) {
        myClosePolygonCheckButton->setText("true");
    } else {
        myClosePolygonCheckButton->setText("false");
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::GEOAttributes - methods
// ---------------------------------------------------------------------------

GNEFrame::GEOAttributes::GEOAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "GEO Attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {

    // Create Frame for GEOAttribute
    myGEOAttributeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myGEOAttributeLabel = new FXLabel(myGEOAttributeFrame, "Undefined GEO Attribute", 0, GUIDesignLabelAttribute);
    myGEOAttributeTextField = new FXTextField(myGEOAttributeFrame, GUIDesignTextFieldNCol, this, MID_GNEFRAME_GEOATTRIBUTE, GUIDesignTextField);

    // Create Frame for use GEO
    myUseGEOFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myUseGEOLabel = new FXLabel(myUseGEOFrame, "Use GEO", 0, GUIDesignLabelAttribute);
    myUseGEOCheckButton = new FXCheckButton(myUseGEOFrame, "false", this, MID_GNEFRAME_USEGEO, GUIDesignCheckButtonAttribute);

    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrame::GEOAttributes::~GEOAttributes() {}


void
GNEFrame::GEOAttributes::showGEOAttributes(const std::vector<GNEAttributeCarrier*>& ACs) {
    // make sure that ACs has elements
    if (ACs.size() > 0) {
        // set myACs with the inspected elements
        myACs = ACs;
        if (ACs.front()->getTag() == SUMO_TAG_POLY) {
            myGEOAttribute = SUMO_ATTR_GEOSHAPE;
            // set label name
            myGEOAttributeLabel->setText(toString(myGEOAttribute).c_str());
            // fill attributes using refresh attributes
            refreshGEOAttributes();
            // show FXGroupBox
            FXGroupBox::show();
        } else if (ACs.front()->getTag() == SUMO_TAG_POI) {
            myGEOAttribute = SUMO_ATTR_GEOPOSITION;
            // set label name
            myGEOAttributeLabel->setText(toString(myGEOAttribute).c_str());
            // fill attributes using refresh attributes
            refreshGEOAttributes();
            // show FXGroupBox
            FXGroupBox::show();
        } else {
            // hide GEO Attributes
            hideGEOAttributes();
        }
    } else {
        // hide GEO Attributes
        hideGEOAttributes();
    }
}


void
GNEFrame::GEOAttributes::hideGEOAttributes() {
    myACs.clear();
    myGEOAttribute = SUMO_ATTR_NOTHING;
    // hide FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrame::GEOAttributes::refreshGEOAttributes() {
    // only refresh element if myACs has elements
    if (myACs.size() > 0) {
        // hide GEOAttribute Frame
        myGEOAttributeFrame->hide();
        // check if we're handling a single or multiple selection
        if (myACs.size() > 1) {
            // only useGEO can be changed in multiple selections
            bool useGEO = true;
            for (auto i : myACs) {
                useGEO &= GNEAttributeCarrier::parse<bool>(i->getAttribute(SUMO_ATTR_GEO));
            }
            myUseGEOCheckButton->setCheck(useGEO);
        } else {
            myUseGEOCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(myACs.front()->getAttribute(SUMO_ATTR_GEO)));
            // show GEO Attribute (GNEShape or GNEPosition)
            myGEOAttributeFrame->show();
            myGEOAttributeTextField->setText(myACs.front()->getAttribute(myGEOAttribute).c_str());
            myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
        }
        // set text orf GEO button
        if (myUseGEOCheckButton->getCheck()) {
            myUseGEOCheckButton->setText("true");
        } else {
            myUseGEOCheckButton->setText("false");
        }
    }
}


std::map<SumoXMLAttr, std::string>
GNEFrame::GEOAttributes::getGEOAttributes() const {
    std::map<SumoXMLAttr, std::string> attributes;
    // fill map with the GEO Attributes
    attributes[myGEOAttribute] = myGEOAttributeTextField->getText().text();
    attributes[SUMO_ATTR_GEO] = myUseGEOCheckButton->getCheck() ? "true" : "false";
    return attributes;
}


long
GNEFrame::GEOAttributes::onCmdSetGEOAttribute(FXObject*, FXSelector, void*) {
    if (myGEOAttributeTextField->getText().empty()) {
        WRITE_WARNING("GEO Shapes cannot be empty.");
    } else if (myACs.front()->isValid(myGEOAttribute, myGEOAttributeTextField->getText().text())) {
        myACs.front()->setAttribute(myGEOAttribute, myGEOAttributeTextField->getText().text(), myFrameParent->getViewNet()->getUndoList());
        myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
    } else {
        myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
        myGEOAttributeTextField->killFocus();
    }
    // refresh values of current inspected item (because attribute shape changes)
    myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->refreshValues();
    return 0;
}


long
GNEFrame::GEOAttributes::onCmdUseGEOParameters(FXObject*, FXSelector, void*) {
    // change label of Check button depending of check
    if (myUseGEOCheckButton->getCheck()) {
        myUseGEOCheckButton->setText("true");
    } else {
        myUseGEOCheckButton->setText("false");
    }
    // update GEO Attribute of entire selection
    for (auto i : myACs) {
        i->setAttribute(SUMO_ATTR_GEO, myUseGEOCheckButton->getText().text(), myFrameParent->getViewNet()->getUndoList());
    }
    return 1;
}


long
GNEFrame::GEOAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "GEO attributes Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << " SUMO uses the World Geodetic System 84 (WGS84/UTM).\n"
            << " For a GEO-referenced network, geo coordinates are represented as pairs of Longitude and Latitude\n"
            << " in decimal degrees without extra symbols. (N,W..)\n"
            << " - Longitude: East-west position of a point on the Earth's surface.\n"
            << " - Latitude: North-south position of a point on the Earth's surface.\n"
            << " - CheckBox 'use GEO' enables or disables saving position in GEO coordinates\n";
    new FXLabel(helpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::DrawingMode - methods
// ---------------------------------------------------------------------------

GNEFrame::DrawingMode::DrawingMode(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Drawing", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(this, "Start drawing", 0, this, MID_GNE_POLYGONFRAME_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(this, "Stop drawing", 0, this, MID_GNE_POLYGONFRAME_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(this, "Abort drawing", 0, this, MID_GNE_POLYGONFRAME_ABORTDRAWING, GUIDesignButton);

    // create information label
    std::ostringstream information;
    information
            << "- 'Start drawing' or ENTER\n"
            << "  draws polygon boundary.\n"
            << "- 'Stop drawing' or ENTER\n"
            << "  creates polygon.\n"
            << "- 'Abort drawing' or ESC\n"
            << "  removes drawed polygon.";
    myInformationLabel = new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEFrame::DrawingMode::~DrawingMode() {}


void GNEFrame::DrawingMode::showDrawingMode() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEFrame::DrawingMode::hideDrawingMode() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrame::DrawingMode::startDrawing() {
    // Only start drawing if DrawingMode modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrame::DrawingMode::stopDrawing() {
    // check if shape has to be closed
    if (myFrameParent->getNeteditAttributes()->isCloseShapeEnabled()) {
        myTemporalShapeShape.closePolygon();
    }
    // try to build polygon
    /**
    NOTE: This solution using dynamic_cast is provisional, and has to be changed
          for task #1112.
    **/
    if (dynamic_cast<GNEPolygonFrame*>(myFrameParent)->buildPoly(myTemporalShapeShape)) {
        // clear created points
        myTemporalShapeShape.clear();
        myFrameParent->getViewNet()->update();
        // change buttons
        myStartDrawingButton->enable();
        myStopDrawingButton->disable();
        myAbortDrawingButton->disable();
    } else {
        // abort drawing if polygon cannot be created
        abortDrawing();
    }
}


void
GNEFrame::DrawingMode::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->getViewNet()->update();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrame::DrawingMode::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrame::DrawingMode::removeLastPoint() {
    if (myStopDrawingButton->isEnabled()) {
        if (myTemporalShapeShape.size() > 0) {
            myTemporalShapeShape.pop_back();
        }
    } else {
        throw ProcessError("Last point cannot be removed if drawing wasn't started");
    }
}


const PositionVector&
GNEFrame::DrawingMode::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrame::DrawingMode::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


long
GNEFrame::DrawingMode::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrame::DrawingMode::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrame::DrawingMode::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrame - methods
// ---------------------------------------------------------------------------

GNEFrame::GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel) :
    FXVerticalFrame(horizontalFrameParent, GUIDesignAuxiliarFrame),
    myViewNet(viewNet),
    myNeteditAttributes(NULL),
    myGEOAttributes(NULL),
    myDrawingMode(NULL) {

    // Create font
    myFrameHeaderFont = new FXFont(getApp(), "Arial", 14, FXFont::Bold),

    // Create frame for header
    myHeaderFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create frame for left elements of header (By default unused)
    myHeaderLeftFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderLeftFrame->hide();

    // Create titel frame
    myFrameHeaderLabel = new FXLabel(myHeaderFrame, frameLabel.c_str(), 0, GUIDesignLabelFrameInformation);

    // Create frame for right elements of header (By default unused)
    myHeaderRightFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderRightFrame->hide();

    // Add separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);

    // Create frame for contents
    myScrollWindowsContents = new FXScrollWindow(this, GUIDesignContentsScrollWindow);

    // Create frame for contents
    myContentFrame = new FXVerticalFrame(myScrollWindowsContents, GUIDesignContentsFrame);

    // Set font of header
    myFrameHeaderLabel->setFont(myFrameHeaderFont);

    // Hide Frame
    FXVerticalFrame::hide();
}


GNEFrame::~GNEFrame() {
    delete myFrameHeaderFont;
}


void
GNEFrame::focusUpperElement() {
    myFrameHeaderLabel->setFocus();
}


void
GNEFrame::show() {
    // show scroll window
    FXVerticalFrame::show();
    // Show and update Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNEFrame::hide() {
    // hide scroll window
    FXVerticalFrame::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNEFrame::setFrameWidth(int newWidth) {
    setWidth(newWidth);
    myScrollWindowsContents->setWidth(newWidth);
}


GNEViewNet*
GNEFrame::getViewNet() const {
    return myViewNet;
}


FXLabel*
GNEFrame::getFrameHeaderLabel() const {
    return myFrameHeaderLabel;
}


FXFont*
GNEFrame::getFrameHeaderFont() const {
    return myFrameHeaderFont;
}


GNEFrame::NeteditAttributes*
GNEFrame::getNeteditAttributes() const {
    if (myNeteditAttributes) {
        return myNeteditAttributes;
    } else {
        throw ProcessError("Netedit Attributes editor wasn't created");
    }
}


GNEFrame::GEOAttributes*
GNEFrame::getGEOAttributes() const {
    if (myGEOAttributes) {
        return myGEOAttributes;
    } else {
        throw ProcessError("GEO Attributes editor wasn't created");
    }
}


GNEFrame::DrawingMode*
GNEFrame::getDrawingMode() const {
    if (myDrawingMode) {
        return myDrawingMode;
    } else {
        throw ProcessError("Drawing Mode editor wasn't created");
    }
}

/****************************************************************************/
