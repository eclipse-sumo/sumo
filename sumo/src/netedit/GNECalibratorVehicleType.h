/****************************************************************************/
/// @file    GNECalibratorVehicleType.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// VehicleType used by GNECalibrators
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
#ifndef GNECalibratorVehicleType_h
#define GNECalibratorVehicleType_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNECalibrator;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorVehicleType
 * vehicleType vehicleType used by GNECalibrators
 */
class GNECalibratorVehicleType {
public:


    /// @brief constructor
    GNECalibratorVehicleType(GNECalibrator* calibratorParent);

    /// @brief parameter constructor
    GNECalibratorVehicleType(GNECalibrator* calibratorParent, std::string vehicleTypeID);

    /// @brief destructor
    ~GNECalibratorVehicleType();

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get VehicleTypeID
    std::string getVehicleTypeID() const;

    /**@brief set vehicleType ID
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setVehicleTypeID(std::string vehicleTypeID);

private:
    /// @brief pointer to calibrator parent
    GNECalibrator* myCalibratorParent;

    /// @brief vehicleType ID
    std::string myVehicleTypeID;
};

#endif
/****************************************************************************/
