/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <utils/xml/SUMOSAXHandler.h>

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
 * @brief Dialog for edit generic parameters
 */
class GNEGenericParameterDialog : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEGenericParameterDialog)

public:

    // ===========================================================================
    // class GenericParametersValues
    // ===========================================================================

    class GenericParametersValues : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEGenericParameterDialog::GenericParametersValues)

    public:
        /// @brief constructor
        GenericParametersValues(FXHorizontalFrame* frame, std::vector<std::pair<std::string, std::string> >* genericParameters);

        /// @brief destructor
        ~GenericParametersValues();

        /// @brief update values
        void updateValues();

        /// @brief get current edited generic parameters
        const std::vector<std::pair<std::string, std::string> >* getGenericParameters() const;

        /// @brief get a copy of current edited generic parameters
        std::vector<std::pair<std::string, std::string> > getCopyOfGenericParameters() const;

        /// @brief set generic parameters
        void setGenericParameters(const std::vector<std::pair<std::string, std::string> >& newGenericParameters);

        /// @brief add a single generic parameter
        void addGenericParameter(std::pair<std::string, std::string> newGenericParameter);

        /// @brief clear all generic parameters
        void clearGenericParameters();

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
        /// @brief FOX needs this
        GenericParametersValues() {}

    private:
        /// @brief class for generic parameters Row
        class GenericParameterRow {

        public:
            /// @brief constructor
            GenericParameterRow(GenericParametersValues* genericParametersValues, FXVerticalFrame* verticalFrameParent);

            /// @brief destructor
            ~GenericParameterRow();

            /// @brief disable row
            void disableRow();

            /// @brief enable rlow
            void enableRow(const std::string& parameter, const std::string& value) const;

            /// @brief toogle add button
            void toogleAddButton();

            /// @brief check if remove button is in mode "add"
            bool isButtonInAddMode() const;

            /// @brief copy values of other parameter Row
            void copyValues(const GenericParameterRow& other);

            /// @brief TextField for parameter
            FXTextField* keyField;

            /// @brief TextField for value
            FXTextField* valueField;

            /// @brief Button for add or remove row
            FXButton* button;
        private:
            /// @brief frame in which elements of GenericParameterRow are placed
            FXHorizontalFrame* horizontalFrame;
        };

        /// @brief label for key (its neccesary because has to be resized in every onPaint() iteration)
        FXLabel* myKeyLabel;

        /// @brief vertical frame in which rows are placed
        FXVerticalFrame* myVerticalFrameRow;

        /// @brief vector with the GenericParameterRows
        std::vector<GenericParameterRow*> myGenericParameterRows;

        /// @brief edited generic parameters
        std::vector<std::pair<std::string, std::string> >* myGenericParameters;
    };

    // ===========================================================================
    // class GenericParametersOptions
    // ===========================================================================

    class GenericParametersOptions : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEGenericParameterDialog::GenericParametersOptions)

    public:
        /// @brief constructor
        GenericParametersOptions(FXHorizontalFrame* frame, GNEGenericParameterDialog* genericParameterDialogParent);

        /// @brief destructor
        ~GenericParametersOptions();

        /// @name FOX-callbacks
        /// @{
        /// @brief event when user press load generic parameters button
        long onCmdLoadGenericParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press save generic parameters button
        long onCmdSaveGenericParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press clear generic parameters button
        long onCmdClearGenericParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press sort generic parameters button
        long onCmdSortGenericParameters(FXObject*, FXSelector, void*);

        /// @brief event when user press help generic parameters button
        long onCmdHelpGenericParameter(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        GenericParametersOptions() {}

    private:

        /// @class GNEGenericParameterHandler
        /// @brief load generic parameters from a filename
        class GNEGenericParameterHandler : public SUMOSAXHandler {
        public:
            /// @brief Constructor
            GNEGenericParameterHandler(GNEGenericParameterDialog* genericParameterDialogParent, const std::string& file);

            /// @brief Destructor
            ~GNEGenericParameterHandler();

            /// @name inherited from GenericSAXHandler
            /// @{
            /**@brief Called on the opening of a tag;
             * @param[in] element ID of the currently opened element
             * @param[in] attrs Attributes within the currently opened element
             * @exception ProcessError If something fails
             * @see GenericSAXHandler::myStartElement
             */
            void myStartElement(int element, const SUMOSAXAttributes& attrs);

        private:
            /// @brief pointer to genericParameterDialog parent
            GNEGenericParameterDialog* myGenericParameterDialogParent;
        };

        /// @brief pointer to Shape Frame Parent
        GNEGenericParameterDialog* myGenericParameterDialogParent;

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

    /// @brief Constructor
    GNEGenericParameterDialog(GNEViewNet* viewNet, std::vector<std::pair<std::string, std::string> >* genericParameters);

    /// @brief destructor
    ~GNEGenericParameterDialog();

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
    /// @brief FOX needs this
    GNEGenericParameterDialog() {}

    /// @pointer to viewNet
    GNEViewNet* myViewNet;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

private:

    /// @brief pointer to generic parameters values
    GenericParametersValues* myGenericParametersValues;

    /// @brief pointer to generic parameters options
    GenericParametersOptions* myGenericParametersOptions;

    // @brief copy of current edited Generic Parameters (used for reset)
    const std::vector<std::pair<std::string, std::string> > myCopyOfGenericParameters;

    /// @brief Invalidated copy constructor.
    GNEGenericParameterDialog(const GNEGenericParameterDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEGenericParameterDialog& operator=(const GNEGenericParameterDialog&) = delete;
};

#endif
