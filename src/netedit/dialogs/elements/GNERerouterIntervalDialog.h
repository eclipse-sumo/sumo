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

#include "GNEElementDialog.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAdditional;
class GNEChange_Additional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNERerouterIntervalDialog : public GNEElementDialog<GNEAdditional> {
    /// @brief FOX-declaration
    FXDECLARE(GNERerouterIntervalDialog)

public:
    /// @brief constructor
    GNERerouterIntervalDialog(GNEAdditional* rerouterInterval, bool updatingElement);

    /// @brief destructor
    ~GNERerouterIntervalDialog();

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

    /// @brief add element in rerouter interval dialog
    long onCmdElementListAdd(FXObject* obj, FXSelector, void*);

    /// @brief called when user clicks over list
    long onCmdElementListClick(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief called when list is updated
    long onCmdElementListUpdate(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief called when begin or end is changed
    long onCmdChangeBeginEnd(FXObject*, FXSelector, void*);

    /// @}

protected:
    FOX_CONSTRUCTOR(GNERerouterIntervalDialog)

    /// @brief begin time text field
    FXTextField* myBeginTextField;

    /// @brief end time text field
    FXTextField* myEndTextField;

    /// @brief begin/end time check label
    FXLabel* myCheckLabel;

    /// @brief flag to check if begin an end are valid
    bool myBeginEndValid = true;

    /// @brief list with closing lane reroutes
    ElementList<GNEAdditional, GNEChange_Additional>* myClosingLaneReroutes;

    /// @brief list with closing reroutes
    ElementList<GNEAdditional, GNEChange_Additional>* myClosingReroutes;

    /// @brief list with destination probability reroutes
    ElementList<GNEAdditional, GNEChange_Additional>* myDestProbReroutes;

    /// @brief list with route probability reroute
    ElementList<GNEAdditional, GNEChange_Additional>* myRouteProbReroutes;

    /// @brief list with parkingAreaReroutes
    ElementList<GNEAdditional, GNEChange_Additional>* myParkingAreaReroutes;

private:
    /// @brief Invalidated copy constructor.
    GNERerouterIntervalDialog(const GNERerouterIntervalDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERerouterIntervalDialog& operator=(const GNERerouterIntervalDialog&) = delete;
};
