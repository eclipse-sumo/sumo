/****************************************************************************/
/// @file    NIVisumTL.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 07 May 2003
/// @version $Id$
///
// Intermediate class for storing visum traffic lights during their import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVisumTL_h
#define NIVisumTL_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>
#include <netbuild/NBConnectionDefs.h>
#include <netbuild/NBNodeCont.h>
#include <utils/common/SUMOTime.h>

class NBTrafficLightLogicCont;


// ===========================================================================
// class declaration
// ===========================================================================
/**
 * @class NIVisumTL
 * @brief Intermediate class for storing visum traffic lights during their import
 */
class NIVisumTL {
public:
    /** @class TimePeriod
     * @brief A time period with a start and an end time
     */
    class TimePeriod {
    public:
        /// @brief Constructor
        TimePeriod(SUMOTime startTime, SUMOTime endTime) : myStartTime(startTime), myEndTime(endTime) {}

        /// @brief Destructor
        ~TimePeriod() {}

        /// @brief Returns the stored start time
        SUMOTime getStartTime() {
            return myStartTime;
        }

        /// @brief Returns the stored end time
        SUMOTime getEndTime() {
            return myEndTime;
        }

    private:
        /// @brief Start time
        SUMOTime myStartTime;
        /// @brief End time
        SUMOTime myEndTime;
    };



    /** @class Phase
     * @brief A phase
     */
    class Phase : public TimePeriod {
    public:
        /// @brief Constructor
        Phase(SUMOTime startTime, SUMOTime endTime) : NIVisumTL::TimePeriod(startTime, endTime) {}

        /// @brief Destructor
        ~Phase() {}

    };



    /** @class SignalGroup
     * @brief A signal group can be defined either by a time period or by phases
     */
    class SignalGroup : public TimePeriod {
    public:
        /// @brief constructor
        SignalGroup(const std::string& name, SUMOTime startTime, SUMOTime endTime)
            : NIVisumTL::TimePeriod(startTime, endTime), myName(name) {}
        
        /// @brief destructor
        ~SignalGroup() {}

        /// @brief Returns the connections vector
        NBConnectionVector &connections() {
            return myConnections;
        }

        /// @brief Returns the phases map
        std::map<std::string, Phase*> &phases() {
            return myPhases;
        }

    private:
        /// @brief Connections
        NBConnectionVector myConnections;
        /// @brief phases
        std::map<std::string, Phase*> myPhases;
        /// @brief name
        std::string myName;
    };



public:
    /** @brief Constructor
     * @param[in] name The name of the TLS
     * @param[in] cycleTime The cycle time of the TLS
     * @param[in] intermediateTime The name of the TLS
     * @param[in] phaseDefined Whether phases are defined
     */
    NIVisumTL(const std::string& name, SUMOTime cycleTime, SUMOTime intermediateTime,
              bool phaseDefined);

    /// @brief Destructor
    ~NIVisumTL();

    /// @brief Adds a node to control
    void addNode(NBNode *n) {
        myNodes.push_back(n);
    }

    /// @brief Adds a signal group
    void addSignalGroup(const std::string &name, SUMOTime startTime, SUMOTime endTime);

    /// @brief Adds a phase
    void addPhase(const std::string &name, SUMOTime startTime, SUMOTime endTime);

    /// @brief Returns the map of named phases
    std::map<std::string, Phase*>& getPhases() {
        return myPhases;
    }

    /// @brief Returns the named signal group
    SignalGroup& getSignalGroup(const std::string &name);

    /// @brief build the traffic light and add it to the given container
    void build(NBTrafficLightLogicCont& tlc);

private:
    // name of traffic light
    std::string myName;

    // cycle time of traffic light in seconds
    SUMOTime myCycleTime;

    // length of yellow and red-yellow phases
    SUMOTime myIntermediateTime;

    // toogles the usage either of phases or of timeperiods in signalgroups
    bool myPhaseDefined;

    // vector of nodes belonging to this traffic light
    std::vector<NBNode*> myNodes;

    // vector of used phases if phasedefined
    std::map<std::string, Phase*> myPhases;

    // vector of used Signalgroups
    std::map<std::string, SignalGroup*> mySignalGroups;

};


#endif

/****************************************************************************/

