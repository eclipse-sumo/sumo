/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNECalibratorDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
///
// Dialog for edit calibrators
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEElementDialog.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNECalibratorDialog : public GNEElementDialog<GNEAdditional> {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorDialog)

public:
    /// @brief Constructor
    GNECalibratorDialog(GNEAdditional* calibrator);

    /// @brief destructor
    ~GNECalibratorDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief add element in calibrator dialog
    long onCmdElementListAdd(FXObject* obj, FXSelector, void*);

    /// @brief sort element in calibrator dialog
    long onCmdElementListSort(FXObject* obj, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNECalibratorDialog)

    /// @brief list with routes
    ElementList<GNEDemandElement, GNEChange_DemandElement>* myRoutes;

    /// @brief list with vTypes
    ElementList<GNEDemandElement, GNEChange_DemandElement>* myVTypes;

    /// @brief list with calibrator flows
    ElementList<GNEAdditional, GNEChange_Additional>* myCalibratorFlows;

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorDialog(const GNECalibratorDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibratorDialog& operator=(const GNECalibratorDialog&) = delete;
};
