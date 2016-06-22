/****************************************************************************/
/// @file    GNECalibratorDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
/// Dialog for edit rerouters
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECalibratorDialog
 * @brief Dialog for edit rerouters
 */

class GNECalibratorDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorDialog)

public:
    // Constructor
    GNECalibratorDialog(GNECalibrator *calibratorParent);

    // destructor
    ~GNECalibratorDialog();

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
    GNECalibratorDialog() {}

    /// @brief pointer to rerouter parent
    GNECalibrator *myRerouterParent;

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorDialog(const GNECalibratorDialog&);

    /// @brief Invalidated assignment operator.
    GNECalibratorDialog& operator=(const GNECalibratorDialog&);
};

#endif
