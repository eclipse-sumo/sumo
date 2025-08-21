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

#include <netedit/dialogs/elements/GNEElementDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEAdditionalElementList.h"
#include "GNEElementTable.h"
#include "GNEElementList.h"


// ===========================================================================
// class definitions
// ===========================================================================

template <typename T, typename U>
class GNEAdditionalElementList : public GNEElementList {

public:
    /// @brief constructor
    GNEAdditionalElementList(GNEElementDialog<T>* elementDialogParent,
                             FXVerticalFrame* contentFrame, SumoXMLTag tag,
                             const bool allowSortElements, const bool allowOpenDialog,
                             const bool fixHeight) :
        GNEElementList(contentFrame, elementDialogParent->getApplicationWindow()->getTagPropertiesDatabase()->getTagProperty(tag, true),
                       allowSortElements, allowOpenDialog, fixHeight),
        myElementDialogParent(elementDialogParent) {
        // fill edited elements
        for (const auto& child : elementDialogParent->getElement()->getChildAdditionals()) {
            if (child->getTagProperty()->getTag() == tag) {
                myEditedElements.push_back(child);
            }
        }
        // update table
        updateTable();
    }

    /// @brief get edited elements
    const std::vector<T*>& getEditedElements() const {
        return myEditedElements;
    }

    /// @brief insert element
    long insertElement(T* element) {
        // insert in list
        myEditedElements.push_back(element);
        // add change command
        element->getNet()->getViewNet()->getUndoList()->add(new U(element, true), true);
        // update table
        return updateTable();
    }

    /// @brief update table
    long updateTable() {
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
        if (myEditedElements.size() <= 1) {
            return true;
        } else {
            // declare vector for check sorting
            std::vector<std::tuple<double, double, double, double, double, double> > sortedElements;
            // add all elements
            for (size_t i = 0; i < myEditedElements.size(); i++) {
                // create tuple with 6 sortable attributes and the element
                auto tuple = std::make_tuple(0, 0, 0, 0, 0, 0);
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
                sortedElements.push_back(tuple);
            }
            for (int i = 0; i < ((int)sortedElements.size() - 1); i++) {
                if (myEditedElements.at(i) > myEditedElements.at(i)) {
                    return false;
                }
            }
            return true;
        }
    }

    /// @brief open dialog
    long sortRows() {
        // declare set for saving elements sorted by sortable attributes (max 6, the rest will be ignored)
        std::set<std::tuple<double, double, double, double, double, double, T*> > sortedElements;
        // add all elements
        for (size_t i = 0; i < myEditedElements.size(); i++) {
            // create tuple with 6 sortable attributes and the element
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
            sortedElements.insert(tuple);
        }
        // now update edited elements list using map
        myEditedElements.clear();
        for (const auto& element : sortedElements) {
            myEditedElements.push_back(std::get<6>(element));
        }
        // update table
        return updateTable();
    }

    /// @brief remove element (using index)
    long removeElement(const size_t rowIndex) {
        // delete element recursively
        deleteAdditionalElementRecursively(myEditedElements.at(rowIndex));
        // remove element from list
        myEditedElements.erase(myEditedElements.begin() + rowIndex);
        // update table
        return updateTable();
    }

    /// @brief remove element
    long removeElement(const T* element) {
        // search index
        for (size_t rowIndex = 0; rowIndex < myEditedElements.size(); rowIndex++) {
            if (myEditedElements.at(rowIndex) == element) {
                return removeElement(rowIndex);
            }
        }
        // if we reach this point, the element was not found
        throw ProcessError("Element not found in removeElement");
    }

    /// @brief add element
    virtual long addElement() = 0;

    /// @brief open dialog
    virtual long openDialog(const size_t rowIndex) = 0;

protected:
    /// @brief element dialog parent
    GNEElementDialog<T>* myElementDialogParent = nullptr;

private:
    /// @brief edited elements
    std::vector<T*> myEditedElements;

    /// @brief Invalidated copy constructor
    GNEAdditionalElementList(const GNEAdditionalElementList&) = delete;

    /// @brief Invalidated assignment operator
    GNEAdditionalElementList& operator=(const GNEAdditionalElementList&) = delete;
};
