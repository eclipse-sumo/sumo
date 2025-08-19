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

#include "GNEElementDialog.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNERerouterDialog : public GNEElementDialog<GNEAdditional> {
    /// @brief FOX-declaration
    FXDECLARE(GNERerouterDialog)

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
    /// @brief FOX needs thsi
    FOX_CONSTRUCTOR(GNERerouterDialog)

    /// @brief Element list for rerouter intervals
    ElementList<GNEAdditional, GNEChange_Additional>* myRerouterIntervals;

private:
    /// @brief Invalidated copy constructor.
    GNERerouterDialog(const GNERerouterDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERerouterDialog& operator=(const GNERerouterDialog&) = delete;
};
