/****************************************************************************/
/// @file    GNECalibratorVehicleTypeDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id: GNECalibratorVehicleTypeDialog.h 23521 2017-03-17 14:40:17Z palcraft $
///
/// Dialog for edit calibrator vehicleTypes
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
#ifndef GNECalibratorVehicleTypeDialog_h
#define GNECalibratorVehicleTypeDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalDialog.h"
#include "GNECalibratorVehicleType.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibratorDialog;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECalibratorVehicleTypeDialog
 * @brief Dialog for edit rerouter intervals
 */

class GNECalibratorVehicleTypeDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorVehicleTypeDialog)

public:
    /// @brief constructor
    GNECalibratorVehicleTypeDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorVehicleType &calibratorVehicleType);

    /// @brief destructor
    ~GNECalibratorVehicleTypeDialog();

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
    GNECalibratorVehicleTypeDialog() {}

    /// @brief pointer to GNECalibratorDialog parent
    GNECalibratorDialog* myCalibratorDialogParent;

    /// @brief pointer to rerouter interval
    GNECalibratorVehicleType* myCalibratorVehicleType;

    /// @brief flag to check if current calibrator vehicleType is valid
    bool myCalibratorVehicleTypeValid;

    /// @brief update data fields
    void updateCalibratorVehicleTypeValues();

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorVehicleTypeDialog(const GNECalibratorVehicleTypeDialog&);

    /// @brief Invalidated assignment operator.
    GNECalibratorVehicleTypeDialog& operator=(const GNECalibratorVehicleTypeDialog&);
};

#endif
