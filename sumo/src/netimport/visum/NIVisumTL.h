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
// Revision 1.1  2003/05/20 09:55:56  dkrajzew
// visum-traffic light import added (by Markus Hartinger)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <map>
#include <string>
#include <netbuild/NBConnectionDefs.h>
#include <netbuild/NBNodeCont.h>
/* =========================================================================
 * class declaration
 * ======================================================================= */
/* NIViusumTL is an intermediate class for importing visum traffic lights */
class NIVisumTL
{
public:
	// Timeperiod with starttime and endtime
	class TimePeriod
	{
	private:
		double myStartTime;
		double myEndTime;
	public:
		// constructor
		TimePeriod ();
		TimePeriod(double StartTime, double EndTime);
		// destructor
		~TimePeriod();
		void SetStartTime(double StartTime);
		void SetEndTime(double EndTime);
		double GetStartTime();
		double GetEndTime();
	};

	// phase
	class Phase : public TimePeriod	{
	public:
		Phase();
		Phase(double StartTime, double EndTime);
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
		SignalGroup(const std::string &Name, double StartTime, double EndTime);
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
	NIVisumTL(const std::string &Name, double CycleTime, double IntermediateTime,
		bool PhaseDefined);
	// destructor
	~NIVisumTL();
	// nodes and phases
	NodeVector* GetNodes();
	PhaseMap* GetPhases();
    SignalGroupMap* GetSignalGroups();
	// set
	void SetPhaseDefined(bool PhaseDefined);
	void SetCycleTime(double CylcleTime);
	void SetIntermediateTime(double IntermediateTime);
	// get
	bool GetPhaseDefined();
	double GetCycleTime();
	double GetIntermediateTime();
	double GetName();
	SignalGroup* GetSignalGroup(const std::string Name);
	// add
	void AddSignalGroup(const std::string Name, double StartTime, double EndTime);
	void AddPhase(const std::string Name, double StartTime, double EndTime);
	// build the trafficlight
	void build();
private:
	// vector of nodes belonging to this traffic light
	NodeVector myNodes;
	// vector of used phases if phasedefined
	PhaseMap myPhases;
	// vector of used Signalgroups
	SignalGroupMap mySignalGroups;
	// name of traffic light
	std::string myName;
	// toogles the usage either of phases or of timeperiods in signalgroups
    bool myPhaseDefined;
	// cycle time of traffic light in seconds
	double myCycleTime;
	// length of yellow and red-yellow phases
	double myIntermediateTime;
};
/* =========================================================================
 * class declaration
 * ======================================================================= */
/**
 */

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVisumTL.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
