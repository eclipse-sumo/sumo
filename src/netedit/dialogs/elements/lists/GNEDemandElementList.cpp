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
GNEDemandElementList::getEditedDemandElements() const {
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
    // declare set for saving elements sorted by sortable attributes (max 6, the rest will be ignored)
    std::set<std::tuple<double, double, double, double, double, double, GNEDemandElement*> > sortedDemandElements;
    // add all elements
    for (size_t i = 0; i < myEditedDemandElements.size(); i++) {
        // create tuple with 6 sortable attributes and the demand element
        auto tuple = std::make_tuple(0, 0, 0, 0, 0, 0, myEditedDemandElements.at(i));
        // update tuple with sortable attributes
        const auto& sortableAttributes = myElementTable->getColumnHeader()->getSortableAttributes();
        // fill tuple
        if (sortableAttributes.size() > 0) {
            std::get<0>(tuple) = GNEAttributeCarrier::parse<double>(myEditedDemandElements.at(i)->getAttribute(sortableAttributes.at(0)));
        }
        if (sortableAttributes.size() > 1) {
            std::get<1>(tuple) = GNEAttributeCarrier::parse<double>(myEditedDemandElements.at(i)->getAttribute(sortableAttributes.at(1)));
        }
        if (sortableAttributes.size() > 2) {
            std::get<2>(tuple) = GNEAttributeCarrier::parse<double>(myEditedDemandElements.at(i)->getAttribute(sortableAttributes.at(2)));
        }
        if (sortableAttributes.size() > 3) {    
            std::get<3>(tuple) = GNEAttributeCarrier::parse<double>(myEditedDemandElements.at(i)->getAttribute(sortableAttributes.at(3)));
        }
        if (sortableAttributes.size() > 4) {
            std::get<4>(tuple) = GNEAttributeCarrier::parse<double>(myEditedDemandElements.at(i)->getAttribute(sortableAttributes.at(4)));
        }
        if (sortableAttributes.size() > 5) {
            std::get<5>(tuple) = GNEAttributeCarrier::parse<double>(myEditedDemandElements.at(i)->getAttribute(sortableAttributes.at(5)));
        }
        sortedDemandElements.insert(tuple);
    }
    // now update edited elements list using map
    myEditedDemandElements.clear();
    for (const auto& element : sortedDemandElements) {
        myEditedDemandElements.push_back(std::get<6>(element));
    }
    // update table
    return updateTable();
}


long
GNEDemandElementList::removeElement(const size_t rowIndex) {
    // add change command
    myEditedDemandElements.at(rowIndex)->getNet()->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(myEditedDemandElements.at(rowIndex), false), true);
    // remove element from list
    myEditedDemandElements.erase(myEditedDemandElements.begin() + rowIndex);
    // update table
    return updateTable();
}

/****************************************************************************/
