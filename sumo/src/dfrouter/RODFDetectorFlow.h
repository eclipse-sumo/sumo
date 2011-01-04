/****************************************************************************/
/// @file    RODFDetectorFlow.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RODFDetectorFlow_h
#define RODFDetectorFlow_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODFDetectorFlows
 * @brief A container for flows
 */
class RODFDetectorFlows {
public:
    RODFDetectorFlows(SUMOTime startTime, SUMOTime endTime,
                      SUMOTime stepOffset);
    ~RODFDetectorFlows();
    void addFlow(const std::string &detector_id, int timestamp,
                 const FlowDef &fd);
    void removeFlow(const std::string &detector_id);
    void setFlows(const std::string &detector_id, std::vector<FlowDef> &);

    const std::vector<FlowDef> &getFlowDefs(const std::string &id) const;
    bool knows(const std::string &det_id) const;
    bool knows(const std::string &det_id, SUMOTime time) const;
    SUMOReal getFlowSumSecure(const std::string &id) const;
    SUMOReal getMaxDetectorFlow() const;
    void printAbsolute() const;

    void mesoJoin(const std::string &nid, const std::vector<std::string> &oldids);

protected:
    std::map<std::string, std::vector<FlowDef> > myFastAccessFlows;
    SUMOTime myBeginTime, myEndTime, myStepOffset;
    mutable SUMOReal myMaxDetectorFlow;

private:
    /// @brief Invalidated copy constructor
    RODFDetectorFlows(const RODFDetectorFlows &src);

    /// @brief Invalidated assignment operator
    RODFDetectorFlows &operator=(const RODFDetectorFlows &src);

};


#endif

/****************************************************************************/

