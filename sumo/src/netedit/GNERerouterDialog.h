/****************************************************************************/
/// @file    GNERerouterDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// Dialog for edit rerouters
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

    /// @brief get rerouter parent
    GNERerouter* getRerouterParent() const;

    /// @brief check if a interval exists
    bool findInterval(SUMOReal begin, SUMOReal end) const;

    /// @brief check begin and end of an new interval
    bool checkInterval(SUMOReal begin, SUMOReal end) const;

    /// @brief check if begin and end of an existent interval can be modified
    bool checkModifyInterval(SUMOReal oldBegin, SUMOReal oldEnd, SUMOReal newBegin, SUMOReal newEnd) const;

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

    /// @brief pointer to rerouter parent
    GNERerouter* myRerouterParent;

    /// @brief button for add new interval
    FXButton* myAddInterval;

    /// @brief list with intervals
    FXTable* myIntervalList;

    /// @brief set with a copy of rerouter intervals
    std::vector<GNERerouterInterval> myCopyOfRerouterIntervals;

    /// @brief update data table
    void updateIntervalTable();

private:
    /// @brief Invalidated copy constructor.
    GNERerouterDialog(const GNERerouterDialog&);

    /// @brief Invalidated assignment operator.
    GNERerouterDialog& operator=(const GNERerouterDialog&);
};

#endif
