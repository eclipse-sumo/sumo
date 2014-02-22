/****************************************************************************/
/// @file    NIVisumTL.cpp
/// @author  Daniel Krajzewicz
/// @date    Thr, 08 May 2003
/// @version $Id$
///
// Intermediate class for storing visum traffic lights during their import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/options/OptionsCont.h>
#include <netbuild/NBLoadedTLDef.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include "NIVisumTL.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVisumTL::NIVisumTL(const std::string& name, SUMOTime cycleTime, SUMOTime offset,
                     SUMOTime intermediateTime, bool phaseDefined)
    : myName(name), myCycleTime(cycleTime), myOffset(offset),
      myIntermediateTime(intermediateTime), myPhaseDefined(phaseDefined)
{}


NIVisumTL::~NIVisumTL() {
    for (std::map<std::string, Phase*>::iterator i = myPhases.begin(); i != myPhases.end(); ++i) {
        delete i->second;
    }
    for (std::map<std::string, SignalGroup*>::iterator i = mySignalGroups.begin(); i != mySignalGroups.end(); ++i) {
        delete i->second;
    }
}


void
NIVisumTL::addSignalGroup(const std::string& name, SUMOTime startTime, SUMOTime endTime, SUMOTime yellowTime) {
    mySignalGroups[name] = new NIVisumTL::SignalGroup(name, startTime, endTime, yellowTime);
}


void
NIVisumTL::addPhase(const std::string& name, SUMOTime startTime, SUMOTime endTime, SUMOTime yellowTime) {
    myPhases[name] = new NIVisumTL::Phase(startTime, endTime, yellowTime);
}


NIVisumTL::SignalGroup&
NIVisumTL::getSignalGroup(const std::string& name) {
    return *mySignalGroups.find(name)->second;
}


void
NIVisumTL::build(NBTrafficLightLogicCont& tlc) {
    for (std::vector<NBNode*>::iterator ni = myNodes.begin(); ni != myNodes.end(); ni++) {
        NBNode* node = (*ni);
        TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(OptionsCont::getOptions().getString("tls.default-type"));
        NBLoadedTLDef* def = new NBLoadedTLDef(node->getID(), node, myOffset, type);
        tlc.insert(def);
        def->setCycleDuration((unsigned int) myCycleTime);
        // signalgroups
        for (std::map<std::string, SignalGroup*>::iterator gi = mySignalGroups.begin(); gi != mySignalGroups.end(); gi++) {
            std::string groupName = (*gi).first;
            NIVisumTL::SignalGroup& SG = *(*gi).second;
            def->addSignalGroup(groupName);
            def->addToSignalGroup(groupName, SG.connections());
            // phases
            SUMOTime yellowTime = -1;
            if (myPhaseDefined) {
                for (std::map<std::string, Phase*>::iterator pi = SG.phases().begin(); pi != SG.phases().end(); pi++) {
                    NIVisumTL::Phase& PH = *(*pi).second;
                    def->addSignalGroupPhaseBegin(groupName, PH.getStartTime(), NBTrafficLightDefinition::TLCOLOR_GREEN);
                    def->addSignalGroupPhaseBegin(groupName, PH.getEndTime(), NBTrafficLightDefinition::TLCOLOR_RED);
                    yellowTime = MAX2(PH.getYellowTime(), yellowTime);
                };
            } else {
                def->addSignalGroupPhaseBegin(groupName, SG.getStartTime(), NBTrafficLightDefinition::TLCOLOR_GREEN);
                def->addSignalGroupPhaseBegin(groupName, SG.getEndTime(), NBTrafficLightDefinition::TLCOLOR_RED);
                yellowTime = MAX2(SG.getYellowTime(), yellowTime);
            }
            // yellowTime can be -1 if not given in the input; it will be "patched" later
            def->setSignalYellowTimes(groupName, myIntermediateTime, yellowTime);
        }
    }
}



/****************************************************************************/

