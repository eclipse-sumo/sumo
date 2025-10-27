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
/// @file    GNEDistributionRefDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog for edit attribute carriers
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeCarrier;
class MFXTextFieldIcon;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDistributionRefDialog : public GNEDialog {
    // FOX-declarations
    FXDECLARE(GNEDistributionRefDialog)

public:
    /// @brief constructor
    GNEDistributionRefDialog(GNEAttributeCarrier* distributionParent);

    /// @brief destructor
    ~GNEDistributionRefDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after choose a reference in the comboBox
    long onCmdSetReference(FXObject*, FXSelector, void*);

    /// @brief event after choose a reference in the probability
    long onCmdSetProbability(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEDistributionRefDialog)

    /// @brief pointer to distribution parent
    GNEAttributeCarrier* myDistributionParent = nullptr;

    /// @brief comboBox for references
    MFXComboBoxIcon* myReferencesComboBox = nullptr;

    /// @brief textField probability
    MFXTextFieldIcon* myProbabilityTextField = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEDistributionRefDialog(const GNEDistributionRefDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDistributionRefDialog& operator=(const GNEDistributionRefDialog&) = delete;
};
