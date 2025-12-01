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
/// @file    GNEVClassesDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2017
///
// Dialog for edit allow VClass attribute
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/SUMOVehicleClass.h>

#include "GNEDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEVClassesDialog : public GNEDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEVClassesDialog)

public:
    /// @brief vclass row
    class VClassRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(VClassRow)

    public:
        /// @brief Constructor
        VClassRow(FXVerticalFrame* contentsFrame, SUMOVehicleClass vClass,
                  GUIIcon vClassIcon, const std::string& description);

        /// @brief destructor
        ~VClassRow();

        /// @brief get vclass in string format
        const std::string& getVClassString() const;

        /// @brief set vClass button status
        void setVClassButtonStatus(const bool enabled);

        /// @brief check if vClass button is enabled
        bool isVClassButtonEnabled() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief event when user toogle the vClass button
        long onCmdToggleVClass(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(VClassRow)

        /// @brief vclass button
        FXButton* myVClassButton = nullptr;

        /// @brief vclass in string format
        const std::string myVClassString;

    private:
        /// @brief Invalidated copy constructor.
        VClassRow(const VClassRow&) = delete;

        /// @brief Invalidated assignment operator.
        VClassRow& operator=(const VClassRow&) = delete;
    };

    /// @brief Constructor
    GNEVClassesDialog(GNEApplicationWindow* applicationWindow, FXWindow* restoringFocusWindow,
                      SumoXMLAttr attr, const std::string originalVClasses);

    /// @brief destructor
    ~GNEVClassesDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief get vClasses modified by this dialog
    std::string getModifiedVClasses() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief event when user press select all VClasses button
    long onCmdSelectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press unselect all VClasses button
    long onCmdUnselectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press select only road button
    long onCmdSelectOnlyRoad(FXObject*, FXSelector, void*);

    /// @brief event when user press select only rail button
    long onCmdSelectOnlyRail(FXObject*, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEVClassesDialog)

    /// @brief original vClasses (used for reset)
    const std::string myOriginalVClasses;

    /// @brief edited vClasses
    std::string myEditedVClasses;

    /// @brief map with the buttons for every VClass
    std::map<SUMOVehicleClass, VClassRow* > myVClassMap;

private:
    /// @brief Invalidated copy constructor.
    GNEVClassesDialog(const GNEVClassesDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEVClassesDialog& operator=(const GNEVClassesDialog&) = delete;
};
