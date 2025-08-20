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
    // declare set for saving elements sorted by sortable attributes (max 6, the rest will be ignored)
    std::set<std::tuple<double, double, double, double, double, double, GNEAdditional*> > sortedAdditionalElements;
    // add all elements
    for (size_t i = 0; i < myEditedAdditionalElements.size(); i++) {
        // create tuple with 6 sortable attributes and the additional element
        auto tuple = std::make_tuple(0, 0, 0, 0, 0, 0, myEditedAdditionalElements.at(i));
        // update tuple with sortable attributes
        const auto& sortableAttributes = myElementTable->getColumnHeader()->getSortableAttributes();
        // fill tuple
        if (sortableAttributes.size() > 0) {
            std::get<0>(tuple) = GNEAttributeCarrier::parse<double>(myEditedAdditionalElements.at(i)->getAttribute(sortableAttributes.at(0)));
        }
        if (sortableAttributes.size() > 1) {
            std::get<1>(tuple) = GNEAttributeCarrier::parse<double>(myEditedAdditionalElements.at(i)->getAttribute(sortableAttributes.at(1)));
        }
        if (sortableAttributes.size() > 2) {
            std::get<2>(tuple) = GNEAttributeCarrier::parse<double>(myEditedAdditionalElements.at(i)->getAttribute(sortableAttributes.at(2)));
        }
        if (sortableAttributes.size() > 3) {    
            std::get<3>(tuple) = GNEAttributeCarrier::parse<double>(myEditedAdditionalElements.at(i)->getAttribute(sortableAttributes.at(3)));
        }
        if (sortableAttributes.size() > 4) {
            std::get<4>(tuple) = GNEAttributeCarrier::parse<double>(myEditedAdditionalElements.at(i)->getAttribute(sortableAttributes.at(4)));
        }
        if (sortableAttributes.size() > 5) {
            std::get<5>(tuple) = GNEAttributeCarrier::parse<double>(myEditedAdditionalElements.at(i)->getAttribute(sortableAttributes.at(5)));
        }
        sortedAdditionalElements.insert(tuple);
    }
    // now update edited elements list using map
    myEditedAdditionalElements.clear();
    for (const auto& element : sortedAdditionalElements) {
        myEditedAdditionalElements.push_back(std::get<6>(element));
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
