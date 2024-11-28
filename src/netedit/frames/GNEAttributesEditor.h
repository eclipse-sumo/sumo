/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @date    Nov 2024
///
// Table used for pack GNEAttributeRows
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNEAttributeCarrier;
class GNEAttributesEditorRow;

// ===========================================================================
// class GNEAttributesEditor
// ===========================================================================

class GNEAttributesEditor : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEAttributesEditor)

    /// @brief declare friend class
    friend class GNEAttributesEditorRow;

public:

    /// @brief Options for filter attributes
    enum EditorOptions {
        EXTENDED_ATTRIBUTES = 1 << 0,
        FLOW_ATTRIBUTES     = 1 << 1,
        GEO_ATTRIBUTES      = 1 << 2,
    };

    /// @brief constructor
    GNEAttributesEditor(GNEFrame* frameParent, const int editorOptions);

    /// @brief edit attributes of the given AC (usually the edited template AC)
    void showAttributeTableModule(GNEAttributeCarrier* AC);

    /// @brief edit attributes of the given hash of ACs (usually the inspected ACs)
    void showAttributeTableModule(const std::unordered_set<GNEAttributeCarrier*>& ACs);

    /// @brief hide attribute editor
    void hideAttributeTableModule();

    /// @brief refresh attribute editor
    void refreshAttributeTable();

    /// @brief pointer to GNEFrame parent
    GNEFrame* getFrameParent() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when user press the help button
    long onCmdAttributeTableHelp(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief fox need this
    FOX_CONSTRUCTOR(GNEAttributesEditor)

    /// @brief set attribute in the current ACs (Callend from row)
    void setAttribute(SumoXMLAttr attr, const std::string& value);

    /// @brief set attribute in the current ACs (Callend from row)
    void toggleEnableAttribute(SumoXMLAttr attr, const bool value);

    /// @brief inspect parent
    void inspectParent();

    /// @brief move lane up
    void moveLaneUp();

    /// @brief move lane down
    void moveLaneDown();

private:
    /// @brief pointer to GNEFrame parent
    GNEFrame* myFrameParent;

    /// @brief current edited ACs
    std::vector<GNEAttributeCarrier*> myEditedACs;

    /// @brief list of attributes editor rows
    std::vector<GNEAttributesEditorRow*> myAttributesEditorRows;

    /// @brief button for help
    FXButton* myHelpButton = nullptr;

    /// @brief variable use for packing attribute editor options
    int myEditorOptions = 0;
};
