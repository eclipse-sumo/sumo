/****************************************************************************/
/// @file    MSPhoneCell.h
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id: $
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

#include <string>
#include <vector>
#include <map>
#include "MSCORN.h"
#include "devices/MSDevice_CPhone.h"

enum CallType{
    STATICIN,
    STATICOUT,
    DYNIN,
    DYNOUT
};

struct Call
{
    CallType ct;
//	std::string id;
};


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

    /// Destructor
    ~MSPhoneCell();

    /// Adds a call (?!!!)
    void addCall(int callid, CallType ct);

    /// Removes a named call from the cell
    void remCall(int callid);

    /// Returns the information whether the named call takes place in this cell
    bool hasCall(int callid);

    /// !!!?
    int  getIntervall();

    /// Sets the number of static calls for the given period
    void setStatParams(int interval, int statcallcount);

    /// Sets the parameter of dynamic calls for the given period
    void setDynParams(int interval, int count, float duration, float deviation);

    /// !!!?
    void writeOutput(SUMOTime time);

    /// !!!?
    void writeSQLOutput(SUMOTime time);

    /// !!!?
    void setnextexpectData(SUMOTime time);

    /// !!!?
    //int getExpectDynCallCount() { return dyncallcount; }

    /// !!!?
    //void decrementDynCallCount(){ --dyncallcount; }

    /// Returns the number of vehicles calling within this cell
    size_t getVehicleNumber() const
    {
        return myCalls.size();
    }

    void addCPhone(const std::string &device_id, MSDevice_CPhone* device_pointer);
    bool hasCPhone(const std::string &device_id);
    void remCPhone(const std::string &device_id);
private:
    int myCellId;                   /* the id of the gsm-cell */
    int myStaticCallsIn;            /* the number of stacic incoming calls for this interval */
    int myStaticCallsOut;           /* the number of stativ outgoing calls for this interval */
    int myDynCallsIn;               /* the number of dynamic incomming / outgoing calls for this... */
    int myDynCallsOut;              /* ... interval which where singaled by a cphone.*/
    int mySumCalls;                 /* the total number of calls in this interval.*/
    int myDynOwnStarted;            /* the dynamic calls which where started in this cell during this interval*/
    int myIntervalBegin;            /* the begintime of the current interval*/
    int myIntervalEnd;              /* the endtime of the current interval*/
    int myCurrentExpectedCallCount;
    float myCallDuration;
    float myCallDeviation;
    bool myConnectionTypSelector;
    float myCallProbability;

    /*myConnectionTypSelector is for selecting if a connection is
      going in( == true ) or out ( == false ).*/
    struct DynParam
    {
        int count;
        float duration;
        float deviation;
    };

    std::map<int, CallType> myCalls;
    std::map<int, CallType>::iterator myitCalls;


    std::map< int, int > myExpectedStaticCalls;

    std::map< int, DynParam> myExpectedDynamicCalls;

    //std::vector<std::pair<int, int > > vexpectStatCount;
    //std::vector<std::pair<int, int > >::iterator itCount;
    std::vector<std::pair<int, DynParam> > vexpectDuration;
    std::vector<std::pair<int, DynParam> >::iterator itDuration;

    std::map<std::string, MSDevice_CPhone*> myRegisteredDevices;
};


#endif

/****************************************************************************/

