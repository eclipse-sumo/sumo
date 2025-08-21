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

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/dialogs/elements/lists/GNEAdditionalElementList.h>
#include <netedit/dialogs/elements/lists/GNEDemandElementList.h>
#include <netedit/elements/additional/GNEAdditional.h>

#include "GNEElementDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNECalibratorDialog : public GNEElementDialog<GNEAdditional> {

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

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief route list
    class RoutesList : public GNEDemandElementList {

    public:
        /// @brief constructor
        RoutesList(GNECalibratorDialog* rerouterDialog, FXVerticalFrame* contentFrame);

        /// @brief add element
        long addElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    private:
        /// @brief Invalidated copy constructor
        RoutesList(const RoutesList&) = delete;

        /// @brief Invalidated assignment operator
        RoutesList& operator=(const RoutesList&) = delete;
    };

    /// @brief vTypes list
    class VTypesList : public GNEDemandElementList {

    public:
        /// @brief constructor
        VTypesList(GNECalibratorDialog* rerouterDialog, FXVerticalFrame* contentFrame);

        /// @brief add element
        long addElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    private:
        /// @brief Invalidated copy constructor
        VTypesList(const VTypesList&) = delete;

        /// @brief Invalidated assignment operator
        VTypesList& operator=(const VTypesList&) = delete;
    };

    /// @brief calibrator flows list
    class CalibratorFlowsList : public GNEAdditionalElementList<GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        CalibratorFlowsList(GNECalibratorDialog* rerouterDialog, FXVerticalFrame* contentFrame,
                            RoutesList* routesList, VTypesList* vTypesList);

        /// @brief add element
        long addElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    protected:
        /// @brief pointer to routes list
        RoutesList* myRoutesList = nullptr;

        /// @brief pointer to vTypes list
        VTypesList* myVTypesList = nullptr;

    private:
        /// @brief Invalidated copy constructor
        CalibratorFlowsList(const CalibratorFlowsList&) = delete;

        /// @brief Invalidated assignment operator
        CalibratorFlowsList& operator=(const CalibratorFlowsList&) = delete;
    };

    /// @brief list with routes
    RoutesList* myRoutes;

    /// @brief list with vTypes
    VTypesList* myVTypes;

    /// @brief list with calibrator flows
    CalibratorFlowsList* myCalibratorFlows;

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorDialog(const GNECalibratorDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibratorDialog& operator=(const GNECalibratorDialog&) = delete;
};
