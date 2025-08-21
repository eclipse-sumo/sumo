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
/// @file    GNERerouterIntervalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2017
///
// Dialog for edit rerouter intervals
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/dialogs/elements/lists/GNEAdditionalElementList.h>
#include <netedit/elements/additional/GNEAdditional.h>

#include "GNEElementDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNERerouterIntervalDialog : public GNEElementDialog<GNEAdditional> {

public:
    /// @brief constructor
    GNERerouterIntervalDialog(GNEAdditional* rerouterInterval);

    /// @brief destructor
    ~GNERerouterIntervalDialog();

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
    /// @brief closing  reroutes list
    class ClosingReroutesList : public GNEAdditionalElementList<GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        ClosingReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame);

        /// @brief add new element
        long addNewElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    private:
        /// @brief Invalidated copy constructor
        ClosingReroutesList(const ClosingReroutesList&) = delete;

        /// @brief Invalidated assignment operator
        ClosingReroutesList& operator=(const ClosingReroutesList&) = delete;
    };

    /// @brief closing lane reroutes list
    class ClosingLaneReroutesList : public GNEAdditionalElementList<GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        ClosingLaneReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame);

        /// @brief add new element
        long addNewElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    private:
        /// @brief Invalidated copy constructor
        ClosingLaneReroutesList(const ClosingLaneReroutesList&) = delete;

        /// @brief Invalidated assignment operator
        ClosingLaneReroutesList& operator=(const ClosingLaneReroutesList&) = delete;
    };

    /// @brief dest prob reroutes list
    class DestProbReroutesList : public GNEAdditionalElementList<GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        DestProbReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame);

        /// @brief add new element
        long addNewElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    private:
        /// @brief Invalidated copy constructor
        DestProbReroutesList(const DestProbReroutesList&) = delete;

        /// @brief Invalidated assignment operator
        DestProbReroutesList& operator=(const DestProbReroutesList&) = delete;
    };

    /// @brief route prob reroutes list
    class RouteProbReroutesList : public GNEAdditionalElementList<GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        RouteProbReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame);

        /// @brief add new element
        long addNewElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    private:
        /// @brief Invalidated copy constructor
        RouteProbReroutesList(const RouteProbReroutesList&) = delete;

        /// @brief Invalidated assignment operator
        RouteProbReroutesList& operator=(const RouteProbReroutesList&) = delete;
    };

    /// @brief parking area reroutes list
    class ParkingAreaReroutesList : public GNEAdditionalElementList<GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        ParkingAreaReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame);

        /// @brief add new element
        long addNewElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

    private:
        /// @brief Invalidated copy constructor
        ParkingAreaReroutesList(const ParkingAreaReroutesList&) = delete;

        /// @brief Invalidated assignment operator
        ParkingAreaReroutesList& operator=(const ParkingAreaReroutesList&) = delete;
    };

    /// @brief list with closing reroutes
    ClosingReroutesList* myClosingReroutes;

    /// @brief list with closing lane reroutes
    ClosingLaneReroutesList* myClosingLaneReroutes;

    /// @brief list with destination probability reroutes
    DestProbReroutesList* myDestProbReroutes;

    /// @brief list with route probability reroute
    RouteProbReroutesList* myRouteProbReroutes;

    /// @brief list with parkingAreaReroutes
    ParkingAreaReroutesList* myParkingAreaReroutes;

private:
    /// @brief Invalidated copy constructor.
    GNERerouterIntervalDialog(const GNERerouterIntervalDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERerouterIntervalDialog& operator=(const GNERerouterIntervalDialog&) = delete;
};
