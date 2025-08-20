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
// Table used in GNEElementList, specific for demand elements
/****************************************************************************/

#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEDemandElementList.h"
#include "GNEElementTable.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEDemandElementList::GNEDemandElementList(GNEElementDialog<GNEAdditional>* elementDialogParent, FXVerticalFrame* contentFrame,
                                           SumoXMLTag tag, const bool allowSortElements, const bool allowOpenDialog, const bool fixHeight) :
    GNEElementList(contentFrame, elementDialogParent->getApplicationWindow()->getTagPropertiesDatabase()->getTagProperty(tag, true),
                   allowSortElements, allowOpenDialog, fixHeight),
    myElementDialogParent(elementDialogParent) {
    // fill edited demand elements
    for (const auto& child : elementDialogParent->getElement()->getChildDemandElements()) {
        if (child->getTagProperty()->getTag() == tag) {
            myEditedDemandElements.push_back(child);
        }
    }
    // update table
    updateTable();
}


const std::vector<GNEDemandElement*>&
GNEDemandElementList::getEditedDemands() const {
    return myEditedDemandElements;
}


long
GNEDemandElementList::addDemandElement(GNEDemandElement* demandElement) {
    // insert in list
    myEditedDemandElements.push_back(demandElement);
    // add change command
    demandElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(demandElement, true), true);
    // update table
    return updateTable();
}


long
GNEDemandElementList::updateTable() {
    // first resize table (used if we removed some elements)
    myElementTable->resizeTable(myEditedDemandElements.size());
    // now update all rows
    for (size_t i = 0; i < myEditedDemandElements.size(); i++) {
        myElementTable->updateRow(i, myEditedDemandElements.at(i));
    }
    return 1;
}


long
GNEDemandElementList::sortRows() {
    // declare set for saving elements sorted by first and second attribute
    std::set<std::tuple<std::string, std::string, std::string, GNEDemandElement*> > sortedDemandElements;
    // add all elements
    for (size_t i = 0; i < myEditedDemandElements.size(); i++) {
        if (myElementTable->getNumColumns() == 1) {
            sortedDemandElements.insert(std::make_tuple(myElementTable->getValue(i, 0), "", "", myEditedDemandElements.at(i)));
        } else if (myElementTable->getNumColumns() == 2) {
            sortedDemandElements.insert(std::make_tuple(myElementTable->getValue(i, 0), myElementTable->getValue(i, 1), "", myEditedDemandElements.at(i)));
        } else {
            sortedDemandElements.insert(std::make_tuple(myElementTable->getValue(i, 0), myElementTable->getValue(i, 1), myElementTable->getValue(i, 2), myEditedDemandElements.at(i)));
        }
    }
    // now update edited elements list using map
    myEditedDemandElements.clear();
    for (const auto& element : sortedDemandElements) {
        myEditedDemandElements.push_back(std::get<3>(element));
    }
    // update table
    return updateTable();
}


long
GNEDemandElementList::removeRow(const size_t rowIndex) {
    // remove element from list
    myEditedDemandElements.erase(myEditedDemandElements.begin() + rowIndex);
    // update table
    return updateTable();
}

/****************************************************************************/
