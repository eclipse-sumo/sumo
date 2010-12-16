/****************************************************************************/
/// @file    MSTLLogicControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that stores and controls tls and switching of their programs
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <algorithm>
#include <cassert>
#include <iterator>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"
#include "MSTLLogicControl.h"
#include "MSOffTrafficLightLogic.h"
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSTLLogicControl::TLSLogicVariants - methods
 * ----------------------------------------------------------------------- */
MSTLLogicControl::TLSLogicVariants::TLSLogicVariants() throw()
        : myCurrentProgram(0) {
}


MSTLLogicControl::TLSLogicVariants::~TLSLogicVariants() throw() {
    std::map<std::string, MSTrafficLightLogic *>::const_iterator j;
    for (std::map<std::string, MSTrafficLightLogic *>::iterator j=myVariants.begin(); j!=myVariants.end(); ++j) {
        delete(*j).second;
    }
    for (std::vector<OnSwitchAction*>::iterator i=mySwitchActions.begin(); i!=mySwitchActions.end(); ++i) {
        delete *i;
    }
}


bool
MSTLLogicControl::TLSLogicVariants::checkOriginalTLS() const throw() {
    bool hadErrors = false;
    for (std::map<std::string, MSTrafficLightLogic *>::const_iterator j=myVariants.begin(); j!=myVariants.end(); ++j) {
        const MSTrafficLightLogic::Phases &phases = (*j).second->getPhases();
        unsigned int linkNo = (unsigned int)(*j).second->getLinks().size();
        bool hadProgramErrors = false;
        for (MSTrafficLightLogic::Phases::const_iterator i=phases.begin(); i!=phases.end(); ++i) {
            if ((*i)->getState().length()<linkNo) {
                hadProgramErrors = true;
            }
        }
        if (hadProgramErrors) {
            MsgHandler::getErrorInstance()->inform("Mismatching phase size in tls '" + (*j).second->getID() + "', program '" + (*j).first + "'.");
            hadErrors = true;
        }
    }
    return !hadErrors;
}


void
MSTLLogicControl::TLSLogicVariants::saveInitialStates() {
    myOriginalLinkStates = myCurrentProgram->collectLinkStates();
}


bool
MSTLLogicControl::TLSLogicVariants::addLogic(const std::string &programID,
        MSTrafficLightLogic*logic,
        bool netWasLoaded,
        bool isNewDefault) throw(ProcessError) {
    if (myVariants.find(programID)!=myVariants.end()) {
        return false;
    }
    // assert the links are set
    if (netWasLoaded) {
        // this one has not yet its links set
        if (myCurrentProgram==0) {
            throw ProcessError("No initial signal plan loaded for tls '" + logic->getID() + "'.");
        }
        logic->adaptLinkInformationFrom(*myCurrentProgram);
        if (logic->getLinks().size()>logic->getPhase(0).getState().size()) {
            throw ProcessError("Mismatching phase size in tls '" + logic->getID() + "', program '" + programID + "'.");
        }
    }
    // add to the list of active
    if (myVariants.size()==0||isNewDefault) {
        myCurrentProgram = logic;
    }
    // add to the list of logic
    myVariants[programID] = logic;
    if (myVariants.size()==1||isNewDefault) {
        logic->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
        executeOnSwitchActions();
    }
    return true;
}


MSTrafficLightLogic*
MSTLLogicControl::TLSLogicVariants::getLogic(const std::string &programID) const {
    if (myVariants.find(programID)==myVariants.end()) {
        return 0;
    }
    return myVariants.find(programID)->second;
}


MSTrafficLightLogic*
MSTLLogicControl::TLSLogicVariants::getLogicInstantiatingOff(MSTLLogicControl &tlc,
        const std::string &programID) {
    if (myVariants.find(programID)==myVariants.end()) {
        if (programID=="off") {
            // build an off-tll if this switch indicates it
            if (!addLogic("off", new MSOffTrafficLightLogic(tlc, myCurrentProgram->getID()), true, false)) {
                // inform the user if this fails
                throw ProcessError("Could not build an off-state for tls '" + myCurrentProgram->getID() + "'.");
            }
        } else {
            // inform the user about a missing logic
            throw ProcessError("Can not switch tls '" + myCurrentProgram->getID() + "' to program '" + programID + "';\n The program is not known.");
        }
    }
    return getLogic(programID);
}


void
MSTLLogicControl::TLSLogicVariants::addSwitchCommand(OnSwitchAction *c) {
    mySwitchActions.push_back(c);
}


std::vector<MSTrafficLightLogic*>
MSTLLogicControl::TLSLogicVariants::getAllLogics() const {
    std::vector<MSTrafficLightLogic*> ret;
    std::map<std::string, MSTrafficLightLogic*>::const_iterator i;
    for (i=myVariants.begin(); i!=myVariants.end(); ++i) {
        ret.push_back((*i).second);
    }
    return ret;
}


bool
MSTLLogicControl::TLSLogicVariants::isActive(const MSTrafficLightLogic *tl) const {
    return tl==myCurrentProgram;
}


MSTrafficLightLogic*
MSTLLogicControl::TLSLogicVariants::getActive() const {
    return myCurrentProgram;
}


void
MSTLLogicControl::TLSLogicVariants::switchTo(MSTLLogicControl &tlc, const std::string &programID) {
    // set the found wished sub-program as this tls' current one
    myCurrentProgram = getLogicInstantiatingOff(tlc, programID);
    // in the case we have switched to an off-state, we'll reset the links
    if (programID=="off") {
        myCurrentProgram->resetLinkStates(myOriginalLinkStates);
    }
}


void
MSTLLogicControl::TLSLogicVariants::executeOnSwitchActions() const {
    for (std::vector<OnSwitchAction*>::const_iterator i=mySwitchActions.begin(); i!=mySwitchActions.end(); ++i) {
        (*i)->execute();
    }
}


void
MSTLLogicControl::TLSLogicVariants::addLink(MSLink *link, MSLane *lane, unsigned int pos) throw() {
    for (std::map<std::string, MSTrafficLightLogic *>::iterator i=myVariants.begin(); i!=myVariants.end(); ++i) {
        (*i).second->addLink(link, lane, pos);
    }
}



/* -------------------------------------------------------------------------
 * method definitions for the Switching Procedures
 * ----------------------------------------------------------------------- */
/* -------------------------------------------------------------------------
 * method definitions for WAUTSwitchProcedure
 * ----------------------------------------------------------------------- */
unsigned int
MSTLLogicControl::WAUTSwitchProcedure::getGSPValue(const MSTrafficLightLogic &logic) const {
    std::string val = logic.getParameterValue("GSP");
    if (val.length()==0) {
        return 0;
    }
    return TplConvert<char>::_2int(val.c_str());
}


bool
MSTLLogicControl::WAUTSwitchProcedure::isPosAtGSP(SUMOTime currentTime, const MSTrafficLightLogic &logic) throw() {
    SUMOTime gspTime = TIME2STEPS(getGSPValue(logic)) % logic.getDefaultCycleTime();
    SUMOTime programTime = logic.getOffsetFromIndex(logic.getCurrentPhaseIndex())
        + (logic.getCurrentPhaseDef().duration - (logic.getNextSwitchTime() - currentTime));
    return gspTime==programTime;
}


SUMOTime
MSTLLogicControl::WAUTSwitchProcedure::getDiffToStartOfPhase(MSTrafficLightLogic &logic, SUMOTime toTime) throw() {
    unsigned int stepOfMyPos = logic.getIndexFromOffset(toTime);
    SUMOTime startOfPhase = logic.getOffsetFromIndex(stepOfMyPos);
    assert(toTime >= startOfPhase);
    return toTime - startOfPhase;
}


void
MSTLLogicControl::WAUTSwitchProcedure::switchToPos(SUMOTime simStep, MSTrafficLightLogic &logic, SUMOTime toTime) {
    unsigned int stepTo = logic.getIndexFromOffset(toTime);
    SUMOTime diff = getDiffToStartOfPhase(logic, toTime);
    const MSPhaseDefinition &phase = logic.getPhase(stepTo);
    SUMOTime leftDuration = phase.duration - diff;
    logic.changeStepAndDuration(myControl, simStep, stepTo, leftDuration);
}



/* -------------------------------------------------------------------------
 * method definitions for WAUTSwitchProcedure_JustSwitch
 * ----------------------------------------------------------------------- */
MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::WAUTSwitchProcedure_JustSwitch(
    MSTLLogicControl &control, WAUT &waut,
    MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron) throw()
        : MSTLLogicControl::WAUTSwitchProcedure(control, waut, from, to, synchron) {}


MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::~WAUTSwitchProcedure_JustSwitch() throw() {}


bool
MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::trySwitch(SUMOTime) throw() {
    return true;
}



/* -------------------------------------------------------------------------
 * method definitions for WAUTSwitchProcedure_GSP
 * ----------------------------------------------------------------------- */
MSTLLogicControl::WAUTSwitchProcedure_GSP::WAUTSwitchProcedure_GSP(
    MSTLLogicControl &control, WAUT &waut,
    MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron) throw()
        : MSTLLogicControl::WAUTSwitchProcedure(control, waut, from, to, synchron) {}


MSTLLogicControl::WAUTSwitchProcedure_GSP::~WAUTSwitchProcedure_GSP() throw() {}


bool
MSTLLogicControl::WAUTSwitchProcedure_GSP::trySwitch(SUMOTime step) throw() {
    // switch to the next programm if the GSP is reached
    if (isPosAtGSP(step, *myFrom)) {
        // adapt program's state
        if (mySwitchSynchron) {
            adaptLogic(step);
        } else {
            switchToPos(step, *myTo, TIME2STEPS(getGSPValue(*myTo)));
        }
        // switch to destination program
        return true;
    }
    // do not switch, yet
    return false;
}


void
MSTLLogicControl::WAUTSwitchProcedure_GSP::adaptLogic(SUMOTime step) throw() {
    SUMOTime gspTo = TIME2STEPS(getGSPValue(*myTo));
    unsigned int stepTo = myTo->getIndexFromOffset(gspTo);
    SUMOTime cycleTimeTo = myTo->getDefaultCycleTime();
    if (gspTo == cycleTimeTo) {
        gspTo=0;
    }

    SUMOTime currentPosTo = myTo->getOffsetFromIndex(myTo->getCurrentPhaseIndex());
    currentPosTo += (myTo->getCurrentPhaseDef().duration - (myTo->getNextSwitchTime() - step));
    SUMOTime diff = getDiffToStartOfPhase(*myTo, gspTo);

    SUMOTime deltaToStretch = 0;
    if (gspTo >= currentPosTo) {
        deltaToStretch = (gspTo - currentPosTo);
    } else {
        deltaToStretch = (cycleTimeTo - currentPosTo + gspTo);
    }
    unsigned int newdur = (unsigned int) myTo->getPhase(stepTo).duration - diff + deltaToStretch;
    myTo->changeStepAndDuration(myControl, step, stepTo, newdur);
}



/* -------------------------------------------------------------------------
 * method definitions for WAUTSwitchProcedure_Stretch
 * ----------------------------------------------------------------------- */
MSTLLogicControl::WAUTSwitchProcedure_Stretch::WAUTSwitchProcedure_Stretch(
    MSTLLogicControl &control, WAUT &waut,
    MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron) throw()
        : MSTLLogicControl::WAUTSwitchProcedure(control, waut, from, to, synchron) {}


MSTLLogicControl::WAUTSwitchProcedure_Stretch::~WAUTSwitchProcedure_Stretch() throw() {}


bool
MSTLLogicControl::WAUTSwitchProcedure_Stretch::trySwitch(SUMOTime step) throw() {
    // switch to the next programm if the GSP is reached
    if (isPosAtGSP(step, *myFrom)) {
        // adapt program's state
        if (mySwitchSynchron) {
            adaptLogic(step);
        } else {
            switchToPos(step, *myTo, TIME2STEPS(getGSPValue(*myTo)));
        }
        // switch to destination program
        return true;
    }
    // do not switch, yet
    return false;
}


void
MSTLLogicControl::WAUTSwitchProcedure_Stretch::adaptLogic(SUMOTime step) throw() {
    SUMOTime gspTo = TIME2STEPS(getGSPValue(*myTo));
    SUMOTime cycleTime = myTo->getDefaultCycleTime();
    // the position, where the logic has to be after synchronisation
    SUMOTime posAfterSyn = myTo->getPhaseIndexAtTime(step);
    // calculate the difference, that has to be equalized
    SUMOTime deltaToCut = 0;
    if (posAfterSyn < gspTo) {
        deltaToCut = posAfterSyn + cycleTime - gspTo;
    } else {
        deltaToCut =  posAfterSyn - gspTo;
    }
    // test, wheter cutting of the Signalplan is possible
    SUMOTime deltaPossible = 0;
    int areasNo = getStretchAreaNo(myTo);
    for (int i=0; i<areasNo; i++) {
        StretchBereichDef def = getStretchBereichDef(myTo, i+1);
        assert(def.end >= def.begin) ;
        deltaPossible += TIME2STEPS(def.end - def.begin);
    }
    int stretchUmlaufAnz = (int) TplConvert<char>::_2SUMOReal(myTo->getParameterValue("StretchUmlaufAnz").c_str());
    deltaPossible = stretchUmlaufAnz * deltaPossible;
    if ((deltaPossible > deltaToCut)&&(deltaToCut < (cycleTime / 2))) {
        cutLogic(step, gspTo, deltaToCut);
    } else {
        SUMOTime deltaToStretch = (cycleTime - deltaToCut) % cycleTime;
        stretchLogic(step, gspTo, deltaToStretch);
    }
}


void
MSTLLogicControl::WAUTSwitchProcedure_Stretch::cutLogic(SUMOTime step, SUMOTime startPos, SUMOTime allCutTime) throw() {
    unsigned int actStep = myTo->getIndexFromOffset(startPos);
    // switches to startPos and cuts this phase, if there is a "Bereich"
    int areasNo = getStretchAreaNo(myTo);
    SUMOTime toCut = 0;
    for (int i=0; i<areasNo; i++) {
        StretchBereichDef def = getStretchBereichDef(myTo, i+1);
        SUMOTime begin = TIME2STEPS(def.begin);
        unsigned int end = TIME2STEPS(def.end);
        size_t stepOfBegin = myTo->getIndexFromOffset(begin);
        if (stepOfBegin == actStep)	{
            if (begin < startPos) {
                toCut = end - startPos;
            } else {
                toCut = end - begin;
            }
            toCut = MIN2(allCutTime, toCut);
            allCutTime = allCutTime - toCut;
        }
    }
    SUMOTime remainingDur = myTo->getPhase(actStep).duration - getDiffToStartOfPhase(*myTo, startPos);
    SUMOTime newDur = remainingDur - toCut;
    myTo->changeStepAndDuration(myControl,step,actStep,newDur);

    // changes the duration of all other phases
    int currStep = (actStep + 1) % myTo->getPhases().size();
    while (allCutTime > 0) {
        for (int i=currStep; i<(int) myTo->getPhases().size(); i++) {
            SUMOTime beginOfPhase = myTo->getOffsetFromIndex(i);
            SUMOTime durOfPhase = myTo->getPhase(i).duration;
            SUMOTime endOfPhase = beginOfPhase + durOfPhase;
            for (int i=0; i<areasNo; i++) {
                StretchBereichDef def = getStretchBereichDef(myTo, i+1);
                SUMOTime begin = TIME2STEPS(def.begin);
                SUMOTime end = TIME2STEPS(def.end);
                if ((beginOfPhase <= begin) && (endOfPhase >= end)) {
                    SUMOTime maxCutOfPhase = MIN2(end - begin, allCutTime);
                    allCutTime = allCutTime - maxCutOfPhase;
                    durOfPhase = durOfPhase - maxCutOfPhase;
                }
            }
            myTo->addOverridingDuration(durOfPhase);
        }
        currStep = 0;
    }
}

void
MSTLLogicControl::WAUTSwitchProcedure_Stretch::stretchLogic(SUMOTime step, SUMOTime startPos, SUMOTime allStretchTime) throw() {
    unsigned int currStep = myTo->getIndexFromOffset(startPos);
    SUMOTime durOfPhase = myTo->getPhase(currStep).duration;
    SUMOTime remainingStretchTime = allStretchTime;
    SUMOTime StretchTimeOfPhase = 0;
    unsigned int stretchUmlaufAnz = (unsigned int) TplConvert<char>::_2SUMOReal(myTo->getParameterValue("StretchUmlaufAnz").c_str());
    SUMOReal facSum = 0;
    int areasNo = getStretchAreaNo(myTo);
    for (int x=0; x<areasNo; x++) {
        StretchBereichDef def = getStretchBereichDef(myTo, x+1);
        facSum += def.fac;
    }
    facSum *= stretchUmlaufAnz;

    //switch to startPos and stretch this phase, if there is a end of "bereich" between startpos and end of phase
    SUMOTime diffToStart = getDiffToStartOfPhase(*myTo, startPos);
    for (int x=0; x<areasNo; x++) {
        StretchBereichDef def = getStretchBereichDef(myTo, x+1);
        SUMOTime end = TIME2STEPS(def.end);
        SUMOTime endOfPhase = (startPos + durOfPhase - diffToStart);
        if (end <= endOfPhase && end >= startPos) {
            SUMOReal fac = def.fac;
            SUMOReal actualfac = fac / facSum;
            facSum = facSum - fac;
            StretchTimeOfPhase = TIME2STEPS((int) (STEPS2TIME(remainingStretchTime) * actualfac + 0.5));
            remainingStretchTime = allStretchTime - StretchTimeOfPhase;
        }
    }
    durOfPhase = durOfPhase - diffToStart + StretchTimeOfPhase;
    myTo->changeStepAndDuration(myControl,step,currStep,durOfPhase);

    currStep = (currStep+1) % myTo->getPhases().size();
    // stretch all other phases, if there is a "bereich"
    while (remainingStretchTime > 0) {
        for (unsigned int i=currStep; i<myTo->getPhases().size() && remainingStretchTime > 0; i++) {
            durOfPhase = myTo->getPhase(i).duration;
            SUMOTime beginOfPhase = myTo->getOffsetFromIndex(i);
            SUMOTime endOfPhase = beginOfPhase + durOfPhase;
            for (int j=0; j<areasNo && remainingStretchTime > 0; j++) {
                StretchBereichDef def = getStretchBereichDef(myTo, j+1);
                SUMOTime end = TIME2STEPS(def.end);
                SUMOReal fac = def.fac;
                if ((beginOfPhase <= end) && (endOfPhase >= end)) {
                    SUMOReal actualfac = fac / facSum;
                    StretchTimeOfPhase = TIME2STEPS((int) (STEPS2TIME(remainingStretchTime) * actualfac + 0.5));
                    facSum -= fac;
                    durOfPhase += StretchTimeOfPhase;
                    remainingStretchTime -= StretchTimeOfPhase;
                }
            }
            myTo->addOverridingDuration(durOfPhase);
        }
        currStep = 0;
    }
}

int
MSTLLogicControl::WAUTSwitchProcedure_Stretch::getStretchAreaNo(MSTrafficLightLogic *from) const throw() {
    int no = 0;
    while (from->getParameterValue("B" + toString(no+1) + ".begin")!="") {
        no++;
    }
    return no;
}


MSTLLogicControl::WAUTSwitchProcedure_Stretch::StretchBereichDef
MSTLLogicControl::WAUTSwitchProcedure_Stretch::getStretchBereichDef(MSTrafficLightLogic *from, int index) const throw() {
    StretchBereichDef def;
    def.begin = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".begin").c_str());
    def.end = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".end").c_str());
    def.fac = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".factor").c_str());
    return def;
}



/* -------------------------------------------------------------------------
 * method definitions for MSTLLogicControl
 * ----------------------------------------------------------------------- */
MSTLLogicControl::MSTLLogicControl() throw()
        : myNetWasLoaded(false) {}


MSTLLogicControl::~MSTLLogicControl() throw() {
    // delete tls
    for (std::map<std::string, TLSLogicVariants*>::const_iterator i=myLogics.begin(); i!=myLogics.end(); ++i) {
        delete(*i).second;
    }
    // delete WAUTs
    for (std::map<std::string, WAUT*>::const_iterator i=myWAUTs.begin(); i!=myWAUTs.end(); ++i) {
        delete(*i).second;
    }
}


void
MSTLLogicControl::setTrafficLightSignals(SUMOTime t) const throw() {
    for (std::map<std::string, TLSLogicVariants*>::const_iterator i=myLogics.begin(); i!=myLogics.end(); ++i) {
        (*i).second->getActive()->setTrafficLightSignals(t);
    }
}


std::vector<MSTrafficLightLogic*>
MSTLLogicControl::getAllLogics() const throw() {
    std::vector<MSTrafficLightLogic*> ret;
    std::map<std::string, TLSLogicVariants*>::const_iterator i;
    for (i=myLogics.begin(); i!=myLogics.end(); ++i) {
        std::vector<MSTrafficLightLogic*> s = (*i).second->getAllLogics();
        copy(s.begin(), s.end(), back_inserter(ret));
    }
    return ret;
}

MSTLLogicControl::TLSLogicVariants &
MSTLLogicControl::get(const std::string &id) const throw(InvalidArgument) {
    std::map<std::string, TLSLogicVariants*>::const_iterator i = myLogics.find(id);
    if (i==myLogics.end()) {
        throw InvalidArgument("The tls '" + id + "' is not known.");
    }
    return *(*i).second;
}


MSTrafficLightLogic*
    MSTLLogicControl::get(const std::string &id, const std::string &programID) const throw() {
    std::map<std::string, TLSLogicVariants*>::const_iterator i = myLogics.find(id);
    if (i==myLogics.end()) {
        return 0;
    }
    return (*i).second->getLogic(programID);
}


std::vector<std::string>
MSTLLogicControl::getAllTLIds() const throw() {
    std::vector<std::string> ret;
    for (std::map<std::string, TLSLogicVariants*>::const_iterator i=myLogics.begin(); i!=myLogics.end(); ++i) {
        ret.push_back((*i).first);
    }
    return ret;
}


bool
MSTLLogicControl::add(const std::string &id, const std::string &programID,
                      MSTrafficLightLogic *logic, bool newDefault) throw(ProcessError) {
    if (myLogics.find(id)==myLogics.end()) {
        myLogics[id] = new TLSLogicVariants();
    }
    std::map<std::string, TLSLogicVariants*>::iterator i = myLogics.find(id);
    TLSLogicVariants *tlmap = (*i).second;
    return tlmap->addLogic(programID, logic, myNetWasLoaded, newDefault);
}


bool
MSTLLogicControl::knows(const std::string &id) const throw() {
    std::map<std::string, TLSLogicVariants*>::const_iterator i = myLogics.find(id);
    if (i==myLogics.end()) {
        return false;
    }
    return true;
}


bool
MSTLLogicControl::closeNetworkReading() throw() {
    bool hadErrors = false;
    for (std::map<std::string, TLSLogicVariants*>::iterator i=myLogics.begin(); i!=myLogics.end(); ++i) {
        hadErrors |= !(*i).second->checkOriginalTLS();
        (*i).second->saveInitialStates();
    }
    myNetWasLoaded = true;
    return !hadErrors;
}


bool
MSTLLogicControl::isActive(const MSTrafficLightLogic *tl) const throw() {
    std::map<std::string, TLSLogicVariants*>::const_iterator i = myLogics.find(tl->getID());
    if (i==myLogics.end()) {
        return false;
    }
    return (*i).second->isActive(tl);
}


MSTrafficLightLogic*
    MSTLLogicControl::getActive(const std::string &id) const throw() {
    std::map<std::string, TLSLogicVariants*>::const_iterator i = myLogics.find(id);
    if (i==myLogics.end()) {
        return 0;
    }
    return (*i).second->getActive();
}


void
MSTLLogicControl::switchTo(const std::string &id, const std::string &programID) throw(ProcessError) {
    // try to get the tls program definitions
    std::map<std::string, TLSLogicVariants*>::iterator i = myLogics.find(id);
    // handle problems
    if (i==myLogics.end()) {
        throw ProcessError("Could not switch tls '" + id + "' to program '" + programID + "': No such tls exists.");
    }
    (*i).second->switchTo(*this, programID);
}


void
MSTLLogicControl::addWAUT(SUMOTime refTime, const std::string &id,
                          const std::string &startProg) throw(InvalidArgument) {
    // check whether the waut was already defined
    if (myWAUTs.find(id)!=myWAUTs.end()) {
        // report an error if so
        throw InvalidArgument("Waut '" + id + "' was already defined.");
    }
    WAUT *w = new WAUT;
    w->id = id;
    w->refTime = refTime;
    w->startProg = startProg;
    myWAUTs[id] = w;
}


void
MSTLLogicControl::addWAUTSwitch(const std::string &wautid,
                                SUMOTime when, const std::string &to) throw(InvalidArgument) {
    // try to get the waut
    if (myWAUTs.find(wautid)==myWAUTs.end()) {
        // report an error if the waut is not known
        throw InvalidArgument("Waut '" + wautid + "' was not yet defined.");
    }
    // build and save the waut switch definition
    WAUTSwitch s;
    s.to = to;
    s.when = (myWAUTs[wautid]->refTime + when) % 86400000;
    myWAUTs[wautid]->switches.push_back(s);
}


void
MSTLLogicControl::addWAUTJunction(const std::string &wautid,
                                  const std::string &tls,
                                  const std::string &proc,
                                  bool synchron) throw(InvalidArgument, ProcessError) {
    // try to get the waut
    if (myWAUTs.find(wautid)==myWAUTs.end()) {
        // report an error if the waut is not known
        throw InvalidArgument("Waut '" + wautid + "' was not yet defined.");
    }
    // try to get the tls to switch
    if (myLogics.find(tls)==myLogics.end()) {
        // report an error if the tls is not known
        throw InvalidArgument("TLS '" + tls + "' to switch in WAUT '" + wautid + "' was not yet defined.");
    }
    WAUTJunction j;
    j.junction = tls;
    j.procedure = proc;
    j.synchron = synchron;
    myWAUTs[wautid]->junctions.push_back(j);

    std::string initProg = myWAUTs[wautid]->startProg;
    std::vector<WAUTSwitch>::const_iterator first = myWAUTs[wautid]->switches.end();
    SUMOTime minExecTime = -1;
    for (std::vector<WAUTSwitch>::const_iterator i=myWAUTs[wautid]->switches.begin(); i!=myWAUTs[wautid]->switches.end(); ++i) {
        if ((*i).when>MSNet::getInstance()->getCurrentTimeStep()&&(minExecTime==-1||(*i).when<minExecTime)) {
            minExecTime = (*i).when;
            first = i;
        }
        if (first!=myWAUTs[wautid]->switches.begin()) {
            initProg = (*(first-1)).to;
        }
    }
    // activate the first one
    switchTo(tls, initProg);
}


void
MSTLLogicControl::closeWAUT(const std::string &wautid) throw(InvalidArgument) {
    // try to get the waut
    if (myWAUTs.find(wautid)==myWAUTs.end()) {
        // report an error if the waut is not known
        throw InvalidArgument("Waut '" + wautid + "' was not yet defined.");
    }
    WAUT *w = myWAUTs.find(wautid)->second;
    std::string initProg = myWAUTs[wautid]->startProg;
    // get the switch to be performed as first
    std::vector<WAUTSwitch>::const_iterator first = w->switches.end();
    SUMOTime minExecTime = -1;
    for (std::vector<WAUTSwitch>::const_iterator i=w->switches.begin(); i!=w->switches.end(); ++i) {
        if ((*i).when>MSNet::getInstance()->getCurrentTimeStep()&&(minExecTime==-1||(*i).when<minExecTime)) {
            minExecTime = (*i).when;
            first = i;
        }
    }
    // activate the first one
    if (first!=w->switches.end()) {
        std::vector<WAUTSwitch>::const_iterator mbegin = w->switches.begin();
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new SwitchInitCommand(*this, wautid, (unsigned int)distance(mbegin, first)),
            (*first).when, MSEventControl::NO_CHANGE);
    }
    /*
    // set the current program to all junctions
    for(std::vector<WAUTJunction>::const_iterator i=w->junctions.begin(); i!=w->junctions.end(); ++i) {
        switchTo((*i).junction, initProg);
    }
    */
}


SUMOTime
MSTLLogicControl::initWautSwitch(MSTLLogicControl::SwitchInitCommand &cmd) {
    const std::string &wautid = cmd.getWAUTID();
    unsigned int &index = cmd.getIndex();
    WAUTSwitch s = myWAUTs[wautid]->switches[index];
    for (std::vector<WAUTJunction>::iterator i=myWAUTs[wautid]->junctions.begin(); i!=myWAUTs[wautid]->junctions.end(); ++i) {
        // get the current program and the one to instantiate
        TLSLogicVariants *vars = myLogics.find((*i).junction)->second;
        MSTrafficLightLogic *from = vars->getActive();
        MSTrafficLightLogic *to = vars->getLogicInstantiatingOff(*this, s.to);
        WAUTSwitchProcedure *proc = 0;
        if ((*i).procedure=="GSP") {
            proc = new WAUTSwitchProcedure_GSP(*this, *myWAUTs[wautid], from, to, (*i).synchron);
        } else if ((*i).procedure=="Stretch") {
            proc = new WAUTSwitchProcedure_Stretch(*this, *myWAUTs[wautid], from, to, (*i).synchron);
        } else {
            proc = new WAUTSwitchProcedure_JustSwitch(*this, *myWAUTs[wautid], from, to, (*i).synchron);
        }

        WAUTSwitchProcess p;
        p.junction = (*i).junction;
        p.proc = proc;
        p.from = from;
        p.to = to;

        myCurrentlySwitched.push_back(p);
    }
    index++;
    if (index==(int) myWAUTs[wautid]->switches.size()) {
        return 0;
    }
    return myWAUTs[wautid]->switches[index].when - MSNet::getInstance()->getCurrentTimeStep();
}


void
MSTLLogicControl::check2Switch(SUMOTime step) throw() {
    for (std::vector<WAUTSwitchProcess>::iterator i=myCurrentlySwitched.begin(); i!=myCurrentlySwitched.end();) {
        const WAUTSwitchProcess &proc = *i;
        if (proc.proc->trySwitch(step)) {
            delete proc.proc;
            switchTo((*i).to->getID(), (*i).to->getProgramID());
            i = myCurrentlySwitched.erase(i);
        } else {
            ++i;
        }
    }
}


std::pair<SUMOTime, MSPhaseDefinition>
MSTLLogicControl::getPhaseDef(const std::string &tlid) const throw() {
    MSTrafficLightLogic *tl = getActive(tlid);
    return std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), tl->getCurrentPhaseDef());
}



/****************************************************************************/

