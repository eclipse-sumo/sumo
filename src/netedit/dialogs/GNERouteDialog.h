/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
// Dialog for edit calibrator routes
/****************************************************************************/
#ifndef GNERouteDialog_h
#define GNERouteDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include "GNEDemandElementDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNERoute;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNERouteDialog
 * @brief Dialog for edit Calibrator Routes
 */

class GNERouteDialog : public GNEDemandElementDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNERouteDialog)

public:
    /// @brief constructor
    GNERouteDialog(GNEDemandElement* editedCalibratorRoute, bool updatingElement);

    /// @brief destructor
    ~GNERouteDialog();

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
    GNERouteDialog() {}

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
    GNERouteDialog(const GNERouteDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERouteDialog& operator=(const GNERouteDialog&) = delete;
};

#endif
