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
/// @file    GNESingleParametersDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2018
///
// Dialog for edit parameters
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/foxtools/MFXDialogBox.h>
#include <utils/xml/SUMOSAXHandler.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrier;
class GNEAttributesEditorType;
class GNEViewNet;
class NBLoadedSUMOTLDef;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNESingleParametersDialog
 * @brief Dialog for edit  parameters
 */
class GNESingleParametersDialog : public MFXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNESingleParametersDialog)

public:

    // ===========================================================================
    // class ParametersValues
    // ===========================================================================

    class ParametersValues : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNESingleParametersDialog::ParametersValues)

        /// @brief declare class
        class ParameterRow;

    public:
        /// @brief constructor
        ParametersValues(FXHorizontalFrame* frame, const std::string& name);

        /// @brief destructor
        ~ParametersValues();

        /// @brief set  parameters
        void setParameters(const std::vector<std::pair<std::string, std::string> >& newParameters);

        /// @brief add a single  parameter
        void addParameter(std::pair<std::string, std::string> newParameter);

        /// @brief clear all  parameters
        void clearParameters();

        /// @brief get vector with the ParameterRows
        const std::vector<ParameterRow*> getParameterRows() const;

        /// @brief check if given key exist already
        bool keyExist(const std::string& key) const;

        /// @name FOX-callbacks
        /// @{
        /// @brief on paint function (reimplemented from FXGroupBox)
        long onPaint(FXObject* o, FXSelector f, void* p);

        /// @brief event when user change an attribute
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief event when user press a remove (or add) button
        long onCmdButtonPress(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ParametersValues)

    private:
        /// @brief class for  parameters Row
        class ParameterRow {

        public:
            /// @brief constructor
            ParameterRow(ParametersValues* ParametersValues, FXVerticalFrame* verticalFrameParent);

            /// @brief destructor
            ~ParameterRow();

            /// @brief disable row
            void disableRow();

            /// @brief enable row
            void enableRow(const std::string& parameter, const std::string& value) const;

            /// @brief toggle add button
            void toggleAddButton();

            /// @brief check if remove button is in mode "add"
            bool isButtonInAddMode() const;

            /// @brief copy values of other parameter Row
            void copyValues(const ParameterRow& other);

            /// @brief TextField for parameter
            FXTextField* keyField;

            /// @brief TextField for value
            FXTextField* valueField;

            /// @brief Button for add or remove row
            FXButton* button;
        private:
            /// @brief frame in which elements of ParameterRow are placed
            FXHorizontalFrame* horizontalFrame;
        };

        /// @brief label for key (its neccesary because has to be resized in every onPaint() iteration)
        FXLabel* myKeyLabel;

        /// @brief vertical frame in which rows are placed
        FXVerticalFrame* myVerticalFrameRow;

        /// @brief vector with the ParameterRows
        std::vector<ParameterRow*> myParameterRows;
    };

    // ===========================================================================
    // class ParametersOperations
    // ===========================================================================

    class ParametersOperations : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNESingleParametersDialog::ParametersOperations)

    public:
        /// @brief constructor
        ParametersOperations(FXHorizontalFrame* frame, GNESingleParametersDialog* ParameterDialogParent);

        /// @brief destructor
        ~ParametersOperations();

        /// @name FOX-callbacks
        /// @{
        /// @brief event when user press load  parameters button
        long onCmdLoadParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press save  parameters button
        long onCmdSaveParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press clear  parameters button
        long onCmdClearParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press sort  parameters button
        long onCmdSortParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press help  parameters button
        long onCmdHelpParameter(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ParametersOperations)

    private:
        /// @class GNEParameterHandler
        /// @brief load  parameters from a filename
        class GNEParameterHandler : public SUMOSAXHandler {
        public:
            /// @brief Constructor
            GNEParameterHandler(ParametersOperations* ParametersOperationsParent, const std::string& file);

            /// @brief Destructor
            ~GNEParameterHandler();

            /// @name inherited from SAXHandler
            /// @{
            /**@brief Called on the opening of a tag;
             * @param[in] element ID of the currently opened element
             * @param[in] attrs Attributes within the currently opened element
             * @exception ProcessError If something fails
             * @see SAXHandler::myStartElement
             */
            void myStartElement(int element, const SUMOSAXAttributes& attrs);

        private:
            /// @brief pointer to ParametersOperations parent
            ParametersOperations* myParametersOperationsParent;
        };

        /// @brief pointer to Shape Frame Parent
        GNESingleParametersDialog* myParameterDialogParent;

        /// @brief sort button
        FXButton* mySortButton;

        /// @brief clear button
        FXButton* myClearButton;

        /// @brief load button
        FXButton* myLoadButton;

        /// @brief save button
        FXButton* mySaveButton;

        /// @brief help button
        FXButton* myHelpButton;
    };

    /// @brief Constructor for attributes editor
    GNESingleParametersDialog(GNEAttributesEditorType* attributesEditor);

    /// @brief Constructor for Vehicle Type Row (Vehicle Type Dialog)
    GNESingleParametersDialog(GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow* VTypeAttributeRow, GNEViewNet* viewNet);

    /// @brief Constructor for attribute carriers (used in GNECreateEdgeFrame)
    GNESingleParametersDialog(GNEAttributeCarrier* attributeCarrier);

    /// @brief Constructor for attribute carriers (used in GNETLSEditorFrame)
    GNESingleParametersDialog(FXApp* app, NBLoadedSUMOTLDef* TLDef);

    /// @brief destructor
    ~GNESingleParametersDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogTest* modalArguments);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNESingleParametersDialog)

    /// @brief pointer to attributes editor
    GNEAttributesEditorType* myAttributesEditor = nullptr;

    /// @brief pointer to VTypeAttributeRow
    GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow* VTypeAttributeRow = nullptr;

    /// @brief pointer to GNEAttributeCarrier
    GNEAttributeCarrier* myAttributeCarrier = nullptr;

    /// @brief pointer to TLDef
    NBLoadedSUMOTLDef* myTLDef = nullptr;

    /// @brief pointer to parameters values
    ParametersValues* myParametersValues = nullptr;

    /// @brief pointer to parameters operations
    ParametersOperations* myParametersOperations = nullptr;

    /// @brief accept button
    FXButton* myKeepOldButton = nullptr;

    /// @brief cancel button
    FXButton* myCancelButton = nullptr;

    /// @brief cancel button
    FXButton* myResetButton = nullptr;

private:
    /// @brief auxiliar constructor
    void constructor(const std::string& name);

    /// @brief Invalidated copy constructor.
    GNESingleParametersDialog(const GNESingleParametersDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNESingleParametersDialog& operator=(const GNESingleParametersDialog&) = delete;
};
