/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_AllowDisallow.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2017
/// @version $Id$
///
// Dialog for edit rerouters
/****************************************************************************/
#ifndef GNEDialog_AllowDisallow_h
#define GNEDialog_AllowDisallow_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOVehicleClass.h>

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDialog_AllowDisallow
 * @brief Dialog for edit rerouters
 */
class GNEDialog_AllowDisallow : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEDialog_AllowDisallow)

public:
    /// @brief Constructor
    GNEDialog_AllowDisallow(FXApp* app, std::string* allow);

    /// @brief destructor
    ~GNEDialog_AllowDisallow();

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user press a enable/disable button
    long onCmdValueChanged(FXObject*, FXSelector, void*);

    /// @brief event when user press select all VClasses button
    long onCmdSelectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press unselect all VClasses button
    long onCmdUnselectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press select only non road button
    long onCmdSelectOnlyNonRoad(FXObject*, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEDialog_AllowDisallow() {}

    /// @brief pointer to current allowed vehicles
    std::string* myAllow;

    /// @brief copy of current allowed vehicles
    std::string myCopyOfAllow;

    /// @brief select all VClass
    FXButton* mySelectAllVClassButton;

    /// @brief unselect all VCLass
    FXButton* myUnselectAllVClassButton;

    /// @brief select only non-road vehicles
    FXButton* mySelectOnlyNonRoadVClassButton;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

    /// @brief map with the buttons for every VClass
    std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> > myVClassMap;

private:
    /// @brief build VClass
    void buildVClass(FXVerticalFrame* contentsFrame, SUMOVehicleClass vclass, GUIIcon vclassIcon, const std::string& description);

    /// @brief Invalidated copy constructor.
    GNEDialog_AllowDisallow(const GNEDialog_AllowDisallow&);

    /// @brief Invalidated assignment operator.
    GNEDialog_AllowDisallow& operator=(const GNEDialog_AllowDisallow&);
};

#endif
