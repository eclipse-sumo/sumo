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

}


void
GNEAttributesEditor::showAttributesEditor(GNEAttributeCarrier* AC) {

}


void
GNEAttributesEditor::showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs) {

}


void
GNEAttributesEditor::hideAttributesEditor() {

}


void
GNEAttributesEditor::refreshAttributesEditor() {

}


void
GNEAttributesEditor::disableAttributesEditor() {

}


bool
GNEAttributesEditor::checkAttributes(const bool showWarning) {

    return true;
}


SumoXMLAttr
GNEAttributesEditor::fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject) const {

    return SUMO_ATTR_NOTHING;
}

/****************************************************************************/
