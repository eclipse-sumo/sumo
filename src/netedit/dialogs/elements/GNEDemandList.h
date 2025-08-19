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
// Table used in GNEElementList
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <utils/tests/InternalTestStep.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEDialog;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief edit table
template <typename U, typename V>
class ElementList : protected FXVerticalFrame {

public:
    /// @brief constructor
    ElementList(GNEElementDialog<T>* elementDialogParent, FXVerticalFrame* contentFrame, SumoXMLTag tag,
                const GNEHierarchicalContainerChildren<U*>& elementChildren, const bool fixHeight) :
        FXVerticalFrame(contentFrame, GUIDesignAuxiliarVerticalFrame) {
        // get tag property
        const auto* tagPropertiesDatabase = elementDialogParent->getElement()->getNet()->getViewNet()->getNet()->getTagPropertiesDatabase();
        const auto* tagProperty = tagPropertiesDatabase->getTagProperty(tag, true);
        // fill editable attributes
        for (const auto& attrProperty : tagProperty->getAttributeProperties()) {
            if (attrProperty->isDialogEditor()) {
                myAttrProperties.push_back(attrProperty);
            }
        }
        // horizontal frame for buttons
        FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
        // create buttons and labels
        myAddButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::ADD),
                                                elementDialogParent, MID_GNE_ELEMENTLIST_ADD, GUIDesignButtonIcon);
        mySortButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::RELOAD),
                       elementDialogParent, MID_GNE_ELEMENTLIST_SORT, GUIDesignButtonIcon);
        myLabel = new FXLabel(buttonFrame, TLF("%s", tagProperty->getTagStr()).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
        // create element table
        myElementTable = new GNEElementTable(this, elementDialogParent, tagProperty, fixHeight);
        // fill edited elements
        for (const auto& child : elementChildren) {
            if (child->getTagProperty()->getTag() == tag) {
                myEditedElements.push_back(child);
            }
        }
        // update table
        updateTable();
    }

    /// @brief get elements
    const std::vector<U*>& getEditedElements() const {
        return myEditedElements;
    }

    /// @brief check if the given object is a button
    bool checkButtons(const FXObject* obj) const {
        if (obj == myAddButton) {
            return true;
        } else if (obj == mySortButton) {
            return true;
        } else {
            return false;
        }
    }

    /// @brief add element
    long addElement(U* element) {
        // insert in list
        myEditedElements.push_back(element);
        // add change command
        element->getNet()->getViewNet()->getUndoList()->add(new V(element, true), true);
        // update table
        updateTable();
        return 1;
    }

    /// @brief sort elements
    long sortElements() {
        // declare set for saving elements sorted by first and second attribute
        std::set<std::tuple<std::string, std::string, std::string, U*> > sortedElements;
        // add all elements
        for (size_t i = 0; i < myEditedElements.size(); i++) {
            if (myElementTable->getNumColumns() == 1) {
                sortedElements.insert(std::make_tuple(myElementTable->getValue(i, 0), "", "", myEditedElements.at(i)));
            } else if (myElementTable->getNumColumns() == 2) {
                sortedElements.insert(std::make_tuple(myElementTable->getValue(i, 0), myElementTable->getValue(i, 1), "", myEditedElements.at(i)));
            } else {
                sortedElements.insert(std::make_tuple(myElementTable->getValue(i, 0), myElementTable->getValue(i, 1), myElementTable->getValue(i, 2), myEditedElements.at(i)));
            }
        }
        // now update edited elements list using map
        myEditedElements.clear();
        for (const auto& element : sortedElements) {
            myEditedElements.push_back(std::get<3>(element));
        }
        // update table
        updateTable();
        return 1;
    }

    /// @brief disable list
    void disableList(const std::string& reason) {
        myElementTable->disableTable();
        myAddButton->disable();
        myLabel->disable();
        myLabel->setText(reason.c_str());
    }

    /// @brief check if the current table is valid
    bool isValid() const {
        return myElementTable->isValid();
    }

    /// @brief update table
    void updateTable() {
        // first resize table (used if we removed some elements)
        myElementTable->resizeTable(myEditedElements.size());
        // now update all rows
        for (size_t i = 0; i < myEditedElements.size(); i++) {
            myElementTable->updateRow(i, myEditedElements.at(i));
        }
    }

protected:
    /// @brief list of edited attrs
    std::vector<const GNEAttributeProperties*> myAttrProperties;

    /// @brief edited elements
    std::vector<U*> myEditedElements;

    /// @brief add button
    FXButton* myAddButton = nullptr;

    /// @brief sort button
    FXButton* mySortButton = nullptr;

    /// @brief label
    FXLabel* myLabel = nullptr;

    /// @brief element table
    GNEElementTable* myElementTable = nullptr;

private:
    /// @brief Invalidated copy constructor
    ElementList(const ElementList&) = delete;

    /// @brief Invalidated assignment operator
    ElementList& operator=(const ElementList&) = delete;
};
