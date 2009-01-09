/****************************************************************************/
/// @file    NIVisumTL.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 07 May 2003
/// @version $Id$
///
// Intermediate class for storing visum traffic lights during their import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
class NIVisumTL
{
public:
    /// Timeperiod with starttime and endtime
    class TimePeriod
    {
    public:
        // constructor
        TimePeriod();
        TimePeriod(SUMOTime StartTime, SUMOTime EndTime);
        // destructor
        ~TimePeriod();
        void SetStartTime(SUMOTime StartTime);
        void SetEndTime(SUMOTime EndTime);
        SUMOTime GetStartTime();
        SUMOTime GetEndTime();
    private:
        SUMOTime myStartTime;
        SUMOTime myEndTime;
    };

    // phase
    class Phase : public TimePeriod
    {
    public:
        Phase();
        Phase(SUMOTime StartTime, SUMOTime EndTime);
        ~Phase();
    };

    // Vectors
    typedef std::vector<NBNode*> NodeVector;
    typedef std::map<std::string, Phase*> PhaseMap;

    // SignalGroup
    // a Signal Group can be defined either by a time period or by phases
    class SignalGroup : public TimePeriod
    {
    public:
        // constructor
        SignalGroup(const std::string &Name, SUMOTime StartTime, SUMOTime EndTime);
        // destructor
        ~SignalGroup();
    private:
        NBConnectionVector myConnections;
        PhaseMap myPhases;
        std::string myName;
    public:
        NBConnectionVector* GetConnections();
        PhaseMap* GetPhases();
        std::string GetName();
    };

    // SignalGroupVector
    typedef std::map<std::string, SignalGroup*> SignalGroupMap;
public:
    // constructor
    NIVisumTL();
    NIVisumTL(const std::string &Name, SUMOTime CycleTime, SUMOTime IntermediateTime,
              bool PhaseDefined);
    // destructor
    ~NIVisumTL();
    // nodes and phases
    NodeVector* GetNodes();
    PhaseMap* GetPhases();
    SignalGroupMap* GetSignalGroups();
    // set
    void SetPhaseDefined(bool PhaseDefined);
    void SetCycleTime(SUMOTime CylcleTime);
    void SetIntermediateTime(SUMOTime IntermediateTime);
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
    void build(NBTrafficLightLogicCont &tlc);
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
    NodeVector myNodes;

    // vector of used phases if phasedefined
    PhaseMap myPhases;

    // vector of used Signalgroups
    SignalGroupMap mySignalGroups;

};


#endif

/****************************************************************************/

