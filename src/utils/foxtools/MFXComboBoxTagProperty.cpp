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
/// @file    MFXComboBoxTagProperty.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// ComboBox icon specific for tag properties
/****************************************************************************/

#include <netedit/GNETagProperties.h>

#include "MFXComboBoxTagProperty.h"

// ===========================================================================
// member method definitions
// ===========================================================================

MFXComboBoxTagProperty::MFXComboBoxTagProperty(FXComposite* p, MFXStaticToolTip* staticToolTip, const bool canSearch, const int visibleItems,
        FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    MFXComboBoxIcon(p, staticToolTip, canSearch, visibleItems, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb) {
}


MFXComboBoxTagProperty::~MFXComboBoxTagProperty() {}


FXint
MFXComboBoxTagProperty::appendTagItem(const GNETagProperties* tagProperties, FXColor bgColor, void* ptr) {
    myTagProperties.push_back(tagProperties);
    return MFXComboBoxIcon::appendIconItem(tagProperties->getSelectorText().c_str(), GUIIconSubSys::getIcon(tagProperties->getGUIIcon()), bgColor, ptr);
}


const GNETagProperties*
MFXComboBoxTagProperty::getTagProperties(FXint index) const {
    return myTagProperties.at(index);
}


const GNETagProperties*
MFXComboBoxTagProperty::getCurrentTagProperty() const {
    const auto currentIndex = MFXComboBoxIcon::getCurrentItem();
    if (currentIndex >= 0) {
        return myTagProperties.at(currentIndex);
    } else {
        return nullptr;
    }
}


bool
MFXComboBoxTagProperty::hasTagProperty(const GNETagProperties* tagProperties) const {
    return std::find(myTagProperties.begin(), myTagProperties.end(), tagProperties) != myTagProperties.end();
}


long
MFXComboBoxTagProperty::setCurrentItem(const GNETagProperties* tagProperties, FXbool notify) {
    for (int i = 0; i < (int)myTagProperties.size(); i++) {
        if (myTagProperties.at(i) == tagProperties) {
            return MFXComboBoxIcon::setCurrentItem(i, notify);
        }
    }
    fxerror("%s::setItem: index out of range.\n", getClassName());
    return 0;
}


void
MFXComboBoxTagProperty::clearItems() {
    MFXComboBoxIcon::clearItems();
    myTagProperties.clear();
}
