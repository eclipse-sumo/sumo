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
// Revision 1.1  2003/05/20 09:55:56  dkrajzew
// visum-traffic light import added (by Markus Hartinger)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "NIVisumTL.h"

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

NIVisumTL::TimePeriod::TimePeriod(double StartTime, double EndTime)
{
	myStartTime = StartTime;
	myEndTime = EndTime;
}

NIVisumTL::TimePeriod::~TimePeriod()
{

}

double NIVisumTL::TimePeriod::GetEndTime()
{
	return myEndTime;
}

double NIVisumTL::TimePeriod::GetStartTime()
{
	return myStartTime;
}

void NIVisumTL::TimePeriod::SetEndTime(double EndTime)
{
	myEndTime = EndTime;
}

void NIVisumTL::TimePeriod::SetStartTime(double StartTime)
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

NIVisumTL::Phase::Phase(double StartTime, double EndTime)
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
	double StartTime, double EndTime)
	: myName(Name), NIVisumTL::TimePeriod(StartTime, EndTime)
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

NIVisumTL::PhaseMap* NIVisumTL::SignalGroup::GetPhases()
{
	return &myPhases;
}

 /* -------------------------------------------------------------------------
 * methods from NIVisumTL
 * ----------------------------------------------------------------------- */
NIVisumTL::NIVisumTL()
{

}

NIVisumTL::NIVisumTL(const std::string &Name, double CycleTime,
					 double IntermediateTime, bool PhaseDefined)
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

double NIVisumTL::GetCycleTime()
{
	return myCycleTime;
}

double NIVisumTL::GetIntermediateTime()
{
	return myIntermediateTime;
}

bool NIVisumTL::GetPhaseDefined()
{
	return myPhaseDefined;
}

NIVisumTL::NodeVector* NIVisumTL::GetNodes()
{
	return &myNodes;
}

NIVisumTL::PhaseMap* NIVisumTL::GetPhases()
{
	return &myPhases;
}

NIVisumTL::SignalGroupMap* NIVisumTL::GetSignalGroups()
{
	return &mySignalGroups;
}

void NIVisumTL::SetCycleTime(double CycleTime)
{
	myCycleTime = CycleTime;
}

void NIVisumTL::SetIntermediateTime(double IntermediateTime)
{
	myIntermediateTime = IntermediateTime;
}

void NIVisumTL::SetPhaseDefined(bool PhaseDefined)
{
	myPhaseDefined = PhaseDefined;
}

void NIVisumTL::AddSignalGroup(const std::string Name, double StartTime, double EndTime)
{
	mySignalGroups[Name] = new NIVisumTL::SignalGroup(Name, StartTime, EndTime);
}

void NIVisumTL::AddPhase(const std::string Name, double StartTime, double EndTime)
{
	myPhases[Name] = new NIVisumTL::Phase(StartTime, EndTime);
}

NIVisumTL::SignalGroup* NIVisumTL::GetSignalGroup(const std::string Name)
{
	return (*mySignalGroups.find(Name)).second;
}

void NIVisumTL::build()
{
	for(NodeVector::iterator ni = myNodes.begin(); ni != myNodes.end(); ni++)
	{
		NBNode *Node = (*ni);
		Node->setCycleDuration(myCycleTime);
		Node->setType(NBNode::TYPE_SIMPLE_TRAFFIC_LIGHT);
		// signalgroups
		for(SignalGroupMap::iterator gi = mySignalGroups.begin(); gi != mySignalGroups.end(); gi++ )
		{
			std::string GroupName = (*gi).first;
			NIVisumTL::SignalGroup &SG = *(*gi).second;
			Node->addSignalGroup(GroupName);
			Node->addToSignalGroup(GroupName, *SG.GetConnections());
			Node->setSignalYellowTimes(GroupName, myIntermediateTime, myIntermediateTime);
			// phases
			if (myPhaseDefined) {
				for(PhaseMap::iterator pi = SG.GetPhases()->begin(); pi!= SG.GetPhases()->end(); pi++)
				{
					NIVisumTL::Phase &PH = *(*pi).second;
					Node->addSignalGroupPhaseBegin(GroupName, PH.GetStartTime(), NBNode::TLCOLOR_GREEN);
					Node->addSignalGroupPhaseBegin(GroupName, PH.GetEndTime(), NBNode::TLCOLOR_RED);
				};
			} else {
				Node->addSignalGroupPhaseBegin(GroupName, SG.GetStartTime(), NBNode::TLCOLOR_GREEN);
				Node->addSignalGroupPhaseBegin(GroupName, SG.GetEndTime(), NBNode::TLCOLOR_RED);
			}
		}
	}
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumTL.icc"
//#endif

// Local Variables:
// mode:C++
// End:

