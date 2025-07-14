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
/// @file    GNEAllowVClassesDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2017
///
// Dialog for edit allow VClass attribute
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/SUMOVehicleClass.h>
#include <utils/foxtools/MFXDialogBox.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrier;
class GNEInternalTest;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAllowVClassesDialog : public MFXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEAllowVClassesDialog)

public:
    /// @brief Constructor
    GNEAllowVClassesDialog(GNEViewNet* viewNet);

    /// @brief destructor
    ~GNEAllowVClassesDialog();

    /// @brief open dialog
    int openDialog(SumoXMLAttr attr, const std::string originalVClasses, GNEInternalTest* internalTests);

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::FixDialogTest* dialogTest);

    /// @brief get vClasses modified by this dialog
    std::string getModifiedVClasses() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user press a enable/disable button
    long onCmdValueChanged(FXObject*, FXSelector, void*);

    /// @brief event when user press select all VClasses button
    long onCmdSelectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press unselect all VClasses button
    long onCmdUnselectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press select only road button
    long onCmdSelectOnlyRoad(FXObject*, FXSelector, void*);

    /// @brief event when user press select only rail button
    long onCmdSelectOnlyRail(FXObject*, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject* sender, FXSelector sel, void* arg);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject* sender, FXSelector sel, void* arg);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEAllowVClassesDialog)

    /// @pointer to viewNet
    GNEViewNet* myViewNet;

    /// @brief original vClasses
    std::string myOriginalVClasses;

    /// @brief edited vClasses
    std::string myEditedVClasses;

    /// @brief accept button
    FXButton* myKeepOldButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

    /// @brief map with the buttons for every VClass
    std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> > myVClassMap;

private:
    /// @brief build VClass
    void buildVClass(FXVerticalFrame* contentsFrame, SUMOVehicleClass vclass, GUIIcon vclassIcon, const std::string& description);

    /// @brief Invalidated copy constructor.
    GNEAllowVClassesDialog(const GNEAllowVClassesDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAllowVClassesDialog& operator=(const GNEAllowVClassesDialog&) = delete;
};
