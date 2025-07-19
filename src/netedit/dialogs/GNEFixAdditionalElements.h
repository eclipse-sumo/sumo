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
/// @file    GNEFixAdditionalElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to fix additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEFixElementsDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEStoppingPlace;
class GNEDetector;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixAdditionalElements : public GNEFixElementsDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEFixAdditionalElements)

public:
    /// @brief Constructor
    GNEFixAdditionalElements(GNEViewNet* viewNet);

    /// @brief destructor
    ~GNEFixAdditionalElements();

    /// @brief open fix additional dialog
    FXuint openDialog(const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals, const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals);

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
    FOX_CONSTRUCTOR(GNEFixAdditionalElements)

    /// @brief groupbox for list
    class AdditionalList : protected FXGroupBox {

    public:
        /// @brief constructor
        AdditionalList(GNEFixAdditionalElements* fixAdditionalPositions);

        /// @brief update list with the invalid additionals
        void updateList(const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals, const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals);

        /// @brief vector with the invalid single-lane additionals
        std::vector<GNEAdditional*> myInvalidSingleLaneAdditionals;

        /// @brief vector with the invalid multi-lane additionals
        std::vector<GNEAdditional*> myInvalidMultiLaneAdditionals;

        /// @brief list with the stoppingPlaces and detectors
        FXTable* myTable;

    private:
        /// @brief Invalidated copy constructor.
        AdditionalList(const AdditionalList&) = delete;

        /// @brief Invalidated assignment operator.
        AdditionalList& operator=(const AdditionalList&) = delete;
    };

    /// @brief groupbox for group all radio buttons related to additionals with single lanes
    class PositionOptions : public MFXGroupBoxModule {

    public:
        /// @brief build Position Options
        PositionOptions(GNEFixAdditionalElements* fixAdditionalPositions);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief enable position options
        void enablePositionOptions();

        /// @brief disable position options
        void disablePositionOptions();

        /// @brief Option "Activate friendlyPos and save"
        FXRadioButton* activateFriendlyPosition;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* fixPositions;

        /// @brief Option "Save invalid"
        FXRadioButton* saveInvalids;

        /// @brief Option "Select invalid stops and cancel"
        FXRadioButton* selectInvalids;

    private:
        /// @brief Invalidated copy constructor.
        PositionOptions(const PositionOptions&) = delete;

        /// @brief Invalidated assignment operator.
        PositionOptions& operator=(const PositionOptions&) = delete;
    };

    /// @brief groupbox for group all radio buttons related to additionals with consecutive lanes
    class ConsecutiveLaneOptions : public MFXGroupBoxModule {

    public:
        /// @brief build consecutive lane Options
        ConsecutiveLaneOptions(GNEFixAdditionalElements* fixAdditionalPositions);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief enable consecutive lane options
        void enableConsecutiveLaneOptions();

        /// @brief disable consecutive lane options
        void disableConsecutiveLaneOptions();

        /// @brief Option "build connections between lanes"
        FXRadioButton* buildConnectionBetweenLanes;

        /// @brief Option "remove invalid elements"
        FXRadioButton* removeInvalidElements;

        /// @brief Option "Activate friendlyPos and save"
        FXRadioButton* activateFriendlyPosition;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* fixPositions;

    private:
        /// @brief Invalidated copy constructor.
        ConsecutiveLaneOptions(const ConsecutiveLaneOptions&) = delete;

        /// @brief Invalidated assignment operator.
        ConsecutiveLaneOptions& operator=(const ConsecutiveLaneOptions&) = delete;
    };

    /// @brief Additional List
    AdditionalList* myAdditionalList;

    /// @brief position options
    PositionOptions* myPositionOptions;

    /// @brief consecutive lane options
    ConsecutiveLaneOptions* myConsecutiveLaneOptions;

private:
    /// @brief Invalidated copy constructor.
    GNEFixAdditionalElements(const GNEFixAdditionalElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixAdditionalElements& operator=(const GNEFixAdditionalElements&) = delete;
};
