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
/// @file    MFXComboBoxAttrProperty.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// ComboBox icon specific for attr properties
/****************************************************************************/

#include <netedit/GNEAttributeProperties.h>


#include "MFXComboBoxAttrProperty.h"

// ===========================================================================
// member method definitions
// ===========================================================================

MFXComboBoxAttrProperty::MFXComboBoxAttrProperty(FXComposite* p, FXint cols, const bool canSearch, const int visibleItems,
        FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    MFXComboBoxIcon(p, cols, canSearch, visibleItems, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb) {
}


MFXComboBoxAttrProperty::~MFXComboBoxAttrProperty() {}


FXint
MFXComboBoxAttrProperty::appendAttrItem(const GNEAttributeProperties* attrProperties, FXColor bgColor, void* ptr) {
    myAttrProperties.push_back(attrProperties);
    return MFXComboBoxIcon::appendIconItem(attrProperties->getAttrStr().c_str(), nullptr, bgColor, ptr);
}


const GNEAttributeProperties*
MFXComboBoxAttrProperty::getAttrProperties(FXint index) const {
    return myAttrProperties.at(index);
}


void
MFXComboBoxAttrProperty::clearItems() {
    MFXComboBoxIcon::clearItems();
    myAttrProperties.clear();
}
