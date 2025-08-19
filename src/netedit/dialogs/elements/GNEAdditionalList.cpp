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
/// @file    GNEElementTable.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Table used in GNEElementList, specific for additional elements
/****************************************************************************/

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/foxtools/MFXMenuButtonTooltip.h>
#include <utils/foxtools/MFXTextFieldTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEAdditionalList.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalList::GNEAdditionalList(GNEElementDialog<GNEAdditional>* elementDialogParent,
                                     FXVerticalFrame* contentFrame, SumoXMLTag tag, const bool fixHeight) :
    GNEElementList(this, elementDialogParent->getApplicationWindow()->getTagPropertiesDatabase()->getTagProperty(tag, true), fixHeight) {
    // fill edited additional elements
    for (const auto& child : elementDialogParent->getElement()->getChildAdditionals()) {
        if (child->getTagProperty()->getTag() == tag) {
            myEditedAdditionalElements.push_back(child);
        }
    }
    // update table
    updateTable();
}


const std::vector<GNEAdditional*>&
GNEAdditionalList::getEditedAdditionals() const {
    return myEditedAdditionalElements;
}


long
GNEAdditionalList::addAdditionalElement(GNEAdditional* additionalElement) {
    // insert in list
    myEditedAdditionalElements.push_back(additionalElement);
    // add change command
    additionalElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(additionalElement, true), true);
    // update table
    return updateTable();
}


long
GNEAdditionalList::updateTable() {
    // first resize table (used if we removed some elements)
    myElementTable->resizeTable(myEditedAdditionalElements.size());
    // now update all rows
    for (size_t i = 0; i < myEditedAdditionalElements.size(); i++) {
        myElementTable->updateRow(i, myEditedAdditionalElements.at(i));
    }
    return 1;
}


long
GNEAdditionalList::sortRows() {
    // declare set for saving elements sorted by first and second attribute
    std::set<std::tuple<std::string, std::string, std::string, GNEAdditional*> > sortedAdditionalElements;
    // add all elements
    for (size_t i = 0; i < myEditedAdditionalElements.size(); i++) {
        if (myElementTable->getNumColumns() == 1) {
            sortedAdditionalElements.insert(std::make_tuple(myElementTable->getValue(i, 0), "", "", myEditedAdditionalElements.at(i)));
        } else if (myElementTable->getNumColumns() == 2) {
            sortedAdditionalElements.insert(std::make_tuple(myElementTable->getValue(i, 0), myElementTable->getValue(i, 1), "", myEditedAdditionalElements.at(i)));
        } else {
            sortedAdditionalElements.insert(std::make_tuple(myElementTable->getValue(i, 0), myElementTable->getValue(i, 1), myElementTable->getValue(i, 2), myEditedAdditionalElements.at(i)));
        }
    }
    // now update edited elements list using map
    myEditedAdditionalElements.clear();
    for (const auto& element : sortedAdditionalElements) {
        myEditedAdditionalElements.push_back(std::get<3>(element));
    }
    // update table
    return updateTable();
}


long
GNEAdditionalList::removeRow(const size_t rowIndex) {
    // remove element from list
    myEditedAdditionalElements.erase(myEditedAdditionalElements.begin() + rowIndex);
    // update table
    return updateTable();
}

/****************************************************************************/
