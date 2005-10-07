#ifndef NIVisumTL_h
#define NIVisumTL_h
/***************************************************************************
                          NIVisumTL.h
			  Intermediate class for importing Visum traffic lights
                             -------------------
    project              : SUMO
    begin                : Wed, 07 May 2003
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
// $Log$
// Revision 1.8  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:03:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2004/01/12 15:36:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.3  2003/07/18 12:35:05  dkrajzew
// removed some warnings
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

#include <vector>
#include <map>
#include <string>
#include <netbuild/NBConnectionDefs.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/common/SUMOTime.h>

class NBTrafficLightLogicCont;


/* =========================================================================
 * class declaration
 * ======================================================================= */
/**
 * @class NIVisumTL
 * NIViusumTL is an intermediate class for importing visum traffic lights
 */
class NIVisumTL
{
public:
	/// Timeperiod with starttime and endtime
	class TimePeriod
	{
	public:
		// constructor
		TimePeriod ();
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
	class Phase : public TimePeriod	{
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

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
