/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNENetworkSelector.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// NetworkElement selector module
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNENetworkElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNENetworkSelector : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNENetworkSelector)

public:
    /// @brief network element selector type
    enum class Type {
        EDGE,
        LANE,
    };

    /// @brief constructor
    GNENetworkSelector(GNEFrame* frameParent, const Type networkElementType);

    /// @brief destructor
    ~GNENetworkSelector();

    /// @brief get selected IDs
    std::vector<std::string> getSelectedIDs() const;

    /// @brief check if the given networkElement is being selected
    bool isNetworkElementSelected(const GNENetworkElement* networkElement) const;

    /// @brief show GNENetworkSelector Module
    void showNetworkElementsSelector();

    /// @brief hide GNENetworkSelector Module
    void hideNetworkElementsSelector();

    /// @brief return true if module is shown
    bool isShown() const;

    /// @brief toggle selected networkElement
    bool toggleSelectedElement(const GNENetworkElement* networkElement);

    /// @brief clear selection
    void clearSelection();

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user pres button use selected edges
    long onCmdUseSelectedElements(FXObject*, FXSelector, void*);

    /// @brief called when clear selection button is pressed
    long onCmdClearSelection(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX need this
    GNENetworkSelector();

private:
    /// @brief pointer to frame parent
    GNEFrame* myFrameParent;

    /// @brief button for use selected edges
    FXButton* myUseSelected = nullptr;

    /// @brief List of GNENetworkSelector
    FXList* myList = nullptr;

    /// @brief button for clear selection
    FXButton* myClearSelection = nullptr;

    /// @brrief network element type
    const Type myNetworkElementType;
};
