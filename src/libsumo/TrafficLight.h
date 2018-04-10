/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TrafficLight.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef TrafficLight_h
#define TrafficLight_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSRoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TrafficLight
* @brief C++ TraCI client API implementation
*/
namespace libsumo {
class TrafficLight {
public:

    static std::vector<std::string> getIDList();
    static int getIDCount();
    static std::string getRedYellowGreenState(const std::string& tlsID);
    static std::vector<TraCILogic> getCompleteRedYellowGreenDefinition(const std::string& tlsID);
    static std::vector<std::string> getControlledJunctions(const std::string& tlsID);
    static std::vector<std::string> getControlledLanes(const std::string& tlsID);
    static std::vector<std::vector<TraCILink> > getControlledLinks(const std::string& tlsID);
    static std::string getProgram(const std::string& tlsID);
    static int getPhase(const std::string& tlsID);
    static SUMOTime getPhaseDuration(const std::string& tlsID);
    static SUMOTime getNextSwitch(const std::string& tlsID);
    static std::string getParameter(const std::string& tlsID, const std::string& paramName);

    static void setRedYellowGreenState(const std::string& tlsID, const std::string& state);
    static void setPhase(const std::string& tlsID, const int index);
    static void setProgram(const std::string& tlsID, const std::string& programID);
    static void setPhaseDuration(const std::string& tlsID, const SUMOTime phaseDuration);
    static void setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCILogic& logic);
    static void setParameter(const std::string& tlsID, const std::string& paramName, const std::string& value);

private:
    static MSTLLogicControl::TLSLogicVariants& getTLS(const std::string& id);

    /// @brief invalidated standard constructor
    TrafficLight();

    /// @brief invalidated copy constructor
    TrafficLight(const TrafficLight& src);

    /// @brief invalidated assignment operator
    TrafficLight& operator=(const TrafficLight& src);
};
}


#endif

/****************************************************************************/
