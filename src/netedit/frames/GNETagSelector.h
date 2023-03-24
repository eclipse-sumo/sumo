/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNETagSelector : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNETagSelector)

public:
    /// @brief constructor
    GNETagSelector(GNEFrame* frameParent, GNETagProperties::TagType type, SumoXMLTag tag, bool onlyDrawables = true);

    /// @brief destructor
    ~GNETagSelector();

    /// @brief show item selector
    void showTagSelector();

    /// @brief hide item selector
    void hideTagSelector();

    /// @brief get templateAC
    GNEAttributeCarrier* getTemplateAC(SumoXMLTag ACTag) const;

    /// @brief get current templateAC
    GNEAttributeCarrier* getCurrentTemplateAC() const;

    /// @brief set current type manually
    void setCurrentTagType(GNETagProperties::TagType tagType, const bool onlyDrawables, const bool notifyFrameParent = true);

    /// @brief set current type manually
    void setCurrentTag(SumoXMLTag newTag, const bool notifyFrameParent = true);

    /// @brief refresh tagSelector (used when frameParent is show)
    void refreshTagSelector();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user select an elementin ComboBox
    long onCmdSelectTag(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNETagSelector)

private:
    class ACTemplate {

    public:
        /// @brief constructor
        ACTemplate(GNENet* net, const GNETagProperties tagProperty);

        /// @brief destructor
        ~ACTemplate();

        /// @brief get template AC
        GNEAttributeCarrier* getAC() const;

    private:
        /// @brief editedAC
        GNEAttributeCarrier* myAC;

        /// @brief Invalidated copy constructor.
        ACTemplate(const ACTemplate&) = delete;

        /// @brief Invalidated assignment operator
        ACTemplate& operator=(const ACTemplate& src) = delete;
    };

    /// @brief pointer to Frame Parent
    GNEFrame* myFrameParent;

    /// @brief current tagType
    GNETagProperties::TagType myTagType;

    /// @brief comboBox with the tags
    MFXComboBoxIcon* myTagsMatchBox;

    /// @brief current templateAC;
    GNEAttributeCarrier* myCurrentTemplateAC;

    /// @brief list with ACTemplates
    std::vector<ACTemplate*> myACTemplates;
};
