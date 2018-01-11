/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERerouterDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
// Dialog for edit rerouters
/****************************************************************************/
#ifndef GNERerouterDialog_h
#define GNERerouterDialog_h

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

class GNERerouter;
class GNERerouterInterval;
class GNERerouterIntervalDialog;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNERerouterDialog
 * @brief Dialog for edit rerouters
 */
class GNERerouterDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNERerouterDialog)

public:
    /// @brief Constructor
    GNERerouterDialog(GNERerouter* rerouterParent);

    /// @brief destructor
    ~GNERerouterDialog();

    /// @brief get edited rerouter
    GNERerouter* getEditedRerouter() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief add new interval
    long onCmdAddInterval(FXObject*, FXSelector, void*);

    /// @brief remove or edit interval
    long onCmdClickedInterval(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNERerouterDialog() {}

    /// @brief pointer to edited rerouter
    GNERerouter* myEditedRerouter;

    /// @brief button for add new interval
    FXButton* myAddInterval;

    /// @brief list with intervals
    FXTable* myIntervalTable;

private:
    /// @brief update data table
    void updateIntervalTable();

    /// @brief Invalidated copy constructor.
    GNERerouterDialog(const GNERerouterDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERerouterDialog& operator=(const GNERerouterDialog&) = delete;
};

#endif
