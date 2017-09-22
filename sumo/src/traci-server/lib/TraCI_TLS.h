/****************************************************************************/
/// @file    TraCI_TLS.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCI_TLS_h
#define TraCI_TLS_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <traci-server/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSRoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TraCI_TLS
* @brief C++ TraCI client API implementation
*/
class TraCI_TLS {
public:

    static std::vector<std::string> getIDList();
    static int getIDCount();
    static std::string getRedYellowGreenState(const std::string& tlsID);
    static std::vector<TraCILogic> getCompleteRedYellowGreenDefinition(const std::string& tlsID);
    static std::vector<std::string> getControlledLanes(const std::string& tlsID);
    static std::vector<std::vector<TraCILink> > getControlledLinks(const std::string& tlsID);
    static std::string getProgram(const std::string& tlsID);
    static int getPhase(const std::string& tlsID);
    static SUMOTime gePhaseDuration(const std::string& tlsID);
    static SUMOTime getNextSwitch(const std::string& tlsID);
    static std::string getParameter(const std::string& tlsID, const std::string& paramName);

    static void setRedYellowGreenState(const std::string& tlsID, const std::string& state);
    static void setPhase(const std::string& tlsID, const int index);
    static void setProgram(const std::string& tlsID, const std::string& programID);
    static void setPhaseDuration(const std::string& tlsID, const SUMOTime phaseDuration);
    static void setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCILogic& logic);

private:
    static MSTLLogicControl::TLSLogicVariants& getTLS(const std::string& id);

    /// @brief invalidated standard constructor
    TraCI_TLS();

    /// @brief invalidated copy constructor
    TraCI_TLS(const TraCI_TLS& src);

    /// @brief invalidated assignment operator
    TraCI_TLS& operator=(const TraCI_TLS& src);
};


#endif

/****************************************************************************/

