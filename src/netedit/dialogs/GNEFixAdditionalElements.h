/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFixAdditionalElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id$
///
// Dialog used to fix additional elements
/****************************************************************************/
#ifndef GNEFixAdditionalElements_h
#define GNEFixAdditionalElements_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <fx.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEStoppingPlace;
class GNEDetector;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEFixAdditionalElements
 * @brief Dialog for edit rerouters
 */
class GNEFixAdditionalElements : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEFixAdditionalElements)

public:
    /// @brief Constructor
    GNEFixAdditionalElements(GNEViewNet* viewNet, const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals, const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals);

    /// @brief destructor
    ~GNEFixAdditionalElements();

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
    /// @brief groupbox for list
    class AdditionalList : protected FXGroupBox {

    public:
        /// @brief constructor
        AdditionalList(GNEFixAdditionalElements* fixAdditionalPositions, const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals, const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals);

        /// @brief vector with the invalid single-lane additionals
        std::vector<GNEAdditional*> myInvalidSingleLaneAdditionals;

        /// @brief vector with the invalid multi-lane additionals
        std::vector<GNEAdditional*> myInvalidMultiLaneAdditionals;

        /// @brief list with the stoppingPlaces and detectors
        FXTable* myTable;
    };

    /// @brief groupbox for group all radio buttons related to additionals with single lanes
    class PositionOptions : public FXGroupBox {

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
        FXRadioButton* activateFriendlyPositionAndSave;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* fixPositionsAndSave;

        /// @brief Option "Save invalid"
        FXRadioButton* saveInvalid;

        /// @brief Option "Select invalid stops and cancel"
        FXRadioButton* selectInvalidStopsAndCancel;
    };

    /// @brief groupbox for group all radio buttons related to additionals with consecutive lanes
    class ConsecutiveLaneOptions : public FXGroupBox {

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
        FXRadioButton* activateFriendlyPositionAndSave;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* fixPositionsAndSave;
    };

    /// @brief FOX needs this
    GNEFixAdditionalElements() {}

    /// @brief view net
    GNEViewNet* myViewNet;

    /// @brief main
    FXVerticalFrame* myMainFrame;

    /// @brief Additional List
    AdditionalList* myAdditionalList;

    /// @brief position options
    PositionOptions* myPositionOptions;

    /// @brief consecutive lane options
    ConsecutiveLaneOptions* myConsecutiveLaneOptions;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

private:
    /// @brief Invalidated copy constructor.
    GNEFixAdditionalElements(const GNEFixAdditionalElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixAdditionalElements& operator=(const GNEFixAdditionalElements&) = delete;
};

#endif
