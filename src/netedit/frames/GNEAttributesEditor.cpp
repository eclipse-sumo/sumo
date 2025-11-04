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
/// @file    GNEAttributesEditor.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2025
///
// pack of all GNEAttributesEditorTypes
/****************************************************************************/

#include "GNEAttributesEditor.h"

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNETagProperties.h>

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditor::GNEAttributesEditor(GNEFrame* frameParent, GNEAttributesEditorType::EditorType editorType) {
    myBasicAttributesEditor = new GNEAttributesEditorType(frameParent, this, TL("Internal attributes"), editorType, GNEAttributesEditorType::AttributeType::BASIC);
    myExtendedAttributesEditor = new GNEAttributesEditorType(frameParent, this, TL("Extended attributes"), editorType, GNEAttributesEditorType::AttributeType::EXTENDED);
    myFlowAttributesEditor = new GNEAttributesEditorType(frameParent, this, TL("Flow attributes"), editorType, GNEAttributesEditorType::AttributeType::FLOW);
    myGeoAttributesEditor = new GNEAttributesEditorType(frameParent, this, TL("Geo attributes"), editorType, GNEAttributesEditorType::AttributeType::GEO);
    myParametersAttributesEditor = new GNEAttributesEditorType(frameParent, this, TL("Parameters"), editorType, GNEAttributesEditorType::AttributeType::PARAMETERS);
    myNeteditAttributesEditor = new GNEAttributesEditorType(frameParent, this, TL("Netedit attributes"), editorType, GNEAttributesEditorType::AttributeType::NETEDIT);
}


void
GNEAttributesEditor::showAttributesEditor(GNEAttributeCarrier* AC, const bool primaryAttributeEditor) {
    myBasicAttributesEditor->showAttributesEditor(AC, primaryAttributeEditor);
    myExtendedAttributesEditor->showAttributesEditor(AC, primaryAttributeEditor);
    myFlowAttributesEditor->showAttributesEditor(AC, primaryAttributeEditor);
    myGeoAttributesEditor->showAttributesEditor(AC, primaryAttributeEditor);
    myParametersAttributesEditor->showAttributesEditor(AC, primaryAttributeEditor);
    myNeteditAttributesEditor->showAttributesEditor(AC, primaryAttributeEditor);
}


void
GNEAttributesEditor::showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs, const bool primaryAttributeEditor) {
    myBasicAttributesEditor->showAttributesEditor(ACs, primaryAttributeEditor);
    myExtendedAttributesEditor->showAttributesEditor(ACs, primaryAttributeEditor);
    myFlowAttributesEditor->showAttributesEditor(ACs, primaryAttributeEditor);
    myGeoAttributesEditor->showAttributesEditor(ACs, primaryAttributeEditor);
    myParametersAttributesEditor->showAttributesEditor(ACs, primaryAttributeEditor);
    myNeteditAttributesEditor->showAttributesEditor(ACs, primaryAttributeEditor);
}


void
GNEAttributesEditor::hideAttributesEditor() {
    myBasicAttributesEditor->hideAttributesEditor();
    myExtendedAttributesEditor->hideAttributesEditor();
    myFlowAttributesEditor->hideAttributesEditor();
    myGeoAttributesEditor->hideAttributesEditor();
    myParametersAttributesEditor->hideAttributesEditor();
    myNeteditAttributesEditor->hideAttributesEditor();
}


void
GNEAttributesEditor::refreshAttributesEditor() {
    myBasicAttributesEditor->refreshAttributesEditor();
    myExtendedAttributesEditor->refreshAttributesEditor();
    myFlowAttributesEditor->refreshAttributesEditor();
    myGeoAttributesEditor->refreshAttributesEditor();
    myParametersAttributesEditor->refreshAttributesEditor();
    myNeteditAttributesEditor->refreshAttributesEditor();
}


void
GNEAttributesEditor::disableAttributesEditor() {
    myBasicAttributesEditor->disableAttributesEditor();
    myExtendedAttributesEditor->disableAttributesEditor();
    myFlowAttributesEditor->disableAttributesEditor();
    myGeoAttributesEditor->disableAttributesEditor();
    myParametersAttributesEditor->disableAttributesEditor();
    myNeteditAttributesEditor->disableAttributesEditor();
}


GNEAttributesEditorType*
GNEAttributesEditor::getNeteditAttributesEditor() const {
    return myNeteditAttributesEditor;
}


bool
GNEAttributesEditor::checkAttributes(const bool showWarning) {
    if (!myBasicAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myExtendedAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myFlowAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myGeoAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myParametersAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myNeteditAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else {
        return true;
    }
}


SumoXMLAttr
GNEAttributesEditor::fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject) const {
    // check if edited AC is a vehicle (needed to avoid empty attributes in SUMOVehicleParser
    bool useSUMOVehicleparser = false;
    if (myBasicAttributesEditor->getEditedAttributeCarriers().size() > 0) {
        if (myBasicAttributesEditor->getEditedAttributeCarriers().front()->getTagProperty()->isVehicle()) {
            useSUMOVehicleparser = true;
        } else if (myBasicAttributesEditor->getEditedAttributeCarriers().front()->getTagProperty()->isPerson()) {
            useSUMOVehicleparser = true;
        } else if (myBasicAttributesEditor->getEditedAttributeCarriers().front()->getTagProperty()->isContainer()) {
            useSUMOVehicleparser = true;
        }
    }
    SumoXMLAttr fillResult = SUMO_ATTR_DEFAULT;
    fillResult = myBasicAttributesEditor->fillSumoBaseObject(baseObject, !useSUMOVehicleparser);
    if (fillResult != SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myExtendedAttributesEditor->fillSumoBaseObject(baseObject, true);
    if (fillResult != SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myFlowAttributesEditor->fillSumoBaseObject(baseObject, true);
    if (fillResult != SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myGeoAttributesEditor->fillSumoBaseObject(baseObject, true);
    if (fillResult != SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myParametersAttributesEditor->fillSumoBaseObject(baseObject, true);
    if (fillResult != SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myNeteditAttributesEditor->fillSumoBaseObject(baseObject, true);
    if (fillResult != SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    return fillResult;
}


bool
GNEAttributesEditor::isReparenting() const {
    return myNeteditAttributesEditor->isReparenting();
}


bool
GNEAttributesEditor::checkNewParent(const GNEAttributeCarrier* AC) const {
    return myNeteditAttributesEditor->checkNewParent(AC);
}


void
GNEAttributesEditor::setNewParent(const GNEAttributeCarrier* AC) {
    myNeteditAttributesEditor->setNewParent(AC);
}


void
GNEAttributesEditor::abortReparenting() {
    myNeteditAttributesEditor->abortReparenting();
}

/****************************************************************************/
