/****************************************************************************/
/// @file    GNECalibratorRouteDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrator routes
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
#ifndef GNECalibratorRouteDialog_h
#define GNECalibratorRouteDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalDialog.h"
#include "GNECalibratorRoute.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibratorDialog;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECalibratorRouteDialog
 * @brief Dialog for edit rerouter intervals
 */

class GNECalibratorRouteDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorRouteDialog)

public:
    /// @brief constructor
    GNECalibratorRouteDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorRoute &calibratorRoute);

    /// @brief destructor
    ~GNECalibratorRouteDialog();

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
    GNECalibratorRouteDialog() {}

    /// @brief pointer to GNECalibratorDialog parent
    GNECalibratorDialog* myCalibratorDialogParent;

    /// @brief pointer to rerouter interval
    GNECalibratorRoute* myCalibratorRoute;

    /// @brief flag to check if current calibrator route is valid
    bool myCalibratorRouteValid;

    /// @brief update data fields
    void updateCalibratorRouteValues();

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorRouteDialog(const GNECalibratorRouteDialog&);

    /// @brief Invalidated assignment operator.
    GNECalibratorRouteDialog& operator=(const GNECalibratorRouteDialog&);
};

#endif
