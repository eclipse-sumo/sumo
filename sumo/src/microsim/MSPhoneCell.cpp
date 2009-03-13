/****************************************************************************/
/// @file    MSPhoneCell.cpp
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// A cell of a cellular network (GSM)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include <utils/options/OptionsCont.h>
#include <utils/common/WrappingCommand.h>
#include "devices/MSDevice_CPhone.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSPhoneCell::MSPhoneCell(int id)
        : myCellId(id), myStaticCallsIn(0), myStaticCallsOut(0), myDynCallsIn(0),
        myDynCallsOut(0), mySumCalls(0), myDynOwnStarted(0), myDynIncomingStarted(0),
        myIntervalBegin(0), myIntervalEnd(0),
        myCurrentExpectedCallCount(0), myCurrentExpectedEntering(0), myCallDuration(0),
        myCallDeviation(0), myConnectionTypSelector(true),
        myDynIntervalDuration(0), myLaterDynamicStarted(0),
        myVehiclesEntered(0), myVehicleTimes(0) {
    OptionsCont &oc = OptionsCont::getOptions();
    myStaticCallCountScaleFactor = oc.getFloat("cell-static-callcount-scale-factor");
    myDynamicCallCountScaleFactor = oc.getFloat("cell-dynamic-callcount-scale-factor");
    myDynamicCallDeviationScaleFactor = oc.getFloat("cell-dynamic-calldeviation-scale-factor");
    myDynamicCallDurationScaleFactor = oc.getFloat("cell-dynamic-callduration-scale-factor");
}


MSPhoneCell::~MSPhoneCell() {}


void
MSPhoneCell::addCall(int callid, CallType ct, int cellCount) {
    myCalls[callid] = ct;
    /*
    if (cellCount < 1) {
        switch (ct) {
        case STATICIN:
        case DYNIN:
            //++myStaticCallsIn;
            break;
        case STATICOUT:
        case DYNOUT:
            //++myStaticCallsOut;
            break;
        }
    } else {
    */
    switch (ct) {
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
    //}
    //if (cellCount == 0 && (ct == DYNIN || ct == DYNOUT))
    ++myDynOwnStarted;
    //++mySumCalls;
}


void
MSPhoneCell::remCall(int callid) {
    myitCalls = myCalls.find(callid);
    if (myitCalls!=myCalls.end()) {
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
MSPhoneCell::hasCall(int callid) {
    return myCalls.find(callid) != myCalls.end();
}

void
MSPhoneCell::addCPhone(const std::string &device_id, MSDevice_CPhone* device_pointer) {
    myRegisteredDevices[device_id] = device_pointer;
    if (device_pointer->GetState()==MSDevice_CPhone::STATE_CONNECTED_IN||device_pointer->GetState()==MSDevice_CPhone::STATE_CONNECTED_OUT) {
        myDynIncomingStarted++;
    } else {
        SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
        if (useAsIncomingDynamic(time)) {
            if (RandHelper::rand()>.5) {
                device_pointer->SetState(MSDevice_CPhone::STATE_CONNECTED_IN , (int)(getCallDuration() / 2.5));
            } else {
                device_pointer->SetState(MSDevice_CPhone::STATE_CONNECTED_OUT , (int)(getCallDuration() / 2.5));
            }
        }
    }
}

bool
MSPhoneCell::hasCPhone(const std::string &device_id) {
    return myRegisteredDevices.find(device_id) != myRegisteredDevices.end();
}

void
MSPhoneCell::remCPhone(const std::string &device_id) {
    std::map<std::string, MSDevice_CPhone*>::iterator idevice = myRegisteredDevices.find(device_id);
    if (idevice != myRegisteredDevices.end()) {
        myRegisteredDevices.erase(idevice);
    }
}


SUMOTime
MSPhoneCell::wrappedSetStatParamsExecute(SUMOTime time) throw(ProcessError) {
    return nextStatPeriod(time);
}



void
MSPhoneCell::setStatParams(int interval, int statcallcount) {
    if (myExpectedStaticCalls.size()==0) {
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new WrappingCommand< MSPhoneCell >(this, &MSPhoneCell::wrappedSetStatParamsExecute),
            interval, MSEventControl::NO_CHANGE);
        myExpectedStaticCalls.push_back(make_pair(interval, (int)((SUMOReal) statcallcount*myStaticCallCountScaleFactor)));
    } else {
        myExpectedStaticCalls.push_back(make_pair(interval, (int)((SUMOReal) statcallcount*myStaticCallCountScaleFactor)));
    }
}


SUMOTime
MSPhoneCell::wrappedSetDynParamsExecute(SUMOTime time) throw(ProcessError) {
    return nextDynPeriod(time);
}


void
MSPhoneCell::setDynParams(int interval, int count, SUMOReal duration, SUMOReal deviation,
                          int entering) {
    DynParam p;
    p.count = (int)((SUMOReal) count * myDynamicCallCountScaleFactor);
    p.deviation = deviation * myDynamicCallDeviationScaleFactor;
    p.duration = duration * myDynamicCallDurationScaleFactor;
    p.entering = (int)((SUMOReal) entering*myDynamicCallCountScaleFactor);
    if (myExpectedDynamicCalls.size()==0) {
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new WrappingCommand< MSPhoneCell >(this, &MSPhoneCell::wrappedSetDynParamsExecute),
            interval, MSEventControl::NO_CHANGE);
        myExpectedDynamicCalls.push_back(make_pair(interval, p));
    } else {
        /*
        DynParam p2;
        p2.count = 0;
        p2.deviation = 0;
        p2.duration = 300;
        p2.entering = 0;
        while((*(myExpectedDynamicCalls.end()-1)).first + 3600!=interval) {
            myExpectedDynamicCalls.push_back(make_pair((*(myExpectedDynamicCalls.end()-1)).first + 3600, p2));
        }
        */
        myExpectedDynamicCalls.push_back(make_pair(interval, p));
    }
}


SUMOTime
MSPhoneCell::nextStatPeriod(SUMOTime time) {
    if (myExpectedStaticCalls.size()==0) {
        myStaticCallsIn = 0;
        myStaticCallsOut = 0;
        return 0;
    }
    std::pair<int, int> next = *myExpectedStaticCalls.begin();
    if (next.first!=time) {
        myStaticCallsIn = 0;
        myStaticCallsOut = 0;
        return 300;
    }
    myIntervalBegin = next.first;
    int counts = next.second;
    if (counts < 2) {
        if (RandHelper::rand()>.5) {
            myStaticCallsIn = counts;
            myStaticCallsOut = 0;
        } else {
            myStaticCallsIn = 0;
            myStaticCallsOut = counts;
        }
    } else {
        myStaticCallsIn = counts/2;
        myStaticCallsOut = counts - myStaticCallsIn;
    }
    myExpectedStaticCalls.erase(myExpectedStaticCalls.begin());
    return 300;//myExpectedStaticCalls.begin()->first - time;
}


SUMOTime
MSPhoneCell::nextDynPeriod(SUMOTime time) {
    if (myExpectedDynamicCalls.size()==0) {
        myCurrentExpectedCallCount  = 0;
        myCallDuration              = 300;
        myCallDeviation             = 0;
        myCurrentExpectedEntering   = 0;
        myDynOwnStarted = 0;
        myDynIncomingStarted = 0;
        return 0;
    }
    myDynIntervalBegin = time;
    std::pair<int, DynParam> next = *myExpectedDynamicCalls.begin();
    if (next.first!=time) {
        myCurrentExpectedCallCount  = 0;
        myCallDuration              = 300;
        myCallDeviation             = 0;
        myCurrentExpectedEntering   = 0;
        myDynOwnStarted = 0;
        myDynIncomingStarted = 0;
        return 3600;
    }
    myCurrentExpectedCallCount  = next.second.count;
    myCallDuration              = (SUMOTime) next.second.duration;
    myCallDeviation             = next.second.deviation;
    myCurrentExpectedEntering   = next.second.entering;
    myDynOwnStarted = 0;
    myDynIncomingStarted = 0;
    myExpectedDynamicCalls.erase(myExpectedDynamicCalls.begin());
    SUMOTime offset = 3600;
    /*
    if (myExpectedDynamicCalls.size()!=0) {
        offset = myExpectedDynamicCalls.begin()->first - time;
    }
    */
    myDynIntervalDuration = offset;
    myCallProbability = (SUMOReal) myCurrentExpectedCallCount / (SUMOReal) offset;
    return 3600;//offset;
}


bool
MSPhoneCell::useAsIncomingDynamic(SUMOTime time) {
    return false;
    if (myDynIntervalDuration==0) {
        return false;
    }
    SUMOReal absPerEachStep = (SUMOReal) myCurrentExpectedEntering / (SUMOReal) myDynIntervalDuration;
    SUMOReal should = (SUMOReal)(absPerEachStep/2. + absPerEachStep * (SUMOReal)(time-myDynIntervalBegin));
    SUMOReal toStart = should - myDynIncomingStarted;
    if (toStart<=(SUMOReal) 0) {
        return false;
    }
    myDynIncomingStarted++;
    return true;
}


void
MSPhoneCell::setDynamicCalls(SUMOTime time) {
    return;
    if (myDynIntervalDuration==0) {
        return;
    }
    SUMOReal absPerEachStep = (SUMOReal) myCurrentExpectedCallCount / (SUMOReal) myDynIntervalDuration;
    SUMOReal should = (SUMOReal)(absPerEachStep/2. + absPerEachStep * (SUMOReal)(time-myDynIntervalBegin));
    SUMOReal toStart = should - myDynOwnStarted;
    if (toStart<=(SUMOReal) 0) {
        return;
    }
    if (toStart-(int)toStart!=0) {
        if (RandHelper::rand()<toStart-(int)toStart) {
            toStart++;
        }
    }

    int itoStart = (int) toStart;
    std::map<std::string, MSDevice_CPhone*>::iterator itdev = myRegisteredDevices.end();
    for (itdev = myRegisteredDevices.begin(); itoStart>0&&itdev != myRegisteredDevices.end(); itdev++) {
        if (itdev->second->GetState() == MSDevice_CPhone::STATE_IDLE && !itdev->second->getNotTriggeredByCell()) {
            if (myConnectionTypSelector) {
                itdev->second->SetState(MSDevice_CPhone::STATE_CONNECTED_IN , (int) myCallDuration);
            } else {
                itdev->second->SetState(MSDevice_CPhone::STATE_CONNECTED_OUT , (int) myCallDuration);
            }
            myConnectionTypSelector = !myConnectionTypSelector;
            myLaterDynamicStarted++;
            itoStart--;
        }
    }
}


void
MSPhoneCell::writeOutput(SUMOTime t) {
    if (OptionsCont::getOptions().isSet("ss2-cell-output")) {
        std::string timestr= OptionsCont::getOptions().getString("device.cell-phone.sql-date");
        timestr = timestr + " " + StringUtils::toTimeString(t);
        long t1 = myVehicleTimes;
        std::map<MSVehicle *, SUMOTime>::iterator i;
        for (i=myVehicles.begin(); i!=myVehicles.end(); ++i) {
            t1 = t1 + (long)((t - (*i).second));
        }
        OutputDevice::getDeviceByOption("ss2-cell-output")
        << "02;" << timestr << ';'
        << myCellId << ';'
        << myStaticCallsIn << ';' << myStaticCallsOut << ';'
        << myDynCallsIn << ';' << myDynCallsOut << ';'
        << (mySumCalls + myStaticCallsIn + myStaticCallsOut) << ';'
        << t << ';'
        << myLaterDynamicStarted << ';'
        << myVehiclesEntered << ';'
        << t1 << "\n";
        myVehiclesEntered = 0;
        myVehicleTimes = 0;
        for (i=myVehicles.begin(); i!=myVehicles.end(); ++i) {
            (*i).second = t;
        }
    }
    if (OptionsCont::getOptions().isSet("ss2-sql-cell-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("ss2-sql-cell-output");
        std::string timestr= OptionsCont::getOptions().getString("device.cell-phone.sql-date");
        timestr = timestr + " " + StringUtils::toTimeString((int) t);
        if (od.getBoolMarker("hadFirstCall")) {
            od << "," << "\n";
        } else {
            od.setBoolMarker("hadFirstCall", true);
        }
        od
        << "(NULL, \' \', '" << timestr << "',"
        << myCellId << ','
        << myStaticCallsIn << ',' << myStaticCallsOut << ','
        << myDynCallsIn << ',' << myDynCallsOut << ','
        << (mySumCalls + myStaticCallsIn + myStaticCallsOut) << ','
        << t << ")";
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

#include <microsim/MSVehicle.h>
void MSPhoneCell::incVehiclesEntered(MSVehicle& veh, SUMOTime t) {
    myVehiclesEntered++;
    assert(myVehicles.find(&veh)==myVehicles.end());
    myVehicles[&veh] = t;
}

void MSPhoneCell::removeVehicle(MSVehicle& veh, SUMOTime t) {
    if (LastCells.find(&veh)!=LastCells.end()) {
        LastCells.erase(LastCells.find(&veh));
    }
    SUMOTime prev = myVehicles[&veh];
    assert(t>=prev);
    assert(prev>t-300);
    myVehicleTimes = myVehicleTimes + (long)(t-prev);
    assert(myVehicles.find(&veh)!=myVehicles.end());
    myVehicles.erase(myVehicles.find(&veh));
}




/****************************************************************************/

