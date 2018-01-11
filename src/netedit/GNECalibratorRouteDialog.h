/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorRouteDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
// Dialog for edit calibrator routes
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

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibratorRoute;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECalibratorRouteDialog
 * @brief Dialog for edit Calibrator Routes
 */

class GNECalibratorRouteDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorRouteDialog)

public:
    /// @brief constructor
    GNECalibratorRouteDialog(GNECalibratorRoute* editedCalibratorRoute, bool updatingElement);

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

    /// @brief event after change value
    long onCmdSetVariable(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNECalibratorRouteDialog() {}

    /// @brief pointer to modified rerouter interval
    GNECalibratorRoute* myEditedCalibratorRoute;

    /// @brief flag to indicate if flow are being created or modified
    bool myUpdatingElement;

    /// @brief flag to check if current calibrator vehicleType is valid
    bool myCalibratorRouteValid;

    /// @brief current sumo attribute invalid
    SumoXMLAttr myInvalidAttr;

    /// @brief route ID
    FXTextField* myTextFieldRouteID;

    /// @brief list of edges (string)
    FXTextField* myTextFieldEdges;

    /// @brief color of route
    FXTextField* myTextFieldColor;

    /// @brief update data fields
    void updateCalibratorRouteValues();

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorRouteDialog(const GNECalibratorRouteDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibratorRouteDialog& operator=(const GNECalibratorRouteDialog&) = delete;
};

#endif
