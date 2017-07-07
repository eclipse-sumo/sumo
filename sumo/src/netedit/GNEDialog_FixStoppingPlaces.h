/****************************************************************************/
/// @file    GNEDialog_FixStoppingPlaces.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id: GNEDialog_FixStoppingPlaces.h 24108 2017-04-27 18:43:30Z behrisch $
///
/// Dialog used to fix invalid stopping places
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
    GNEDialog_FixStoppingPlaces(FXApp* app, std::vector<GNEAdditional*> invalidStoppingPlacesAndE2);

    /// @brief destructor
    ~GNEDialog_FixStoppingPlaces();

    /// @name FOX-callbacks
    /// @{
    
    /* /// @brief event when user press a enable/disable button
    long onCmdValueChanged(FXObject*, FXSelector, void*);

    /// @brief event when user press select all VClasses button
    long onCmdSelectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press unselect all VClasses button
    long onCmdUnselectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press select only non road button
    long onCmdSelectOnlyNonRoad(FXObject*, FXSelector, void*);
    */
    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    GNEDialog_FixStoppingPlaces() {}

    /// @brief vector with the invalid stoppingplaces and E2
    std::vector<GNEAdditional*> myInvalidStoppingPlacesAndE2;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

private:
    /// @brief Invalidated copy constructor.
    GNEDialog_FixStoppingPlaces(const GNEDialog_FixStoppingPlaces&);

    /// @brief Invalidated assignment operator.
    GNEDialog_FixStoppingPlaces& operator=(const GNEDialog_FixStoppingPlaces&);
};

#endif
