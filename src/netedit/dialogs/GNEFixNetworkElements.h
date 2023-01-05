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
/// @file    GNEFixNetworkElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog used to fix network elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNENetworkElement;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEFixNetworkElements
 * @brief Dialog fix network elements
 */
class GNEFixNetworkElements : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEFixNetworkElements)

public:
    /// @brief Constructor
    GNEFixNetworkElements(GNEViewNet* viewNet, const std::vector<GNENetworkElement*>& invalidNetworkElements);

    /// @brief destructor
    ~GNEFixNetworkElements();

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user select a option
    long onCmdSelectOption(FXObject* obj, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEFixNetworkElements)

    /// @brief general GroupBox for fix options
    class FixOptions : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        FixOptions(FXVerticalFrame* frameParent, const std::string& title, GNEViewNet* viewNet);

        /// @brief set invalid network elements
        void setInvalidElements(const std::vector<GNENetworkElement*>& invalidElements);

        /// @brief fix elements
        virtual void fixElements(bool& abortSaving) = 0;

    protected:
        /// @brief save contents
        bool saveContents() const;

        /// @brief vertical left frame
        FXVerticalFrame* myLeftFrame = nullptr;

        /// @brief vertical right frame
        FXVerticalFrame* myRightFrame = nullptr;

        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief vector with the invalid network elements
        std::vector<GNENetworkElement*> myInvalidElements;

    private:
        /// @brief enable options
        virtual void enableOptions() = 0;

        /// @brief disable options
        virtual void disableOptions() = 0;

        /// @brief Table with the network elements
        FXTable* myTable = nullptr;
    };

    /// @brief groupbox for all radio buttons related with fix edges options
    class FixEdgeOptions : public FixOptions {

    public:
        /// @brief constructor
        FixEdgeOptions(GNEFixNetworkElements* fixNetworkElementsParent, GNEViewNet* viewNet);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief fix elements
        void fixElements(bool& abortSaving);

        /// @brief Option "Remove invalid edges"
        FXRadioButton* removeInvalidEdges;

        /// @brief Option "Save invalid edges"
        FXRadioButton* saveInvalidEdges;

        /// @brief Option "Select invalid edges and cancel"
        FXRadioButton* selectInvalidEdgesAndCancel;
    private:
        /// @brief enable edge options
        void enableOptions();

        /// @brief disable edge options
        void disableOptions();
    };

    /// @brief groupbox for all radio buttons related with fix crossing options
    class FixCrossingOptions : public FixOptions {

    public:
        /// @brief constructor
        FixCrossingOptions(GNEFixNetworkElements* fixNetworkElementsParent, GNEViewNet* viewNet);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief fix elements
        void fixElements(bool& abortSaving);

        /// @brief Option "remove invalid elements"
        FXRadioButton* removeInvalidCrossings;

        /// @brief Option "save invalid crossings"
        FXRadioButton* saveInvalidCrossings;

        /// @brief Option "Select invalid crossings and cancel"
        FXRadioButton* selectInvalidCrossingsAndCancel;

    private:
        /// @brief enable crossing options
        void enableOptions();

        /// @brief disable crossing options
        void disableOptions();
    };

    /// @brief horizontal frame for buttons
    class Buttons : public FXHorizontalFrame {

    public:
        /// @brief build Position Options
        Buttons(GNEFixNetworkElements* fixNetworkElementsParent);

    private:
        /// @brief accept button
        FXButton* myAcceptButton = nullptr;

        /// @brief cancel button
        FXButton* myCancelButton = nullptr;
    };

    /// @brief view net
    GNEViewNet* myViewNet = nullptr;

    /// @brief main frame
    FXVerticalFrame* myMainFrame = nullptr;

    /// @brief vertical left frame
    FXVerticalFrame* myLeftFrame = nullptr;

    /// @brief vertical right frame
    FXVerticalFrame* myRightFrame = nullptr;

    /// @brief fix edge options
    FixEdgeOptions* myFixEdgeOptions = nullptr;

    /// @brief fix crossing options
    FixCrossingOptions* myFixCrossingOptions = nullptr;

    /// @brief buttons
    Buttons* myButtons = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixNetworkElements(const GNEFixNetworkElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixNetworkElements& operator=(const GNEFixNetworkElements&) = delete;
};

