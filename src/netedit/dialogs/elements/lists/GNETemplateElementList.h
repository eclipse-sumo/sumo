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
/// @file    GNEElementList.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Template used for Element lists
/****************************************************************************/
#pragma once

#include <netedit/dialogs/elements/GNETemplateElementDialog.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEElementTable.h"
#include "GNEElementList.h"

// ===========================================================================
// class definitions
// ===========================================================================

template <typename elementDialogType, typename elementType, typename GNEChange_Type>
class GNETemplateElementList : public GNEElementList {

public:
    /// @brief constructor
    GNETemplateElementList(GNETemplateElementDialog<elementDialogType>* elementDialogParent,
                           FXVerticalFrame* contentFrame, SumoXMLTag tag, const int options) :
        GNEElementList(contentFrame, elementDialogParent->getApplicationWindow()->getTagPropertiesDatabase()->getTagProperty(tag, true), options),
        myElementDialogParent(elementDialogParent) {
        // update table
        updateList();
    }

    /// @brief get edited elements
    const std::vector<elementType*>& getEditedElements() const {
        return myEditedElements;
    }

    /// @brief insert element
    long insertElement(elementType* element) {
        // add change command
        element->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Type(element, true), true);
        // update table
        return updateList();
    }

    /// @brief update element list
    long updateList() {
        // reset edited element
        myEditedElements.clear();
        for (const auto& child : myElementDialogParent->getElement()->getChildren().template get<elementType*>()) {
            if (child->getTagProperty()->getTag() == myTagProperty->getTag()) {
                myEditedElements.push_back(child);
            }
        }
        // first resize table (used if we removed some elements)
        myElementTable->resizeTable(myEditedElements.size());
        // now update all rows
        for (size_t i = 0; i < myEditedElements.size(); i++) {
            myElementTable->updateRow(i, myEditedElements.at(i));
        }
        return 1;
    }

    /// @brief check if the elements are sorted
    bool checkSort() const {
        // get sort tuples
        const auto tuples = getSortTuples(false);
        // check if the elements are sorted
        for (int i = 0; i < ((int)tuples.size() - 1); i++) {
            if (tuples.at(i) > tuples.at(i)) {
                return false;
            }
        }
        return true;
    }

    /// @brief open dialog
    long sortRows() {
        // get sort tuples sorted
        const auto sortedTuples = getSortTuples(true);
        // now update edited elements list using sortTuples
        myEditedElements.clear();
        for (const auto& element : sortedTuples) {
            myEditedElements.push_back(std::get<6>(element));
        }
        // update table
        return updateList();
    }

    /// @brief remove element (using index)
    long removeElement(const size_t rowIndex) {
        // delete element recursively
        removeElementRecursively(myEditedElements.at(rowIndex));
        // update table
        return updateList();
    }

    /// @brief remove element
    long removeElement(const elementType* element) {
        // search index
        for (size_t rowIndex = 0; rowIndex < myEditedElements.size(); rowIndex++) {
            if (myEditedElements.at(rowIndex) == element) {
                return removeElement(rowIndex);
            }
        }
        // if we reach this point, the element was not found
        throw ProcessError("Element not found in removeElement");
    }

    /// @brief add new element (must be implemented in children)
    virtual long addNewElement() = 0;

    /// @brief open element dialog (optional
    virtual long openElementDialog(const size_t rowIndex) = 0;

protected:
    /// @brief element dialog parent
    GNETemplateElementDialog<elementDialogType>* myElementDialogParent = nullptr;

    /// @brief edited elements
    std::vector<elementType*> myEditedElements;

private:
    /// @brief typedef used for sorting elements by attributes
    typedef std::tuple<double, double, double, double, double, double, elementType*> SortTuple;

    /// @brief get element sorted
    std::vector<SortTuple> getSortTuples(const bool sort) const {
        // declare sorted element set
        std::vector<SortTuple> elementSortKeyVector;
        // add all elements
        for (size_t i = 0; i < myEditedElements.size(); i++) {
            // create tuple with max 6 sortable attributes
            auto tuple = std::make_tuple(0, 0, 0, 0, 0, 0, myEditedElements.at(i));
            // update tuple with sortable attributes
            const auto& sortableAttributes = myElementTable->getColumnHeader()->getSortableAttributes();
            // fill tuple
            if (sortableAttributes.size() > 0) {
                std::get<0>(tuple) = GNEAttributeCarrier::parse<double>(myEditedElements.at(i)->getAttribute(sortableAttributes.at(0)));
            }
            if (sortableAttributes.size() > 1) {
                std::get<1>(tuple) = GNEAttributeCarrier::parse<double>(myEditedElements.at(i)->getAttribute(sortableAttributes.at(1)));
            }
            if (sortableAttributes.size() > 2) {
                std::get<2>(tuple) = GNEAttributeCarrier::parse<double>(myEditedElements.at(i)->getAttribute(sortableAttributes.at(2)));
            }
            if (sortableAttributes.size() > 3) {
                std::get<3>(tuple) = GNEAttributeCarrier::parse<double>(myEditedElements.at(i)->getAttribute(sortableAttributes.at(3)));
            }
            if (sortableAttributes.size() > 4) {
                std::get<4>(tuple) = GNEAttributeCarrier::parse<double>(myEditedElements.at(i)->getAttribute(sortableAttributes.at(4)));
            }
            if (sortableAttributes.size() > 5) {
                std::get<5>(tuple) = GNEAttributeCarrier::parse<double>(myEditedElements.at(i)->getAttribute(sortableAttributes.at(5)));
            }
            elementSortKeyVector.push_back(tuple);
        }
        // check if sort tuples
        if (sort) {
            std::set<SortTuple> elementSortKeySet;
            for (const auto& element : elementSortKeyVector) {
                // insert tuple into set
                elementSortKeySet.insert(element);
            }
            // clear vector and copy set into vector
            elementSortKeyVector.clear();
            for (const auto& element : elementSortKeySet) {
                elementSortKeyVector.push_back(element);
            }
        }
        return elementSortKeyVector;
    }

    /// @brief Invalidated copy constructor
    GNETemplateElementList(const GNETemplateElementList&) = delete;

    /// @brief Invalidated assignment operator
    GNETemplateElementList& operator=(const GNETemplateElementList&) = delete;
};
