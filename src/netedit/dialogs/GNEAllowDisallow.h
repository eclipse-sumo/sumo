/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAllowDisallow.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2017
/// @version $Id$
///
// Dialog for edit allow and disallow attributes
/****************************************************************************/
#ifndef GNEAllowDisallow_h
#define GNEAllowDisallow_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/common/SUMOVehicleClass.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrier;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAllowDisallow
 * @brief Dialog for edit rerouters
 */
class GNEAllowDisallow : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEAllowDisallow)

public:
    /// @brief Constructor
    GNEAllowDisallow(GNEViewNet* viewNet, GNEAttributeCarrier* AC);

    /// @brief destructor
    ~GNEAllowDisallow();

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user press a enable/disable button
    long onCmdValueChanged(FXObject*, FXSelector, void*);

    /// @brief event when user press select all VClasses button
    long onCmdSelectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press unselect all VClasses button
    long onCmdUnselectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press select only road button
    long onCmdSelectOnlyRoad(FXObject*, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEAllowDisallow() {}

    /// @pointer to viewNet
    GNEViewNet* myViewNet;

    /// @brief edited AC
    GNEAttributeCarrier* myAC;

    /// @brief select all VClass
    FXButton* mySelectAllVClassButton;

    /// @brief unselect all VCLass
    FXButton* myUnselectAllVClassButton;

    /// @brief select only road vehicles
    FXButton* mySelectOnlyRoadVClassButton;

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
    GNEAllowDisallow(const GNEAllowDisallow&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAllowDisallow& operator=(const GNEAllowDisallow&) = delete;
};

#endif
