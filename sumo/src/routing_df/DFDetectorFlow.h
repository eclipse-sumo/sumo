#ifndef DFDETECTOR_FLOW_h
#define DFDETECTOR_FLOW_h
/***************************************************************************
                          DFDetector.h
    Storage for flows within the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.7  2006/04/05 05:35:26  dkrajzew
// further work on the dfrouter
//
// Revision 1.6  2006/03/17 09:04:25  dkrajzew
// class-documentation added/patched
//
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

#include <utils/common/SUMOTime.h>
#include <map>
#include <string>
#include <vector>
//#include <routing_df/DFDetector.h>

/* =========================================================================
 * struct definitions
 * ======================================================================= */
/**
 * @class FlowDef
 * @brief Definition of the traffic during a certain time containing the flows and speeds
 */
struct FlowDef {
    // Number of passenger vehicles that passed within the described time
    SUMOReal qPKW;
    // Number of heavy duty vehicles that passed within the described time
    SUMOReal qLKW;
    // Mean velocity of passenger vehicles within the described time
    SUMOReal vPKW;
    // Mean velocity of heavy duty vehicles within the described time
    SUMOReal vLKW;
    // begin time (in s)
//    int time;
    // probability for having a heavy duty vehicle(qKFZ!=0 ? (qLKW / qKFZ) : 0;)
    SUMOReal fLKW;
    // initialise with 0
    mutable SUMOReal isLKW;
    //
    bool firstSet;
};


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class DFDetector
 * @brief A container for flows
 */
class DFDetectorFlows
{
public:
	DFDetectorFlows(SUMOTime startTime, SUMOTime endTime,
        SUMOTime stepOffset);
	~DFDetectorFlows();
	void addFlow(const std::string &detector_id, int timestamp,
        const FlowDef &fd );
	void removeFlow(const std::string &detector_id);
	void setFlows(const std::string &detector_id, std::vector<FlowDef> & );
	//const FlowDef &getFlowDef(const std::string &det_id, SUMOTime timestamp) const;
	//const std::map< int, FlowDef > &getFlowDefs( const std::string &id ) const;

    const std::vector<FlowDef> &getFlowDefs( const std::string &id ) const;
    bool knows( const std::string &det_id ) const;
    bool knows( const std::string &det_id, SUMOTime time ) const;

    //void buildFastAccess(SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
protected:
//    std::map<std::string, std::map<SUMOTime, FlowDef> > myCurrentFlows;
    std::map<std::string, std::vector<FlowDef> > myFastAccessFlows;
    SUMOTime myBeginTime, myEndTime, myStepOffset;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
