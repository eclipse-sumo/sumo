/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_CalibratorItem.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2017
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

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibrator;
class GNECalibratorFlow;
class GNECalibratorRoute;
class GNECalibratorVehicleType;

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
    /**@brief Constructor
    * @param[in] calibratorFlow calibrator flow to be added or removed
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_CalibratorItem(GNECalibratorFlow* calibratorFlow, bool forward);

    /**@brief Constructor
    * @param[in] calibratorRoute calibrator route to be added or removed
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_CalibratorItem(GNECalibratorRoute* calibratorRoute, bool forward);

    /**@brief Constructor
    * @param[in] calibratorVehicleType calibrator vehicle type to be added or removed
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_CalibratorItem(GNECalibratorVehicleType* calibratorVehicleType, bool forward);

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
    /// @brief modified calibrator flow
    GNECalibratorFlow* myCalibratorFlow;

    /// @brief modified calibrator route
    GNECalibratorRoute* myCalibratorRoute;

    /// @brief modified calibrator vehicle type
    GNECalibratorVehicleType* myCalibratorVehicleType;
};

#endif
/****************************************************************************/
