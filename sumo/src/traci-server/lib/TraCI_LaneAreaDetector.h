/****************************************************************************/
/// @file    TraCI_LaneAreaDetector.h
/// @author  Michael Behrisch
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCI_LaneAreaDetector_h
#define TraCI_LaneAreaDetector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSE2Collector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_LaneAreaDetector
 * @brief C++ TraCI client API implementation
 */
class TraCI_LaneAreaDetector {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static int getJamLengthVehicle(const std::string& detID);
    static double getJamLengthMeters(const std::string& detID);
    static double getLastStepMeanSpeed(const std::string& detID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& detID);
    static double getLastStepOccupancy(const std::string& detID);
    static double getPosition(const std::string& detID);
    static std::string getLaneID(const std::string& detID);
    static double getLength(const std::string& detID);
    static int getLastStepVehicleNumber(const std::string& detID);
    static int getLastStepHaltingNumber(const std::string& detID);

private:
    static MSE2Collector* getDetector(const std::string& detID);

    /// @brief invalidated standard constructor
    TraCI_LaneAreaDetector();

    /// @brief invalidated copy constructor
    TraCI_LaneAreaDetector(const TraCI_LaneAreaDetector& src);

    /// @brief invalidated assignment operator
    TraCI_LaneAreaDetector& operator=(const TraCI_LaneAreaDetector& src);

};


#endif

/****************************************************************************/

