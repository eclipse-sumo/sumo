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
/// @file    GNEFrameAttributeModules.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliary class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/GNEAllowVClassesDialog.h>
#include <netedit/dialogs/GNEMultipleParametersDialog.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/demand/GNETypeFrame.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/POIIcons.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFrameAttributeModules.h"
#include "GNEFlowEditor.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameAttributeModules::GenericDataAttributes) GenericDataAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_PARAMETERS,    GNEFrameAttributeModules::GenericDataAttributes::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,                  GNEFrameAttributeModules::GenericDataAttributes::onCmdSetParameters)
};

// Object implementation
FXIMPLEMENT(GNEFrameAttributeModules::GenericDataAttributes,        MFXGroupBoxModule,      GenericDataAttributesMap,       ARRAYNUMBER(GenericDataAttributesMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::GenericDataAttributes - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::GenericDataAttributes::GenericDataAttributes(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Attributes")),
    myFrameParent(frameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit attributes"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_PARAMETERS, GUIDesignButton);
}


GNEFrameAttributeModules::GenericDataAttributes::~GenericDataAttributes() {}


void
GNEFrameAttributeModules::GenericDataAttributes::showGenericDataAttributes() {
    // refresh GenericDataAttributes
    refreshGenericDataAttributes();
    // show groupbox
    show();
}


void
GNEFrameAttributeModules::GenericDataAttributes::hideGenericDataAttributes() {
    // hide groupbox
    hide();
}


void
GNEFrameAttributeModules::GenericDataAttributes::refreshGenericDataAttributes() {
    myTextFieldParameters->setText(getParametersStr().c_str());
    myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
    myTextFieldParameters->killFocus();
}


const Parameterised::Map&
GNEFrameAttributeModules::GenericDataAttributes::getParametersMap() const {
    return myParameters;
}


std::string
GNEFrameAttributeModules::GenericDataAttributes::getParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : myParameters) {
        result += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEFrameAttributeModules::GenericDataAttributes::getParameters() const {
    std::vector<std::pair<std::string, std::string> > result;
    // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto& parameter : myParameters) {
        result.push_back(std::make_pair(parameter.first, parameter.second));
    }
    return result;
}


void
GNEFrameAttributeModules::GenericDataAttributes::setParameters(const std::vector<std::pair<std::string, std::string> >& parameters) {
    // declare result string
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : parameters) {
        result += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    // set result in textField (and call onCmdEditParameters)
    myTextFieldParameters->setText(result.c_str(), TRUE);
}


GNEFrame*
GNEFrameAttributeModules::GenericDataAttributes::getFrameParent() const {
    return myFrameParent;
}


bool
GNEFrameAttributeModules::GenericDataAttributes::areAttributesValid() const {
    if (myTextFieldParameters->getText().empty()) {
        return true;
    } else if (myTextFieldParameters->getTextColor() == FXRGB(255, 0, 0)) {
        return false;
    } else {
        return Parameterised::areAttributesValid(getParametersStr());
    }
}


long
GNEFrameAttributeModules::GenericDataAttributes::onCmdEditParameters(FXObject*, FXSelector, void*) {
    if (GNESingleParametersDialog(this).execute()) {
        // Refresh parameter EditorCreator
        refreshGenericDataAttributes();
    }
    return 1;
}


long
GNEFrameAttributeModules::GenericDataAttributes::onCmdSetParameters(FXObject*, FXSelector, void*) {
    // clear current existent parameters
    myParameters.clear();
    // check if current given string is valid
    if (Parameterised::areParametersValid(myTextFieldParameters->getText().text(), true)) {
        // parsed parameters ok, then set text field black and continue
        myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
        myTextFieldParameters->killFocus();
        // obtain parameters "key=value"
        std::vector<std::string> parameters = StringTokenizer(myTextFieldParameters->getText().text(), "|", true).getVector();
        // iterate over parameters
        for (const auto& parameter : parameters) {
            // obtain key, value
            std::vector<std::string> keyParam = StringTokenizer(parameter, "=", true).getVector();
            // save it in myParameters
            myParameters[keyParam.front()] = keyParam.back();
        }
        // overwrite myTextFieldParameters (to remove duplicated parameters
        myTextFieldParameters->setText(getParametersStr().c_str(), FALSE);
    } else {
        myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC) {
    const auto tagProperty = AC->getTagProperty();
    if (viewNet->getEditModes().isCurrentSupermodeNetwork()) {
        if (tagProperty->isNetworkElement() || tagProperty->isAdditionalElement()) {
            return true;
        } else if ((tagProperty->getTag() == SUMO_TAG_TAZSOURCE) || (tagProperty->getTag() == SUMO_TAG_TAZSINK)) {
            return true;
        } else {
            return false;
        }
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() &&
               tagProperty->isDemandElement()) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeData() &&
               (tagProperty->isDataElement() || tagProperty->isMeanData())) {
        return true;
    } else {
        return false;
    }
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties* attributeProperties) {
    if (attributeProperties->getTagPropertyParent()->isNetworkElement() || attributeProperties->getTagPropertyParent()->isAdditionalElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeNetwork());
    } else if (attributeProperties->getTagPropertyParent()->isDemandElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeDemand());
    } else if (attributeProperties->getTagPropertyParent()->isDataElement() || attributeProperties->getTagPropertyParent()->isMeanData()) {
        return (viewNet->getEditModes().isCurrentSupermodeData());
    } else {
        return false;
    }
}

/****************************************************************************/
