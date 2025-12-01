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
/// @file    GNERerouterDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// Dialog for edit rerouters
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/dialogs/elements/lists/GNETemplateElementList.h>
#include <netedit/elements/additional/GNEAdditional.h>

#include "GNETemplateElementDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNERerouterDialog : public GNETemplateElementDialog<GNEAdditional> {

public:
    /// @brief Constructor
    GNERerouterDialog(GNEAdditional* rerouter);

    /// @brief destructor
    ~GNERerouterDialog();

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
    /// @brief rerouter interval list
    class RerouterIntervalsList : public GNETemplateElementList<GNEAdditional, GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        RerouterIntervalsList(GNERerouterDialog* rerouterDialog);

        /// @brief add new element
        long addNewElement();

        /// @brief open element dialog
        long openElementDialog(const size_t rowIndex);

        /// @briec check if there is overlapping between intervals
        bool isOverlapping() const;

    private:
        /// @brief Invalidated copy constructor
        RerouterIntervalsList(const RerouterIntervalsList&) = delete;

        /// @brief Invalidated assignment operator
        RerouterIntervalsList& operator=(const RerouterIntervalsList&) = delete;
    };

    /// @brief Element list for rerouter intervals
    RerouterIntervalsList* myRerouterIntervals;

private:
    /// @brief Invalidated copy constructor.
    GNERerouterDialog(const GNERerouterDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERerouterDialog& operator=(const GNERerouterDialog&) = delete;
};
