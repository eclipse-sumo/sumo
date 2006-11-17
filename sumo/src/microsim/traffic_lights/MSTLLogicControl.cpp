//---------------------------------------------------------------------------//
//                        MSTLLogicControl.cpp -
//  A class that holds all traffic light logics used
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.25  2006/11/17 09:09:58  dkrajzew
// warnings removed
//
// Revision 1.24  2006/11/16 13:56:45  dkrajzew
// warnings removed
//
// Revision 1.23  2006/11/14 06:41:15  dkrajzew
// tls tracker now support switches between logics
//
// Revision 1.22  2006/10/12 09:28:14  dkrajzew
// patched building under windows
//
// Revision 1.21  2006/10/12 08:07:17  dkrajzew
// added an error handling for adding a tls definition for an unknown tls
//
// Revision 1.20  2006/09/20 09:18:33  jringel
// stretch: some bugs removed
//
// Revision 1.19  2006/09/08 12:31:28  jringel
// Stretch added
//
// Revision 1.18  2006/08/04 11:47:48  jringel
// WAUTSwitchProcedure_GSP::adaptLogic(...) added
//
// Revision 1.17  2006/05/29 12:57:44  dkrajzew
// added a reference to the tlcontrols to switch procedures
//
// Revision 1.16  2006/05/23 10:29:55  dkrajzew
// added retrieval of the waut reference time
//
// Revision 1.15  2006/05/15 06:01:51  dkrajzew
// added the possibility to stretch/change the current phase and consecutive phases
//
// Revision 1.14  2006/05/05 09:53:55  jringel
// *** empty log message ***
//
// Revision 1.12  2006/04/11 10:59:07  dkrajzew
// all structures now return their id via getID()
//
// Revision 1.11  2006/03/17 08:57:51  dkrajzew
// changed the Event-interface (execute now gets the current simulation time, event handlers are non-static)
//
// Revision 1.10  2006/03/08 13:13:20  dkrajzew
// debugging
//
// Revision 1.9  2006/02/27 12:05:32  dkrajzew
// WAUTs and WAUT-API added
//
// Revision 1.8  2006/02/23 11:27:57  dkrajzew
// tls may have now several programs
//
// Revision 1.7  2005/11/09 06:36:48  dkrajzew
// changing the LSA-API: MSEdgeContinuation added; changed the calling API
//
// Revision 1.6  2005/10/10 11:56:09  dkrajzew
// reworking the tls-API: made tls-control non-static; made net an element of traffic lights
//
// Revision 1.5  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:09:53  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:22:19  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.1  2004/11/23 10:18:42  dkrajzew
// all traffic lights moved to microsim/traffic_lights
//
// Revision 1.3  2003/08/04 11:42:35  dkrajzew
// missing deletion of traffic light logics on closing a network added
//
// Revision 1.2  2003/07/30 09:16:10  dkrajzew
// a better (correct?) processing of yellow lights added; debugging
//
// Revision 1.1  2003/06/05 16:08:36  dkrajzew
// traffic lights are no longer junction-bound; a separate control is necessary
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <algorithm>
#include <cassert>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"
#include "MSTLLogicControl.h"
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * method definitions for the Switching Procedures
 * ----------------------------------------------------------------------- */

SUMOReal
MSTLLogicControl::WAUTSwitchProcedure::getGSPValue(MSTrafficLightLogic *from) const
{
    string val = from->getParameterValue("GSP");
    if(val.length()==0) {
        return -1;
    }
    return TplConvert<char>::_2SUMOReal(val.c_str());
}

bool
MSTLLogicControl::WAUTSwitchProcedure::isPosAtGSP(SUMOTime step, MSSimpleTrafficLightLogic *testLogic)
{
	MSSimpleTrafficLightLogic *givenLogic = (MSSimpleTrafficLightLogic*) testLogic;
	size_t CycleTime = givenLogic->getCycleTime();
	SUMOReal gspFrom = getGSPValue(givenLogic);
	///get the position of the given signalprogramm at the actual simulationsecond
	size_t posFrom = givenLogic -> getPosition (step);

	if (gspFrom == CycleTime)	{
		gspFrom = 0;
	}
	///compare the position of the given programm with the GSP (GSP = "GünstigerSchaltPunkt")
	if (gspFrom == posFrom) {
		return true;
	}
	else {
		return false;
	}
}

size_t
MSTLLogicControl::WAUTSwitchProcedure::getDiffToStartOfPhase(MSSimpleTrafficLightLogic *givenLogic, size_t pos)
{
	MSSimpleTrafficLightLogic *myLogic = givenLogic;
	size_t myPos = pos;
	size_t stepOfMyPos = myLogic->getStepFromPos(myPos);
	size_t startOfPhase = myLogic->getPosFromStep(stepOfMyPos);
	MSPhaseDefinition myPhase = myLogic->getPhaseFromStep(stepOfMyPos);
	size_t durOfPhase = myPhase.duration;

	assert (myPos >= startOfPhase);
	size_t diff = myPos - startOfPhase;
	assert (diff <= durOfPhase);
	return diff;
}

void
MSTLLogicControl::WAUTSwitchProcedure::switchToPos(SUMOTime simStep, MSSimpleTrafficLightLogic *givenLogic, size_t pos)
{
	MSSimpleTrafficLightLogic *myLogic = givenLogic;
	size_t posTo = pos;
	size_t stepTo = myLogic->getStepFromPos(posTo);
	size_t diff = getDiffToStartOfPhase (myLogic, posTo);
	MSPhaseDefinition myPhase = myLogic->getPhaseFromStep(stepTo);
	size_t dur = myPhase.duration - diff;
	myLogic->changeStepAndDuration( myControl ,simStep, stepTo, dur);
}



MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::WAUTSwitchProcedure_JustSwitch(
        MSTLLogicControl &control, WAUT &waut,
        MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron)
    : MSTLLogicControl::WAUTSwitchProcedure(control, waut, from, to, synchron)
{
}


MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::~WAUTSwitchProcedure_JustSwitch()
{
}


bool
MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::trySwitch(SUMOTime /*step*/)
{
    return true;
}






MSTLLogicControl::WAUTSwitchProcedure_GSP::WAUTSwitchProcedure_GSP(
        MSTLLogicControl &control, WAUT &waut,
        MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron)
    : MSTLLogicControl::WAUTSwitchProcedure(control, waut, from, to, synchron)
{
}


MSTLLogicControl::WAUTSwitchProcedure_GSP::~WAUTSwitchProcedure_GSP()
{
}


bool
MSTLLogicControl::WAUTSwitchProcedure_GSP::trySwitch(SUMOTime step)
{
	MSSimpleTrafficLightLogic *LogicFrom = (MSSimpleTrafficLightLogic*) myFrom;
	MSSimpleTrafficLightLogic *LogicTo = (MSSimpleTrafficLightLogic*) myTo;
	SUMOReal posTo = 0;
	///switch to the next programm if the GSP is reached
	if (isPosAtGSP(step, LogicFrom)==true) {
		posTo = getGSPValue(myTo);
		if (mySwitchSynchron) {
			adaptLogic(step);
		}
		else {
			switchToPos(step, LogicTo, (size_t) posTo);
		}
		return true;
	}
	return false;
}


void
MSTLLogicControl::WAUTSwitchProcedure_GSP::adaptLogic(SUMOTime step)
{
	SUMOTime simStep = step;
	MSSimpleTrafficLightLogic *LogicTo = (MSSimpleTrafficLightLogic*) myTo;
	SUMOReal gspTo = getGSPValue(myTo);
	size_t stepTo = LogicTo->getStepFromPos((size_t) gspTo);
	size_t cycleTimeTo = LogicTo->getCycleTime();
	// gets the actual position from the myToLogic
	size_t actPosTo = LogicTo->getPosition(simStep);
	size_t deltaToStretch= 0;
	size_t diff = getDiffToStartOfPhase(LogicTo, (size_t) gspTo);
	if (gspTo >= actPosTo) {
		deltaToStretch = (size_t) (gspTo - actPosTo);
	}
	else {
		deltaToStretch = (size_t) (cycleTimeTo - actPosTo + gspTo);
	}
	size_t newdur = LogicTo->getPhaseFromStep(stepTo).duration - diff + deltaToStretch;
	LogicTo->changeStepAndDuration (myControl, simStep, stepTo, newdur);
}










MSTLLogicControl::WAUTSwitchProcedure_Stretch::WAUTSwitchProcedure_Stretch(
        MSTLLogicControl &control, WAUT &waut,
        MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron)
    : MSTLLogicControl::WAUTSwitchProcedure(control, waut, from, to, synchron)
{
}


MSTLLogicControl::WAUTSwitchProcedure_Stretch::~WAUTSwitchProcedure_Stretch()
{
}


bool
MSTLLogicControl::WAUTSwitchProcedure_Stretch::trySwitch(SUMOTime step)
{
	MSSimpleTrafficLightLogic *LogicFrom = (MSSimpleTrafficLightLogic*) myFrom;
	MSSimpleTrafficLightLogic *LogicTo = (MSSimpleTrafficLightLogic*) myTo;
	SUMOReal posTo = 0;
	///switch to the next programm if the GSP is reached
	if (isPosAtGSP(step, LogicFrom)==true) {
		posTo = getGSPValue(myTo);
		if (mySwitchSynchron) {
			adaptLogic(step, posTo);
		}
		else {
			switchToPos(step, LogicTo, (size_t) posTo);
		}
		return true;
	}
	return false;
}


void
MSTLLogicControl::WAUTSwitchProcedure_Stretch::adaptLogic(SUMOTime step, SUMOReal position)
{
	MSSimpleTrafficLightLogic *LogicTo = (MSSimpleTrafficLightLogic*) myTo;
	size_t cycleTime = LogicTo->getCycleTime();
	// the position, in which the logic has to be switched
	size_t startPos = (size_t) position;
	// this is the position, where the Logic have to be after synchronisation
	size_t posAfterSyn = LogicTo->getPosition(step);

	// switch the toLogic to the startPosition
	// fehlt!!!!
	// erfolgt in cutLogic und/oder stretchLogic!


	// calculate the difference, that has to be equalized
	size_t deltaToCut = 0;
	if (posAfterSyn < startPos) {
		deltaToCut = posAfterSyn + cycleTime - startPos;
	}
	else deltaToCut =  posAfterSyn - startPos;
	// test, wheter cutting of the Signalplan is possible
	size_t deltaPossible = 0;
	int noBereiche = getStretchBereicheNo(myTo);
	for(int i=0; i<noBereiche; i++) {
        StretchBereichDef def = getStretchBereichDef(myTo, i+1);
	    assert (def.end >= def.begin) ;
	    deltaPossible = deltaPossible + (size_t) (def.end - def.begin);
	}
	int stretchUmlaufAnz = (int) TplConvert<char>::_2SUMOReal(LogicTo->getParameterValue("StretchUmlaufAnz").c_str());
	deltaPossible = stretchUmlaufAnz * deltaPossible;
	if ((deltaPossible > deltaToCut)&&(deltaToCut < (cycleTime / 2))) {
		cutLogic(step, startPos, deltaToCut);
	}
	else {
		size_t deltaToStretch = cycleTime - deltaToCut;
		if (deltaToStretch == cycleTime) {
			deltaToStretch = 0;
		}
		stretchLogic(step, startPos, deltaToStretch);
	}

}


void
MSTLLogicControl::WAUTSwitchProcedure_Stretch::cutLogic(SUMOTime step, size_t startPos, size_t deltaToCut)
{
	MSSimpleTrafficLightLogic *LogicTo = (MSSimpleTrafficLightLogic*) myTo;
	size_t actStep = LogicTo->getStepNo();
	size_t allCutTime = deltaToCut;
	// cuts the actual phase, if there is a "Bereich"
	int noBereiche = getStretchBereicheNo(myTo);
    for(int i=0; i<noBereiche; i++) {
        StretchBereichDef def = getStretchBereichDef(myTo, i+1);
        size_t begin = (size_t) def.begin;
		size_t end = (size_t) def.end;
		size_t stepOfBegin = LogicTo->getStepFromPos(begin);
		if (stepOfBegin == actStep)	{
			size_t toCut = 0;
			if (begin < startPos) {
				toCut = end - startPos;
			}
			else {
				toCut = end - begin;
			}
			if  (allCutTime < toCut) {
				toCut = allCutTime;
			}
			size_t oldDur = LogicTo->getPhaseFromStep(actStep).duration;
			size_t newDur = oldDur - toCut;
			allCutTime = allCutTime - toCut;
			myTo->changeStepAndDuration(myControl,step,actStep,newDur);
		}
    }
	// changes the duration of all other phases
	int currStep = actStep + 1;
	if (currStep == (int) LogicTo->getPhases().size() ) {
		currStep = 0;
	}
	while(allCutTime > 0) {
		for(int i=currStep; i<(int) LogicTo->getPhases().size(); i++) {
			size_t beginOfPhase = LogicTo->getPosFromStep(i);
			size_t durOfPhase = LogicTo->getPhaseFromStep(i).duration;
			size_t endOfPhase = beginOfPhase + durOfPhase;
			for(int i=0; i<noBereiche; i++) {
				StretchBereichDef def = getStretchBereichDef(myTo, i+1);
				size_t begin = (size_t) def.begin;
				size_t end = (size_t) def.end;
				if((beginOfPhase <= begin) && (endOfPhase >= begin)) {
					size_t maxCutOfPhase = end - begin;
					if (allCutTime< maxCutOfPhase) {
						maxCutOfPhase = allCutTime;
					}
					allCutTime = allCutTime - maxCutOfPhase;
					durOfPhase = durOfPhase - maxCutOfPhase;
				}
			}
			LogicTo->addOverridingDuration(durOfPhase);
		}
	currStep = 0;
	}
}

void
MSTLLogicControl::WAUTSwitchProcedure_Stretch::stretchLogic(SUMOTime step, size_t startPos, size_t deltaToStretch)
{
	MSSimpleTrafficLightLogic *LogicTo = (MSSimpleTrafficLightLogic*) myTo;
	size_t currPos = startPos;
	size_t currStep = LogicTo->getStepFromPos(currPos);
	size_t durOfPhase = LogicTo->getPhaseFromStep(currStep).duration;
	size_t allStretchTime = deltaToStretch;
	size_t remainingStretchTime = allStretchTime;
	int StretchTimeOfPhase = 0;
	size_t stretchUmlaufAnz = (size_t) TplConvert<char>::_2SUMOReal(LogicTo->getParameterValue("StretchUmlaufAnz").c_str());
	float facSum = 0;
	int noBereiche = getStretchBereicheNo(myTo);
	for(int x=0; x<noBereiche; x++) {
				StretchBereichDef def = getStretchBereichDef(myTo, x+1);
				size_t fac = (size_t) def.fac;
				facSum = facSum + fac;
	}
	facSum = facSum * stretchUmlaufAnz;


	//switch to startPos and stretch this phase, if there is a end of "bereich" between startpos and end of phase
	size_t diffToStart = getDiffToStartOfPhase(LogicTo, currPos);
	for(int x=0; x<noBereiche; x++) {
		StretchBereichDef def = getStretchBereichDef(myTo, x+1);
		size_t end = (size_t) def.end;
		size_t endOfPhase = (size_t) (currPos + durOfPhase - diffToStart);
		if (end <= endOfPhase && end >= currPos) {
			float fac = def.fac;
			float actualfac = fac / facSum;
			facSum = facSum - fac;
			StretchTimeOfPhase = (int)((float)remainingStretchTime * actualfac + 0.5);
			remainingStretchTime = allStretchTime - StretchTimeOfPhase;
		}
	}
	durOfPhase = durOfPhase - diffToStart + StretchTimeOfPhase;
	myTo->changeStepAndDuration(myControl,step,currStep,durOfPhase);

	currStep ++;
	if (currStep >= LogicTo->getPhases().size() ) {
		currStep = 0;
	}

	// stretch all other phases, if there is a "bereich"
	while(remainingStretchTime > 0) {
		for(int i=currStep; i<(int) LogicTo->getPhases().size(); i++) {
			durOfPhase = LogicTo->getPhaseFromStep(i).duration;
			size_t beginOfPhase = LogicTo->getPosFromStep(i);
			size_t endOfPhase = beginOfPhase + durOfPhase;
			for(int j=0; j<noBereiche; j++) {
				StretchBereichDef def = getStretchBereichDef(myTo, j+1);
				size_t end = (size_t) def.end;
				SUMOReal fac = def.fac;
				if((beginOfPhase <= end) && (endOfPhase >= end)) {
					float actualfac = fac / facSum;
					StretchTimeOfPhase = (int)((float)remainingStretchTime * actualfac + 0.5) ;
					facSum = facSum - fac;
					durOfPhase = durOfPhase + StretchTimeOfPhase;
					remainingStretchTime = remainingStretchTime - StretchTimeOfPhase;
				}
			}
			LogicTo->addOverridingDuration(durOfPhase);
		}
	currStep = 0;
	}
}

int
MSTLLogicControl::WAUTSwitchProcedure_Stretch::getStretchBereicheNo(MSTrafficLightLogic *from) const
{
    int no = 0;
    while(from->getParameterValue("B" + toString(no+1) + ".begin")!="") {
        no++;
    }
    return no;
}


MSTLLogicControl::WAUTSwitchProcedure_Stretch::StretchBereichDef
MSTLLogicControl::WAUTSwitchProcedure_Stretch::getStretchBereichDef(MSTrafficLightLogic *from, int index) const
{
    StretchBereichDef def;
    def.begin = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".begin").c_str());
    def.end = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".end").c_str());
    def.fac = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".factor").c_str());
    return def;
}


/* -------------------------------------------------------------------------
 * method definitions for MSTLLogicControl
 * ----------------------------------------------------------------------- */
MSTLLogicControl::MSTLLogicControl()
    : myNetWasLoaded(false)
{
}


MSTLLogicControl::~MSTLLogicControl()
{
    std::map<std::string, TLSLogicVariants>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); ++i) {
        const TLSLogicVariants &vars = (*i).second;
        std::map<std::string, MSTrafficLightLogic *>::const_iterator j;
        for(j=vars.ltVariants.begin(); j!=vars.ltVariants.end(); ++j) {
            delete (*j).second;
        }
    }
}


void
MSTLLogicControl::maskRedLinks()
{
    for_each(myActiveLogics.begin(), myActiveLogics.end(), mem_fun(&MSTrafficLightLogic::maskRedLinks));
}


void
MSTLLogicControl::maskYellowLinks()
{
    for_each(myActiveLogics.begin(), myActiveLogics.end(), mem_fun(&MSTrafficLightLogic::maskYellowLinks));
}


std::vector<MSTrafficLightLogic*>
MSTLLogicControl::getAllLogics() const
{
    std::vector<MSTrafficLightLogic*> ret;
    std::map<std::string, TLSLogicVariants>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); ++i) {
        const TLSLogicVariants &vars = (*i).second;
        std::map<std::string, MSTrafficLightLogic *>::const_iterator j;
        for(j=vars.ltVariants.begin(); j!=vars.ltVariants.end(); ++j) {
            ret.push_back((*j).second);
        }
    }
    return ret;
}

const MSTLLogicControl::TLSLogicVariants &
MSTLLogicControl::get(const std::string &id) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        throw 1;
    }
    return (*i).second;
}


MSTrafficLightLogic *
MSTLLogicControl::get(const std::string &id, const std::string &subid) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return 0;
    }
    const std::map<std::string, MSTrafficLightLogic *> &vars = (*i).second.ltVariants;
    std::map<std::string, MSTrafficLightLogic *>::const_iterator j = vars.find(subid);
    if(j==vars.end()) {
        return 0;
    }
    return (*j).second;
}


bool
MSTLLogicControl::add(const std::string &id, const std::string &subID,
                      MSTrafficLightLogic *logic, bool newDefault)
{
    if(myLogics.find(id)==myLogics.end()) {
        TLSLogicVariants var;
        var.defaultTL = 0;
        myLogics[id] = var;
    }
    std::map<std::string, TLSLogicVariants>::iterator i = myLogics.find(id);
    TLSLogicVariants &tlmap = (*i).second;
    if(tlmap.ltVariants.find(subID)!=tlmap.ltVariants.end()) {
        return false;
    }
    // assert the liks are set
    if(myNetWasLoaded) {
        // this one has not yet its links set
        if(tlmap.defaultTL==0) {
            MsgHandler::getErrorInstance()->inform("No initial signal plan loaded for tls '" + id + "'.");
            throw ProcessError();
        }
        logic->adaptLinkInformationFrom(*(tlmap.defaultTL));
    }
    // add to the list of active
    if(tlmap.ltVariants.size()==0) {
        tlmap.defaultTL = logic;
        myActiveLogics.push_back(logic);
    } else if(newDefault) {
        std::vector<MSTrafficLightLogic*>::iterator i =
            find(myActiveLogics.begin(), myActiveLogics.end(), tlmap.defaultTL);
        assert(i!=myActiveLogics.end());
        *i = logic;
        tlmap.defaultTL = logic;
    }
    // add to the list of logic
    tlmap.ltVariants[subID] = logic;
    return true;
}


bool
MSTLLogicControl::knows(const std::string &id) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return false;
    }
    return true;
}


void
MSTLLogicControl::markNetLoadingClosed()
{
    myNetWasLoaded = true;
}


bool
MSTLLogicControl::isActive(const MSTrafficLightLogic *tl) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(tl->getID());
    if(i==myLogics.end()) {
        return false;
    }
    return (*i).second.defaultTL == tl;
}


MSTrafficLightLogic *
MSTLLogicControl::getActive(const std::string &id) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return 0;
    }
    return (*i).second.defaultTL;
}


bool
MSTLLogicControl::switchTo(const std::string &id, const std::string &subid)
{
    std::map<std::string, TLSLogicVariants>::iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return false;
    }
    MSTrafficLightLogic *touse = (*i).second.ltVariants[subid];
    std::vector<MSTrafficLightLogic*>::iterator j =
        find(myActiveLogics.begin(), myActiveLogics.end(), (*i).second.defaultTL);
    if(j==myActiveLogics.end()) {
        return false;
    }
    *j = touse;
    (*i).second.defaultTL = touse;
    return true;
}


bool
MSTLLogicControl::addWAUT(SUMOTime refTime, const std::string &id,
                          const std::string &startProg)
{
    if(myWAUTs.find(id)!=myWAUTs.end()) {
        return false;
    }
    WAUT *w = new WAUT;
    w->id = id;
    w->refTime = refTime;
    w->startProg = startProg;
    myWAUTs[id] = w;
    return true;
}


bool
MSTLLogicControl::addWAUTSwitch(const std::string &wautid,
                                SUMOTime when, const std::string &to)
{
    if(myWAUTs.find(wautid)==myWAUTs.end()) {
        return false;
    }
    WAUTSwitch s;
    s.to = to;
    s.when = when;
    myWAUTs[wautid]->switches.push_back(s);
    if(myWAUTs[wautid]->switches.size()==1) {
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new SwitchInitCommand(*this, wautid),
            when-myWAUTs[wautid]->refTime, MSEventControl::NO_CHANGE);
    }
    return true;
}


bool
MSTLLogicControl::addWAUTJunction(const std::string &wautid,
                                  const std::string &junc,
                                  const std::string &proc,
                                  bool synchron)
{
    if(myWAUTs.find(wautid)==myWAUTs.end()) {
        return false;
    }
    if(myLogics.find(junc)==myLogics.end()) {
        return false;
    }
    WAUTJunction j;
    j.junction = junc;
    j.procedure = proc;
    j.synchron = synchron;
    myWAUTs[wautid]->junctions.push_back(j);
    // set the current program
    TLSLogicVariants &vars = myLogics.find(junc)->second;
    switchTo(vars.defaultTL->getID(), myWAUTs[wautid]->startProg);
    return true;
}


SUMOTime
MSTLLogicControl::initWautSwitch(MSTLLogicControl::SwitchInitCommand &cmd)
{
    const std::string &wautid = cmd.getWAUTID();
    int &index = cmd.getIndex();
    WAUTSwitch s = myWAUTs[wautid]->switches[index];
    for(std::vector<WAUTJunction>::iterator i=myWAUTs[wautid]->junctions.begin(); i!=myWAUTs[wautid]->junctions.end(); ++i) {

        TLSLogicVariants &vars = myLogics.find((*i).junction)->second;
        MSTrafficLightLogic *from = vars.defaultTL;
        MSTrafficLightLogic *to = vars.ltVariants.find(s.to)->second;

        WAUTSwitchProcedure *proc = 0;
        if((*i).procedure=="GSP") {
            proc = new WAUTSwitchProcedure_GSP(*this, *myWAUTs[wautid], from, to, (*i).synchron);
        } else if((*i).procedure=="Stretch") {
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
    if(index==(int) myWAUTs[wautid]->switches.size()) {
        return 0;
    }
    return myWAUTs[wautid]->switches[index].when - myWAUTs[wautid]->switches[index-1].when;
}


void
MSTLLogicControl::check2Switch(SUMOTime step)
{
    for(std::vector<WAUTSwitchProcess>::iterator i=myCurrentlySwitched.begin(); i!=myCurrentlySwitched.end(); ) {
        const WAUTSwitchProcess &proc = *i;
        if(proc.proc->trySwitch(step)) {
            delete proc.proc;
            switchTo((*i).to->getID(), (*i).to->getSubID());
            i = myCurrentlySwitched.erase(i);
        } else {
            ++i;
        }
    }
}


CompletePhaseDef
MSTLLogicControl::getPhaseDef(const std::string &tlid) const
{
    MSTrafficLightLogic *tl = getActive(tlid);
    return CompletePhaseDef(
        MSNet::getInstance()->getCurrentTimeStep(),
        SimplePhaseDef(tl->allowed(), tl->yellowMask()));
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


