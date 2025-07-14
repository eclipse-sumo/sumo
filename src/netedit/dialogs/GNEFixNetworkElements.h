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
/// @file    GNEFixNetworkElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog used to fix network elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEFixElementsDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNENetworkElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixNetworkElements : public GNEFixElementsDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEFixNetworkElements)

public:
    /// @brief Constructor
    GNEFixNetworkElements(GNEViewNet* viewNet);

    /// @brief destructor
    ~GNEFixNetworkElements();

    /// @brief open fix network elements dialog
    FXuint openDialog(const std::vector<GNENetworkElement*>& invalidNetworkElements);

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogTest* modalArguments);

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
    /// @brief FOX needs this
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

        /// @brief Invalidated copy constructor.
        FixOptions(const FixOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixOptions& operator=(const FixOptions&) = delete;
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

        /// @brief Invalidated copy constructor.
        FixEdgeOptions(const FixEdgeOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixEdgeOptions& operator=(const FixEdgeOptions&) = delete;
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
        FXRadioButton* selectInvalidCrossings;

    private:
        /// @brief enable crossing options
        void enableOptions();

        /// @brief disable crossing options
        void disableOptions();

        /// @brief Invalidated copy constructor.
        FixCrossingOptions(const FixCrossingOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixCrossingOptions& operator=(const FixCrossingOptions&) = delete;
    };

    /// @brief vertical left frame
    FXVerticalFrame* myLeftFrame = nullptr;

    /// @brief vertical right frame
    FXVerticalFrame* myRightFrame = nullptr;

    /// @brief fix edge options
    FixEdgeOptions* myFixEdgeOptions = nullptr;

    /// @brief fix crossing options
    FixCrossingOptions* myFixCrossingOptions = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixNetworkElements(const GNEFixNetworkElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixNetworkElements& operator=(const GNEFixNetworkElements&) = delete;
};
