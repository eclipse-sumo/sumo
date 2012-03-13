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



    // SignalGroupVector
    typedef std::map<std::string, SignalGroup*> SignalGroupMap;
public:
    // constructor
    NIVisumTL(const std::string& Name, SUMOTime CycleTime, SUMOTime IntermediateTime,
              bool PhaseDefined);
    // destructor
    ~NIVisumTL();
    // nodes and phases
    std::vector<NBNode*>* GetNodes();
    std::map<std::string, Phase*>* GetPhases();
    // get
    bool GetPhaseDefined();
    SUMOTime GetCycleTime();
    SUMOTime GetIntermediateTime();
    SUMOTime GetName();
    SignalGroup* GetSignalGroup(const std::string Name);
    // add
    void AddSignalGroup(const std::string Name, SUMOTime StartTime, SUMOTime EndTime);
    void AddPhase(const std::string Name, SUMOTime StartTime, SUMOTime EndTime);
    // build the trafficlight
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
    SignalGroupMap mySignalGroups;

};


#endif

/****************************************************************************/

