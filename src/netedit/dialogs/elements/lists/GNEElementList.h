/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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

// ===========================================================================
// class declaration
// ===========================================================================

class GNEDialog;
class GNEElementTable;
class GNETagProperties;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEElementList : public FXVerticalFrame {
    /// @brief FOX-declaration
    FXDECLARE_ABSTRACT(GNEElementList)

public:
    /// @brief enum class for element list options
    enum class Options : int {
        NONE =              1 << 0, // nothing to show
        SORTELEMENTS =      1 << 1, // show button sort elements
        DIALOG_ELEMENT =    1 << 2, // show button for open dialog element
        DIALOG_VCLASS =     1 << 3, // show button for open dialog vClass
        FIXED_HEIGHT =      1 << 4, // fixed height
    };

    /// @brief constructor
    GNEElementList(GNEDialog* parentDialog, FXVerticalFrame* contentFrame, SumoXMLTag tag, GNEElementList::Options options);

    /// @brief destructor
    ~GNEElementList();

    /// @brief enable list
    void enableList();

    /// @brief disable list
    void disableList(const std::string& reason);

    /// @brief check if the current list is valid
    bool isListValid() const;

    /// @brief pointer to dialog parent
    GNEDialog* getDialogParent();

    /// @brief update element list
    virtual long updateList() = 0;

    /// @brief add element
    virtual long addNewElement() = 0;

    /// @brief open dialog
    virtual long sortRows() = 0;

    /// @brief remove element
    virtual long removeElement(const size_t rowIndex) = 0;

    /// @brief open element dialog
    virtual long openElementDialog(const size_t rowIndex) = 0;

    /// @name FOX callbacks
    /// @{

    /// @brief called when user press add button
    long onCmdAddRow(FXObject* sender, FXSelector, void*);

    /// @brief called when user press sort button
    long onCmdSort(FXObject* sender, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEElementList)

    /// @brief pointer to dialog parent
    GNEDialog* myDialogParent = nullptr;

    /// @brief pointer to tag property
    const GNETagProperties* myTagProperty = nullptr;

    /// @brief element table
    GNEElementTable* myElementTable = nullptr;

    /// @brief delete additional element recursively
    void removeElementRecursively(GNEAdditional* additionalElement) const;

    /// @brief delete demand element recursively
    void removeElementRecursively(GNEDemandElement* demandElement) const;

private:
    /// @brief add button
    FXButton* myAddButton = nullptr;

    /// @brief sort button
    FXButton* mySortButton = nullptr;

    /// @brief label
    FXLabel* myLabel = nullptr;

    /// @brief Invalidated copy constructor
    GNEElementList(const GNEElementList&) = delete;

    /// @brief Invalidated assignment operator
    GNEElementList& operator=(const GNEElementList&) = delete;
};

/// @brief override attribute parent bit operator
constexpr GNEElementList::Options operator|(GNEElementList::Options a, GNEElementList::Options b) {
    return static_cast<GNEElementList::Options>(static_cast<int>(a) | static_cast<int>(b));
}

/// @brief override attribute parent bit operator
constexpr bool operator&(GNEElementList::Options a, GNEElementList::Options b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}
