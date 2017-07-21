/****************************************************************************/
/// @file    GNEDialog_FixStoppingPlaces.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id$
///
// Dialog used to fix invalid stopping places
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEDialog_FixStoppingPlaces_h
#define GNEDialog_FixStoppingPlaces_h

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
class GNEAdditional;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDialog_FixStoppingPlaces
 * @brief Dialog for edit rerouters
 */
class GNEDialog_FixStoppingPlaces : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEDialog_FixStoppingPlaces)

public:
    /// @brief Constructor
    GNEDialog_FixStoppingPlaces(GNEViewNet *viewNet, std::vector<GNEAdditional*> invalidStoppingPlacesAndE2);

    /// @brief destructor
    ~GNEDialog_FixStoppingPlaces();

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
    GNEDialog_FixStoppingPlaces() {}

    /// @brief view net
    GNEViewNet *myViewNet;

    /// @brief vector with the invalid stoppingplaces and E2
    std::vector<GNEAdditional*> myInvalidStoppingPlacesAndE2;

    /// @brief list with the stoppingPlaces and E2
    FXTable *myTable;

    /// @brief Option "Activate friendlyPos and save"
    FXRadioButton *myOptionA;

    /// @brief Option "Fix Positions and save"
    FXRadioButton *myOptionB;

    /// @brief Option "Save invalid"
    FXRadioButton *myOptionC;

    /// @brief Option "Select invalid stops and cancel"
    FXRadioButton *myOptionD;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

private:
    /// @brief Invalidated copy constructor.
    GNEDialog_FixStoppingPlaces(const GNEDialog_FixStoppingPlaces&);

    /// @brief Invalidated assignment operator.
    GNEDialog_FixStoppingPlaces& operator=(const GNEDialog_FixStoppingPlaces&);
};

#endif
