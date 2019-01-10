/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// The Widget for add additional elements
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
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/additionals/GNECalibrator.h>
#include <netedit/additionals/GNEClosingLaneReroute.h>
#include <netedit/additionals/GNEClosingReroute.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/additionals/GNEDestProbReroute.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNENet.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNERerouterInterval.h>
#include <netedit/GNEUndoList.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNEVariableSpeedSign.h>
#include <netedit/additionals/GNEVariableSpeedSignStep.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>

#include "GNEAdditionalFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame::AdditionalSelector) SelectorAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEAdditionalFrame::AdditionalSelector::onCmdselectAttributeCarrier),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributeSingle) AdditionalAttributeSingleMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_TEXT,     GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetBooleanAttribute),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributes) AdditionalAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEAdditionalFrame::AdditionalAttributes::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::NeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_TEXT,     GNEAdditionalFrame::NeteditAttributes::onCmdSetLength),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEAdditionalFrame::NeteditAttributes::onCmdSetBlocking),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,               GNEAdditionalFrame::NeteditAttributes::onCmdSelectReferencePoint),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                       GNEAdditionalFrame::NeteditAttributes::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::SelectorParentEdges) SelectorParentEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorParentEdges::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorParentEdges::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorParentEdges::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorParentEdges::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorParentEdges::onCmdSelectEdge),
};

FXDEFMAP(GNEAdditionalFrame::SelectorParentLanes) SelectorParentLanesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorParentLanes::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorParentLanes::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorParentLanes::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorParentLanes::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorParentLanes::onCmdSelectLane),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame::AdditionalSelector,             FXGroupBox,         SelectorAdditionalMap,          ARRAYNUMBER(SelectorAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributeSingle,      FXHorizontalFrame,  AdditionalAttributeSingleMap,   ARRAYNUMBER(AdditionalAttributeSingleMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributes,           FXGroupBox,         AdditionalAttributesMap,        ARRAYNUMBER(AdditionalAttributesMap))
FXIMPLEMENT(GNEAdditionalFrame::NeteditAttributes,              FXGroupBox,         NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentEdges,            FXGroupBox,         SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentLanes,            FXGroupBox,         SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalSelector - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalSelector::AdditionalSelector(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Additional element", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myCurrentAdditionalType(SUMO_TAG_NOTHING) {

    // Create FXListBox in myGroupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);

    // Add options to myAdditionalMatchBox
    auto listOfTags = GNEAttributeCarrier::allowedAdditionalTags(true);
    for (auto i : listOfTags) {
        myAdditionalMatchBox->appendItem(toString(i).c_str());
    }

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems());

    // set busstop as default additional
    myAdditionalMatchBox->setCurrentItem(10);

    // AdditionalSelector is always shown
    show();
}


GNEAdditionalFrame::AdditionalSelector::~AdditionalSelector() {}


SumoXMLTag
GNEAdditionalFrame::AdditionalSelector::getCurrentAdditionalType() const {
    return myCurrentAdditionalType;
}


void
GNEAdditionalFrame::AdditionalSelector::setCurrentAdditional(SumoXMLTag actualAdditionalType) {
    // Set new actualAdditionalType
    myCurrentAdditionalType = actualAdditionalType;
    // Check that current additional type is valid
    if (myCurrentAdditionalType != SUMO_TAG_NOTHING) {
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentAdditionalType);
        // first check if additional can block movement, then show neteditParameters
        if (tagValue.canBlockMovement()) {
            myAdditionalFrameParent->getNeteditAttributes()->showNeteditAttributes(false);
        } else {
            myAdditionalFrameParent->getNeteditAttributes()->hideNeteditAttributes();
        }
        // Clear internal attributes
        myAdditionalFrameParent->getAdditionalParameters()->clearAttributes();
        // iterate over attributes of myCurrentAdditionalType
        for (auto i : tagValue) {
            // only show attributes that aren't uniques
            if (!i.second.isUnique()) {
                myAdditionalFrameParent->getAdditionalParameters()->addAttribute(i.first);
            } else if (i.first == SUMO_ATTR_ENDPOS) {
                myAdditionalFrameParent->getNeteditAttributes()->showNeteditAttributes(true);
            }
        }
        myAdditionalFrameParent->getAdditionalParameters()->showAdditionalParameters();
        // Show myFirstAdditionalParentSelector if we're adding a additional with parent
        if (tagValue.hasParent()) {
            myAdditionalFrameParent->getAdditionalParentSelector()->showListOfAdditionalParents(tagValue.getParentTag());
        } else {
            myAdditionalFrameParent->getAdditionalParentSelector()->hideListOfAdditionalParents();
        }
        // Show SelectorParentEdges if we're adding an additional that own the attribute SUMO_ATTR_EDGES
        if (tagValue.hasAttribute(SUMO_ATTR_EDGES)) {
            myAdditionalFrameParent->getEdgeParentsSelector()->showList();
        } else {
            myAdditionalFrameParent->getEdgeParentsSelector()->hideList();
        }
        // Show SelectorParentLanes if we're adding an additional that own the attribute SUMO_ATTR_LANES
        if (tagValue.hasAttribute(SUMO_ATTR_LANES)) {
            myAdditionalFrameParent->getLaneParentsSelector()->showList();
        } else {
            myAdditionalFrameParent->getLaneParentsSelector()->hideList();
        }
    } else {
        // hide all groupbox if additional isn't valid
        myAdditionalFrameParent->getAdditionalParameters()->hideAdditionalParameters();
        myAdditionalFrameParent->getNeteditAttributes()->hideNeteditAttributes();
        myAdditionalFrameParent->getAdditionalParentSelector()->hideListOfAdditionalParents();
        myAdditionalFrameParent->getEdgeParentsSelector()->hideList();
        myAdditionalFrameParent->getLaneParentsSelector()->hideList();
    }
}


long
GNEAdditionalFrame::AdditionalSelector::onCmdselectAttributeCarrier(FXObject*, FXSelector, void*) {
    // Check if value of myAdditionalMatchBox correspond of an allowed additional tags
    auto listOfTags = GNEAttributeCarrier::allowedAdditionalTags(false);
    for (auto i : listOfTags) {
        if (toString(i) == myAdditionalMatchBox->getText().text()) {
            myAdditionalMatchBox->setTextColor(FXRGB(0, 0, 0));
            setCurrentAdditional(i);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected additional '" + myAdditionalMatchBox->getText() + "' in AdditionalSelector").text());
            return 1;
        }
    }
    // if additional name isn't correct, hidde all
    setCurrentAdditional(SUMO_TAG_NOTHING);
    myAdditionalMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid additional in AdditionalSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributeSingle - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributeSingle::AdditionalAttributeSingle(AdditionalAttributes* additionalAttributesParent) :
    FXHorizontalFrame(additionalAttributesParent, GUIDesignAuxiliarHorizontalFrame),
    myAdditionalAttributesParent(additionalAttributesParent),
    myAdditionalAttr(SUMO_ATTR_NOTHING) {
    // Create visual elements
    myLabel = new FXLabel(this, "name", 0, GUIDesignLabelAttribute);
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldInt);
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldReal);
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextField);
    myBoolCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    // Hide elements
    hideParameter();
}


GNEAdditionalFrame::AdditionalAttributeSingle::~AdditionalAttributeSingle() {}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLAttr additionalAttr, std::string value) {
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    // Retrieve attribute properties
    const auto& attributeProperties = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType()).getAttribute(additionalAttr);
    if (attributeProperties.isInt()) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->setText(toString(value).c_str());
        myTextFieldInt->show();
    } else if (attributeProperties.isFloat()) {
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->setText(toString(value).c_str());
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
GNEAdditionalFrame::AdditionalAttributeSingle::hideParameter() {
    myAdditionalAttr = SUMO_ATTR_NOTHING;
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myBoolCheckButton->hide();
    hide();
}


SumoXMLAttr
GNEAdditionalFrame::AdditionalAttributeSingle::getAttr() const {
    return myAdditionalAttr;
}


std::string
GNEAdditionalFrame::AdditionalAttributeSingle::getValue() const {
    // obtain attribute property (only for improve code legibility)
    const auto& attrValue = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->getAdditionalFrameParent()->getAdditionalSelector()->getCurrentAdditionalType()).getAttribute(myAdditionalAttr);
    // return value depending of attribute type
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
GNEAdditionalFrame::AdditionalAttributeSingle::isAttributeValid() const {
    return myInvalidValue;
}


long
GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // get attribute Values (only for improve efficiency)
    const auto& attrValues = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->getAdditionalFrameParent()->getAdditionalSelector()->getCurrentAdditionalType()).getAttribute(myAdditionalAttr);
    // Check if format of current value of myTextField is correct
    if (attrValues.isInt()) {
        if (GNEAttributeCarrier::canParse<int>(myTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myTextFieldInt->getText().text());
            // Check if int value must be positive
            if (attrValues.isPositive() && (intValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'int' format";
        }
    } else if (attrValues.isTime()) {
        // time attributes work as positive doubles
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if parsed value is negative
            if (doubleValue < 0) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'time' format";
        }
    } else if (attrValues.isFloat()) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if double value must be positive
            if (attrValues.isPositive() && (doubleValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
                // check if double value is a probability
            } else if (attrValues.isProbability() && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' takes only values between 0 and 1";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'float' format";
        }
    } else if (attrValues.isFilename()) {
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidFilename(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        }
    } else if (attrValues.isVClass() && attrValues.isList()) {
        // check if VClasses are valid
        if (canParseVehicleClasses(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "list of VClass isn't valid";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_ROUTEPROBE) {
        // check if filename format is valid
        if (!SUMOXMLDefinitions::isValidNetID(myTextFieldStrings->getText().text())) {
            myInvalidValue = "RouteProbe ID contains invalid characters";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_NAME) {
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidAttribute(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "Name contains invalid characters";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_VTYPES) {
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidListOfTypeID(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "Ids contains invalid characters for vehicle type ids";
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
GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetBooleanAttribute(FXObject*, FXSelector, void*) {
    if (myBoolCheckButton->getCheck()) {
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setText("false");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributes::AdditionalAttributes(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create single parameters
    for (int i = 0; i < GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myVectorOfsingleAdditionalParameter.push_back(new AdditionalAttributeSingle(this));
    }
    // Create help button
    new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEAdditionalFrame::AdditionalAttributes::~AdditionalAttributes() {}


void
GNEAdditionalFrame::AdditionalAttributes::clearAttributes() {
    // Hide all fields
    for (int i = 0; i < (int)myVectorOfsingleAdditionalParameter.size(); i++) {
        myVectorOfsingleAdditionalParameter.at(i)->hideParameter();
    }
}


void
GNEAdditionalFrame::AdditionalAttributes::addAttribute(SumoXMLAttr AdditionalAttributeSingle) {
    // obtain attribute property (only for improve code legibility)
    const auto& attrvalue = GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->getAdditionalSelector()->getCurrentAdditionalType()).getAttribute(AdditionalAttributeSingle);
    myVectorOfsingleAdditionalParameter.at(attrvalue.getPositionListed())->showParameter(AdditionalAttributeSingle, attrvalue.getDefaultValue());
}


void
GNEAdditionalFrame::AdditionalAttributes::showAdditionalParameters() {
    recalc();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributes::hideAdditionalParameters() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEAdditionalFrame::AdditionalAttributes::getAttributesAndValues() const {
    std::map<SumoXMLAttr, std::string> values;
    // get standard parameters
    for (int i = 0; i < (int)myVectorOfsingleAdditionalParameter.size(); i++) {
        if (myVectorOfsingleAdditionalParameter.at(i)->getAttr() != SUMO_ATTR_NOTHING) {
            values[myVectorOfsingleAdditionalParameter.at(i)->getAttr()] = myVectorOfsingleAdditionalParameter.at(i)->getValue();
        }
    }
    return values;
}


void
GNEAdditionalFrame::AdditionalAttributes::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standard parameters
    for (auto i : GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType())) {
        if (errorMessage.empty()) {
            // Return string with the error if at least one of the parameter isn't valid
            std::string attributeValue = myVectorOfsingleAdditionalParameter.at(i.second.getPositionListed())->isAttributeValid();
            if (attributeValue.size() != 0) {
                errorMessage = attributeValue;
            }
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + toString(myAdditionalFrameParent->getAdditionalSelector()->getCurrentAdditionalType()) + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + toString(myAdditionalFrameParent->getAdditionalSelector()->getCurrentAdditionalType()) + ": " + extra;
    }

    // set message in status bar
    myAdditionalFrameParent->getViewNet()->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEAdditionalFrame::AdditionalAttributes::areValuesValid() const {
    // iterate over standar parameters
    for (auto i : GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType())) {
        // Return false if error message of attriuve isn't empty
        if (myVectorOfsingleAdditionalParameter.at(i.second.getPositionListed())->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


GNEAdditionalFrame*
GNEAdditionalFrame::AdditionalAttributes::getAdditionalFrameParent() const {
    return myAdditionalFrameParent;
}


long
GNEAdditionalFrame::AdditionalAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myAdditionalFrameParent->openHelpAttributesDialog(myAdditionalFrameParent->getAdditionalSelector()->getCurrentAdditionalType());
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::NeteditAttributes::NeteditAttributes(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT),
    myCurrentLengthValid(true) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");
    // Create Frame for Length Label and textField
    FXHorizontalFrame* lengthFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLengthLabel = new FXLabel(lengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelAttribute);
    myLengthTextField = new FXTextField(lengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    myBlockMovementCheckButton->setCheck(false);
    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEAdditionalFrame::NeteditAttributes::~NeteditAttributes() {}



void
GNEAdditionalFrame::NeteditAttributes::showNeteditAttributes(bool includeLengthAndReferencePoint) {
    show();
    if (includeLengthAndReferencePoint) {
        myLengthLabel->show();
        myLengthTextField->show();
        myReferencePointMatchBox->show();
    } else {
        myLengthLabel->hide();
        myLengthTextField->hide();
        myReferencePointMatchBox->hide();
    }
}


void
GNEAdditionalFrame::NeteditAttributes::hideNeteditAttributes() {
    hide();
}


GNEAdditionalFrame::NeteditAttributes::additionalReferencePoint
GNEAdditionalFrame::NeteditAttributes::getActualReferencePoint() const {
    return myActualAdditionalReferencePoint;
}


double
GNEAdditionalFrame::NeteditAttributes::getLength() const {
    return GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
}


bool
GNEAdditionalFrame::NeteditAttributes::isBlockEnabled() const {
    return myBlockMovementCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEAdditionalFrame::NeteditAttributes::isCurrentLengthValid() const {
    return myCurrentLengthValid;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdSetLength(FXObject*, FXSelector, void*) {
    // change color of text field depending of the input length
    if (GNEAttributeCarrier::canParse<double>(myLengthTextField->getText().text()) &&
            GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text()) > 0) {
        myLengthTextField->setTextColor(FXRGB(0, 0, 0));
        myLengthTextField->killFocus();
        myCurrentLengthValid = true;
    } else {
        myLengthTextField->setTextColor(FXRGB(255, 0, 0));
        myCurrentLengthValid = false;
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdSelectReferencePoint(FXObject*, FXSelector, void*) {
    // Cast actual reference point type
    if (myReferencePointMatchBox->getText() == "reference left") {
        myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
        myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_LEFT;
        myLengthTextField->enable();
    } else if (myReferencePointMatchBox->getText() == "reference right") {
        myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
        myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_RIGHT;
        myLengthTextField->enable();
    } else if (myReferencePointMatchBox->getText() == "reference center") {
        myLengthTextField->enable();
        myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
        myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_CENTER;
        myLengthTextField->enable();
    } else {
        myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
        myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_INVALID;
        myLengthTextField->disable();
    }
    return 1;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdSetBlocking(FXObject*, FXSelector, void*) {
    if (myBlockMovementCheckButton->getCheck()) {
        myBlockMovementCheckButton->setText("true");
    } else {
        myBlockMovementCheckButton->setText("false");
    }
    return 1;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << "- Referece point: Mark the initial position of the additional element.\n"
            << "  Example: If you want to create a busStop with a length of 30 in the point 100 of the lane:\n"
            << "  - Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "  - Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "  - Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "- Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening NeteditAttributes help dialog");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing NeteditAttributes help dialog");
    return 1;
}


std::string
GNEAdditionalFrame::getIdsSelected(const FXList* list) {
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


GNEAdditionalFrame::AdditionalSelector*
GNEAdditionalFrame::getAdditionalSelector() const {
    return myAdditionalSelector;
}


GNEAdditionalFrame::AdditionalAttributes*
GNEAdditionalFrame::getAdditionalParameters() const {
    return myAdditionalParameters;
}


GNEAdditionalFrame::NeteditAttributes*
GNEAdditionalFrame::getNeteditAttributes() const {
    return myNeteditParameters;
}


GNEAdditionalFrame::SelectorParentAdditional*
GNEAdditionalFrame::getAdditionalParentSelector() const {
    return myFirstAdditionalParentSelector;
}


GNEAdditionalFrame::SelectorParentEdges*
GNEAdditionalFrame::getEdgeParentsSelector() const {
    return myEdgeParentsSelector;
}


GNEAdditionalFrame::SelectorParentLanes*
GNEAdditionalFrame::getLaneParentsSelector() const {
    return myLaneParentsSelector;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorParentAdditional - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentAdditional::SelectorParentAdditional(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Parent selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myAdditionalTypeParent(SUMO_TAG_NOTHING) {
    // Create label with the type of SelectorParentAdditional
    myFirstAdditionalParentsLabel = new FXLabel(this, "No additional selected", 0, GUIDesignLabelLeftThick);
    // Create list
    myFirstAdditionalParentsList = new FXList(this, this, MID_GNE_SET_TYPE, GUIDesignListSingleElement, 0, 0, 0, 100);
    // Hide List
    hideListOfAdditionalParents();
}


GNEAdditionalFrame::SelectorParentAdditional::~SelectorParentAdditional() {}


std::string
GNEAdditionalFrame::SelectorParentAdditional::getIdSelected() const {
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        if (myFirstAdditionalParentsList->isItemSelected(i)) {
            return myFirstAdditionalParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEAdditionalFrame::SelectorParentAdditional::setIDSelected(const std::string& id) {
    // first unselect all
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        myFirstAdditionalParentsList->getItem(i)->setSelected(false);
    }
    // select element if correspond to given ID
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        if (myFirstAdditionalParentsList->getItem(i)->getText().text() == id) {
            myFirstAdditionalParentsList->getItem(i)->setSelected(true);
        }
    }
    // recalc myFirstAdditionalParentsList
    myFirstAdditionalParentsList->recalc();
}


void
GNEAdditionalFrame::SelectorParentAdditional::showListOfAdditionalParents(SumoXMLTag additionalType) {
    myAdditionalTypeParent = additionalType;
    myFirstAdditionalParentsLabel->setText(("Parent type: " + toString(additionalType)).c_str());
    refreshListOfAdditionalParents();
    show();
}


void
GNEAdditionalFrame::SelectorParentAdditional::hideListOfAdditionalParents() {
    myAdditionalTypeParent = SUMO_TAG_NOTHING;
    hide();
}


void
GNEAdditionalFrame::SelectorParentAdditional::refreshListOfAdditionalParents() {
    myFirstAdditionalParentsList->clearItems();
    if (myAdditionalTypeParent != SUMO_TAG_NOTHING) {
        // fill list with IDs of additionals
        for (auto i : myAdditionalFrameParent->getViewNet()->getNet()->getAdditionalByType(myAdditionalTypeParent)) {
            myFirstAdditionalParentsList->appendItem(i.first.c_str());
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorParentEdges - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentEdges::SelectorParentEdges(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Edges", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create menuCheck for selected edges
    myUseSelectedEdgesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButtonAttribute);

    // Create search box
    myEdgesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "Clear", 0, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "Invert", 0, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideList();
}


GNEAdditionalFrame::SelectorParentEdges::~SelectorParentEdges() {}


std::string
GNEAdditionalFrame::SelectorParentEdges::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}


void
GNEAdditionalFrame::SelectorParentEdges::showList(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // get all edges of net
    /// @todo this function must be improved.
    std::vector<GNEEdge*> vectorOfEdges = myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(false);
    // iterate over edges of net
    for (auto i : vectorOfEdges) {
        // If search criterium is correct, then append ittem
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedEdges isn't checked
    myUseSelectedEdgesCheckButton->setCheck(false);
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    // Show dialog
    show();
}


void
GNEAdditionalFrame::SelectorParentEdges::hideList() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorParentEdges::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true).size() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


bool
GNEAdditionalFrame::SelectorParentEdges::isUseSelectedEdgesEnable() const {
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        myEdgesSearch->hide();
        myList->hide();
        myClearEdgesSelection->hide();
        myInvertEdgesSelection->hide();
    } else {
        myEdgesSearch->show();
        myList->show();
        myClearEdgesSelection->show();
        myInvertEdgesSelection->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorParentEdges that contains the searched string
    showList(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorParentLanes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentLanes::SelectorParentLanes(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Lanes", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create CheckBox for selected lanes
    myUseSelectedLanesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButtonAttribute);

    // Create search box
    myLanesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideList();
}


GNEAdditionalFrame::SelectorParentLanes::~SelectorParentLanes() {}


std::string
GNEAdditionalFrame::SelectorParentLanes::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}


void
GNEAdditionalFrame::SelectorParentLanes::showList(std::string search) {
    myList->clearItems();
    std::vector<GNELane*> vectorOfLanes = myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(false);
    for (auto i : vectorOfLanes) {
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedLanes isn't checked
    myUseSelectedLanesCheckButton->setCheck(false);
    // Show list
    show();
}


void
GNEAdditionalFrame::SelectorParentLanes::hideList() {
    hide();
}


void
GNEAdditionalFrame::SelectorParentLanes::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true).size() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


bool
GNEAdditionalFrame::SelectorParentLanes::isUseSelectedLanesEnable() const {
    if (myUseSelectedLanesCheckButton->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
    if (myUseSelectedLanesCheckButton->getCheck()) {
        myLanesSearch->hide();
        myList->hide();
        clearLanesSelection->hide();
        invertLanesSelection->hide();
    } else {
        myLanesSearch->show();
        myList->show();
        clearLanesSelection->show();
        invertLanesSelection->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorParentLanes that contains the searched string
    showList(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalFrame::GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Additionals") {

    // create Additional Selector
    myAdditionalSelector = new GNEAdditionalFrame::AdditionalSelector(this);

    // Create additional parameters
    myAdditionalParameters = new GNEAdditionalFrame::AdditionalAttributes(this);

    // Create Netedit parameter
    myNeteditParameters = new GNEAdditionalFrame::NeteditAttributes(this);

    // Create create list for additional Set
    myFirstAdditionalParentSelector = new GNEAdditionalFrame::SelectorParentAdditional(this);

    /// Create list for SelectorParentEdges
    myEdgeParentsSelector = new GNEAdditionalFrame::SelectorParentEdges(this);

    /// Create list for SelectorParentLanes
    myLaneParentsSelector = new GNEAdditionalFrame::SelectorParentLanes(this);

    // set BusStop as default additional
    myAdditionalSelector->setCurrentAdditional(SUMO_TAG_BUS_STOP);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {}


GNEAdditionalFrame::AddAdditionalResult
GNEAdditionalFrame::addAdditional(GNENetElement* netElement, GNEAdditional* additionalElement) {
    // first check that current selected additional is valid
    if (myAdditionalSelector->getCurrentAdditionalType() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }
    // obtain tag and  tagproperty (only for improve code legibility)
    const SumoXMLTag tag = myAdditionalSelector->getCurrentAdditionalType();
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(myAdditionalSelector->getCurrentAdditionalType());

    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement = myAdditionalParameters->getAttributesAndValues();

    // limit position depending if show grid is enabled
    Position currentPosition = myViewNet->snapToActiveGrid(myViewNet->getPositionInformation());

    // Declare pointer to netElements
    GNEJunction* pointed_junction = nullptr;
    GNEEdge* pointed_edge = nullptr;
    GNELane* pointed_lane = nullptr;
    GNECrossing* pointed_crossing = nullptr;

    // If element owns an additional parent, get id of parent from AdditionalParentSelector
    if (tagValue.hasParent()) {
        // if user click over an additional element parent, mark int in AdditionalParentSelector
        if (additionalElement && (additionalElement->getTag() == tagValue.getParentTag())) {
            valuesOfElement[GNE_ATTR_PARENT] = additionalElement->getID();
            myFirstAdditionalParentSelector->setIDSelected(additionalElement->getID());
        }
        // stop if currently there isn't a valid selected parent
        if (myFirstAdditionalParentSelector->getIdSelected() != "") {
            valuesOfElement[GNE_ATTR_PARENT] = myFirstAdditionalParentSelector->getIdSelected();
        } else {
            myAdditionalParameters->showWarningMessage("A " + toString(tagValue.getParentTag()) + " must be selected before insertion of " + toString(myAdditionalSelector->getCurrentAdditionalType()) + ".");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // Check if additional should be placed over a junction
    if (tagValue.hasAttribute(SUMO_ATTR_JUNCTION)) {
        pointed_junction = dynamic_cast<GNEJunction*>(netElement);
        if (pointed_junction != nullptr) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myAdditionalParameters->areValuesValid() == false) {
                myAdditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute junction
            valuesOfElement[SUMO_ATTR_JUNCTION] = pointed_junction->getID();
            // Generate id of element based on the junction
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_junction);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    }
    // Check if additional should be placed over a edge
    else if (tagValue.hasAttribute(SUMO_ATTR_EDGE) ||
             (myAdditionalSelector->getCurrentAdditionalType() == SUMO_TAG_VAPORIZER)) {
        // Due a edge is composed of lanes, its neccesary check if clicked element is an lane
        if (dynamic_cast<GNELane*>(netElement) != nullptr) {
            pointed_edge = &(dynamic_cast<GNELane*>(netElement)->getParentEdge());
        }
        if (pointed_edge != nullptr) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myAdditionalParameters->areValuesValid() == false) {
                myAdditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute edge
            valuesOfElement[SUMO_ATTR_EDGE] = pointed_edge->getID();
            // Generate id of element based on the edge
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_edge);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    }
    // Check if additional should be placed over a lane
    else if (tagValue.hasAttribute(SUMO_ATTR_LANE)) {
        pointed_lane = dynamic_cast<GNELane*>(netElement);
        if (pointed_lane != nullptr) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myAdditionalParameters->areValuesValid() == false) {
                myAdditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute lane
            valuesOfElement[SUMO_ATTR_LANE] = pointed_lane->getID();
            // Generate id of element based on the lane
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_lane);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    }
    // Check if additional should be placed over a crossing
    else if (tagValue.hasAttribute(SUMO_ATTR_CROSSING)) {
        pointed_crossing = dynamic_cast<GNECrossing*>(netElement);
        if (pointed_crossing != nullptr) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myAdditionalParameters->areValuesValid() == false) {
                myAdditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute crossing
            valuesOfElement[SUMO_ATTR_CROSSING] = pointed_crossing->getID();
            // Generate id of element based on the crossing
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_crossing);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    } else {
        // Generate id of element
        valuesOfElement[SUMO_ATTR_ID] = generateID(nullptr);
    }

    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalParameters->areValuesValid() == false) {
        myAdditionalParameters->showWarningMessage();
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }

    // Obtain position attribute if wasn't previously setted
    if (valuesOfElement.find(SUMO_ATTR_POSITION) == valuesOfElement.end()) {
        if (pointed_edge) {
            // Obtain position of the mouse over edge
            double positionOfTheMouseOverEdge = pointed_edge->getLanes().at(0)->getShape().nearest_offset_to_point2D(currentPosition);
            // If element has a StartPosition and EndPosition over edge, extract attributes
            if (tagValue.hasAttribute(SUMO_ATTR_STARTPOS) && tagValue.hasAttribute(SUMO_ATTR_ENDPOS)) {
                // First check that current length is valid
                if (myNeteditParameters->isCurrentLengthValid()) {
                    // check if current reference point is valid
                    if (myNeteditParameters->getActualReferencePoint() == NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                        myAdditionalParameters->showWarningMessage("Current selected reference point isn't valid");
                        return ADDADDITIONAL_INVALID_ARGUMENTS;
                    } else {
                        // set start and end position
                        valuesOfElement[SUMO_ATTR_STARTPOS] = toString(setStartPosition(positionOfTheMouseOverEdge, myNeteditParameters->getLength()));
                        valuesOfElement[SUMO_ATTR_ENDPOS] = toString(setEndPosition(pointed_edge->getLanes().at(0)->getLaneShapeLength(), positionOfTheMouseOverEdge, myNeteditParameters->getLength()));
                    }
                } else {
                    return ADDADDITIONAL_INVALID_ARGUMENTS;
                }
            }
            // Extract position of lane
            valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverEdge);
        } else if (pointed_lane) {
            // Obtain position of the mouse over lane
            double positionOfTheMouseOverLane = pointed_lane->getShape().nearest_offset_to_point2D(currentPosition) / pointed_lane->getLengthGeometryFactor();
            // If element has a StartPosition and EndPosition over lane, extract attributes
            if (tagValue.hasAttribute(SUMO_ATTR_STARTPOS) && tagValue.hasAttribute(SUMO_ATTR_ENDPOS)) {
                // First check that current length is valid
                if (myNeteditParameters->isCurrentLengthValid()) {
                    // check if current reference point is valid
                    if (myNeteditParameters->getActualReferencePoint() == NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                        myAdditionalParameters->showWarningMessage("Current selected reference point isn't valid");
                        return ADDADDITIONAL_INVALID_ARGUMENTS;
                    } else {
                        // set start and end position
                        valuesOfElement[SUMO_ATTR_STARTPOS] = toString(setStartPosition(positionOfTheMouseOverLane, myNeteditParameters->getLength()));
                        valuesOfElement[SUMO_ATTR_ENDPOS] = toString(setEndPosition(pointed_lane->getLaneShapeLength(), positionOfTheMouseOverLane, myNeteditParameters->getLength()));
                    }
                } else {
                    return ADDADDITIONAL_INVALID_ARGUMENTS;
                }
            }
            // Extract position of lane
            valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverLane);
        } else {
            // get position in map
            valuesOfElement[SUMO_ATTR_POSITION] = toString(currentPosition);
        }
    }

    // If additional has a interval defined by a begin or end, check that is valid
    if (tagValue.hasAttribute(SUMO_ATTR_STARTTIME) && tagValue.hasAttribute(SUMO_ATTR_END)) {
        double begin = GNEAttributeCarrier::parse<double>(valuesOfElement[SUMO_ATTR_STARTTIME]);
        double end = GNEAttributeCarrier::parse<double>(valuesOfElement[SUMO_ATTR_END]);
        if (begin > end) {
            myAdditionalParameters->showWarningMessage("Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' cannot be greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.xml
    if (GNEAttributeCarrier::getTagProperties(tag).hasAttribute(SUMO_ATTR_FILE) && valuesOfElement[SUMO_ATTR_FILE] == "") {
        if (tag != SUMO_TAG_CALIBRATOR && tag != SUMO_TAG_REROUTER) {
            // SUMO_ATTR_FILE is optional for calibrators and rerouters (fails to load in sumo when given and the file does not exist)
            valuesOfElement[SUMO_ATTR_FILE] = (valuesOfElement[SUMO_ATTR_ID] + ".xml");
        }
    }

    // Save block value if additional can be blocked
    if (tagValue.canBlockMovement()) {
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditParameters->isBlockEnabled());
    }

    // If element own a list of SelectorParentEdges as attribute
    if (tagValue.hasAttribute(SUMO_ATTR_EDGES)) {
        if (myEdgeParentsSelector->isUseSelectedEdgesEnable()) {
            // Declare a vector of Id's
            std::vector<std::string> vectorOfIds;
            // get Selected edges
            std::vector<GNEEdge*> selectedEdges = myViewNet->getNet()->retrieveEdges(true);
            // Iterate over selectedEdges and getId
            for (auto i : selectedEdges) {
                vectorOfIds.push_back(i->getID());
            }
            // Set saved Ids in attribute edges
            valuesOfElement[SUMO_ATTR_EDGES] = joinToString(vectorOfIds, " ");
        } else {
            valuesOfElement[SUMO_ATTR_EDGES] = myEdgeParentsSelector->getIdsSelected();
        }
        // check if attribute has at least one edge
        if (valuesOfElement[SUMO_ATTR_EDGES] == "") {
            myAdditionalParameters->showWarningMessage("List of " + toString(SUMO_TAG_EDGE) + "s cannot be empty");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // If element own a list of SelectorParentLanes as attribute
    if (tagValue.hasAttribute(SUMO_ATTR_LANES)) {
        if (myLaneParentsSelector->isUseSelectedLanesEnable()) {
            // Declare a vector of Id's
            std::vector<std::string> vectorOfIds;
            // get Selected lanes
            std::vector<GNELane*> selectedLanes = myViewNet->getNet()->retrieveLanes(true);
            // Iterate over selectedLanes and getId
            for (auto i : selectedLanes) {
                vectorOfIds.push_back(i->getID());
            }
            // Set saved Ids in attribute lanes
            valuesOfElement[SUMO_ATTR_LANES] = joinToString(vectorOfIds, " ");
        } else {
            valuesOfElement[SUMO_ATTR_LANES] = myLaneParentsSelector->getIdsSelected();
        }
        // check if attribute has at least a lane
        if (valuesOfElement[SUMO_ATTR_LANES] == "") {
            myAdditionalParameters->showWarningMessage("List of " + toString(SUMO_TAG_LANE) + "s cannot be empty");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // Create additional
    if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalSelector->getCurrentAdditionalType(), valuesOfElement)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        myFirstAdditionalParentSelector->refreshListOfAdditionalParents();
        // clear selected eddges and lanes
        myEdgeParentsSelector->onCmdClearSelection(0, 0, 0);
        myLaneParentsSelector->onCmdClearSelection(0, 0, 0);
        return ADDADDITIONAL_SUCCESS;
    } else {
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }
}

void
GNEAdditionalFrame::removeAdditional(GNEAdditional* additional) {
    myViewNet->getUndoList()->p_begin("delete " + toString(additional->getTag()));
    // first remove all additional childs of this additional calling this function recursively
    while (additional->getAdditionalChilds().size() > 0) {
        removeAdditional(additional->getAdditionalChilds().front());
    }
    // remove additional
    myViewNet->getUndoList()->add(new GNEChange_Additional(additional, false), true);
    myViewNet->getUndoList()->p_end();
}


void
GNEAdditionalFrame::show() {
    // Show frame
    GNEFrame::show();
    // Update UseSelectedLane CheckBox
    myEdgeParentsSelector->updateUseSelectedEdges();
    // Update UseSelectedLane CheckBox
    myLaneParentsSelector->updateUseSelectedLanes();
}


std::string
GNEAdditionalFrame::generateID(GNENetElement* netElement) const {
    // obtain current number of additionals to generate a new index faster
    int additionalIndex = myViewNet->getNet()->getNumberOfAdditionals(myAdditionalSelector->getCurrentAdditionalType());
    std::string currentAdditionalTypeStr = toString(myAdditionalSelector->getCurrentAdditionalType());
    if (netElement) {
        // generate ID using netElement
        while (myViewNet->getNet()->retrieveAdditional(myAdditionalSelector->getCurrentAdditionalType(), currentAdditionalTypeStr + "_" + netElement->getID() + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return currentAdditionalTypeStr + "_" + netElement->getID() + "_" + toString(additionalIndex);
    } else {
        // generate ID without netElement
        while (myViewNet->getNet()->retrieveAdditional(myAdditionalSelector->getCurrentAdditionalType(), currentAdditionalTypeStr + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return currentAdditionalTypeStr + "_" + toString(additionalIndex);
    }
}


double
GNEAdditionalFrame::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) {
    switch (myNeteditParameters->getActualReferencePoint()) {
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNEAdditionalFrame::setEndPosition(double /*laneLength*/, double positionOfTheMouseOverLane, double lengthOfAdditional) {
    switch (myNeteditParameters->getActualReferencePoint()) {
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

/****************************************************************************/
