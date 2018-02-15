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

#include "GNEAdditional.h"
#include "GNEAdditionalFrame.h"
#include "GNEAdditionalHandler.h"
#include "GNEAttributeCarrier.h"
#include "GNECalibrator.h"
#include "GNEChange_Additional.h"
#include "GNEChange_CalibratorItem.h"
#include "GNEChange_RerouterItem.h"
#include "GNEChange_Selection.h"
#include "GNEChange_VariableSpeedSignItem.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNECrossing.h"
#include "GNEDestProbReroute.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERouteProbReroute.h"
#include "GNEUndoList.h"
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignStep.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame::AdditionalSelector) SelectorAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEAdditionalFrame::AdditionalSelector::onCmdSelectAdditional),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributeSingle) AdditionalAttributeSingleMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_TEXT,     GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetBooleanAttribute),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributeList) AdditionalAttributeListMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_ADDROW,     GNEAdditionalFrame::AdditionalAttributeList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_REMOVEROW,  GNEAdditionalFrame::AdditionalAttributeList::onCmdRemoveRow),
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

FXDEFMAP(GNEAdditionalFrame::SelectorParentAdditional) SelectorParentAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEAdditionalFrame::SelectorParentAdditional::onCmdSelectAdditionalParent),
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
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributeList,        FXVerticalFrame,    AdditionalAttributeListMap,     ARRAYNUMBER(AdditionalAttributeListMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributes,           FXGroupBox,         AdditionalAttributesMap,        ARRAYNUMBER(AdditionalAttributesMap))
FXIMPLEMENT(GNEAdditionalFrame::NeteditAttributes,              FXGroupBox,         NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentAdditional,       FXGroupBox,         SelectorParentAdditionalMap,    ARRAYNUMBER(SelectorParentAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentEdges,            FXGroupBox,         SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentLanes,            FXGroupBox,         SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalSelector - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalSelector::AdditionalSelector(GNEAdditionalFrame *additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Additional element", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myCurrentAdditionalType(SUMO_TAG_NOTHING) {

    // Create FXListBox in myGroupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);

    // Add options to myAdditionalMatchBox
    for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
        myAdditionalMatchBox->appendItem(toString(i).c_str());
    }

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems());

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
    if(myCurrentAdditionalType != SUMO_TAG_NOTHING) {
        // first check if additional can block movement, then show neteditParameters
        if (GNEAttributeCarrier::canBlockMovement(myCurrentAdditionalType)) {
            myAdditionalFrameParent->getNeteditAttributes()->show();
            myAdditionalFrameParent->getNeteditAttributes()->hideLengthAndReferencePoint();
        } else {
            myAdditionalFrameParent->getNeteditAttributes()->hide();
        }
        // Clear internal attributes
        myAdditionalFrameParent->getAdditionalParameters()->clearAttributes();
        // iterate over attributes of myCurrentAdditionalType
        for (auto i : GNEAttributeCarrier::allowedAttributes(myCurrentAdditionalType)) {
            // only show attributes that aren't uniques
            if (!GNEAttributeCarrier::isUnique(myCurrentAdditionalType, i.first)) {
                myAdditionalFrameParent->getAdditionalParameters()->addAttribute(i.first);
            } else if (i.first == SUMO_ATTR_ENDPOS) {
                myAdditionalFrameParent->getNeteditAttributes()->showLengthAndReferencePoint();
            }
        }
        // if there are parameters, show and Recalc groupBox
        if (myAdditionalFrameParent->getAdditionalParameters()->getNumberOfAddedAttributes() > 0) {
            myAdditionalFrameParent->getAdditionalParameters()->showAdditionalParameters();
        } else {
            myAdditionalFrameParent->getAdditionalParameters()->hideAdditionalParameters();
        }
        // Show myAdditionalParentSelector if we're adding a Entry/Exit
        if (GNEAttributeCarrier::canHaveParent(myCurrentAdditionalType)) {
            myAdditionalFrameParent->getAdditionalParentSelector()->showListOfAdditionals(SUMO_TAG_E3DETECTOR, true);
        } else {
            myAdditionalFrameParent->getAdditionalParentSelector()->hideListOfAdditionals();
        }
        // Show SelectorParentEdges if we're adding an additional that own the attribute SUMO_ATTR_EDGES
        if (GNEAttributeCarrier::hasAttribute(myCurrentAdditionalType, SUMO_ATTR_EDGES)) {
            myAdditionalFrameParent->getEdgeParentsSelector()->showList();
        } else {
            myAdditionalFrameParent->getEdgeParentsSelector()->hideList();
        }
        // Show SelectorParentLanes if we're adding an additional that own the attribute SUMO_ATTR_LANES
        if (GNEAttributeCarrier::hasAttribute(myCurrentAdditionalType, SUMO_ATTR_LANES)) {
            myAdditionalFrameParent->getLaneParentsSelector()->showList();
        } else {
            myAdditionalFrameParent->getLaneParentsSelector()->hideList();
        }
    } else {
        // hide all groupbox if additional isn't valid
        myAdditionalFrameParent->getAdditionalParameters()->hideAdditionalParameters();
        myAdditionalFrameParent->getNeteditAttributes()->hide();
        myAdditionalFrameParent->getAdditionalParentSelector()->hideListOfAdditionals();
        myAdditionalFrameParent->getEdgeParentsSelector()->hideList();
        myAdditionalFrameParent->getLaneParentsSelector()->hideList();
     }
}


long
GNEAdditionalFrame::AdditionalSelector::onCmdSelectAdditional(FXObject*, FXSelector, void*) {
    // Check if value of myAdditionalMatchBox correspond of an allowed additional tags 
    for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
        if (toString(i) == myAdditionalMatchBox->getText().text()) {
            myAdditionalMatchBox->setTextColor(FXRGB(0, 0, 0));
            setCurrentAdditional(i);
            // Write Warning in console if we're in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING(("Selected additional '" + myAdditionalMatchBox->getText() + "' in AdditionalSelector").text());
            }
            return 1;
        }
    }
    // if additional name isn't correct, hidde all
    setCurrentAdditional(SUMO_TAG_NOTHING);
    myAdditionalMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Selected invalid additional in AdditionalSelector");
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributeSingle - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributeSingle::AdditionalAttributeSingle(AdditionalAttributes *additionalAttributesParent) :
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
    myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
    myTextFieldStrings->setText(value.c_str());
    myTextFieldStrings->show();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLAttr additionalAttr, int value) {
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
    myTextFieldInt->setText(toString(value).c_str());
    myTextFieldInt->show();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLAttr additionalAttr, double value) {
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
    myTextFieldReal->setText(toString(value).c_str());
    myTextFieldReal->show();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLAttr additionalAttr, bool value) {
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    if (value) {
        myBoolCheckButton->setCheck(true);
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setCheck(false);
        myBoolCheckButton->setText("false");
    }
    myBoolCheckButton->show();
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
    if (GNEAttributeCarrier::isBool(myAdditionalAttributesParent->getAdditionalFrameParent()->getAdditionalSelector()->getCurrentAdditionalType(), myAdditionalAttr)) {
        return (myBoolCheckButton->getCheck() == 1) ? "true" : "false";
    } else if (GNEAttributeCarrier::isInt(myAdditionalAttributesParent->getAdditionalFrameParent()->getAdditionalSelector()->getCurrentAdditionalType(), myAdditionalAttr)) {
        return myTextFieldInt->getText().text();
    } else if (GNEAttributeCarrier::isFloat(myAdditionalAttributesParent->getAdditionalFrameParent()->getAdditionalSelector()->getCurrentAdditionalType(), myAdditionalAttr) || 
               GNEAttributeCarrier::isTime(myAdditionalAttributesParent->getAdditionalFrameParent()->getAdditionalSelector()->getCurrentAdditionalType(), myAdditionalAttr)) {
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
    // obtain current additional tag
    SumoXMLTag additionalTag = myAdditionalAttributesParent->getAdditionalFrameParent()->getAdditionalSelector()->getCurrentAdditionalType();
    // Check if format of current value of myTextField is correct
    if (GNEAttributeCarrier::isInt(additionalTag, myAdditionalAttr)) {
        if (GNEAttributeCarrier::canParse<int>(myTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myTextFieldInt->getText().text());
            // Check if int value must be positive
            if (GNEAttributeCarrier::isPositive(additionalTag, myAdditionalAttr) && (intValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'int' format";
        }
    } else if (GNEAttributeCarrier::isTime(additionalTag, myAdditionalAttr)) {
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
    } else if (GNEAttributeCarrier::isFloat(additionalTag, myAdditionalAttr)) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if double value must be positive
            if (GNEAttributeCarrier::isPositive(additionalTag, myAdditionalAttr) && (doubleValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
                // check if double value is a probability
            } else if (GNEAttributeCarrier::isProbability(additionalTag, myAdditionalAttr) && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' takes only values between 0 and 1";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'float' format";
        }
    } else if (GNEAttributeCarrier::isFilename(additionalTag, myAdditionalAttr)) {
        // check if filename format is valid
        if (GNEAttributeCarrier::isValidFilename(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        }
    } else if (GNEAttributeCarrier::isSVCPermissions(additionalTag, myAdditionalAttr)) {
        // check if lists of Vclass are valid
        if (canParseVehicleClasses(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "list of VClass isn't valid";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_ROUTEPROBE) {
        // check if filename format is valid
        if (GNEAttributeCarrier::isValidID(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "RouteProbe ID contains invalid characters";
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
// GNEAdditionalFrame::AdditionalAttributeList - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributeList::AdditionalAttributeList(AdditionalAttributes *additionalAttributesParent) :
    FXVerticalFrame(additionalAttributesParent, GUIDesignAuxiliarHorizontalFrame),
    myAdditionalAttributesParent(additionalAttributesParent),
    myAdditionalAttr(SUMO_ATTR_NOTHING),
    myNumberOfVisibleTextfields(1),
    myMaxNumberOfValuesInParameterList(20) {
    // Create elements
    for (int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myHorizontalFrames.push_back(new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame));
        myLabels.push_back(new FXLabel(myHorizontalFrames.back(), "name", 0, GUIDesignLabelAttribute));
        myTextFields.push_back(new FXTextField(myHorizontalFrames.back(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextField));
    }
    // Create label Row
    myHorizontalFrameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabels.push_back(new FXLabel(myHorizontalFrameButtons, "Rows", 0, GUIDesignLabelAttribute));
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(myHorizontalFrameButtons, GUIDesignAuxiliarHorizontalFrame);
    // Create add button
    myAddButton = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_ADDITIONALFRAME_ADDROW, GUIDesignButtonIcon);
    // Create remove buttons
    myRemoveButton = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), this, MID_GNE_ADDITIONALFRAME_REMOVEROW, GUIDesignButtonIcon);
    // Hide all para meters
    hideParameter();
}


GNEAdditionalFrame::AdditionalAttributeList::~AdditionalAttributeList() {}

void
GNEAdditionalFrame::AdditionalAttributeList::showListParameter(SumoXMLAttr additionalAttr, std::vector<std::string> value) {
    if ((int)value.size() < myMaxNumberOfValuesInParameterList) {
        myAdditionalAttr = additionalAttr;
        myNumberOfVisibleTextfields = (int)value.size();
        if (myNumberOfVisibleTextfields == 0) {
            myNumberOfVisibleTextfields++;
        }
        for (int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
            myLabels.at(i)->setText((toString(myAdditionalAttr) + ": " + toString(i)).c_str());
        }
        for (int i = 0; i < myNumberOfVisibleTextfields; i++) {
            myHorizontalFrames.at(i)->show();
        }
        myHorizontalFrameButtons->show();
        FXVerticalFrame::show();
    }
}


void
GNEAdditionalFrame::AdditionalAttributeList::hideParameter() {
    myAdditionalAttr = SUMO_ATTR_NOTHING;
    for (int i = 0; i < (int)myHorizontalFrames.size(); i++) {
        myHorizontalFrames.at(i)->hide();
    }
    myHorizontalFrameButtons->hide();
    FXVerticalFrame::hide();
}


SumoXMLAttr
GNEAdditionalFrame::AdditionalAttributeList::getAttr() const {
    return myAdditionalAttr;
}


std::string
GNEAdditionalFrame::AdditionalAttributeList::getListValues() {
    // Declare, fill and return a string with the list values
    std::string value;
    for (int i = 0; i < myNumberOfVisibleTextfields; i++) {
        if (!myTextFields.at(i)->getText().empty()) {
            value += (myTextFields.at(i)->getText().text() + std::string(" "));
        }
    }
    return value;
}


long
GNEAdditionalFrame::AdditionalAttributeList::onCmdAddRow(FXObject*, FXSelector, void*) {
    if (myNumberOfVisibleTextfields < (myMaxNumberOfValuesInParameterList - 1)) {
        myHorizontalFrames.at(myNumberOfVisibleTextfields)->show();
        myNumberOfVisibleTextfields++;
        getParent()->recalc();
    }
    return 1;
}


long
GNEAdditionalFrame::AdditionalAttributeList::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    if (myNumberOfVisibleTextfields > 1) {
        myNumberOfVisibleTextfields--;
        myHorizontalFrames.at(myNumberOfVisibleTextfields)->hide();
        myTextFields.at(myNumberOfVisibleTextfields)->setText("");
        getParent()->recalc();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributes::AdditionalAttributes(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myIndexParameter(0),
    myIndexParameterList(0),
    myMaxNumberOfParameters(GNEAttributeCarrier::getHigherNumberOfAttributes()),
    myMaxNumberOfListParameters(2) {

    // Create single parameters
    for (int i = 0; i < myMaxNumberOfParameters; i++) {
        myVectorOfsingleAdditionalParameter.push_back(new AdditionalAttributeSingle(this));
    }

    // Create single list parameters
    for (int i = 0; i < myMaxNumberOfListParameters; i++) {
        myVectorOfsingleAdditionalParameterList.push_back(new AdditionalAttributeList(this));
    }

    // Create help button
    new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEAdditionalFrame::AdditionalAttributes::~AdditionalAttributes() {}


void
GNEAdditionalFrame::AdditionalAttributes::clearAttributes() {
    // Hidde al fields
    for (int i = 0; i < myMaxNumberOfParameters; i++) {
        myVectorOfsingleAdditionalParameter.at(i)->hideParameter();
    }

    // Hidde al list fields
    for (int i = 0; i < myMaxNumberOfListParameters; i++) {
        myVectorOfsingleAdditionalParameterList.at(i)->hideParameter();
    }

    // Reset indexs
    myIndexParameterList = 0;
    myIndexParameter = 0;
}


void
GNEAdditionalFrame::AdditionalAttributes::addAttribute(SumoXMLAttr AdditionalAttributeSingle) {
    // obtain parameter type
    SumoXMLTag currentType = myAdditionalFrameParent->getAdditionalSelector()->getCurrentAdditionalType();
    // If  parameter is of type list
    if (GNEAttributeCarrier::isList(currentType, AdditionalAttributeSingle)) {
        // If parameter can be show
        if (myIndexParameterList < myMaxNumberOfListParameters) {
            myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue< std::vector<std::string> >(myAdditionalFrameParent->getAdditionalSelector()->getCurrentAdditionalType(), AdditionalAttributeSingle));
            // Update index
            myIndexParameterList++;
        } else {
            WRITE_ERROR("Max number of list attributes reached (" + toString(myMaxNumberOfListParameters) + ").");
        }
    } else {
        if (myIndexParameter < myMaxNumberOfParameters) {
            if (GNEAttributeCarrier::isBool(currentType, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<bool>(currentType, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isInt(currentType, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<int>(currentType, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isFloat(currentType, AdditionalAttributeSingle) || GNEAttributeCarrier::isTime(currentType, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<double>(currentType, AdditionalAttributeSingle));
            } else {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<std::string>(currentType, AdditionalAttributeSingle));
            }
            // Update index parameter
            myIndexParameter++;
        } else {
            WRITE_ERROR("Max number of attributes reached (" + toString(myMaxNumberOfParameters) + ").");
        }
    }
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
    // get standar Parameters
    for (int i = 0; i < myIndexParameter; i++) {
        values[myVectorOfsingleAdditionalParameter.at(i)->getAttr()] = myVectorOfsingleAdditionalParameter.at(i)->getValue();
    }
    // get list parameters
    for (int i = 0; i < myIndexParameterList; i++) {
        values[myVectorOfsingleAdditionalParameterList.at(i)->getAttr()] = myVectorOfsingleAdditionalParameterList.at(i)->getListValues();
    }
    return values;
}


void
GNEAdditionalFrame::AdditionalAttributes::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standar parameters
    for (int i = 0; (i < myIndexParameter) && errorMessage.empty(); i++) {
        // Return string with the error if at least one of the parameter isn't valid
        std::string attributeValue = myVectorOfsingleAdditionalParameter.at(i)->isAttributeValid();
        if (attributeValue.size() != 0) {
            errorMessage = attributeValue;
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
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING(errorMessage);
    }
}


bool
GNEAdditionalFrame::AdditionalAttributes::areValuesValid() const {
    // iterate over standar parameters
    for (int i = 0; i < myIndexParameter; i++) {
        // Return false if error message of attriuve isn't empty
        if (myVectorOfsingleAdditionalParameter.at(i)->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


int
GNEAdditionalFrame::AdditionalAttributes::getNumberOfAddedAttributes() const {
    return (myIndexParameter + myIndexParameterList);
}


GNEAdditionalFrame *
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

GNEAdditionalFrame::NeteditAttributes::NeteditAttributes(GNEAdditionalFrame *additionalFrameParent) :
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
GNEAdditionalFrame::NeteditAttributes::showLengthAndReferencePoint() {
    myLengthLabel->show();
    myLengthTextField->show();
    myReferencePointMatchBox->show();
    show();
}


void
GNEAdditionalFrame::NeteditAttributes::hideLengthAndReferencePoint() {
    myLengthLabel->hide();
    myLengthTextField->hide();
    myReferencePointMatchBox->hide();
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
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Opening NeteditAttributes dialog for tag '"/** Finish + toString(currentTag) **/);
    }
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Closing NeteditAttributes dialog for tag '"/** Finish + toString(currentTag) **/);
    }
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
    return myAdditionalParentSelector;
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

GNEAdditionalFrame::SelectorParentAdditional::SelectorParentAdditional(GNEAdditionalFrame *additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Additional Set selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myUniqueSelection(false) {

    // Create label with the type of SelectorParentAdditional
    mySetLabel = new FXLabel(this, "No additional selected", 0, GUIDesignLabelLeftThick);

    // Create list
    myList = new FXList(this, this, MID_GNE_SET_TYPE, GUIDesignList, 0, 0, 0, 100);

    // Hide List
    hideListOfAdditionals();
}


GNEAdditionalFrame::SelectorParentAdditional::~SelectorParentAdditional() {}


std::string
GNEAdditionalFrame::SelectorParentAdditional::getIdSelected() const {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->isItemSelected(i)) {
            return myList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEAdditionalFrame::SelectorParentAdditional::showListOfAdditionals(SumoXMLTag type, bool uniqueSelection) {
    myUniqueSelection = uniqueSelection;
    mySetLabel->setText(("" + toString(type)).c_str());
    myList->clearItems();
    // obtain all additionals of class "type"
    std::vector<GNEAdditional*> vectorOfAdditionalParents = myAdditionalFrameParent->getViewNet()->getNet()->getAdditionals(type);
    // fill list with IDs of additionals
    for (auto i : vectorOfAdditionalParents) {
        myList->appendItem(i->getID().c_str());
    }
    show();
}


void
GNEAdditionalFrame::SelectorParentAdditional::hideListOfAdditionals() {
    hide();
}


long
GNEAdditionalFrame::SelectorParentAdditional::onCmdSelectAdditionalParent(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorParentEdges - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentEdges::SelectorParentEdges(GNEAdditionalFrame *additionalFrameParent) :
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

GNEAdditionalFrame::SelectorParentLanes::SelectorParentLanes(GNEAdditionalFrame *additionalFrameParent) :
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
    myAdditionalParentSelector = new GNEAdditionalFrame::SelectorParentAdditional(this);

    /// Create list for SelectorParentEdges
    myEdgeParentsSelector = new GNEAdditionalFrame::SelectorParentEdges(this);

    /// Create list for SelectorParentLanes
    myLaneParentsSelector = new GNEAdditionalFrame::SelectorParentLanes(this);

    // set BusStop as default additional
    myAdditionalSelector->setCurrentAdditional(SUMO_TAG_BUS_STOP);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    gSelected.remove2Update();
}


GNEAdditionalFrame::AddAdditionalResult
GNEAdditionalFrame::addAdditional(GNENetElement* netElement, GUISUMOAbstractView* abstractViewParent) {
    // check if current selected additional is valid
    if (myAdditionalSelector->getCurrentAdditionalType() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }

    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement = myAdditionalParameters->getAttributesAndValues();

    // limit position depending if show grid is enabled
    Position currentPosition = abstractViewParent->snapToActiveGrid(abstractViewParent->getPositionInformation());

    // Declare pointer to netElements
    GNEJunction* pointed_junction = NULL;
    GNEEdge* pointed_edge = NULL;
    GNELane* pointed_lane = NULL;
    GNECrossing* pointed_crossing = NULL;

    // Check if additional should be placed over a junction
    if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_JUNCTION)) {
        pointed_junction = dynamic_cast<GNEJunction*>(netElement);
        if (pointed_junction != NULL) {
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
    else if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_EDGE)) {
        // Due a edge is composed of lanes, its neccesary check if clicked element is an lane
        if (dynamic_cast<GNELane*>(netElement) != NULL) {
            pointed_edge = &(dynamic_cast<GNELane*>(netElement)->getParentEdge());
        }
        if (pointed_edge != NULL) {
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
    else if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_LANE)) {
        pointed_lane = dynamic_cast<GNELane*>(netElement);
        if (pointed_lane != NULL) {
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
    else if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_CROSSING)) {
        pointed_crossing = dynamic_cast<GNECrossing*>(netElement);
        if (pointed_crossing != NULL) {
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
        valuesOfElement[SUMO_ATTR_ID] = generateID(NULL);
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
            if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_ENDPOS)) {
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
                }
                else {
                    return ADDADDITIONAL_INVALID_ARGUMENTS;
                }
            }
            // Extract position of lane
            valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverEdge);
        }
        else if (pointed_lane) {
            // Obtain position of the mouse over lane
            double positionOfTheMouseOverLane = pointed_lane->getShape().nearest_offset_to_point2D(currentPosition);
            // If element has a StartPosition and EndPosition over lane, extract attributes
            if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_ENDPOS)) {
                // First check that current length is valid
                if (myNeteditParameters->isCurrentLengthValid()) {
                    // check if current reference point is valid
                    if (myNeteditParameters->getActualReferencePoint() == NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                        myAdditionalParameters->showWarningMessage("Current selected reference point isn't valid");
                        return ADDADDITIONAL_INVALID_ARGUMENTS;
                    }
                    else {
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
        }
        else {
            // get position in map
            valuesOfElement[SUMO_ATTR_POSITION] = toString(currentPosition);
        }
    }

    // If additional has a interval defined by a begin or end, check that is valid
    if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_STARTTIME) && GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_END)) {
        double begin = GNEAttributeCarrier::parse<double>(valuesOfElement[SUMO_ATTR_STARTTIME]);
        double end = GNEAttributeCarrier::parse<double>(valuesOfElement[SUMO_ATTR_END]);
        if (begin > end) {
            myAdditionalParameters->showWarningMessage("Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' cannot be greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.txt
    if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_FILE) && valuesOfElement[SUMO_ATTR_FILE] == "") {
        valuesOfElement[SUMO_ATTR_FILE] = (valuesOfElement[SUMO_ATTR_ID] + ".txt");
    }

    // If additional own the attribute SUMO_ATTR_OUTPUT but was't defined, will defined as <ID>.txt
    if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_OUTPUT) && valuesOfElement[SUMO_ATTR_OUTPUT] == "") {
        valuesOfElement[SUMO_ATTR_OUTPUT] = (valuesOfElement[SUMO_ATTR_ID] + ".txt");
    }

    // Save block value if additional can be blocked
    if (GNEAttributeCarrier::canBlockMovement(myAdditionalSelector->getCurrentAdditionalType())) {
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditParameters->isBlockEnabled());
    }

    // If element belongst to an additional Set, get id of parent from myAdditionalParentSelector
    if ((myAdditionalSelector->getCurrentAdditionalType() == SUMO_TAG_DET_ENTRY) || (myAdditionalSelector->getCurrentAdditionalType() == SUMO_TAG_DET_EXIT)) {
        if (myAdditionalParentSelector->getIdSelected() != "") {
            valuesOfElement[GNE_ATTR_PARENT] = myAdditionalParentSelector->getIdSelected();
        } else {
            myAdditionalParameters->showWarningMessage("A " + toString(SUMO_TAG_E3DETECTOR) + " must be selected before insertion of " + toString(myAdditionalSelector->getCurrentAdditionalType()) + ".");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // If element own a list of SelectorParentEdges as attribute
    if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_EDGES)) {
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
    if (GNEAttributeCarrier::hasAttribute(myAdditionalSelector->getCurrentAdditionalType(), SUMO_ATTR_LANES)) {
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
        return ADDADDITIONAL_SUCCESS;
    } else {
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }
}

void
GNEAdditionalFrame::removeAdditional(GNEAdditional* additional) {
    myViewNet->getUndoList()->p_begin("delete " + toString(additional->getTag()));
    // save selection status
    if (gSelected.isSelected(GLO_ADDITIONAL, additional->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(additional->getGlID());
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), deselected, true), true);
    }
    // first remove all additional childs of this additional calling this function recursively
    while (additional->getAdditionalChilds().size() > 0) {
        removeAdditional(additional->getAdditionalChilds().front());
    }
    // if Additional is a calibrator, remove all calibrator items manually
    if ((additional->getTag() == SUMO_TAG_CALIBRATOR) || (additional->getTag() == SUMO_TAG_LANECALIBRATOR)) {
        GNECalibrator *calibrator = dynamic_cast<GNECalibrator*>(additional);
        // Clear flows (Always first)
        while(calibrator->getCalibratorFlows().size() > 0) {
            myViewNet->getUndoList()->add(new GNEChange_CalibratorItem(calibrator->getCalibratorFlows().front(), false), true);
        }
        // Clear VTypes
        while(calibrator->getCalibratorVehicleTypes().size() > 0) {
            myViewNet->getUndoList()->add(new GNEChange_CalibratorItem(calibrator->getCalibratorVehicleTypes().front(), false), true);
        }
        // Clear Routes
        while(calibrator->getCalibratorRoutes().size() > 0) {
            myViewNet->getUndoList()->add(new GNEChange_CalibratorItem(calibrator->getCalibratorRoutes().front(), false), true);
        }
    }
    // if Additional is a rerouter, remove all rerouter items manually
    if (additional->getTag() == SUMO_TAG_REROUTER) {
        GNERerouter *rerouter = dynamic_cast<GNERerouter*>(additional);
        // Clear rerouter intervals
        while(rerouter->getRerouterIntervals().size() > 0) {
            // clear closing lane reroutes
            while(rerouter->getRerouterIntervals().front()->getClosingReroutes().size() > 0) {
                myViewNet->getUndoList()->add(new GNEChange_RerouterItem(rerouter->getRerouterIntervals().front()->getClosingReroutes().front(), false), true);
            }
            // clear closing lane reroutes
            while(rerouter->getRerouterIntervals().front()->getClosingLaneReroutes().size() > 0) {
                myViewNet->getUndoList()->add(new GNEChange_RerouterItem(rerouter->getRerouterIntervals().front()->getClosingLaneReroutes().front(), false), true);
            }
            // clear dest prob reroutes
            while(rerouter->getRerouterIntervals().front()->getDestProbReroutes().size() > 0) {
                myViewNet->getUndoList()->add(new GNEChange_RerouterItem(rerouter->getRerouterIntervals().front()->getDestProbReroutes().front(), false), true);
            }
            // clear route porb reroutes
            while(rerouter->getRerouterIntervals().front()->getRouteProbReroutes().size() > 0) {
                myViewNet->getUndoList()->add(new GNEChange_RerouterItem(rerouter->getRerouterIntervals().front()->getRouteProbReroutes().front(), false), true);
            }
            // remove rerouter interval
            myViewNet->getUndoList()->add(new GNEChange_RerouterItem(rerouter->getRerouterIntervals().front(), false), true);
        }
    }
    // if Additional is a Variable Speed Singn, remove all steps
    if (additional->getTag() == SUMO_TAG_VSS) {
        GNEVariableSpeedSign *variableSpeedSign = dynamic_cast<GNEVariableSpeedSign*>(additional);
        // Clear vss steps
        while(variableSpeedSign->getVariableSpeedSignSteps().size() > 0) {
            myViewNet->getUndoList()->add(new GNEChange_VariableSpeedSignItem(variableSpeedSign->getVariableSpeedSignSteps().front(), false), true);
        }
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
        while (myViewNet->getNet()->getAdditional(myAdditionalSelector->getCurrentAdditionalType(), currentAdditionalTypeStr + "_" + netElement->getID() + "_" + toString(additionalIndex)) != NULL) {
            additionalIndex++;
        }
        return currentAdditionalTypeStr + "_" + netElement->getID() + "_" + toString(additionalIndex);
    } else {
        // generate ID without netElement
        while (myViewNet->getNet()->getAdditional(myAdditionalSelector->getCurrentAdditionalType(), currentAdditionalTypeStr + "_" + toString(additionalIndex)) != NULL) {
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
