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
/// @file    GNEAttributeCarrierDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog for edit attribute carriers
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNETemplateElementDialog.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeCarrier;
class MFXTextFieldIcon;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrierDialog : public GNETemplateElementDialog<GNEAttributeCarrier> {

public:
    /// @brief attribute text field
    class AttributeTextField : public FXHorizontalFrame {
        // FOX-declarations
        FXDECLARE(AttributeTextField)

    public:
        /// @brief constructor
        AttributeTextField(GNEAttributeCarrierDialog* ACDialog, FXVerticalFrame* verticalFrame,
                           const GNEAttributeProperties* attrProperty);

        /// @name FOX-callbacks
        /// @{

        /// @brief event after edit text field
        long onCmdSetAttribute(FXObject* obj, FXSelector, void*);

        /// @brief called when user press "edit color" dialog
        long onCmdOpenColorDialog(FXObject* sender, FXSelector, void* arg);

        /// @brief called when user press vClass dialog
        long onCmdOpenVClassDialog(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(AttributeTextField)

        /// @brief pointer to ACDialog parent
        GNEAttributeCarrierDialog* myACDialogParent = nullptr;

        /// @brief attribute property
        const GNEAttributeProperties* myAttrProperty = nullptr;

        /// @brief attribute button (color or allow)
        MFXButtonTooltip* myAttributeButton = nullptr;

        /// @brief text field for attribute
        MFXTextFieldIcon* myTextField = nullptr;

        /// @brief check button for true/false
        FXCheckButton* myCheckButton = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        AttributeTextField(const AttributeTextField&) = delete;

        /// @brief Invalidated assignment operator.
        AttributeTextField& operator=(const AttributeTextField&) = delete;
    };

    /// @brief constructor
    GNEAttributeCarrierDialog(GNEAttributeCarrier* AC, FXWindow* restoringFocusWindow);

    /// @brief destructor
    ~GNEAttributeCarrierDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief list of attribute text fields
    std::vector<AttributeTextField*> myAttributeTextFields;

private:
    /// @brief Invalidated copy constructor.
    GNEAttributeCarrierDialog(const GNEAttributeCarrierDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAttributeCarrierDialog& operator=(const GNEAttributeCarrierDialog&) = delete;
};
