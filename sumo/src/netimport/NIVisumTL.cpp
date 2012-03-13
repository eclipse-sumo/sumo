/****************************************************************************/
/// @file    NIVisumTL.cpp
/// @author  Daniel Krajzewicz
/// @date    Thr, 08 May 2003
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
NIVisumTL::NIVisumTL(const std::string& Name, SUMOTime CycleTime,
                     SUMOTime IntermediateTime, bool PhaseDefined)
    : myName(Name), myCycleTime(CycleTime), myIntermediateTime(IntermediateTime),
      myPhaseDefined(PhaseDefined) {}

NIVisumTL::~NIVisumTL() {
    for (std::map<std::string, Phase*>::iterator i = myPhases.begin(); i != myPhases.end(); i++) {
        delete(i->second);
    }
    for (NIVisumTL::SignalGroupMap::iterator k = mySignalGroups.begin();
            k != mySignalGroups.end(); k++) {
        delete(k->second);
    }
}

SUMOTime
NIVisumTL::GetCycleTime() {
    return myCycleTime;
}

SUMOTime
NIVisumTL::GetIntermediateTime() {
    return myIntermediateTime;
}

bool
NIVisumTL::GetPhaseDefined() {
    return myPhaseDefined;
}

std::vector<NBNode*>*
NIVisumTL::GetNodes() {
    return &myNodes;
}


std::map<std::string, NIVisumTL::Phase*>*
NIVisumTL::GetPhases() {
    return &myPhases;
}

void NIVisumTL::AddSignalGroup(const std::string Name, SUMOTime StartTime, SUMOTime EndTime) {
    mySignalGroups[Name] = new NIVisumTL::SignalGroup(Name, StartTime, EndTime);
}

void NIVisumTL::AddPhase(const std::string Name, SUMOTime StartTime, SUMOTime EndTime) {
    myPhases[Name] = new NIVisumTL::Phase(StartTime, EndTime);
}

NIVisumTL::SignalGroup* NIVisumTL::GetSignalGroup(const std::string Name) {
    return (*mySignalGroups.find(Name)).second;
}

void
NIVisumTL::build(NBTrafficLightLogicCont& tlc) {
    for (std::vector<NBNode*>::iterator ni = myNodes.begin(); ni != myNodes.end(); ni++) {
        NBNode* Node = (*ni);
        NBLoadedTLDef* def = new NBLoadedTLDef(Node->getID(), Node);
        tlc.insert(def);
        def->setCycleDuration((unsigned int) myCycleTime);
        // signalgroups
        for (SignalGroupMap::iterator gi = mySignalGroups.begin(); gi != mySignalGroups.end(); gi++) {
            std::string GroupName = (*gi).first;
            NIVisumTL::SignalGroup& SG = *(*gi).second;
            def->addSignalGroup(GroupName);
            def->addToSignalGroup(GroupName, SG.connections());
            def->setSignalYellowTimes(GroupName, myIntermediateTime, myIntermediateTime);
            // phases
            if (myPhaseDefined) {
                for (std::map<std::string, Phase*>::iterator pi = SG.phases().begin(); pi != SG.phases().end(); pi++) {
                    NIVisumTL::Phase& PH = *(*pi).second;
                    def->addSignalGroupPhaseBegin(GroupName, PH.getStartTime(), NBTrafficLightDefinition::TLCOLOR_GREEN);
                    def->addSignalGroupPhaseBegin(GroupName, PH.getEndTime(), NBTrafficLightDefinition::TLCOLOR_RED);
                };
            } else {
                def->addSignalGroupPhaseBegin(GroupName, SG.getStartTime(), NBTrafficLightDefinition::TLCOLOR_GREEN);
                def->addSignalGroupPhaseBegin(GroupName, SG.getEndTime(), NBTrafficLightDefinition::TLCOLOR_RED);
            }
        }
    }
}



/****************************************************************************/

