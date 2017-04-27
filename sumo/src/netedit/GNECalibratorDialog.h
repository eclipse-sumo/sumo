/****************************************************************************/
/// @file    GNECalibratorDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrators
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
#ifndef GNECalibratorDialog_h
#define GNECalibratorDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalDialog.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibrator;
class GNECalibratorRoute;
class GNECalibratorFlow;
class GNECalibratorVehicleType;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECalibratorDialog
 * @brief Dialog for edit calibrators
 */
class GNECalibratorDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorDialog)

public:
    /// @brief Constructor
    GNECalibratorDialog(GNECalibrator* calibratorParent);

    /// @brief destructor
    ~GNECalibratorDialog();

    /// @brief get calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief add new route
    long onCmdAddRoute(FXObject*, FXSelector, void*);

    /// @brief remove or edit route
    long onCmdClickedRoute(FXObject*, FXSelector, void*);

    /// @brief add new flow
    long onCmdAddFlow(FXObject*, FXSelector, void*);

    /// @brief remove or edit flow
    long onCmdClickedFlow(FXObject*, FXSelector, void*);

    /// @brief add new vehicle type
    long onCmdAddVehicleType(FXObject*, FXSelector, void*);

    /// @brief remove or edit vehicle type
    long onCmdClickedVehicleType(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNECalibratorDialog() {}

    /// @brief pointer to calibrator parent
    GNECalibrator* myCalibratorParent;

    /// @brief button for add new route
    FXButton* myAddRoute;

    /// @brief list with routes
    FXTable* myRouteList;

    /// @brief label for flows
    FXLabel* myLabelFlow;

    /// @brief button for add new flow
    FXButton* myAddFlow;

    /// @brief list with flows
    FXTable* myFlowList;

    /// @brief button for add new vehicle type
    FXButton* myAddVehicleType;

    /// @brief list with vehicle types
    FXTable* myVehicleTypeList;

    /// @brief vector with a copy of calibrator routes (used by reset)
    std::vector<GNECalibratorRoute> myCopyOfCalibratorRoutes;

    /// @brief vector with a copy of calibrator flows (used by reset)
    std::vector<GNECalibratorFlow> myCopyOfCalibratorFlows;

    /// @brief vector with a copy of calibrator vehicle types (used by reset)
    std::vector<GNECalibratorVehicleType> myCopyOfCalibratorVehicleTypes;

    /// @brief update data table with routes
    void updateRouteTable();

    /// @brief update data table with flows
    void updateFlowTable();

    /// @brief update data table with vehicle types
    void updateVehicleTypeTable();

    /// @brief update flow and label button
    void updateFlowAndLabelButton();

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorDialog(const GNECalibratorDialog&);

    /// @brief Invalidated assignment operator.
    GNECalibratorDialog& operator=(const GNECalibratorDialog&);
};

#endif
