/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEChange_Attribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 1017
/// @version $Id$
///
// A change in the values of Calibrators in netedit
/****************************************************************************/
#ifndef GNEChange_CalibratorItem_h
#define GNEChange_CalibratorItem_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GNEChange.h"
#include "GNECalibratorRoute.h"
#include "GNECalibratorFlow.h"
#include "GNECalibratorVehicleType.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibrator;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_CalibratorItem
* A change to the network selection
*/
class GNEChange_CalibratorItem : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_CalibratorItem)

public:
    /**@brief Constructor for modifying selection
     * @param[in] calibrator GNECalibrator in which apply changes
     * @param[in] newCalibratorRoutes vector with the new calibratorRoutes of calibrator
     * @param[in] newCalibratorFlows vector with the new calibratorFlows of calibrator
     * @param[in] newCalibratorVehicleTypes vector with the new CalibratorVehicleType of calibrator    
     */
    GNEChange_CalibratorItem(GNECalibrator *calibrator, const std::vector<GNECalibratorRoute>& newCalibratorRoutes, 
                             const std::vector<GNECalibratorFlow>& newCalibratorFlows,
                             const std::vector<GNECalibratorVehicleType>& newCalibratorVehicleTypes);

    /// @brief Destructor
    ~GNEChange_CalibratorItem();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    FXString undoName() const;

    /// @brief get Redo name
    FXString redoName() const;

    /// @brief undo action
    void undo();

    /// @brief redo action
    void redo();
    /// @}


private:
    /// @brief calibrator in which apply changes
    GNECalibrator *myCalibrator;

    /// @brief old calibrator route values
    std::vector<GNECalibratorRoute> myOldCalibratorRoutes;

    /// @brief old calibrator flow values
    std::vector<GNECalibratorFlow> myOldCalibratorFlows;

    /// @brief old calibrator vehicleType values
    std::vector<GNECalibratorVehicleType> myOldCalibratorVehicleTypes;

    /// @brief old calibrator route values
    std::vector<GNECalibratorRoute> myNewCalibratorRoutes;

    /// @brief old calibrator flow values

    std::vector<GNECalibratorFlow> myNewCalibratorFlows;

    /// @brief old calibrator vehicleType values
    std::vector<GNECalibratorVehicleType> myNewCalibratorVehicleTypes;
};

#endif
/****************************************************************************/
