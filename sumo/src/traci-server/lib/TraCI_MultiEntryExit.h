/****************************************************************************/
/// @file    TraCI_MultiEntryExit.h
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
#ifndef TraCI_MultiEntryExit_h
#define TraCI_MultiEntryExit_h


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
 * @class TraCI_MultiEntryExit
 * @brief C++ TraCI client API implementation
 */
class TraCI_MultiEntryExit {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static int getLastStepVehicleNumber(const std::string& detID);
    static double getLastStepMeanSpeed(const std::string& detID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& detID);
    static int getLastStepHaltingNumber(const std::string& detID);

private:
    static MSE3Collector* getDetector(const std::string& detID);

    /// @brief invalidated standard constructor
    TraCI_MultiEntryExit();

    /// @brief invalidated copy constructor
    TraCI_MultiEntryExit(const TraCI_MultiEntryExit& src);

    /// @brief invalidated assignment operator
    TraCI_MultiEntryExit& operator=(const TraCI_MultiEntryExit& src);

};


#endif

/****************************************************************************/

