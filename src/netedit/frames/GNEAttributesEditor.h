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
/// @file    GNEAttributesEditor.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2025
///
// pack of all GNEAttributesEditorTypes
/****************************************************************************/
#pragma once
#include <config.h>

# include "GNEAttributesEditorType.h"

// ===========================================================================
// class GNEAttributesEditor
// ===========================================================================

class GNEAttributesEditor {

public:
    /// @brief constructor
    GNEAttributesEditor(GNEFrame* frameParent, GNEAttributesEditorType::EditorType editorType);

    /// @brief edit attributes of the given AC (usually the edited template AC)
    void showAttributesEditor(GNEAttributeCarrier* AC, const bool primaryAttributeEditor);

    /// @brief edit attributes of the given hash of ACs (usually the inspected ACs)
    void showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs, const bool primaryAttributeEditor);

    /// @brief hide attribute editor
    void hideAttributesEditor();

    /// @brief refresh attribute editor
    void refreshAttributesEditor();

    /// @brief disable attribute editor
    void disableAttributesEditor();

    /// @brief get netedit attributes editor
    GNEAttributesEditorType* getNeteditAttributesEditor() const;

    /// @brief check if current edited attributes are valid
    bool checkAttributes(const bool showWarning);

    /// @brief fill sumo Base object
    SumoXMLAttr fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject) const;

    /// @name Functions related with selecting parents (only affect the Netedit Attributes editor)
    /// @{
    /// @brief check if we're selecting a parent clicking over view
    bool isReparenting() const;

    /// @brief check if the given AC can be a new parent
    bool checkNewParent(const GNEAttributeCarrier* AC) const;

    /// @brief set new parent
    void setNewParent(const GNEAttributeCarrier* AC);

    /// @brief abort selecting parent
    void abortReparenting();

    /// @}

protected:
    /// @brief basic attributes editor
    GNEAttributesEditorType* myBasicAttributesEditor = nullptr;

    /// @brief extended attributes editor
    GNEAttributesEditorType* myExtendedAttributesEditor = nullptr;

    /// @brief flow attributes editor
    GNEAttributesEditorType* myFlowAttributesEditor = nullptr;

    /// @brief geo attributes editor
    GNEAttributesEditorType* myGeoAttributesEditor = nullptr;

    /// @brief parameteres attributes editor
    GNEAttributesEditorType* myParametersAttributesEditor = nullptr;

    /// @brief netedit attributes editor
    GNEAttributesEditorType* myNeteditAttributesEditor = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEAttributesEditor(GNEAttributesEditor*) = delete;

    /// @brief Invalidated assignment operator.
    GNEAttributesEditor& operator=(GNEAttributesEditor*) = delete;
};
