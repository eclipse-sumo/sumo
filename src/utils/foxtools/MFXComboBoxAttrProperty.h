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
// class definitions
// ===========================================================================

class MFXComboBoxAttrProperty : public MFXComboBoxIcon {

public:

    /// @brief Construct a Combo Box widget with room to display cols columns of text
    MFXComboBoxAttrProperty(FXComposite* p, FXint cols, const bool canSearch,
                            const int visibleItems, FXObject* tgt, FXSelector sel = 0, FXuint opts = COMBOBOX_NORMAL,
                            FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                            FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief Destructor
    ~MFXComboBoxAttrProperty();

private:
    /// @brief invalidate copy constructor
    MFXComboBoxAttrProperty(const MFXComboBoxAttrProperty&);

    /// @brief invalidate assignment operator
    MFXComboBoxAttrProperty& operator=(const MFXComboBoxAttrProperty&) = delete;
};
