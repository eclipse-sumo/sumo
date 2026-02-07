/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNETagSelector.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for select tags
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNETagProperties.h>
#include <netedit/frames/common/GNEGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNETagSelector : public GNEGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNETagSelector)

public:
    /// @brief constructor
    GNETagSelector(GNEFrame* frameParent, const GNETagProperties::Type type, const SumoXMLTag tag);

    /// @brief destructor
    ~GNETagSelector();

    /// @brief show item selector
    void showTagSelector();

    /// @brief hide item selector
    void hideTagSelector();

    /// @brief get current templateAC
    GNEAttributeCarrier* getCurrentTemplateAC() const;

    /// @brief update tag types to select
    void updateTagTypes(const GNETagProperties::Type type, const SumoXMLTag tag, const bool informParent);

    /// @brief set current type manually
    void setCurrentTag(SumoXMLTag newTag, const bool informParent = true);

    /// @brief refresh tagSelector (used when frameParent is show)
    void refreshTagSelector();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user select an elementin ComboBox
    long onCmdSelectTag(FXObject*, FXSelector, void* ptr);
    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNETagSelector)

private:
    /// @brief pointer to Frame Parent
    GNEFrame* myFrameParent;

    /// @brief comboBox with the tags
    MFXComboBoxIcon* myTagsMatchBox;

    /// @brief current templateAC;
    GNEAttributeCarrier* myCurrentTemplateAC;
};
