/****************************************************************************/
/// @file    DFDetectorFlow.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// missing_desc
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
#ifndef DFDetectorFlow_h
#define DFDetectorFlow_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <map>
#include <string>
#include <vector>


// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @class FlowDef
 * @brief Definition of the traffic during a certain time containing the flows and speeds
 */
struct FlowDef
{
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


// ===========================================================================
// class definitions
// ===========================================================================
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
                 const FlowDef &fd);
    void removeFlow(const std::string &detector_id);
    void setFlows(const std::string &detector_id, std::vector<FlowDef> &);
    //const FlowDef &getFlowDef(const std::string &det_id, SUMOTime timestamp) const;
    //const std::map< int, FlowDef > &getFlowDefs( const std::string &id ) const;

    const std::vector<FlowDef> &getFlowDefs(const std::string &id) const;
    bool knows(const std::string &det_id) const;
    bool knows(const std::string &det_id, SUMOTime time) const;
    SUMOReal getFlowSumSecure(const std::string &id) const;
    SUMOReal getMaxDetectorFlow() const;

    void mesoJoin(const std::string &nid, const std::vector<std::string> &oldids);

    //void buildFastAccess(SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
protected:
//    std::map<std::string, std::map<SUMOTime, FlowDef> > myCurrentFlows;
    std::map<std::string, std::vector<FlowDef> > myFastAccessFlows;
    SUMOTime myBeginTime, myEndTime, myStepOffset;
    mutable SUMOReal myMaxDetectorFlow;

};


#endif

/****************************************************************************/

