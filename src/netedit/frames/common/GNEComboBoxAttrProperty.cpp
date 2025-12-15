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
/// @file    GNEComboBoxAttrProperty.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// ComboBox icon specific for attr properties
/****************************************************************************/

#include <netedit/GNEAttributeProperties.h>

#include "GNEComboBoxAttrProperty.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEComboBoxAttrProperty::GNEComboBoxAttrProperty(FXComposite* p, MFXStaticToolTip* staticToolTip, const bool canSearch, const int visibleItems,
        FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    MFXComboBoxIcon(p, staticToolTip, canSearch, visibleItems, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb) {
}


GNEComboBoxAttrProperty::~GNEComboBoxAttrProperty() {}


FXint
GNEComboBoxAttrProperty::appendAttrItem(const GNEAttributeProperties* attrProperties, FXColor bgColor, void* ptr) {
    myAttrProperties.push_back(attrProperties);
    return MFXComboBoxIcon::appendIconItem(attrProperties->getAttrStr().c_str(), nullptr, bgColor, ptr);
}


const GNEAttributeProperties*
GNEComboBoxAttrProperty::getAttrProperties(FXint index) const {
    return myAttrProperties.at(index);
}


const GNEAttributeProperties*
GNEComboBoxAttrProperty::getCurrentAttrProperty() const {
    const auto currentIndex = MFXComboBoxIcon::getCurrentItem();
    if (currentIndex >= 0) {
        return myAttrProperties.at(currentIndex);
    } else {
        return nullptr;
    }
}


long
GNEComboBoxAttrProperty::setCurrentItem(const GNEAttributeProperties* attributeProperties, FXbool notify) {
    for (int i = 0; i < (int)myAttrProperties.size(); i++) {
        if (myAttrProperties.at(i) == attributeProperties) {
            return MFXComboBoxIcon::setCurrentItem(i, notify);
        }
    }
    fxerror("%s::setItem: index out of range.\n", getClassName());
    return 0;
}


bool
GNEComboBoxAttrProperty::hasAttrProperty(const GNEAttributeProperties* attrProperties) {
    return std::find(myAttrProperties.begin(), myAttrProperties.end(), attrProperties) != myAttrProperties.end();
}


void
GNEComboBoxAttrProperty::clearItems() {
    MFXComboBoxIcon::clearItems();
    myAttrProperties.clear();
}


/****************************************************************************/
