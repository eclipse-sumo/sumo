/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    MFXComboBoxAttrProperty.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// ComboBox icon specific for attr properties
/****************************************************************************/
#pragma once
#include <config.h>

#include "MFXComboBoxIcon.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeProperties;

// ===========================================================================
// class definitions
// ===========================================================================

class MFXComboBoxAttrProperty : public MFXComboBoxIcon {

public:
    /// @brief Construct a Combo Box widget with room to display cols columns of text
    MFXComboBoxAttrProperty(FXComposite* p, const bool canSearch, const int visibleItems, FXObject* tgt, FXSelector sel, FXuint opts,
                            FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                            FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief Destructor
    ~MFXComboBoxAttrProperty();

    /// @brief append item
    FXint appendAttrItem(const GNEAttributeProperties* attrProperties, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr);

    /// @brief get attribute properties
    const GNEAttributeProperties* getAttrProperties(FXint index) const;

    /// @brief get current attribute property
    const GNEAttributeProperties* getCurrentAttrProperty() const;

    /// @brief Set the current item
    long setCurrentItem(const GNEAttributeProperties* attrProperties, FXbool notify = FALSE);

    /// @brief check if the given attribute exist in comboBox
    bool hasAttrProperty(const GNEAttributeProperties* attrProperties);

    /// @brief Remove all items from the list
    void clearItems();

private:
    /// @brief vector with tag properties
    std::vector<const GNEAttributeProperties*> myAttrProperties;

    /// @brief delete original replace the item at index
    FXint updateIconItem(FXint index, const FXString& text, FXIcon* icon = nullptr, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr) = delete;

    /// @brief delete original insert icon item in the given position
    FXint insertIconItem(FXint index, const FXString& text, FXIcon* icon = nullptr, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr) = delete;

    /// @brief delete original append icon item in the last position
    FXint appendIconItem(const FXString& text, FXIcon* icon = nullptr, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr) = delete;

    /// @brief invalidate copy constructor
    MFXComboBoxAttrProperty(const MFXComboBoxAttrProperty&) = delete;

    /// @brief invalidate assignment operator
    MFXComboBoxAttrProperty& operator=(const MFXComboBoxAttrProperty&) = delete;
};
