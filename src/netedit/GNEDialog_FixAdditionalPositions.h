/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_FixAdditionalPositions.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id$
///
// Dialog used to fix invalid stopping places
/****************************************************************************/
#ifndef GNEDialog_FixAdditionalPositions_h
#define GNEDialog_FixAdditionalPositions_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
 * @class GNEDialog_FixAdditionalPositions
 * @brief Dialog for edit rerouters
 */
class GNEDialog_FixAdditionalPositions : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEDialog_FixAdditionalPositions)

public:
    /// @brief Constructor
    GNEDialog_FixAdditionalPositions(GNEViewNet* viewNet, const std::vector<GNEStoppingPlace*>& invalidStoppingPlaces, const std::vector<GNEDetector*>& invalidDetectors);

    /// @brief destructor
    ~GNEDialog_FixAdditionalPositions();

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
    GNEDialog_FixAdditionalPositions() {}

    /// @brief view net
    GNEViewNet* myViewNet;

    /// @brief vector with the invalid stoppingplaces
    std::vector<GNEStoppingPlace*> myInvalidStoppingPlaces;

    /// @brief vector with the invalid stoppingplaces
    std::vector<GNEDetector*> myInvalidDetectors;

    /// @brief list with the stoppingPlaces and detectors
    FXTable* myTable;

    /// @brief Option "Activate friendlyPos and save"
    FXRadioButton* myOptionA;

    /// @brief Option "Fix Positions and save"
    FXRadioButton* myOptionB;

    /// @brief Option "Save invalid"
    FXRadioButton* myOptionC;

    /// @brief Option "Select invalid stops and cancel"
    FXRadioButton* myOptionD;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

private:
    /// @brief Invalidated copy constructor.
    GNEDialog_FixAdditionalPositions(const GNEDialog_FixAdditionalPositions&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDialog_FixAdditionalPositions& operator=(const GNEDialog_FixAdditionalPositions&) = delete;
};

#endif
