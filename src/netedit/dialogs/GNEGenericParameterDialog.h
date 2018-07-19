/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEGenericParameterDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2018
/// @version $Id$
///
// Dialog for edit rerouters
/****************************************************************************/
#ifndef GNEGenericParameterDialog_h
#define GNEGenericParameterDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/common/SUMOVehicleClass.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrier;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEGenericParameterDialog
 * @brief Dialog for edit rerouters
 */
class GNEGenericParameterDialog : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEGenericParameterDialog)

public:
    /// @brief Constructor
    GNEGenericParameterDialog(GNEViewNet* viewNet, std::vector<GNEAttributeCarrier::GenericParameter> *genericParameters);

    /// @brief destructor
    ~GNEGenericParameterDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user change an attribute
    long onCmdSetAttribute(FXObject*, FXSelector, void*);

    /// @brief event when user press a remove (or add) button
    long onCmdButtonPress(FXObject*, FXSelector, void*);

    /// @brief event when user press load generic parameters button
    long onCmdLoadGenericParameters(FXObject*, FXSelector, void*);

    /// @brief event when user press save generic parameters button
    long onCmdSaveGenericParameters(FXObject*, FXSelector, void*);

    /// @brief event when user press clear generic parameters button
    long onCmdClearGenericParameters(FXObject*, FXSelector, void*);

    /// @brief event when user press sort generic parameters button
    long onCmdSortGenericParameters(FXObject*, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEGenericParameterDialog() {}

    /// @pointer to viewNet
    GNEViewNet* myViewNet;

    /// @brief edited generic parameters
    std::vector<GNEAttributeCarrier::GenericParameter> *myGenericParameters;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

private:
    /// @brief struct for generic parameters Row
    struct GenericParameterRow {
        /// @brief constructor
        GenericParameterRow(GNEGenericParameterDialog * genericParametersEditor, FXVerticalFrame* _frameParent);

        /// @brief disable row
        void disableRow();

        /// @brief enable rlow
        void enableRow(const std::string &parameter, const std::string &value) const;

        /// @brief toogle add button
        void toogleAddButton();

        /// @brief check if remove button is in mode "add"
        bool isButtonInAddMode() const;

        /// @brief copy values of other parameter Row
        void copyValues(const GenericParameterRow & other);

        /// @brief frame parent in whith this GenericParameterRow is laced
        FXVerticalFrame* frameParent;

        /// @brief TextField for parameter
        FXTextField *keyField;

        /// @brief TextField for value
        FXTextField *valueField;

        /// @brief Button for add or remove row
        FXButton *button;
    private:
        /// @brief frame in which elements of GenericParameterRow are placed
        FXHorizontalFrame* horizontalFrame;
    };

    /// @brief sort button
    FXButton *mySortButton;

    /// @brief clear button
    FXButton *myClearButton;

    /// @brief load button
    FXButton *myLoadButton;

    /// @brief save button
    FXButton *mySaveButton;

    /// @brief vector with the GenericParameterRows
    std::vector<GenericParameterRow> myGenericParameterRows;

    // @brief copy of current edited Generic Parameters (used for reset)
    const std::vector<GNEAttributeCarrier::GenericParameter> myCopyOfGenericParameters;

    /// @brief update values
    void updateValues();

    /// @brief Invalidated copy constructor.
    GNEGenericParameterDialog(const GNEGenericParameterDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEGenericParameterDialog& operator=(const GNEGenericParameterDialog&) = delete;
};

#endif
