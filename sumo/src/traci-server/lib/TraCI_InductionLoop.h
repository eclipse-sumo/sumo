/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    TraCI_InductionLoop.h
/// @author  Michael Behrisch
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef TraCI_InductionLoop_h
#define TraCI_InductionLoop_h


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
class MSInductLoop;
struct TraCIVehicleData;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_InductionLoop
 * @brief C++ TraCI client API implementation
 */
class TraCI_InductionLoop {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static double getPosition(const std::string& detID);
    static std::string getLaneID(const std::string& detID);
    static int getLastStepVehicleNumber(const std::string& detID);
    static double getLastStepMeanSpeed(const std::string& detID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& detID);
    static double getLastStepOccupancy(const std::string& detID);
    static double getLastStepMeanLength(const std::string& detID);
    static double getTimeSinceDetection(const std::string& detID);
    static std::vector<TraCIVehicleData> getVehicleData(const std::string& detID);

private:
    static MSInductLoop* getDetector(const std::string& detID);

    /// @brief invalidated standard constructor
    TraCI_InductionLoop();

    /// @brief invalidated copy constructor
    TraCI_InductionLoop(const TraCI_InductionLoop& src);

    /// @brief invalidated assignment operator
    TraCI_InductionLoop& operator=(const TraCI_InductionLoop& src);

};


#endif

/****************************************************************************/

