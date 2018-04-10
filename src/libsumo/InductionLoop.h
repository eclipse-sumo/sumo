/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    InductionLoop.h
/// @author  Michael Behrisch
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef InductionLoop_h
#define InductionLoop_h


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
namespace libsumo {
struct TraCIVehicleData;
}


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class InductionLoop
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class InductionLoop {
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
    static std::vector<libsumo::TraCIVehicleData> getVehicleData(const std::string& detID);

    /** @brief Returns a tree filled with inductive loop instances
     * @return The rtree of inductive loops
     */
    static NamedRTree* getTree();

private:
    static MSInductLoop* getDetector(const std::string& detID);

    /// @brief invalidated standard constructor
    InductionLoop();

    /// @brief invalidated copy constructor
    InductionLoop(const InductionLoop& src);

    /// @brief invalidated assignment operator
    InductionLoop& operator=(const InductionLoop& src);

};
}


#endif

/****************************************************************************/
