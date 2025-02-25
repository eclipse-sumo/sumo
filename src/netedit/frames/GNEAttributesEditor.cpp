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

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditor::GNEAttributesEditor(GNEFrame* frameParent, GNEAttributesEditorType::EditorType editorType) {
    myBasicAttributesEditor = new GNEAttributesEditorType(frameParent, TL("Internal plan attributes"), editorType, GNEAttributesEditorType::AttributeType::CHILD);
    myChildAttributesEditor = new GNEAttributesEditorType(frameParent, TL("Internal plan attributes"), editorType, GNEAttributesEditorType::AttributeType::CHILD);
    myExtendedAttributesEditor = new GNEAttributesEditorType(frameParent, TL("Internal plan attributes"), editorType, GNEAttributesEditorType::AttributeType::CHILD);
    myFlowAttributesEditor = new GNEAttributesEditorType(frameParent, TL("Internal plan attributes"), editorType, GNEAttributesEditorType::AttributeType::CHILD);
    myGeoAttributesEditor = new GNEAttributesEditorType(frameParent, TL("Internal plan attributes"), editorType, GNEAttributesEditorType::AttributeType::CHILD);
    myParametersAttributesEditor = new GNEAttributesEditorType(frameParent, TL("Internal plan attributes"), editorType, GNEAttributesEditorType::AttributeType::CHILD);
}


void
GNEAttributesEditor::showAttributesEditor(GNEAttributeCarrier* AC) {
    myBasicAttributesEditor->showAttributesEditor(AC);
    myChildAttributesEditor->showAttributesEditor(AC);
    myExtendedAttributesEditor->showAttributesEditor(AC);
    myFlowAttributesEditor->showAttributesEditor(AC);
    myGeoAttributesEditor->showAttributesEditor(AC);
    myParametersAttributesEditor->showAttributesEditor(AC);
}


void
GNEAttributesEditor::showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs) {
    myBasicAttributesEditor->showAttributesEditor(ACs);
    myChildAttributesEditor->showAttributesEditor(ACs);
    myExtendedAttributesEditor->showAttributesEditor(ACs);
    myFlowAttributesEditor->showAttributesEditor(ACs);
    myGeoAttributesEditor->showAttributesEditor(ACs);
    myParametersAttributesEditor->showAttributesEditor(ACs);
}


void
GNEAttributesEditor::hideAttributesEditor() {
    myBasicAttributesEditor->hideAttributesEditor();
    myChildAttributesEditor->hideAttributesEditor();
    myExtendedAttributesEditor->hideAttributesEditor();
    myFlowAttributesEditor->hideAttributesEditor();
    myGeoAttributesEditor->hideAttributesEditor();
    myParametersAttributesEditor->hideAttributesEditor();
}


void
GNEAttributesEditor::refreshAttributesEditor() {
    myBasicAttributesEditor->refreshAttributesEditor();
    myChildAttributesEditor->refreshAttributesEditor();
    myExtendedAttributesEditor->refreshAttributesEditor();
    myFlowAttributesEditor->refreshAttributesEditor();
    myGeoAttributesEditor->refreshAttributesEditor();
    myParametersAttributesEditor->refreshAttributesEditor();
}


void
GNEAttributesEditor::disableAttributesEditor() {
    myBasicAttributesEditor->disableAttributesEditor();
    myChildAttributesEditor->disableAttributesEditor();
    myExtendedAttributesEditor->disableAttributesEditor();
    myFlowAttributesEditor->disableAttributesEditor();
    myGeoAttributesEditor->disableAttributesEditor();
    myParametersAttributesEditor->disableAttributesEditor();
}


bool
GNEAttributesEditor::checkAttributes(const bool showWarning) {
    if (!myBasicAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myChildAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myExtendedAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myFlowAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myGeoAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else if (!myParametersAttributesEditor->checkAttributes(showWarning)) {
        return false;
    } else {
        return true;
    }
}


SumoXMLAttr
GNEAttributesEditor::fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject) const {
    SumoXMLAttr fillResult = SUMO_ATTR_DEFAULT;
    fillResult = myBasicAttributesEditor->fillSumoBaseObject(baseObject);
    if (fillResult == SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myChildAttributesEditor->fillSumoBaseObject(baseObject);
    if (fillResult == SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myExtendedAttributesEditor->fillSumoBaseObject(baseObject);
    if (fillResult == SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myFlowAttributesEditor->fillSumoBaseObject(baseObject);
    if (fillResult == SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myGeoAttributesEditor->fillSumoBaseObject(baseObject);
    if (fillResult == SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    fillResult = myParametersAttributesEditor->fillSumoBaseObject(baseObject);
    if (fillResult == SUMO_ATTR_NOTHING) {
        return fillResult;
    }
    return fillResult;
}

/****************************************************************************/
