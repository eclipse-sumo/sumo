/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPolygonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id$
///
// The Widget for add polygons
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/netelements/GNELane.h>

#include "GNEPolygonFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPolygonFrame::ShapeSelector) ShapeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEPolygonFrame::ShapeSelector::onCmdselectAttributeCarrier),
};

FXDEFMAP(GNEPolygonFrame::ShapeAttributeSingle) ShapeAttributeSingleMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_TEXT,     GNEPolygonFrame::ShapeAttributeSingle::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEPolygonFrame::ShapeAttributeSingle::onCmdSetBooleanAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEPolygonFrame::ShapeAttributeSingle::onCmdSetColorAttribute),
};

FXDEFMAP(GNEPolygonFrame::ShapeAttributes) ShapeAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEPolygonFrame::ShapeAttributes::onCmdHelp),
};

FXDEFMAP(GNEPolygonFrame::NeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEPolygonFrame::NeteditAttributes::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEPolygonFrame::NeteditAttributes::onCmdHelp),
};

FXDEFMAP(GNEPolygonFrame::DrawingMode) DrawingModeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_STARTDRAWING,  GNEPolygonFrame::DrawingMode::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_STOPDRAWING,   GNEPolygonFrame::DrawingMode::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_POLYGONFRAME_ABORTDRAWING,  GNEPolygonFrame::DrawingMode::onCmdAbortDrawing),
};

// Object implementation
FXIMPLEMENT(GNEPolygonFrame::ShapeSelector,         FXGroupBox,         ShapeSelectorMap,           ARRAYNUMBER(ShapeSelectorMap))
FXIMPLEMENT(GNEPolygonFrame::ShapeAttributeSingle,  FXHorizontalFrame,  ShapeAttributeSingleMap,    ARRAYNUMBER(ShapeAttributeSingleMap))
FXIMPLEMENT(GNEPolygonFrame::ShapeAttributes,       FXGroupBox,         ShapeAttributesMap,         ARRAYNUMBER(ShapeAttributesMap))
FXIMPLEMENT(GNEPolygonFrame::NeteditAttributes,     FXGroupBox,         NeteditAttributesMap,       ARRAYNUMBER(NeteditAttributesMap))
FXIMPLEMENT(GNEPolygonFrame::DrawingMode,           FXGroupBox,         DrawingModeMap,             ARRAYNUMBER(DrawingModeMap))


// ---------------------------------------------------------------------------
// GNEPolygonFrame::ShapeSelector - methods
// ---------------------------------------------------------------------------

GNEPolygonFrame::ShapeSelector::ShapeSelector(GNEPolygonFrame* shapeFrameParent) :
    FXGroupBox(shapeFrameParent->myContentFrame, "Shape element", GUIDesignGroupBoxFrame),
    myShapeFrameParent(shapeFrameParent),
    myCurrentShapeType(SUMO_TAG_NOTHING) {

    // Create FXListBox in myGroupBoxForMyShapeMatchBox
    myShapeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);

    // Add options to myShapeMatchBox
    auto listOfTags = GNEAttributeCarrier::allowedShapeTags(false);
    for (auto i : listOfTags) {
        myShapeMatchBox->appendItem(toString(i).c_str());
    }

    // Set visible items
    myShapeMatchBox->setNumVisible((int)myShapeMatchBox->getNumItems());

    // set busstop as default additional
    myShapeMatchBox->setCurrentItem(2);

    // ShapeSelector is always shown
    show();
}


GNEPolygonFrame::ShapeSelector::~ShapeSelector() {}


SumoXMLTag
GNEPolygonFrame::ShapeSelector::getCurrentShapeType() const {
    return myCurrentShapeType;
}


void
GNEPolygonFrame::ShapeSelector::setCurrentShape(SumoXMLTag actualShapeType) {
    // Set new actualShapeType
    myCurrentShapeType = actualShapeType;
    // Check that current shape type is valid
    if (myCurrentShapeType != SUMO_TAG_NOTHING) {
        // Clear internal attributes
        myShapeFrameParent->getShapeAttributes()->clearAttributes();
        // Iterate over attributes of myActualShapeType
        for (auto i : GNEAttributeCarrier::getTagProperties(myCurrentShapeType)) {
            if (!i.second.isUnique()) {
                myShapeFrameParent->getShapeAttributes()->addAttribute(i.first);
            }
        }
        // if there are parmeters, show and Recalc groupBox
        if (myShapeFrameParent->getShapeAttributes()->getNumberOfAddedAttributes() > 0) {
            myShapeFrameParent->getShapeAttributes()->showShapeParameters();
        }
        // show netedit attributes
        if (GNEAttributeCarrier::getTagProperties(myCurrentShapeType).canBlockMovement()) {
            myShapeFrameParent->getNeteditAttributes()->showNeteditAttributes(myCurrentShapeType == SUMO_TAG_POLY);
        } else {
            myShapeFrameParent->getNeteditAttributes()->hideNeteditAttributes();
        }
        // show drawing mode
        if (myCurrentShapeType == SUMO_TAG_POLY) {
            myShapeFrameParent->getDrawingMode()->showDrawingMode();
        } else {
            myShapeFrameParent->getDrawingMode()->hideDrawingMode();
        }
    } else {
        // hide all widgets
        myShapeFrameParent->getShapeAttributes()->hideShapeParameters();
        myShapeFrameParent->getNeteditAttributes()->hideNeteditAttributes();
        myShapeFrameParent->getDrawingMode()->hideDrawingMode();
    }
}


long
GNEPolygonFrame::ShapeSelector::onCmdselectAttributeCarrier(FXObject*, FXSelector, void*) {
    // Check if value of myShapeMatchBox correspond of an allowed shape tags
    auto listOfTags = GNEAttributeCarrier::allowedShapeTags(false);
    for (auto i : listOfTags) {
        if (toString(i) == myShapeMatchBox->getText().text()) {
            myShapeMatchBox->setTextColor(FXRGB(0, 0, 0));
            setCurrentShape(i);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected shape '" + myShapeMatchBox->getText() + "' in ShapeSelector").text());
            return 1;
        }
    }
    // if shape name isn't correct, hidde all
    setCurrentShape(SUMO_TAG_NOTHING);
    myShapeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid shape in ShapeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPolygonFrame::ShapeAttributeSingle - methods
// ---------------------------------------------------------------------------

GNEPolygonFrame::ShapeAttributeSingle::ShapeAttributeSingle(ShapeAttributes* shapeAttributesParent) :
    FXHorizontalFrame(shapeAttributesParent, GUIDesignAuxiliarHorizontalFrame),
    myShapeAttributesParent(shapeAttributesParent),
    myShapeAttr(SUMO_ATTR_NOTHING) {
    // Create visual elements
    myLabel = new FXLabel(this, "name", 0, GUIDesignLabelAttribute);
    myColorEditor = new FXButton(this, "ColorButton", 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldInt);
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldReal);
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextField);
    myBoolCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    // Hide elements
    hideParameter();
}


GNEPolygonFrame::ShapeAttributeSingle::~ShapeAttributeSingle() {}


void
GNEPolygonFrame::ShapeAttributeSingle::showParameter(SumoXMLAttr shapeAttr, std::string value) {
    myShapeAttr = shapeAttr;
    myInvalidValue = "";
    // Retrieve attribute properties
    const auto& attributeProperties = GNEAttributeCarrier::getTagProperties(myShapeAttributesParent->getPolygonFrameParent()->myShapeSelector->getCurrentShapeType()).getAttribute(shapeAttr);
    // show label or button for edit colors
    if (attributeProperties.isColor()) {
        myColorEditor->setTextColor(FXRGB(0, 0, 0));
        myColorEditor->setText(toString(shapeAttr).c_str());
        myColorEditor->show();
    } else {
        myLabel->setText(toString(myShapeAttr).c_str());
        myLabel->show();
    }
    if (attributeProperties.isInt()) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->setText(value.c_str());
        myTextFieldInt->show();
    } else if (attributeProperties.isFloat()) {
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->setText(value.c_str());
        myTextFieldReal->show();
    } else if (attributeProperties.isBool()) {
        if (GNEAttributeCarrier::parse<bool>(value)) {
            myBoolCheckButton->setCheck(true);
            myBoolCheckButton->setText("true");
        } else {
            myBoolCheckButton->setCheck(false);
            myBoolCheckButton->setText("false");
        }
        myBoolCheckButton->show();
    } else {
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->setText(value.c_str());
        myTextFieldStrings->show();
    }
    show();
}


void
GNEPolygonFrame::ShapeAttributeSingle::hideParameter() {
    myShapeAttr = SUMO_ATTR_NOTHING;
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myBoolCheckButton->hide();
    myColorEditor->hide();
    hide();
}


SumoXMLAttr
GNEPolygonFrame::ShapeAttributeSingle::getAttr() const {
    return myShapeAttr;
}


std::string
GNEPolygonFrame::ShapeAttributeSingle::getValue() const {
    // obtain attribute property (only for improve code legibility)
    const auto& attrValue = GNEAttributeCarrier::getTagProperties(myShapeAttributesParent->getPolygonFrameParent()->getShapeSelector()->getCurrentShapeType()).getAttribute(myShapeAttr);
    if (attrValue.isBool()) {
        return (myBoolCheckButton->getCheck() == 1) ? "true" : "false";
    } else if (attrValue.isInt()) {
        return myTextFieldInt->getText().text();
    } else if (attrValue.isFloat() || attrValue.isTime()) {
        return myTextFieldReal->getText().text();
    } else {
        return myTextFieldStrings->getText().text();
    }
}


const std::string&
GNEPolygonFrame::ShapeAttributeSingle::isAttributeValid() const {
    return myInvalidValue;
}


GNEPolygonFrame::ShapeAttributes*
GNEPolygonFrame::ShapeAttributeSingle::getShapeAttributesParent() const {
    return myShapeAttributesParent;
}


long
GNEPolygonFrame::ShapeAttributeSingle::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // get attribute Values (only for improve efficiency)
    const auto& attrValues = GNEAttributeCarrier::getTagProperties(myShapeAttributesParent->getPolygonFrameParent()->getShapeSelector()->getCurrentShapeType()).getAttribute(myShapeAttr);
    // Check if format of current value of myTextField is correct
    if (attrValues.isInt()) {
        if (GNEAttributeCarrier::canParse<int>(myTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myTextFieldInt->getText().text());
            // Check if int value must be positive
            if (attrValues.isPositive() && (intValue < 0)) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'int' format";
        }
    } else if (attrValues.isTime()) {
        // time attributes work as positive doubles
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if parsed value is negative
            if (doubleValue < 0) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'time' format";
        }
    } else if (attrValues.isFloat()) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if double value must be positive
            if (attrValues.isPositive() && (doubleValue < 0)) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' cannot be negative";
                // check if double value is a probability
            } else if (attrValues.isProbability() && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' takes only values between 0 and 1";
            }
        } else {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'float' format";
        }
    } else if (attrValues.isColor()) {
        // check if filename format is valid
        if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'RBGColor' format";
        }
    } else if (attrValues.isFilename()) {
        std::string file = myTextFieldStrings->getText().text();
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidFilename(file) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        } else if (myShapeAttr == SUMO_ATTR_IMGFILE) {
            if (!file.empty()) {
                // only load value if file isn't empty
                if (GUITexturesHelper::getTextureID(file) == -1) {
                    myInvalidValue = "doesn't exist image '" + file + "'";
                }
            }
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myInvalidValue.size() == 0) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->killFocus();
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->killFocus();
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEPolygonFrame::ShapeAttributeSingle::onCmdSetBooleanAttribute(FXObject*, FXSelector, void*) {
    if (myBoolCheckButton->getCheck()) {
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setText("false");
    }
    return 0;
}


long GNEPolygonFrame::ShapeAttributeSingle::onCmdSetColorAttribute(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldStrings->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldStrings->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(GNEAttributeCarrier::getTagProperties(myShapeAttributesParent->getPolygonFrameParent()->getShapeSelector()->getCurrentShapeType()).getDefaultValue(myShapeAttr))));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        myTextFieldStrings->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
        onCmdSetAttribute(0, 0, 0);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNEPolygonFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEPolygonFrame::ShapeAttributes::ShapeAttributes(GNEPolygonFrame* polygonFrameParent) :
    FXGroupBox(polygonFrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myPolygonFrameParent(polygonFrameParent) {

    // Create single parameters
    for (int i = 0; i < GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myVectorOfsingleShapeParameter.push_back(new ShapeAttributeSingle(this));
    }

    // Create help button
    new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEPolygonFrame::ShapeAttributes::~ShapeAttributes() {
}


void
GNEPolygonFrame::ShapeAttributes::clearAttributes() {
    // Hide all fields
    for (int i = 0; i < (int)myVectorOfsingleShapeParameter.size(); i++) {
        myVectorOfsingleShapeParameter.at(i)->hideParameter();
    }
}


void
GNEPolygonFrame::ShapeAttributes::addAttribute(SumoXMLAttr ShapeAttributeSingle) {
    // get current tag Properties
    const auto& attrProperties = GNEAttributeCarrier::getTagProperties(myPolygonFrameParent->getShapeSelector()->getCurrentShapeType()).getAttribute(ShapeAttributeSingle);
    myVectorOfsingleShapeParameter.at(attrProperties.getPositionListed())->showParameter(ShapeAttributeSingle, attrProperties.getDefaultValue());
}


void
GNEPolygonFrame::ShapeAttributes::showShapeParameters() {
    recalc();
    show();
}


void
GNEPolygonFrame::ShapeAttributes::hideShapeParameters() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEPolygonFrame::ShapeAttributes::getAttributesAndValues() const {
    std::map<SumoXMLAttr, std::string> values;
    // get standard parameters
    for (int i = 0; i < (int)myVectorOfsingleShapeParameter.size(); i++) {
        if (myVectorOfsingleShapeParameter.at(i)->getAttr() != SUMO_ATTR_NOTHING) {
            values[myVectorOfsingleShapeParameter.at(i)->getAttr()] = myVectorOfsingleShapeParameter.at(i)->getValue();
        }
    }
    return values;
}


void
GNEPolygonFrame::ShapeAttributes::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standar parameters
    for (auto i : GNEAttributeCarrier::getTagProperties(myPolygonFrameParent->myShapeSelector->getCurrentShapeType())) {
        if (errorMessage.empty()) {
            // Return string with the error if at least one of the parameter isn't valid
            std::string attributeValue = myVectorOfsingleShapeParameter.at(i.second.getPositionListed())->isAttributeValid();
            if (attributeValue.size() != 0) {
                errorMessage = attributeValue;
            }
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + toString(myPolygonFrameParent->getShapeSelector()->getCurrentShapeType()) + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + toString(myPolygonFrameParent->getShapeSelector()->getCurrentShapeType()) + ": " + extra;
    }

    // set message in status bar
    myPolygonFrameParent->getViewNet()->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEPolygonFrame::ShapeAttributes::areValuesValid() const {
    // iterate over standar parameters
    for (auto i : GNEAttributeCarrier::getTagProperties(myPolygonFrameParent->myShapeSelector->getCurrentShapeType())) {
        // Return false if error message of attriuve isn't empty
        if (myVectorOfsingleShapeParameter.at(i.second.getPositionListed())->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


int
GNEPolygonFrame::ShapeAttributes::getNumberOfAddedAttributes() const {
    return (1);
}


GNEPolygonFrame*
GNEPolygonFrame::ShapeAttributes::getPolygonFrameParent() const {
    return myPolygonFrameParent;
}


long
GNEPolygonFrame::ShapeAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myPolygonFrameParent->openHelpAttributesDialog(myPolygonFrameParent->getShapeSelector()->getCurrentShapeType());
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPolygonFrame::NeteditAttributes - methods
// ---------------------------------------------------------------------------

GNEPolygonFrame::NeteditAttributes::NeteditAttributes(GNEPolygonFrame* polygonFrameParent) :
    FXGroupBox(polygonFrameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame) {
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockMovementLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBlockMovementCheckButton->setCheck(false);
    // Create Frame for block shape label and checkBox (By default disabled)
    myBlockShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockShapeLabel = new FXLabel(myBlockShapeFrame, "block shape", 0, GUIDesignLabelAttribute);
    myBlockShapeCheckButton = new FXCheckButton(myBlockShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    // Create Frame for block close polygon and checkBox (By default disabled)
    myClosePolygonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myClosePolygonLabel = new FXLabel(myClosePolygonFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myClosePolygonCheckButton = new FXCheckButton(myClosePolygonFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBlockShapeCheckButton->setCheck(false);
    // Create help button
    new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEPolygonFrame::NeteditAttributes::~NeteditAttributes() {}


void
GNEPolygonFrame::NeteditAttributes::showNeteditAttributes(bool shapeEditing) {
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
GNEPolygonFrame::NeteditAttributes::hideNeteditAttributes() {
    FXGroupBox::hide();
}


bool
GNEPolygonFrame::NeteditAttributes::isBlockMovementEnabled() const {
    return myBlockMovementCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEPolygonFrame::NeteditAttributes::isBlockShapeEnabled() const {
    return myBlockShapeCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEPolygonFrame::NeteditAttributes::isCloseShapeEnabled() const {
    return myClosePolygonCheckButton->getCheck() == 1 ? true : false;
}


long
GNEPolygonFrame::NeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myBlockMovementCheckButton) {
        if (myBlockMovementCheckButton->getCheck()) {
            myBlockMovementCheckButton->setText("true");
        } else {
            myBlockMovementCheckButton->setText("false");
        }
    } else if (obj == myBlockShapeCheckButton) {
        if (myBlockShapeCheckButton->getCheck()) {
            myBlockShapeCheckButton->setText("true");
        } else {
            myBlockShapeCheckButton->setText("false");
        }
    } else if (obj == myClosePolygonCheckButton) {
        if (myClosePolygonCheckButton->getCheck()) {
            myClosePolygonCheckButton->setText("true");
        } else {
            myClosePolygonCheckButton->setText("false");
        }
    }
    return 1;
}


long
GNEPolygonFrame::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* polygonNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    polygonNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEPOLYGON));
    // Set help text
    std::ostringstream help;
    help
            << "- Block movement: If enabled, the created polygon element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Block shape: If enabled, the shape of created polygon element will be blocked. i.e. their geometry points\n"
            << "  cannot be edited be moved with the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Close shape: If enabled, the created polygon element will be closed. i.e. the last created geometry point\n"
            << "  will be connected with the first geometry point automatically. This option can be modified inspecting element.";
    // Create label with the help text
    new FXLabel(polygonNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(polygonNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(polygonNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), polygonNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening NeteditAttributes dialog for tag '"/** Finish + toString(currentTag) **/);
    // create Dialog
    polygonNeteditAttributesHelpDialog->create();
    // show in the given position
    polygonNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(polygonNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing NeteditAttributes dialog for tag '"/** Finish + toString(currentTag) **/);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPolygonFrame::DrawingMode - methods
// ---------------------------------------------------------------------------

GNEPolygonFrame::DrawingMode::DrawingMode(GNEPolygonFrame* polygonFrameParent) :
    FXGroupBox(polygonFrameParent->myContentFrame, "Drawing", GUIDesignGroupBoxFrame),
    myPolygonFrameParent(polygonFrameParent),
    myDeleteLastCreatedPoint(false) {
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
            << "- 'Shift + Click'\n"
            << "  removes last created point.\n"
            << "- 'Abort drawing' or ESC\n"
            << "  removes drawed polygon.";
    myInformationLabel = new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEPolygonFrame::DrawingMode::~DrawingMode() {}


void GNEPolygonFrame::DrawingMode::showDrawingMode() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEPolygonFrame::DrawingMode::hideDrawingMode() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEPolygonFrame::DrawingMode::startDrawing() {
    // Only start drawing if DrawingMode modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEPolygonFrame::DrawingMode::stopDrawing() {
    // check if shape has to be closed
    if (myPolygonFrameParent->getNeteditAttributes()->isCloseShapeEnabled()) {
        myTemporalShapeShape.closePolygon();
    }
    // try to build polygon
    if (myPolygonFrameParent->buildPoly(myTemporalShapeShape)) {
        // clear created points
        myTemporalShapeShape.clear();
        myPolygonFrameParent->getViewNet()->update();
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
GNEPolygonFrame::DrawingMode::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myPolygonFrameParent->getViewNet()->update();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEPolygonFrame::DrawingMode::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEPolygonFrame::DrawingMode::removeLastPoint() {

}


const PositionVector&
GNEPolygonFrame::DrawingMode::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEPolygonFrame::DrawingMode::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEPolygonFrame::DrawingMode::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEPolygonFrame::DrawingMode::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEPolygonFrame::DrawingMode::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEPolygonFrame::DrawingMode::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEPolygonFrame::DrawingMode::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ===========================================================================
// method definitions
// ===========================================================================

GNEPolygonFrame::GNEPolygonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Shapes") {

    // Create shape selector
    myShapeSelector = new ShapeSelector(this);

    // Create shape parameters
    myShapeAttributes = new GNEPolygonFrame::ShapeAttributes(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEPolygonFrame::NeteditAttributes(this);

    // Create drawing controls
    myDrawingMode = new GNEPolygonFrame::DrawingMode(this);

    // set polygon as default additional
    myShapeSelector->setCurrentShape(SUMO_TAG_POLY);
}


GNEPolygonFrame::~GNEPolygonFrame() {
}


GNEPolygonFrame::AddShapeResult
GNEPolygonFrame::processClick(const Position& clickedPosition, GNELane* lane) {
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement;
    // check if current selected shape is valid
    if (myShapeSelector->getCurrentShapeType() == SUMO_TAG_POI) {
        // obtain POI values
        valuesOfElement = myShapeAttributes->getAttributesAndValues();
        // show warning dialogbox and stop if input parameters are invalid
        if (myShapeAttributes->areValuesValid() == false) {
            myShapeAttributes->showWarningMessage();
            return ADDSHAPE_INVALID;
        }
        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(myShapeSelector->getCurrentShapeType());
        // obtain position
        valuesOfElement[SUMO_ATTR_POSITION] = toString(clickedPosition);
        // obtain block movement value
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditAttributes->isBlockMovementEnabled());
        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        if (addPOI(valuesOfElement)) {
            return ADDSHAPE_SUCCESS;
        } else {
            return ADDSHAPE_INVALID;
        }
    } else  if (myShapeSelector->getCurrentShapeType() == SUMO_TAG_POILANE) {
        // obtain POILane values
        valuesOfElement = myShapeAttributes->getAttributesAndValues();
        // show warning dialogbox and stop if input parameters are invalid
        if (myShapeAttributes->areValuesValid() == false) {
            myShapeAttributes->showWarningMessage();
            return ADDSHAPE_INVALID;
        }
        // abort if lane is nullptr
        if (lane == nullptr) {
            WRITE_WARNING(toString(SUMO_TAG_POILANE) + " can be only placed over lanes");
            return ADDSHAPE_INVALID;
        }
        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(myShapeSelector->getCurrentShapeType());
        // obtain Lane
        valuesOfElement[SUMO_ATTR_LANE] = lane->getID();
        // obtain position over lane
        valuesOfElement[SUMO_ATTR_POSITION] = toString(lane->getShape().nearest_offset_to_point2D(clickedPosition));
        // obtain block movement value
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditAttributes->isBlockMovementEnabled());
        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        if (addPOILane(valuesOfElement)) {
            return ADDSHAPE_SUCCESS;
        } else {
            return ADDSHAPE_INVALID;
        }
    } else if (myShapeSelector->getCurrentShapeType() == SUMO_TAG_POLY) {
        // obtain Shape values
        valuesOfElement = myShapeAttributes->getAttributesAndValues();
        if (myDrawingMode->isDrawing()) {
            // add or delete a new point depending of flag "delete last created point"
            if (myDrawingMode->getDeleteLastCreatedPoint()) {
                myDrawingMode->removeLastPoint();
            } else {
                myDrawingMode->addNewPoint(clickedPosition);
            }
            return ADDSHAPE_UPDATEDTEMPORALSHAPE;
        } else {
            // return ADDSHAPE_NOTHING if is drawing isn't enabled
            return ADDSHAPE_NOTHING;
        }
    } else {
        myViewNet->setStatusBarText("Current selected shape isn't valid.");
        return ADDSHAPE_INVALID;
    }
}


bool
GNEPolygonFrame::buildPoly(const PositionVector& drawedShape) {
    // show warning dialogbox and stop check if input parameters are valid
    if (myShapeAttributes->areValuesValid() == false) {
        myShapeAttributes->showWarningMessage();
        return false;
    } else {
        // Declare map to keep values
        std::map<SumoXMLAttr, std::string> valuesOfElement = myShapeAttributes->getAttributesAndValues();

        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(SUMO_TAG_POLY);

        // obtain shape
        valuesOfElement[SUMO_ATTR_SHAPE] = toString(drawedShape);

        // obtain geo (by default false)
        valuesOfElement[SUMO_ATTR_GEO] = "false";

        // obtain block movement value
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditAttributes->isBlockMovementEnabled());

        // obtain block shape value
        valuesOfElement[GNE_ATTR_BLOCK_SHAPE] = toString(myNeteditAttributes->isBlockShapeEnabled());

        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        return addPolygon(valuesOfElement);
    }
}


void
GNEPolygonFrame::show() {
    // Show frame
    GNEFrame::show();
}


std::string
GNEPolygonFrame::getIdsSelected(const FXList* list) {
    // Obtain Id's of list
    std::string vectorOfIds;
    for (int i = 0; i < list->getNumItems(); i++) {
        if (list->isItemSelected(i)) {
            if (vectorOfIds.size() > 0) {
                vectorOfIds += " ";
            }
            vectorOfIds += (list->getItem(i)->getText()).text();
        }
    }
    return vectorOfIds;
}


GNEPolygonFrame::ShapeSelector*
GNEPolygonFrame::getShapeSelector() const {
    return myShapeSelector;
}


GNEPolygonFrame::ShapeAttributes*
GNEPolygonFrame::getShapeAttributes() const {
    return myShapeAttributes;
}


GNEPolygonFrame::NeteditAttributes*
GNEPolygonFrame::getNeteditAttributes() const {
    return myNeteditAttributes;
}


GNEPolygonFrame::DrawingMode*
GNEPolygonFrame::getDrawingMode() const {
    return myDrawingMode;
}


bool
GNEPolygonFrame::addPolygon(const std::map<SumoXMLAttr, std::string>& polyValues) {
    bool ok = true;
    // parse attributes from polyValues
    std::string id = polyValues.at(SUMO_ATTR_ID);
    std::string type = polyValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(polyValues.at(SUMO_ATTR_COLOR));
    std::string layerStr = polyValues.at(SUMO_ATTR_LAYER);
    double angle = GNEAttributeCarrier::parse<double>(polyValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = polyValues.at(SUMO_ATTR_IMGFILE);
    bool relativePath = GNEAttributeCarrier::parse<bool>(polyValues.at(SUMO_ATTR_RELATIVEPATH));
    PositionVector shape = GeomConvHelper::parseShapeReporting(polyValues.at(SUMO_ATTR_SHAPE), "user-supplied position", 0, ok, true);
    bool fill = GNEAttributeCarrier::parse<bool>(polyValues.at(SUMO_ATTR_FILL));
    // parse layer
    double layer = GNEAttributeCarrier::canParse<double>(layerStr) ? GNEAttributeCarrier::parse<double>(layerStr) : Shape::DEFAULT_LAYER;
    // create new Polygon only if number of shape points is greather than 2
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POLY));
    if ((shape.size() > 0) && myViewNet->getNet()->addPolygon(id, type, color, layer, angle, imgFile, relativePath, shape, false, fill)) {
        // set manually attributes use GEO, block movement and block shape
        GNEPoly* polygon = myViewNet->getNet()->retrievePolygon(id);
        polygon->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, polyValues.at(GNE_ATTR_BLOCK_MOVEMENT), myViewNet->getUndoList());
        polygon->setAttribute(GNE_ATTR_BLOCK_SHAPE, polyValues.at(GNE_ATTR_BLOCK_SHAPE), myViewNet->getUndoList());
        myViewNet->getUndoList()->p_end();
        return true;
    } else {
        // abort creation
        myViewNet->getUndoList()->p_abort();
        return false;
    }
}


bool
GNEPolygonFrame::addPOI(const std::map<SumoXMLAttr, std::string>& POIValues) {
    bool ok = true;
    // parse attributes from POIValues
    std::string id = POIValues.at(SUMO_ATTR_ID);
    std::string type = POIValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(POIValues.at(SUMO_ATTR_COLOR));
    std::string layerStr = POIValues.at(SUMO_ATTR_LAYER);
    Position pos = GeomConvHelper::parseShapeReporting(POIValues.at(SUMO_ATTR_POSITION), "netedit-given", 0, ok, false)[0];
    double angle = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = POIValues.at(SUMO_ATTR_IMGFILE);
    bool relativePath = GNEAttributeCarrier::parse<bool>(POIValues.at(SUMO_ATTR_RELATIVEPATH));
    double widthPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_WIDTH));
    double heightPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_HEIGHT));
    // parse layer
    double layer = GNEAttributeCarrier::canParse<double>(layerStr) ? GNEAttributeCarrier::parse<double>(layerStr) : Shape::DEFAULT_LAYER_POI;
    // create new POI
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POI));
    if (myViewNet->getNet()->addPOI(id, type, color, pos, false, "", 0, 0, layer, angle, imgFile, relativePath, widthPOI, heightPOI)) {
        // Set manually the attribute block movement
        GNEPOI* poi = myViewNet->getNet()->retrievePOI(id);
        poi->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, POIValues.at(GNE_ATTR_BLOCK_MOVEMENT), myViewNet->getUndoList());
        myViewNet->getUndoList()->p_end();
        return true;
    } else {
        // abort creation
        myViewNet->getUndoList()->p_abort();
        return false;
    }
}


bool
GNEPolygonFrame::addPOILane(const std::map<SumoXMLAttr, std::string>& POIValues) {
    // parse attributes from POIValues
    std::string id = POIValues.at(SUMO_ATTR_ID);
    std::string type = POIValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(POIValues.at(SUMO_ATTR_COLOR));
    std::string layerStr = POIValues.at(SUMO_ATTR_LAYER);
    double angle = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = POIValues.at(SUMO_ATTR_IMGFILE);
    bool relativePath = GNEAttributeCarrier::parse<bool>(POIValues.at(SUMO_ATTR_RELATIVEPATH));
    GNELane* lane = myViewNet->getNet()->retrieveLane(POIValues.at(SUMO_ATTR_LANE));
    double posLane = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_POSITION));
    double posLat = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_POSITION_LAT));
    double widthPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_WIDTH));
    double heightPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_HEIGHT));
    // parse layer
    double layer = GNEAttributeCarrier::canParse<double>(layerStr) ? GNEAttributeCarrier::parse<double>(layerStr) : Shape::DEFAULT_LAYER_POI;
    // create new POILane
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POILANE));
    if (myViewNet->getNet()->addPOI(id, type, color, Position(), false, lane->getID(), posLane, posLat, layer, angle, imgFile, relativePath, widthPOI, heightPOI)) {
        // Set manually the attribute block movement
        GNEPOI* POI = myViewNet->getNet()->retrievePOI(id);
        POI->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, POIValues.at(GNE_ATTR_BLOCK_MOVEMENT), myViewNet->getUndoList());
        myViewNet->getUndoList()->p_end();
        return true;
    } else {
        // abort creation
        myViewNet->getUndoList()->p_abort();
        return false;
    }
}

/****************************************************************************/
