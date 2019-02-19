/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>
#ifndef SWIGJAVA
#ifndef SWIGPYTHON
#include <microsim/traffic_lights/MSTLLogicControl.h>
#endif
#endif


// ===========================================================================
// class declarations
// ===========================================================================
namespace libsumo {
class VariableWrapper;
}


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
    static std::string getPhaseName(const std::string& tlsID);
    static double getPhaseDuration(const std::string& tlsID);
    static double getNextSwitch(const std::string& tlsID);
    static std::string getParameter(const std::string& tlsID, const std::string& paramName);

    static void setRedYellowGreenState(const std::string& tlsID, const std::string& state);
    static void setPhase(const std::string& tlsID, const int index);
    static void setPhaseName(const std::string& tlsID, const std::string& name);
    static void setProgram(const std::string& tlsID, const std::string& programID);
    static void setPhaseDuration(const std::string& tlsID, const double phaseDuration);
    static void setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCILogic& logic);
    static void setParameter(const std::string& tlsID, const std::string& paramName, const std::string& value);

    LIBSUMO_SUBSCRIPTION_API

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper);

private:
#ifndef SWIGJAVA
#ifndef SWIGPYTHON
    static MSTLLogicControl::TLSLogicVariants& getTLS(const std::string& id);
#endif
#endif

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;

    /// @brief invalidated standard constructor
    TrafficLight() = delete;
};


}


#endif

/****************************************************************************/
