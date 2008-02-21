/****************************************************************************/
/// @file    MSPhoneCell.h
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// A cell of a cellular network (GSM)
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
#ifndef MSPhoneCell_h
#define MSPhoneCell_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <map>
#include "MSCORN.h"
#include <utils/common/UtilExceptions.h>

enum CallType {
    STATICIN,
    STATICOUT,
    DYNIN,
    DYNOUT
};

struct Call {
    CallType ct;
//	std::string id;
};


class MSDevice_CPhone;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPhoneCell
 * @brief A cell of a cellular network (GSM)
 */
class MSPhoneCell
{
public:
    /// Constructor
    MSPhoneCell(int ID);
    size_t getDynEntriesNo() const {
        return myExpectedDynamicCalls.size();
    }


    /// Destructor
    ~MSPhoneCell();

    /// Adds a call (?!!!)
    void addCall(int callid, CallType ct, int cellCount);

    /// Removes a named call from the cell
    void remCall(int callid);

    /// Returns the information whether the named call takes place in this cell
    bool hasCall(int callid);

    /// Sets the number of static calls for the given period
    void setStatParams(int interval, int statcallcount);

    /// Sets the parameter of dynamic calls for the given period
    void setDynParams(int interval, int count, SUMOReal duration, SUMOReal deviation,
                      int entering);

    /// !!!?
    void writeOutput(SUMOTime time);
    void setDynamicCalls(SUMOTime time);
    bool useAsIncomingDynamic(SUMOTime time);

    SUMOTime getCallDuration() const {
        return myCallDuration;
    }

    /// !!!?
    //int getExpectDynCallCount() { return dyncallcount; }

    /// !!!?
    //void decrementDynCallCount(){ --dyncallcount; }

    /// Returns the number of vehicles calling within this cell
    size_t getVehicleNumber() const {
        return myCalls.size();
    }

    void addCPhone(const std::string &device_id, MSDevice_CPhone* device_pointer);
    bool hasCPhone(const std::string &device_id);
    void remCPhone(const std::string &device_id);

    void incVehiclesEntered(MSVehicle& veh, SUMOTime t);
    void removeVehicle(MSVehicle& veh, SUMOTime t);
    SUMOTime wrappedSetDynParamsExecute(SUMOTime time) throw(ProcessError);
    SUMOTime wrappedSetStatParamsExecute(SUMOTime time) throw(ProcessError);

private:
    SUMOTime nextStatPeriod(SUMOTime time);
    SUMOTime nextDynPeriod(SUMOTime time);


private:
    int myCellId;                   /* the id of the gsm-cell */
    int myStaticCallsIn;            /* the number of stacic incoming calls for this interval */
    int myStaticCallsOut;           /* the number of static outgoing calls for this interval */
    int myDynCallsIn;               /* the number of dynamic incoming / outgoing calls for this... */
    int myDynCallsOut;              /* ... interval which where singaled by a cphone.*/
    int mySumCalls;                 /* the total number of calls in this interval.*/
    int myDynOwnStarted;            /* the dynamic calls which where started in this cell during this interval*/
    int myDynIncomingStarted;
    int myIntervalBegin;            /* the begintime of the current interval*/
    int myIntervalEnd;              /* the endtime of the current interval*/
    int myLaterDynamicStarted;
    int myCurrentExpectedCallCount;
    int myCurrentExpectedEntering;
    SUMOTime myDynIntervalBegin;
    SUMOTime myDynIntervalDuration;
    SUMOTime myCallDuration;
    SUMOReal myCallDeviation;
    bool myConnectionTypSelector;
    SUMOReal myCallProbability;
    SUMOReal myStaticCallCountScaleFactor;
    SUMOReal myDynamicCallCountScaleFactor;
    SUMOReal myDynamicCallDurationScaleFactor;
    SUMOReal myDynamicCallDeviationScaleFactor;

    /*myConnectionTypSelector is for selecting if a connection is
      going in( == true ) or out ( == false ).*/
    struct DynParam {
        int count;
        SUMOReal duration;
        SUMOReal deviation;
        int entering;
    };

    std::map<int, CallType> myCalls;
    std::map<int, CallType>::iterator myitCalls;


    std::vector<std::pair<int, int> > myExpectedStaticCalls;

    std::vector<std::pair<int, DynParam> > myExpectedDynamicCalls;

    //std::vector<std::pair<int, int > > vexpectStatCount;
    //std::vector<std::pair<int, int > >::iterator itCount;
    std::vector<std::pair<int, DynParam> > vexpectDuration;
    std::vector<std::pair<int, DynParam> >::iterator itDuration;

    std::map<std::string, MSDevice_CPhone*> myRegisteredDevices;

    size_t myVehiclesEntered;
    std::map<MSVehicle *, SUMOTime> myVehicles;
    long myVehicleTimes;
};


#endif

/****************************************************************************/

