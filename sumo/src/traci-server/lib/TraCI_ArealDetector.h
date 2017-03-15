/****************************************************************************/
/// @file    TraCI_ArealDetector.h
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
#ifndef TraCI_ArealDetector_h
#define TraCI_ArealDetector_h


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
// class definitions
// ===========================================================================
/**
 * @class TraCI_ArealDetector
 * @brief C++ TraCI client API implementation
 */
class TraCI_ArealDetector {
public:
    static std::vector<std::string> getIDList();
    static int getJamLengthVehicle(const std::string& laneID);
    static double getJamLengthMeters(const std::string& laneID);

private:
    /// @brief invalidated standard constructor
    TraCI_ArealDetector();

    /// @brief invalidated copy constructor
    TraCI_ArealDetector(const TraCI_ArealDetector& src);

    /// @brief invalidated assignment operator
    TraCI_ArealDetector& operator=(const TraCI_ArealDetector& src);

};


#endif

/****************************************************************************/

