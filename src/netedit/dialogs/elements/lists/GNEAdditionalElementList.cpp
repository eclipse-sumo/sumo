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
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEAdditionalElementList.h"
#include "GNEElementTable.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalElementList::GNEAdditionalElementList(GNEElementDialog<GNEAdditional>* elementDialogParent, FXVerticalFrame* contentFrame,
                                                   SumoXMLTag tag, const bool allowSortElements, const bool allowOpenDialog, const bool fixHeight) :
    GNEElementList(contentFrame, elementDialogParent->getApplicationWindow()->getTagPropertiesDatabase()->getTagProperty(tag, true),
                   allowSortElements, allowOpenDialog, fixHeight),
    myElementDialogParent(elementDialogParent) {
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
GNEAdditionalElementList::getEditedAdditionalElements() const {
    return myEditedAdditionalElements;
}


long
GNEAdditionalElementList::addAdditionalElement(GNEAdditional* additionalElement) {
    // insert in list
    myEditedAdditionalElements.push_back(additionalElement);
    // add change command
    additionalElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(additionalElement, true), true);
    // update table
    return updateTable();
}


long
GNEAdditionalElementList::updateTable() {
    // first resize table (used if we removed some elements)
    myElementTable->resizeTable(myEditedAdditionalElements.size());
    // now update all rows
    for (size_t i = 0; i < myEditedAdditionalElements.size(); i++) {
        myElementTable->updateRow(i, myEditedAdditionalElements.at(i));
    }
    return 1;
}


long
GNEAdditionalElementList::sortRows() {
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
GNEAdditionalElementList::removeElement(const size_t rowIndex) {
    // delete element recursively
    deleteAdditionalElementRecursively(myEditedAdditionalElements.at(rowIndex));
    // remove element from list
    myEditedAdditionalElements.erase(myEditedAdditionalElements.begin() + rowIndex);
    // update table
    return updateTable();
}

/****************************************************************************/
