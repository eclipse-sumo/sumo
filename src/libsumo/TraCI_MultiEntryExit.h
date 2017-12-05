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
/// @file    TraCI_MultiEntryExit.h
/// @author  Michael Behrisch
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
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

