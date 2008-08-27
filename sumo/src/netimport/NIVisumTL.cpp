/****************************************************************************/
/// @file    NIVisumTL.cpp
/// @author  Daniel Krajzewicz
/// @date    Thr, 08 May 2003
/// @version $Id:NIVisumTL.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Intermediate class for storing visum traffic lights during their import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <netbuild/NBLoadedTLDef.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include "NIVisumTL.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods from NIVisumTL::TimePeriod
 * ----------------------------------------------------------------------- */
NIVisumTL::TimePeriod::TimePeriod()
{
    myEndTime = 0;
    myStartTime = 0;
}

NIVisumTL::TimePeriod::TimePeriod(SUMOTime StartTime, SUMOTime EndTime)
{
    myStartTime = StartTime;
    myEndTime = EndTime;
}

NIVisumTL::TimePeriod::~TimePeriod()
{}

SUMOTime
NIVisumTL::TimePeriod::GetEndTime()
{
    return myEndTime;
}

SUMOTime
NIVisumTL::TimePeriod::GetStartTime()
{
    return myStartTime;
}

void
NIVisumTL::TimePeriod::SetEndTime(SUMOTime EndTime)
{
    myEndTime = EndTime;
}

void
NIVisumTL::TimePeriod::SetStartTime(SUMOTime StartTime)
{
    myStartTime = StartTime;
}

/* -------------------------------------------------------------------------
 * methods from NIVisumTL::TimePeriod
 * ----------------------------------------------------------------------- */

NIVisumTL::Phase::Phase()
        : NIVisumTL::TimePeriod()
{}

NIVisumTL::Phase::Phase(SUMOTime StartTime, SUMOTime EndTime)
        : NIVisumTL::TimePeriod(StartTime, EndTime)
{}

NIVisumTL::Phase::~Phase()
{}

/* -------------------------------------------------------------------------
 * methods from NIVisumTL::SignalGroup
 * ----------------------------------------------------------------------- */
NIVisumTL::SignalGroup::SignalGroup(const std::string &Name,
                                    SUMOTime StartTime, SUMOTime EndTime)
        : NIVisumTL::TimePeriod(StartTime, EndTime), myName(Name)
{}


NIVisumTL::SignalGroup::~SignalGroup()
{}

std::string NIVisumTL::SignalGroup::GetName()
{
    return myName;
}


NBConnectionVector*
NIVisumTL::SignalGroup::GetConnections()
{
    return &myConnections;
}


NIVisumTL::PhaseMap*
NIVisumTL::SignalGroup::GetPhases()
{
    return &myPhases;
}


/* -------------------------------------------------------------------------
* methods from NIVisumTL
* ----------------------------------------------------------------------- */
NIVisumTL::NIVisumTL()
{}


NIVisumTL::NIVisumTL(const std::string &Name, SUMOTime CycleTime,
                     SUMOTime IntermediateTime, bool PhaseDefined)
        : myName(Name), myCycleTime(CycleTime), myIntermediateTime(IntermediateTime),
        myPhaseDefined(PhaseDefined)
{}

NIVisumTL::~NIVisumTL()
{
    for (NIVisumTL::PhaseMap::iterator i=myPhases.begin();
            i!=myPhases.end(); i++) {
        delete(i->second);
    }
    for (NIVisumTL::SignalGroupMap::iterator k=mySignalGroups.begin();
            k!=mySignalGroups.end(); k++) {
        delete(k->second);
    }
}

SUMOTime
NIVisumTL::GetCycleTime()
{
    return myCycleTime;
}

SUMOTime
NIVisumTL::GetIntermediateTime()
{
    return myIntermediateTime;
}

bool
NIVisumTL::GetPhaseDefined()
{
    return myPhaseDefined;
}

NIVisumTL::NodeVector*
NIVisumTL::GetNodes()
{
    return &myNodes;
}


NIVisumTL::PhaseMap*
NIVisumTL::GetPhases()
{
    return &myPhases;
}

NIVisumTL::SignalGroupMap* NIVisumTL::GetSignalGroups()
{
    return &mySignalGroups;
}

void NIVisumTL::SetCycleTime(SUMOTime CycleTime)
{
    myCycleTime = CycleTime;
}

void NIVisumTL::SetIntermediateTime(SUMOTime IntermediateTime)
{
    myIntermediateTime = IntermediateTime;
}

void NIVisumTL::SetPhaseDefined(bool PhaseDefined)
{
    myPhaseDefined = PhaseDefined;
}

void NIVisumTL::AddSignalGroup(const std::string Name, SUMOTime StartTime, SUMOTime EndTime)
{
    mySignalGroups[Name] = new NIVisumTL::SignalGroup(Name, StartTime, EndTime);
}

void NIVisumTL::AddPhase(const std::string Name, SUMOTime StartTime, SUMOTime EndTime)
{
    myPhases[Name] = new NIVisumTL::Phase(StartTime, EndTime);
}

NIVisumTL::SignalGroup* NIVisumTL::GetSignalGroup(const std::string Name)
{
    return (*mySignalGroups.find(Name)).second;
}

void
NIVisumTL::build(NBTrafficLightLogicCont &tlc)
{
    for (NodeVector::iterator ni = myNodes.begin(); ni != myNodes.end(); ni++) {
        NBNode *Node = (*ni);
        NBLoadedTLDef *def = new NBLoadedTLDef(Node->getID(), Node);
        tlc.insert(def);
        def->setCycleDuration((size_t) myCycleTime);
        // signalgroups
        for (SignalGroupMap::iterator gi = mySignalGroups.begin(); gi != mySignalGroups.end(); gi++) {
            std::string GroupName = (*gi).first;
            NIVisumTL::SignalGroup &SG = *(*gi).second;
            def->addSignalGroup(GroupName);
            def->addToSignalGroup(GroupName, *SG.GetConnections());
            def->setSignalYellowTimes(GroupName, myIntermediateTime, myIntermediateTime);
            // phases
            if (myPhaseDefined) {
                for (PhaseMap::iterator pi = SG.GetPhases()->begin(); pi!= SG.GetPhases()->end(); pi++) {
                    NIVisumTL::Phase &PH = *(*pi).second;
                    def->addSignalGroupPhaseBegin(GroupName, PH.GetStartTime(), NBTrafficLightDefinition::TLCOLOR_GREEN);
                    def->addSignalGroupPhaseBegin(GroupName, PH.GetEndTime(), NBTrafficLightDefinition::TLCOLOR_RED);
                };
            } else {
                def->addSignalGroupPhaseBegin(GroupName, SG.GetStartTime(), NBTrafficLightDefinition::TLCOLOR_GREEN);
                def->addSignalGroupPhaseBegin(GroupName, SG.GetEndTime(), NBTrafficLightDefinition::TLCOLOR_RED);
            }
        }
    }
}



/****************************************************************************/

