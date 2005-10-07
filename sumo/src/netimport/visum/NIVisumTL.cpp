/***************************************************************************
                          NIVisumTL.cpp
			  Intermediate class for importing Visum traffic lights
                             -------------------
    project              : SUMO
    begin                : Thr, 08 May 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.10  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.5  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.4  2003/07/07 08:31:36  dkrajzew
// adapted the importer to the new node type description
//
// Revision 1.3  2003/06/24 08:19:35  dkrajzew
// some further work on importing traffic lights
//
// Revision 1.2  2003/06/05 11:51:09  dkrajzew
// class templates applied; documentation added
//
// Revision 1.1  2003/05/20 09:55:56  dkrajzew
// visum-traffic light import added (by Markus Hartinger)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

#include <string>
#include <netbuild/NBLoadedTLDef.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include "NIVisumTL.h"

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
{

}

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
{
}

NIVisumTL::Phase::Phase(SUMOTime StartTime, SUMOTime EndTime)
	: NIVisumTL::TimePeriod(StartTime, EndTime)
{
}

NIVisumTL::Phase::~Phase()
{

}

/* -------------------------------------------------------------------------
 * methods from NIVisumTL::SignalGroup
 * ----------------------------------------------------------------------- */
NIVisumTL::SignalGroup::SignalGroup(const std::string &Name,
									SUMOTime StartTime, SUMOTime EndTime)
	: NIVisumTL::TimePeriod(StartTime, EndTime), myName(Name)
{
}


NIVisumTL::SignalGroup::~SignalGroup()
{
}

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
{
}


NIVisumTL::NIVisumTL(const std::string &Name, SUMOTime CycleTime,
					 SUMOTime IntermediateTime, bool PhaseDefined)
    : myName(Name), myCycleTime(CycleTime), myIntermediateTime(IntermediateTime),
    myPhaseDefined(PhaseDefined)
{
}

NIVisumTL::~NIVisumTL()
{
    for( NIVisumTL::PhaseMap::iterator i=myPhases.begin();
         i!=myPhases.end(); i++) {
        delete (i->second);
    }
	for( NIVisumTL::SignalGroupMap::iterator k=mySignalGroups.begin();
         k!=mySignalGroups.end(); k++) {
        delete (k->second);
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
	for(NodeVector::iterator ni = myNodes.begin(); ni != myNodes.end(); ni++)
	{
		NBNode *Node = (*ni);
        NBLoadedTLDef *def = new NBLoadedTLDef(Node->getID(), Node);
        tlc.insert(Node->getID(), def);
		def->setCycleDuration((size_t) myCycleTime);
		// signalgroups
		for(SignalGroupMap::iterator gi = mySignalGroups.begin(); gi != mySignalGroups.end(); gi++ )
		{
			std::string GroupName = (*gi).first;
			NIVisumTL::SignalGroup &SG = *(*gi).second;
			def->addSignalGroup(GroupName);
			def->addToSignalGroup(GroupName, *SG.GetConnections());
 			def->setSignalYellowTimes(GroupName, myIntermediateTime, myIntermediateTime);
			// phases
			if (myPhaseDefined) {
				for(PhaseMap::iterator pi = SG.GetPhases()->begin(); pi!= SG.GetPhases()->end(); pi++)
				{
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
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

