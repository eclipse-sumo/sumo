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
#include <config.h>

#include <netedit/dialogs/GNEMultipleParametersDialog.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/GNEAllowVClassesDialog.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/demand/GNETypeFrame.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/gui/images/POIIcons.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFrameAttributeModules.h"
#include "GNEFlowEditor.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameAttributeModules::GenericDataAttributes) GenericDataAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_PARAMETERS_DIALOG, GNEFrameAttributeModules::GenericDataAttributes::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::GenericDataAttributes::onCmdSetParameters)
};

FXDEFMAP(GNEFrameAttributeModules::ParametersEditor) ParametersEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_PARAMETERS_DIALOG, GNEFrameAttributeModules::ParametersEditor::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::ParametersEditor::onCmdSetParameters)
};

// Object implementation
FXIMPLEMENT(GNEFrameAttributeModules::GenericDataAttributes,        MFXGroupBoxModule,      GenericDataAttributesMap,       ARRAYNUMBER(GenericDataAttributesMap))
FXIMPLEMENT(GNEFrameAttributeModules::ParametersEditor,             MFXGroupBoxModule,      ParametersEditorMap,            ARRAYNUMBER(ParametersEditorMap))

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
    myButtonEditParameters = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit attributes"), "", "", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
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
    // write debug information
    WRITE_DEBUG("Open single parameters dialog");
    if (GNESingleParametersDialog(this).execute()) {
        // write debug information
        WRITE_DEBUG("Close single parameters dialog");
        // Refresh parameter EditorCreator
        refreshGenericDataAttributes();
    } else {
        // write debug information
        WRITE_DEBUG("Cancel single parameters dialog");
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

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::ParametersEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::ParametersEditor::ParametersEditor(GNEInspectorFrame* inspectorFrameParent) :
    MFXGroupBoxModule(inspectorFrameParent, TL("Parameters")),
    myInspectorFrameParent(inspectorFrameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit parameters"), "", "", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::ParametersEditor::ParametersEditor(GNETypeFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Parameters")),
    myTypeFrameParent(typeFrameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit parameters"), "", "", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::ParametersEditor::~ParametersEditor() {}


GNEViewNet*
GNEFrameAttributeModules::ParametersEditor::getViewNet() const {
    return myInspectorFrameParent ? myInspectorFrameParent->getViewNet() : myTypeFrameParent->getViewNet();
}


void
GNEFrameAttributeModules::ParametersEditor::showParametersEditor() {
    if (myInspectorFrameParent) {
        const auto& inspectedElements = myInspectorFrameParent->getViewNet()->getInspectedElements();
        if (inspectedElements.getFirstAC() && inspectedElements.getFirstAC()->getTagProperty().hasParameters()) {
            // refresh ParametersEditor
            refreshParametersEditor();
            // show groupbox
            show();
        } else {
            hideParametersEditor();
        }
    } else if (myTypeFrameParent) {
        if (myTypeFrameParent->getTypeSelector()->getCurrentType() != nullptr) {
            /// refresh ParametersEditor
            refreshParametersEditor();
            // show groupbox
            show();
        } else {
            hideParametersEditor();
        }
    } else {
        hideParametersEditor();
    }
}


void
GNEFrameAttributeModules::ParametersEditor::hideParametersEditor() {
    // hide groupbox
    hide();
}


void
GNEFrameAttributeModules::ParametersEditor::refreshParametersEditor() {
    if (myInspectorFrameParent) {
        const auto& inspectedElements = myInspectorFrameParent->getViewNet()->getInspectedElements();
        // continue depending of markAC
        if (inspectedElements.getFirstAC() && inspectedElements.getFirstAC()->getTagProperty().hasParameters()) {
            // check if we're editing a single or a multiple AC
            if (inspectedElements.isInspectingSingleElement()) {
                // set text field parameters
                myTextFieldParameters->setText(inspectedElements.getFirstAC()->getAttribute(GNE_ATTR_PARAMETERS).c_str());
            } else {
                // check if parameters of all inspected ACs are different
                std::string parameters = inspectedElements.getFirstAC()->getAttribute(GNE_ATTR_PARAMETERS);
                for (const auto& AC : inspectedElements.getACs()) {
                    if (parameters != AC->getAttribute(GNE_ATTR_PARAMETERS)) {
                        parameters = "different parameters";
                    }
                }
                // set text field
                myTextFieldParameters->setText(parameters.c_str());
            }
            // reset color
            myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
            // disable myTextFieldParameters if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
            if (GNEFrameAttributeModules::isSupermodeValid(myInspectorFrameParent->getViewNet(), inspectedElements.getFirstAC())) {
                myTextFieldParameters->enable();
                myButtonEditParameters->enable();
            } else {
                myTextFieldParameters->disable();
                myButtonEditParameters->disable();
            }
        }
    } else if (myTypeFrameParent) {
        // get type
        GNEDemandElement* type = myTypeFrameParent->getTypeSelector()->getCurrentType();
        // continue depending of markAC
        if (type) {
            // set text field parameters
            myTextFieldParameters->setText(type->getAttribute(GNE_ATTR_PARAMETERS).c_str());
            // reset color
            myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
            // disable myTextFieldParameters if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
            if (GNEFrameAttributeModules::isSupermodeValid(myTypeFrameParent->getViewNet(), type)) {
                myTextFieldParameters->enable();
                myButtonEditParameters->enable();
            } else {
                myTextFieldParameters->disable();
                myButtonEditParameters->disable();
            }
        }
    }
}


GNEInspectorFrame*
GNEFrameAttributeModules::ParametersEditor::getInspectorFrameParent() const {
    return myInspectorFrameParent;
}


GNETypeFrame*
GNEFrameAttributeModules::ParametersEditor::getTypeFrameParent() const {
    return myTypeFrameParent;
}


long
GNEFrameAttributeModules::ParametersEditor::onCmdEditParameters(FXObject*, FXSelector, void*) {
    if (myInspectorFrameParent) {
        const auto& inspectedElements = myInspectorFrameParent->getViewNet()->getInspectedElements();
        // continue depending of markAC
        if (inspectedElements.getFirstAC() && inspectedElements.getFirstAC()->getTagProperty().hasParameters()) {
            if (inspectedElements.isInspectingMultipleElements()) {
                // write debug information
                WRITE_DEBUG("Open multiple parameters dialog");
                // open multiple parameters dialog
                if (GNEMultipleParametersDialog(this).execute()) {
                    // write debug information
                    WRITE_DEBUG("Close multiple parameters dialog");
                    // update frame parent after attribute successfully set
                    myInspectorFrameParent->attributeUpdated(GNE_ATTR_PARAMETERS);
                    // Refresh parameter EditorInspector
                    refreshParametersEditor();
                } else {
                    // write debug information
                    WRITE_DEBUG("Cancel multiple parameters dialog");
                }
            } else {
                // write debug information
                WRITE_DEBUG("Open single parameters dialog");
                if (GNESingleParametersDialog(this).execute()) {
                    // write debug information
                    WRITE_DEBUG("Close single parameters dialog");
                    // update frame parent after attribute successfully set
                    myInspectorFrameParent->attributeUpdated(GNE_ATTR_PARAMETERS);
                    // Refresh parameter EditorInspector
                    refreshParametersEditor();
                } else {
                    // write debug information
                    WRITE_DEBUG("Cancel single parameters dialog");
                }
            }
        }
    } else if (myTypeFrameParent) {
        // get type
        GNEDemandElement* type = myTypeFrameParent->getTypeSelector()->getCurrentType();
        // continue depending of type
        if (type) {
            // write debug information
            WRITE_DEBUG("Open single parameters dialog");
            if (GNESingleParametersDialog(this).execute()) {
                // write debug information
                WRITE_DEBUG("Close single parameters dialog");
                // Refresh parameter EditorInspector
                refreshParametersEditor();
            } else {
                // write debug information
                WRITE_DEBUG("Cancel single parameters dialog");
            }
        }
    }
    return 1;
}


long
GNEFrameAttributeModules::ParametersEditor::onCmdSetParameters(FXObject*, FXSelector, void*) {
    if (myInspectorFrameParent) {
        const auto& inspectedElements = myInspectorFrameParent->getViewNet()->getInspectedElements();
        // continue depending of markAC
        if (inspectedElements.getFirstAC() && inspectedElements.getFirstAC()->getTagProperty().hasParameters()) {
            // check if current given string is valid
            if (inspectedElements.getFirstAC()->isValid(GNE_ATTR_PARAMETERS, myTextFieldParameters->getText().text())) {
                // parsed parameters ok, then set text field black and continue
                myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
                myTextFieldParameters->killFocus();
                // check inspected parameters
                if (inspectedElements.isInspectingSingleElement()) {
                    // begin undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->begin(inspectedElements.getFirstAC(), "change parameters");
                    // set parameters
                    inspectedElements.getFirstAC()->setACParameters(myTextFieldParameters->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    // end undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->end();
                } else if (inspectedElements.isInspectingMultipleElements()) {
                    // begin undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->begin(inspectedElements.getFirstAC(), "change multiple parameters");
                    // set parameters in all ACs
                    for (const auto& inspectedAC : inspectedElements.getACs()) {
                        inspectedAC->setACParameters(myTextFieldParameters->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    }
                    // end undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->end();
                }
                // update frame parent after attribute successfully set
                myInspectorFrameParent->attributeUpdated(GNE_ATTR_PARAMETERS);
            } else {
                myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
            }
        }
    } else if (myTypeFrameParent) {
        // get type
        GNEDemandElement* type = myTypeFrameParent->getTypeSelector()->getCurrentType();
        // continue depending of markAC
        if (type) {
            // check if current given string is valid
            if (type->isValid(GNE_ATTR_PARAMETERS, myTextFieldParameters->getText().text())) {
                // parsed parameters ok, then set text field black and continue
                myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
                myTextFieldParameters->killFocus();
                // begin undo list
                myTypeFrameParent->getViewNet()->getUndoList()->begin(type, "change parameters");
                // set parameters
                type->setACParameters(myTextFieldParameters->getText().text(), myTypeFrameParent->getViewNet()->getUndoList());
                // end undo list
                myTypeFrameParent->getViewNet()->getUndoList()->end();
            } else {
                myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
            }
        }
    }
    return 1;
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC) {
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() &&
            (AC->getTagProperty().isNetworkElement() || AC->getTagProperty().isAdditionalElement())) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() &&
               AC->getTagProperty().isDemandElement()) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeData() &&
               (AC->getTagProperty().isDataElement() || AC->getTagProperty().isMeanData())) {
        return true;
    } else {
        return false;
    }
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr) {
    if (ACAttr.getTagPropertyParent().isNetworkElement() || ACAttr.getTagPropertyParent().isAdditionalElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeNetwork());
    } else if (ACAttr.getTagPropertyParent().isDemandElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeDemand());
    } else if (ACAttr.getTagPropertyParent().isDataElement() || ACAttr.getTagPropertyParent().isMeanData()) {
        return (viewNet->getEditModes().isCurrentSupermodeData());
    } else {
        return false;
    }
}

/****************************************************************************/
