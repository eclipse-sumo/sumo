/****************************************************************************/
/// @file    GNERerouterIntervalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2017
/// @version $Id$
///
/// Dialog for edit rerouter intervals
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNERerouterIntervalDialog_h
#define GNERerouterIntervalDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalDialog.h"
#include "GNERerouterInterval.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GNERerouterDialog;


// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNERerouterIntervalDialog
 * @brief Dialog for edit rerouter intervals
 */

class GNERerouterIntervalDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNERerouterIntervalDialog)

public:
    /// @brief constructor
    GNERerouterIntervalDialog(GNERerouterDialog* rerouterDialog, GNERerouterInterval& rerouterInterval);

    /// @brief destructor
    ~GNERerouterIntervalDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief add closing lane reroute
    long onCmdAddClosingLaneReroute(FXObject*, FXSelector, void*);

    /// @brief add closing reroute
    long onCmdAddClosingReroute(FXObject*, FXSelector, void*);

    /// @brief add dest prob reroute
    long onCmdAddDestProbReroute(FXObject*, FXSelector, void*);

    /// @brief add route prob reroute
    long onCmdAddRouteProbReroute(FXObject*, FXSelector, void*);

    /// @brief remove closing lane reroute
    long onCmdClickedClosingLaneReroute(FXObject*, FXSelector, void*);

    /// @brief remove closing reroute
    long onCmdClickedClosingReroute(FXObject*, FXSelector, void*);

    /// @brief remove dest prob reroute
    long onCmdClickedDestProbReroute(FXObject*, FXSelector, void*);

    /// @brief remove route prob reroute
    long onCmdClickedRouteProbReroute(FXObject*, FXSelector, void*);

    /// @brief /edit closing lane reroute
    long onCmdEditClosingLaneReroute(FXObject*, FXSelector, void*);

    /// @brief /edit closing reroute
    long onCmdEditClosingReroute(FXObject*, FXSelector, void*);

    /// @brief /edit dest prob reroute
    long onCmdEditDestProbReroute(FXObject*, FXSelector, void*);

    /// @brief /edit route prob reroute
    long onCmdEditRouteProbReroute(FXObject*, FXSelector, void*);

    /// @brief called when begin is changed
    long onCmdChangeBegin(FXObject*, FXSelector, void*);

    /// @brief called when begin is changed
    long onCmdChangeEnd(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNERerouterIntervalDialog() {}

    /// @brief pointer to GNERerouterDialog parent
    GNERerouterDialog* myRerouterDialogParent;

    /// @brief pointer to rerouter interval
    GNERerouterInterval* myRerouterInterval;

    /// @brief begin time text field
    FXTextField* myBeginTextField;

    /// @brief end time text field
    FXTextField* myEndTextField;

    /// @brief begin/end time check label
    FXLabel* myCheckLabel;

    /// @brief button for add new closing lane reroutes
    FXButton* myAddClosingLaneReroutes;

    /// @brief button for add new closing reroutes
    FXButton* myAddClosingReroutes;

    /// @brief button for add new destiny probability reroutes
    FXButton* myAddDestProbReroutes;

    /// @brief button for add new route probability reroutes
    FXButton* myAddRouteProbReroute;

    /// @brief list with closing lane reroutes
    FXTable* myClosingLaneRerouteList;

    /// @brief list with closing reroutes
    FXTable* myClosingRerouteList;

    /// @brief list with destiny probability reroutes
    FXTable* myDestProbRerouteList;

    /// @brief list with route probability reroute
    FXTable* myRouteProbRerouteList;

    /// @brief flag to check if begin an end are valid
    bool myBeginEndValid;

    /// @brief vector with the closingLaneReroutes
    std::vector<GNEClosingLaneReroute> myCopyOfClosingLaneReroutes;

    /// @brief flag to check if closing lane reroutes are valid
    bool myClosingLaneReroutesValid;

    /// @brief vector with a copy of the closingReroutes
    std::vector<GNEClosingReroute> myCopyOfClosingReroutes;

    /// @brief flag to check if closing reroutes are valid
    bool myClosingReroutesValid;

    /// @brief vector with a copy of the destProbReroutes
    std::vector<GNEDestProbReroute> myCopyOfDestProbReroutes;

    /// @brief flag to check if Destiny probability reroutes are valid
    bool myDestProbReroutesValid;

    /// @brief vector with a copy of the routeProbReroutes
    std::vector<GNERouteProbReroute> myCopyOfRouteProbReroutes;

    /// @brief flag to check if route prob reroutes are valid
    bool myRouteProbReroutesValid;

    /// @brief update data of closing lane reroute table
    void updateClosingLaneReroutesTable();

    /// @brief update data of closing reroute table
    void updateClosingReroutesTable();

    /// @brief update data of destiny probabilitry reroute table
    void updateDestProbReroutesTable();

    /// @brief update data of probabilitry reroutes table
    void updateRouteProbReroutesTable();

private:
    /// @brief Invalidated copy constructor.
    GNERerouterIntervalDialog(const GNERerouterIntervalDialog&);

    /// @brief Invalidated assignment operator.
    GNERerouterIntervalDialog& operator=(const GNERerouterIntervalDialog&);
};

#endif
