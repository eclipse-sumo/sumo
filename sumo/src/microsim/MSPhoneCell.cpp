/****************************************************************************/
/// @file    MSPhoneCell.cpp
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

#include <cassert>
#include "MSPhoneCell.h"
#include <utils/common/RandHelper.h>
#include "MSNet.h"
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSPhoneCell::MSPhoneCell(int id)
        : myCellId(id), myStaticCallsIn(0), myStaticCallsOut(0), myDynCallsIn(0),
        myDynCallsOut(0), mySumCalls(0), myDynOwnStarted(0), myIntervalBegin(0), myIntervalEnd(0),
        myCurrentExpectedCallCount(0), myCallDuration(0),
        myCallDeviation(0), myConnectionTypSelector(true),
        myDynIntervalDuration(0), myLaterDynamicStarted(0)
{}


MSPhoneCell::~MSPhoneCell()
{}


void
MSPhoneCell::addCall(int callid, CallType ct, int cellCount)
{
	myCalls[callid] = ct; 
	if ( cellCount < 2 )
	{
		switch( ct )
		{
		case STATICIN:
		case DYNIN:
			//++myStaticCallsIn;
			break;
		case STATICOUT:
		case DYNOUT:
			//++myStaticCallsOut;
			break;
		}
	}
	else
	{
		switch (ct) 
		{
		case STATICIN:
			//++myStaticCallsIn;
			break;
		case STATICOUT:
			//++myStaticCallsOut;
			break;
		case DYNIN:
			++myDynCallsIn;
			//++myDynOwnStarted;
            ++mySumCalls;
			break;
		case DYNOUT:
			++myDynCallsOut;
            ++mySumCalls;
			//++myDynOwnStarted;
			break;
		}
	}
	if( cellCount == 0 && ( ct == DYNIN || ct == DYNOUT ) )
		++myDynOwnStarted;
	//++mySumCalls;
}


void
MSPhoneCell::remCall(int callid)
{
    myitCalls = myCalls.find(callid);
    if(myitCalls!=myCalls.end()) {
        myCalls.erase(myitCalls);
    }
    /*switch ( myitCalls->second )
    {
    case STATICIN:
    --myStaticCallsIn;
    break;
    case STATICOUT:
    --myStaticCallsOut;
    break;
    case DYNIN:
    --myDynCallsIn;
    break;
    case DYNOUT:
    --myDynCallsOut;
    break;
    }*/
}


bool
MSPhoneCell::hasCall(int callid)
{
    return myCalls.find(callid) != myCalls.end();
}

void
MSPhoneCell::addCPhone(const std::string &device_id, MSDevice_CPhone* device_pointer)
{
    myRegisteredDevices[device_id] = device_pointer;
}

bool
MSPhoneCell::hasCPhone(const std::string &device_id)
{
    return myRegisteredDevices.find(device_id) != myRegisteredDevices.end();
}

void
MSPhoneCell::remCPhone(const std::string &device_id)
{
    std::map<std::string, MSDevice_CPhone*>::iterator idevice = myRegisteredDevices.find(device_id);
    if(idevice != myRegisteredDevices.end()) {
        myRegisteredDevices.erase(idevice);
    }
}

void
MSPhoneCell::setStatParams(int interval, int statcallcount)
{
    if(myExpectedStaticCalls.size()==0) {
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new SetStatParamsCommand(*this),
            interval, MSEventControl::NO_CHANGE);
    }
    myExpectedStaticCalls.push_back(make_pair(interval, statcallcount));
}


void
MSPhoneCell::setDynParams(int interval, int count, float duration, float deviation)
{
    if(myExpectedDynamicCalls.size()==0) {
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new SetDynParamsCommand(*this),
            interval, MSEventControl::NO_CHANGE);
    }
    DynParam p;
    p.count = count;
    p.deviation = deviation;
    p.duration = duration;
    myExpectedDynamicCalls.push_back(make_pair(interval, p));
    //vexpectDuration.push_back( std::make_pair( interval, p ) );
}


SUMOTime 
MSPhoneCell::nextStatPeriod(SUMOTime time)
{
    if(myExpectedStaticCalls.size()==0) {
        return 0;
    }
    std::pair<int, int> next = *myExpectedStaticCalls.begin();
    myIntervalBegin = next.first;
    int counts = next.second;
    if (counts < 2)
        myStaticCallsIn = myStaticCallsOut = counts;
    else
        myStaticCallsIn = myStaticCallsOut = counts/2;
    myExpectedStaticCalls.erase(myExpectedStaticCalls.begin());
    if(myExpectedStaticCalls.size()!=0) {
        return myExpectedStaticCalls.begin()->first - time;
    } else {
        return 0;
    }
}


SUMOTime 
MSPhoneCell::nextDynPeriod(SUMOTime time)
{
    if(myExpectedDynamicCalls.size()==0) {
        return 0;
    }
    myDynIntervalBegin = time;
    std::pair<int, DynParam> next = *myExpectedDynamicCalls.begin();
    myCurrentExpectedCallCount  = next.second.count;
    myCallDuration              = next.second.duration;
    myCallDeviation             = next.second.deviation;
    myDynOwnStarted = 0;
    myExpectedDynamicCalls.erase(myExpectedDynamicCalls.begin());
    SUMOTime offset = 3600;
    if(myExpectedDynamicCalls.size()!=0) {
        offset = myExpectedDynamicCalls.begin()->first - time;
    }
    myDynIntervalDuration = offset;
    myCallProbability = (SUMOReal) myCurrentExpectedCallCount / (SUMOReal) offset;
    return offset;
}


void 
MSPhoneCell::setDynamicCalls(SUMOTime time)
{
    if(myDynIntervalDuration==0) {
        return;
    }
    SUMOReal absPerEachStep = (SUMOReal) myCurrentExpectedCallCount / (SUMOReal) myDynIntervalDuration;
    SUMOReal should = absPerEachStep/2. + absPerEachStep * (SUMOReal) (time-myDynIntervalBegin);
    SUMOReal toStart = should - myDynOwnStarted;
    if(toStart<=(SUMOReal) 0) {
        return;
    }
    if(toStart-(int)toStart!=0) {
        if(randSUMO()<toStart-(int)toStart) {
            toStart++;
        }
    }

    int itoStart = (int) toStart;
    std::map<std::string, MSDevice_CPhone*>::iterator itdev = myRegisteredDevices.end();
    for (itdev = myRegisteredDevices.begin(); itoStart>0&&itdev != myRegisteredDevices.end(); itdev++) {
        if (itdev->second->GetState() == MSDevice_CPhone::STATE_IDLE) {
            if (myConnectionTypSelector) {
                itdev->second->SetState(MSDevice_CPhone::STATE_CONNECTED_IN , (int)myCallDuration);
            } else {
                itdev->second->SetState(MSDevice_CPhone::STATE_CONNECTED_OUT , (int)myCallDuration);
            }
            myConnectionTypSelector = !myConnectionTypSelector;
            myLaterDynamicStarted++;
            itoStart--;
        }
    }
}


void
MSPhoneCell::writeOutput(SUMOTime t)
{
    {
        OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2);
        if(od!=0) {
            std::string timestr="1970-01-01 " + StringUtils::toTimeString(t);
            od->getOStream()
            << "02;" << timestr << ';' 
            << myCellId << ';' 
            << myStaticCallsIn << ';' << myStaticCallsOut << ';' 
            << myDynCallsIn << ';' << myDynCallsOut << ';' 
            << (mySumCalls + myStaticCallsIn + myStaticCallsOut) << ';' 
            << t << ';'
            << myLaterDynamicStarted << endl;
        }
    }
    {
        OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2_SQL);
        if(od!=0) {
            std::string timestr="1970-01-01 " + StringUtils::toTimeString(t);
            if (od->getBoolMarker("hadFirstCall")) {
                od->getOStream() << "," << endl;
            } else {
                od->setBoolMarker("hadFirstCall", true);
            }
            od->getOStream()
            << "(NULL, \' \', '" << timestr << "'," 
            << myCellId << ',' 
            << myStaticCallsIn << ',' << myStaticCallsOut << ','
            << myDynCallsIn << ',' << myDynCallsOut << ',' 
            << (mySumCalls + myStaticCallsIn + myStaticCallsOut) << ',' 
            << t << ")";
        }
    }
    myDynCallsIn = myDynCallsOut = myLaterDynamicStarted = 0;
    mySumCalls = 0;
    for (myitCalls = myCalls.begin(); myitCalls != myCalls.end(); myitCalls++) {
        if (myitCalls->second == DYNIN) {
            ++myDynCallsIn;
        } else if (myitCalls->second == DYNOUT) {
            ++myDynCallsOut;
        }
    }
    mySumCalls = myDynCallsOut + myDynCallsIn;
}



/****************************************************************************/

