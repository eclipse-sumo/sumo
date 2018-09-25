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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/additionals/GNECalibrator.h>
#include <netedit/additionals/GNEClosingLaneReroute.h>
#include <netedit/additionals/GNEClosingReroute.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNERerouterInterval.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/additionals/GNEDestProbReroute.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNEVariableSpeedSign.h>
#include <netedit/additionals/GNEVariableSpeedSignStep.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNENet.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEUndoList.h>

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

FXDEFMAP(GNEAdditionalFrame::SelectorLaneParents) ConsecutiveLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_STOPSELECTION,  GNEAdditionalFrame::SelectorLaneParents::onCmdStopSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GNEAdditionalFrame::SelectorLaneParents::onCmdAbortSelection),
};

FXDEFMAP(GNEAdditionalFrame::SelectorEdgeChilds) SelectorParentEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorEdgeChilds::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorEdgeChilds::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorEdgeChilds::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorEdgeChilds::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorEdgeChilds::onCmdSelectEdge),
};

FXDEFMAP(GNEAdditionalFrame::SelectorLaneChilds) SelectorParentLanesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorLaneChilds::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorLaneChilds::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorLaneChilds::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorLaneChilds::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorLaneChilds::onCmdSelectLane),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame::AdditionalSelector,         FXGroupBox,         SelectorAdditionalMap,          ARRAYNUMBER(SelectorAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributeSingle,  FXHorizontalFrame,  AdditionalAttributeSingleMap,   ARRAYNUMBER(AdditionalAttributeSingleMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributes,       FXGroupBox,         AdditionalAttributesMap,        ARRAYNUMBER(AdditionalAttributesMap))
FXIMPLEMENT(GNEAdditionalFrame::NeteditAttributes,          FXGroupBox,         NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorLaneParents,        FXGroupBox,         ConsecutiveLaneSelectorMap,     ARRAYNUMBER(ConsecutiveLaneSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorEdgeChilds,         FXGroupBox,         SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorLaneChilds,         FXGroupBox,         SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))


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

    // set busstop as default additional (item n. 11 of the list)
    myAdditionalMatchBox->setCurrentItem(11);

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
            myAdditionalFrameParent->myNeteditParameters->showNeteditAttributesModul(false);
        } else {
            myAdditionalFrameParent->myNeteditParameters->hideNeteditAttributesModul();
        }
        // Clear internal attributes
        myAdditionalFrameParent->myAdditionalParameters->clearAttributes();
        // iterate over attributes of myCurrentAdditionalType
        for (auto i : tagValue) {
            // only show attributes that aren't uniques
            if (!i.second.isUnique()) {
                myAdditionalFrameParent->myAdditionalParameters->addAttribute(i.first);
            } else if (i.first == SUMO_ATTR_ENDPOS) {
                myAdditionalFrameParent->myNeteditParameters->showNeteditAttributesModul(true);
            }
        }
        myAdditionalFrameParent->myAdditionalParameters->showAdditionalAttributesModul();
        // Show mySelectorAdditionalParent if we're adding a additional with parent
        if (tagValue.hasParent()) {
            myAdditionalFrameParent->mySelectorAdditionalParent->showSelectorAdditionalParentModul(tagValue.getParentTag());
        } else {
            myAdditionalFrameParent->mySelectorAdditionalParent->hideSelectorAdditionalParentModul();
        }
        // Show SelectorEdgeChilds if we're adding an additional that own the attribute SUMO_ATTR_EDGES
        if (tagValue.hasAttribute(SUMO_ATTR_EDGES)) {
            myAdditionalFrameParent->mySelectorEdgeChilds->showSelectorEdgeChildsModul();
        } else {
            myAdditionalFrameParent->mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
        }
        // Show SelectorLaneChilds or consecutive lane selector if we're adding an additional that own the attribute SUMO_ATTR_LANES
        if (tagValue.hasAttribute(SUMO_ATTR_LANES)) {
            if(tagValue.hasParent() && tagValue.getParentTag() == SUMO_TAG_LANE) {
                myAdditionalFrameParent->mySelectorLaneParents->showSelectorLaneParentsModul();
                myAdditionalFrameParent->mySelectorLaneChilds->hideSelectorLaneChildsModul();
            } else {
                myAdditionalFrameParent->mySelectorLaneChilds->showSelectorLaneChildsModul();
                myAdditionalFrameParent->mySelectorLaneParents->hideSelectorLaneParentsModul();
            }
        } else {
            myAdditionalFrameParent->mySelectorLaneChilds->hideSelectorLaneChildsModul();
            myAdditionalFrameParent->mySelectorLaneParents->hideSelectorLaneParentsModul();
        }
    } else {
        // hide all groupbox if additional isn't valid
        myAdditionalFrameParent->myAdditionalParameters->hideAdditionalAttributesModul();
        myAdditionalFrameParent->myNeteditParameters->hideNeteditAttributesModul();
        myAdditionalFrameParent->mySelectorAdditionalParent->hideSelectorAdditionalParentModul();
        myAdditionalFrameParent->mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
        myAdditionalFrameParent->mySelectorLaneChilds->hideSelectorLaneChildsModul();
        myAdditionalFrameParent->mySelectorLaneParents->hideSelectorLaneParentsModul();
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
    const auto& attrValue = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType()).getAttribute(myAdditionalAttr);
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
    const auto& attrValues = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType()).getAttribute(myAdditionalAttr);
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
    const auto& attrvalue = GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType()).getAttribute(AdditionalAttributeSingle);
    myVectorOfsingleAdditionalParameter.at(attrvalue.getPositionListed())->showParameter(AdditionalAttributeSingle, attrvalue.getDefaultValue());
}


void
GNEAdditionalFrame::AdditionalAttributes::showAdditionalAttributesModul() {
    recalc();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributes::hideAdditionalAttributesModul() {
    hide();
}


void
GNEAdditionalFrame::AdditionalAttributes::getAttributesAndValues(std::map<SumoXMLAttr, std::string> &valuesMap) const {
    // get standard parameters
    for (int i = 0; i < (int)myVectorOfsingleAdditionalParameter.size(); i++) {
        if (myVectorOfsingleAdditionalParameter.at(i)->getAttr() != SUMO_ATTR_NOTHING) {
            valuesMap[myVectorOfsingleAdditionalParameter.at(i)->getAttr()] = myVectorOfsingleAdditionalParameter.at(i)->getValue();
        }
    }
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
        errorMessage = "Invalid input parameter of " + toString(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType()) + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + toString(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType()) + ": " + extra;
    }

    // set message in status bar
    myAdditionalFrameParent->getViewNet()->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEAdditionalFrame::AdditionalAttributes::areCurrentAdditionalAttributesValid() const {
    // iterate over standar parameters
    for (auto i : GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType())) {
        // Return false if error message of attriuve isn't empty
        if (myVectorOfsingleAdditionalParameter.at(i.second.getPositionListed())->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


long
GNEAdditionalFrame::AdditionalAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myAdditionalFrameParent->openHelpAttributesDialog(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType());
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorLaneParents - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorLaneParents::SelectorLaneParents(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Lane Selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // create start and stop buttons
    myStopSelectingButton = new FXButton(this, "Stop selecting", 0, this, MID_GNE_ADDITIONALFRAME_STOPSELECTION, GUIDesignButton);
    myAbortSelectingButton = new FXButton(this, "Abort selecting", 0, this, MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GUIDesignButton);
    // disable stop and abort functions as init
    myStopSelectingButton->disable();
    myAbortSelectingButton->disable();
    // define colors
    myCandidateLaneColor = RGBColor(0, 64, 0, 255);
    mySelectedLaneColor = RGBColor::GREEN;
}


GNEAdditionalFrame::SelectorLaneParents::~SelectorLaneParents() {}


void 
GNEAdditionalFrame::SelectorLaneParents::showSelectorLaneParentsModul() {
    // abort current selection before show
    abortConsecutiveLaneSelector();
    // show FXGroupBox
    FXGroupBox::show();
}


void 
GNEAdditionalFrame::SelectorLaneParents::hideSelectorLaneParentsModul() {
    // abort current selection before hide
    abortConsecutiveLaneSelector();
    // hide FXGroupBox
    FXGroupBox::hide();
}


void 
GNEAdditionalFrame::SelectorLaneParents::startConsecutiveLaneSelector(GNELane *lane, const Position &clickedPosition) {
    // Only start selection if SelectorLaneParents modul is shown
    if (shown()) {
        // change buttons
        myStopSelectingButton->enable();
        myAbortSelectingButton->enable();
        // add lane
        addSelectedLane(lane, clickedPosition);
    }
}


bool 
GNEAdditionalFrame::SelectorLaneParents::stopConsecutiveLaneSelector() {
    // obtain tagproperty (only for improve code legibility)
    const auto& tagValues = GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType());

    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap;

    // fill valuesOfElement with attributes from Frame
    myAdditionalFrameParent->myAdditionalParameters->getAttributesAndValues(valuesMap);

    // Generate id of element
    valuesMap[SUMO_ATTR_ID] = myAdditionalFrameParent->generateID(nullptr);
    // obtain lane IDs
    std::vector<std::string> laneIDs;
    for (auto i : mySelectedLanes) {
        laneIDs.push_back(i.first->getID());
    }
    valuesMap[SUMO_ATTR_LANES] = joinToString(laneIDs, " ");
    // Obtain clicked position over first lane
    valuesMap[SUMO_ATTR_POSITION] = toString(mySelectedLanes.front().second);
    // Obtain clicked position over last lane
    valuesMap[SUMO_ATTR_ENDPOS] = toString(mySelectedLanes.back().second);

    // parse common attributes
    if(!myAdditionalFrameParent->buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }

    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalFrameParent->myAdditionalParameters->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalFrameParent->myAdditionalParameters->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myAdditionalFrameParent->myViewNet, true, myAdditionalFrameParent->myAdditionalSelector->getCurrentAdditionalType(), valuesMap)) {
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        return true;
    } else {
        return false;
    }
}


void 
GNEAdditionalFrame::SelectorLaneParents::abortConsecutiveLaneSelector() {
     // reset color of all candidate lanes
    for (auto i : myCandidateLanes) {
        i->setSpecialColor(0);
    }
    myCandidateLanes.clear();
    // reset color of all selected lanes
    for (auto i : mySelectedLanes) {
        i.first->setSpecialColor(0);
    }
    myCandidateLanes.clear();
    // clear selected lanes
    mySelectedLanes.clear();
    // disable buttons
    myStopSelectingButton->disable();
    myAbortSelectingButton->disable();
    // update view (due colors)
    myAdditionalFrameParent->getViewNet()->update();

}


bool 
GNEAdditionalFrame::SelectorLaneParents::addSelectedLane(GNELane *lane, const Position &clickedPosition) {
    // first check that lane exist
    if(lane == nullptr) {
        return false;
    }
    // check that lane wasn't already selected
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            WRITE_WARNING("Duplicated lanes aren't allowed");
            return false;
        }
    }
    // check that there is candidate lanes
    if(mySelectedLanes.size() > 0) {
        if (myCandidateLanes.empty()) {
            WRITE_WARNING("Only candidate lanes are allowed");
            return false;
        } else if((myCandidateLanes.size() > 0) && (std::find(myCandidateLanes.begin(), myCandidateLanes.end(), lane) == myCandidateLanes.end())) {
            WRITE_WARNING("Only consecutive lanes are allowed");
            return false;
        }
    }
    // select lane and save the clicked position
    mySelectedLanes.push_back(std::make_pair(lane, lane->getShape().nearest_offset_to_point2D(clickedPosition) / lane->getLengthGeometryFactor()));
    // change color of selected lane
    lane->setSpecialColor(&mySelectedLaneColor);
    // restore original color of candidates (except already selected)
    for (auto i : myCandidateLanes) {
        if(!isLaneSelected(i)) {
            i->setSpecialColor(0);
        }
    }
    // clear candidate lanes
    myCandidateLanes.clear();
    // fill candidate lanes
    for (auto i : lane->getParentEdge().getGNEConnections()) {
        // check that possible candidate lane isn't already selected 
        if((lane == i->getLaneFrom()) && (!isLaneSelected(i->getLaneTo()))) {
            // set candidate lane
            i->getLaneTo()->setSpecialColor(&myCandidateLaneColor);
            myCandidateLanes.push_back(i->getLaneTo());
        }
    }
    // update view (due colors)
    myAdditionalFrameParent->getViewNet()->update();
    return true;
}


void 
GNEAdditionalFrame::SelectorLaneParents::removeLastSelectedLane() {
    if(mySelectedLanes.size() > 1) {
        mySelectedLanes.pop_back();
    } else {
        WRITE_WARNING("First lane cannot be removed");
    }
}


bool 
GNEAdditionalFrame::SelectorLaneParents::isSelectingLanes() const {
    return myStopSelectingButton->isEnabled();
}


bool 
GNEAdditionalFrame::SelectorLaneParents::isShown() const {
    return shown();
}


const RGBColor&
GNEAdditionalFrame::SelectorLaneParents::getSelectedLaneColor() const {
    return mySelectedLaneColor;
}


const std::vector<std::pair<GNELane*, double> >&
GNEAdditionalFrame::SelectorLaneParents::getSelectedLanes() const {
    return mySelectedLanes;
}


long
GNEAdditionalFrame::SelectorLaneParents::onCmdStopSelection(FXObject*, FXSelector, void*) {
    stopConsecutiveLaneSelector();
    return 0;
}


long 
GNEAdditionalFrame::SelectorLaneParents::onCmdAbortSelection(FXObject*, FXSelector, void*) {
    abortConsecutiveLaneSelector();
    return 0;
}


bool
GNEAdditionalFrame::SelectorLaneParents::isLaneSelected(GNELane *lane) const {
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            return true;
        }
    }
    return false;
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
GNEAdditionalFrame::NeteditAttributes::showNeteditAttributesModul(bool includeLengthAndReferencePoint) {
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
GNEAdditionalFrame::NeteditAttributes::hideNeteditAttributesModul() {
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


GNEAdditionalFrame::SelectorLaneParents* 
GNEAdditionalFrame::getConsecutiveLaneSelector() const {
    return mySelectorLaneParents;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorAdditionalParent - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorAdditionalParent::SelectorAdditionalParent(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Parent selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myAdditionalTypeParent(SUMO_TAG_NOTHING) {
    // Create label with the type of SelectorAdditionalParent
    myFirstAdditionalParentsLabel = new FXLabel(this, "No additional selected", 0, GUIDesignLabelLeftThick);
    // Create list
    myFirstAdditionalParentsList = new FXList(this, this, MID_GNE_SET_TYPE, GUIDesignListSingleElement, 0, 0, 0, 100);
    // Hide List
    hideSelectorAdditionalParentModul();
}


GNEAdditionalFrame::SelectorAdditionalParent::~SelectorAdditionalParent() {}


std::string
GNEAdditionalFrame::SelectorAdditionalParent::getIdSelected() const {
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        if (myFirstAdditionalParentsList->isItemSelected(i)) {
            return myFirstAdditionalParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEAdditionalFrame::SelectorAdditionalParent::setIDSelected(const std::string& id) {
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


bool
GNEAdditionalFrame::SelectorAdditionalParent::showSelectorAdditionalParentModul(SumoXMLTag additionalType) {
    // make sure that we're editing an additional tag
    for (auto i : GNEAttributeCarrier::allowedAdditionalTags(false)) {
        if (i == additionalType) {
            myAdditionalTypeParent = additionalType;
            myFirstAdditionalParentsLabel->setText(("Parent type: " + toString(additionalType)).c_str());
            refreshSelectorAdditionalParentModul();
            show();
            return true;
        }
    }
    return false;
}


void
GNEAdditionalFrame::SelectorAdditionalParent::hideSelectorAdditionalParentModul() {
    myAdditionalTypeParent = SUMO_TAG_NOTHING;
    hide();
}


void
GNEAdditionalFrame::SelectorAdditionalParent::refreshSelectorAdditionalParentModul() {
    myFirstAdditionalParentsList->clearItems();
    if (myAdditionalTypeParent != SUMO_TAG_NOTHING) {
        // fill list with IDs of additionals
        for (auto i : myAdditionalFrameParent->getViewNet()->getNet()->getAdditionalByType(myAdditionalTypeParent)) {
            myFirstAdditionalParentsList->appendItem(i.first.c_str());
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorEdgeChilds - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorEdgeChilds::SelectorEdgeChilds(GNEAdditionalFrame* additionalFrameParent) :
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
    hideSelectorEdgeChildsModul();
}


GNEAdditionalFrame::SelectorEdgeChilds::~SelectorEdgeChilds() {}


std::string
GNEAdditionalFrame::SelectorEdgeChilds::getEdgeIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        // get Selected edges
        std::vector<GNEEdge*> selectedEdges = myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true);
        // Iterate over selectedEdges and getId
        for (auto i : selectedEdges) {
            vectorOfIds.push_back(i->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return joinToString(vectorOfIds, " ");
}


void
GNEAdditionalFrame::SelectorEdgeChilds::showSelectorEdgeChildsModul(std::string search) {
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
GNEAdditionalFrame::SelectorEdgeChilds::hideSelectorEdgeChildsModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorEdgeChilds::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true).size() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorEdgeChilds::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorEdgeChilds that contains the searched string
    showSelectorEdgeChildsModul(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
// GNEAdditionalFrame::SelectorLaneChilds - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorLaneChilds::SelectorLaneChilds(GNEAdditionalFrame* additionalFrameParent) :
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
    hideSelectorLaneChildsModul();
}


GNEAdditionalFrame::SelectorLaneChilds::~SelectorLaneChilds() {}


std::string
GNEAdditionalFrame::SelectorLaneChilds::getLaneIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedLanesCheckButton->getCheck()) {
        // get Selected lanes
        std::vector<GNELane*> selectedLanes = myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true);
        // Iterate over selectedLanes and getId
        for (auto i : selectedLanes) {
            vectorOfIds.push_back(i->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return joinToString(vectorOfIds, " ");
}


void
GNEAdditionalFrame::SelectorLaneChilds::showSelectorLaneChildsModul(std::string search) {
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
GNEAdditionalFrame::SelectorLaneChilds::hideSelectorLaneChildsModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorLaneChilds::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true).size() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorLaneChilds::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorLaneChilds that contains the searched string
    showSelectorLaneChildsModul(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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

    // Create consecutive Lane Selector
    mySelectorLaneParents = new GNEAdditionalFrame::SelectorLaneParents(this);

    // Create create list for additional Set
    mySelectorAdditionalParent = new GNEAdditionalFrame::SelectorAdditionalParent(this);

    /// Create list for SelectorEdgeChilds
    mySelectorEdgeChilds = new GNEAdditionalFrame::SelectorEdgeChilds(this);

    /// Create list for SelectorLaneChilds
    mySelectorLaneChilds = new GNEAdditionalFrame::SelectorLaneChilds(this);

    // set BusStop as default additional
    myAdditionalSelector->setCurrentAdditional(SUMO_TAG_BUS_STOP);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {}


bool
GNEAdditionalFrame::addAdditional(const GNEViewNet::ObjectsUnderCursor &objectsUnderCursor) {
    // first check that current selected additional is valid
    if (myAdditionalSelector->getCurrentAdditionalType() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return false;
    }
    
    // obtain tagproperty (only for improve code legibility)
    const auto& tagValues = GNEAttributeCarrier::getTagProperties(myAdditionalSelector->getCurrentAdditionalType());

    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap;

    // fill valuesOfElement with attributes from Frame
    myAdditionalParameters->getAttributesAndValues(valuesMap);

    // If element owns an additional parent, get id of parent from AdditionalParentSelector
    if (tagValues.hasParent() && !buildAdditionalWithParent(valuesMap, objectsUnderCursor.additional, tagValues)) {
        return false;
    }
    // parse common attributes
    if(!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // If consecutive Lane Selector is enabled, it means that either we're selecting lanes or we're finished or we'rent started
    if(tagValues.canBePlacedOverEdge()) {
        return buildAdditionalOverEdge(valuesMap, &objectsUnderCursor.lane->getParentEdge());
    } else if(tagValues.canBePlacedOverLane()) {
        return buildAdditionalOverLane(valuesMap, objectsUnderCursor.lane, tagValues);
    } else if(tagValues.canBePlacedOverLanes()) {
        return buildAdditionalOverLanes(valuesMap, objectsUnderCursor.lane, tagValues);
    } else {
        return buildAdditionalOverView(valuesMap, tagValues);
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
    mySelectorEdgeChilds->updateUseSelectedEdges();
    // Update UseSelectedLane CheckBox
    mySelectorLaneChilds->updateUseSelectedLanes();
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


bool 
GNEAdditionalFrame::buildAdditionalWithParent(std::map<SumoXMLAttr, std::string> &valuesMap, GNEAdditional* additionalParent, const GNEAttributeCarrier::TagValues &tagValues) {
    // if user click over an additional element parent, mark int in AdditionalParentSelector
    if (additionalParent && (additionalParent->getTag() == tagValues.getParentTag())) {
        valuesMap[GNE_ATTR_PARENT] = additionalParent->getID();
        mySelectorAdditionalParent->setIDSelected(additionalParent->getID());
    }
    // stop if currently there isn't a valid selected parent
    if (mySelectorAdditionalParent->getIdSelected() != "") {
        valuesMap[GNE_ATTR_PARENT] = mySelectorAdditionalParent->getIdSelected();
    } else {
        myAdditionalParameters->showWarningMessage("A " + toString(tagValues.getParentTag()) + " must be selected before insertion of " + toString(myAdditionalSelector->getCurrentAdditionalType()) + ".");
        return false;
    }
    return true;
}


bool 
GNEAdditionalFrame::buildAdditionalCommonAttributes(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagValues &tagValues) {
    // Save block value if additional can be blocked
    if (tagValues.canBlockMovement()) {
        valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditParameters->isBlockEnabled());
    }
    // If additional has a interval defined by a begin or end, check that is valid
    if (tagValues.hasAttribute(SUMO_ATTR_STARTTIME) && tagValues.hasAttribute(SUMO_ATTR_END)) {
        double begin = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_STARTTIME]);
        double end = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_END]);
        if (begin > end) {
            myAdditionalParameters->showWarningMessage("Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' cannot be greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
            return false;
        }
    }
    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.xml
    if (tagValues.hasAttribute(SUMO_ATTR_FILE) && valuesMap[SUMO_ATTR_FILE] == "") {
        if ((myAdditionalSelector->getCurrentAdditionalType() != SUMO_TAG_CALIBRATOR) && (myAdditionalSelector->getCurrentAdditionalType() != SUMO_TAG_REROUTER)) {
            // SUMO_ATTR_FILE is optional for calibrators and rerouters (fails to load in sumo when given and the file does not exist)
            valuesMap[SUMO_ATTR_FILE] = (valuesMap[SUMO_ATTR_ID] + ".xml");
        }
    }
    // If element own a list of SelectorEdgeChilds as attribute
    if (tagValues.hasAttribute(SUMO_ATTR_EDGES) && !tagValues.canBePlacedOverEdges()) {
        // obtain edge IDs
        valuesMap[SUMO_ATTR_EDGES] = mySelectorEdgeChilds->getEdgeIdsSelected();
        // check if attribute has at least one edge
        if (valuesMap[SUMO_ATTR_EDGES] == "") {
            myAdditionalParameters->showWarningMessage("List of " + toString(SUMO_TAG_EDGE) + "s cannot be empty");
            return false;
        }
    }
    // get values of mySelectorLaneChilds, if tag correspond to an element that has lanes as childs
    if (tagValues.hasAttribute(SUMO_ATTR_LANES) && !tagValues.canBePlacedOverLanes()) {
        // obtain lane IDs
        valuesMap[SUMO_ATTR_LANES] = mySelectorLaneChilds->getLaneIdsSelected();
        // check if attribute has at least a lane
        if (valuesMap[SUMO_ATTR_LANES] == "") {
            myAdditionalParameters->showWarningMessage("List of " + toString(SUMO_TAG_LANE) + "s cannot be empty");
            return false;
        }
    }
    // all ok, continue building additionals
    return true;
}


bool 
GNEAdditionalFrame::buildAdditionalOverEdge(std::map<SumoXMLAttr, std::string> &valuesMap, GNEEdge* edge) {
    // check that edge exist
    if (edge) {
        // Get attribute lane's edge
        valuesMap[SUMO_ATTR_EDGE] = edge->getID();
        // Generate id of element based on the lane's edge
        valuesMap[SUMO_ATTR_ID] = generateID(edge);
    } else {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalParameters->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalParameters->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalSelector->getCurrentAdditionalType(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(0, 0, 0);
        mySelectorLaneChilds->onCmdClearSelection(0, 0, 0);
        return true;
    } else {
        return false;
    }
}


bool 
GNEAdditionalFrame::buildAdditionalOverLane(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagValues &tagValues) {
    // check that lane exist
    if (lane) {
        // Get attribute lane
        valuesMap[SUMO_ATTR_LANE] = lane->getID();
        // Generate id of element based on the lane
        valuesMap[SUMO_ATTR_ID] = generateID(lane);
    } else {
        return false;
    }
    // Obtain position of the mouse over lane (limited over grid)
    double positionOfTheMouseOverLane = lane->getShape().nearest_offset_to_point2D(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
    // If element has a StartPosition and EndPosition over lane, extract attributes
    if (tagValues.hasAttribute(SUMO_ATTR_STARTPOS) && tagValues.hasAttribute(SUMO_ATTR_ENDPOS)) {
        // First check that current length is valid
        if (myNeteditParameters->isCurrentLengthValid()) {
            // check if current reference point is valid
            if (myNeteditParameters->getActualReferencePoint() == NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                myAdditionalParameters->showWarningMessage("Current selected reference point isn't valid");
                return false;
            } else {
                // set start and end position
                valuesMap[SUMO_ATTR_STARTPOS] = toString(setStartPosition(positionOfTheMouseOverLane, myNeteditParameters->getLength()));
                valuesMap[SUMO_ATTR_ENDPOS] = toString(setEndPosition(lane->getLaneShapeLength(), positionOfTheMouseOverLane, myNeteditParameters->getLength()));
            }
        } else {
            return false;
        }
    } else if (tagValues.hasAttribute(SUMO_ATTR_POSITION) && (valuesMap.find(SUMO_ATTR_POSITION) == valuesMap.end())) {
        // Obtain position attribute if wasn't previously set in Frame
        valuesMap[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverLane);
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalParameters->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalParameters->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalSelector->getCurrentAdditionalType(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(0, 0, 0);
        mySelectorLaneChilds->onCmdClearSelection(0, 0, 0);
        return true;
    } else {
        return false;
    }
}


 bool 
GNEAdditionalFrame::buildAdditionalOverLanes(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagValues &tagValues) {
     // stop if lane isn't valid
    if(lane == nullptr) {
        return false;
    }
    if(mySelectorLaneParents->isSelectingLanes()) {
        // select clicked lane, but don't build additional
        mySelectorLaneParents->addSelectedLane(lane, myViewNet->getPositionInformation());
        return false;
    } else if(mySelectorLaneParents->getSelectedLanes().empty()) {
        // if there isn't selected lanes, that means that we will be start selecting lanes
        mySelectorLaneParents->startConsecutiveLaneSelector(lane, myViewNet->getPositionInformation());
        return false;
    } else {
        // Generate id of element based on the first lane
        valuesMap[SUMO_ATTR_ID] = generateID(mySelectorLaneParents->getSelectedLanes().front().first);
        // obtain lane IDs
        std::vector<std::string> laneIDs;
        for (auto i : mySelectorLaneParents->getSelectedLanes()) {
            laneIDs.push_back(i.first->getID());
        }
        valuesMap[SUMO_ATTR_LANES] = joinToString(laneIDs, " ");
        // Check if clicked position over first lane has to be obtained
        if(tagValues.hasAttribute(SUMO_ATTR_POSITION)) {
            valuesMap[SUMO_ATTR_POSITION] = toString(mySelectorLaneParents->getSelectedLanes().front().second);
        }
        // Check if clicked position over last lane has to be obtained
        if(tagValues.hasAttribute(SUMO_ATTR_ENDPOS)) {
            valuesMap[SUMO_ATTR_ENDPOS] = toString(mySelectorLaneParents->getSelectedLanes().back().second);
        }
        // show warning dialogbox and stop check if input parameters are valid
        if (myAdditionalParameters->areCurrentAdditionalAttributesValid() == false) {
            myAdditionalParameters->showWarningMessage();
            return false;
        } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalSelector->getCurrentAdditionalType(), valuesMap)) {
            // Refresh additional Parent Selector (For additionals that have a limited number of childs)
            mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
            // abort lane selector
            mySelectorLaneParents->abortConsecutiveLaneSelector();
            return true;
        } else {
            // additional cannot be build
            return false;
        }
    }
 }


bool
GNEAdditionalFrame::buildAdditionalOverView(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagValues &tagValues) {
    // Generate id of element
    valuesMap[SUMO_ATTR_ID] = generateID(nullptr);
    // Obtain position attribute if wasn't previously set in Frame
    if (tagValues.hasAttribute(SUMO_ATTR_POSITION) && (valuesMap.find(SUMO_ATTR_POSITION) == valuesMap.end())) {
        // An attribute "position" can be either a float or a Position. If isn't float, we get the position over map
        valuesMap[SUMO_ATTR_POSITION] = toString(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation()));
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalParameters->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalParameters->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalSelector->getCurrentAdditionalType(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(0, 0, 0);
        mySelectorLaneChilds->onCmdClearSelection(0, 0, 0);
        return true;
    } else {
        return false;
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
