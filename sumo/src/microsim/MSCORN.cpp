/****************************************************************************/
/// @file    MSCORN.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// A storage for optional things to compute
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "MSVehicle.h"
#include "MSCORN.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
// c2x output files
SUMOTime MSCORN::myLastStepClusterInfoOutput = -1;
SUMOTime MSCORN::myLastStepSavedInfoOutput = -1;
SUMOTime MSCORN::myLastStepTransmittedInfoOutput = -1;
SUMOTime MSCORN::myLastStepVehicleInRangeOutput = -1;

bool MSCORN::myWished[CORN_MAX];
bool MSCORN::myFirstCall[CORN_MAX];


// ===========================================================================
// method definitions
// ===========================================================================
void
MSCORN::init()
{
    // TrafficOnline output files & settings
    for (int i=0; i<CORN_MAX; ++i) {
        myWished[i] = false;
        myFirstCall[i] = true;
    }
    myLastStepClusterInfoOutput = -1;
    myLastStepSavedInfoOutput = -1;
    myLastStepTransmittedInfoOutput = -1;
    myLastStepVehicleInRangeOutput = -1;
}


bool
MSCORN::wished(Function f)
{
    return myWished[(int) f];
}


void
MSCORN::setWished(Function f)
{
    myWished[(int) f] = true;
    switch (f) {
    case CORN_OUT_TRIPDURATIONS:
        setWished(CORN_VEH_DEPART_INFO);
        setWished(CORN_VEH_ARRIVAL_INFO);
        setWished(CORN_VEH_WAITINGTIME);
        setWished(CORN_VEH_VAPORIZED);
        break;
    case CORN_OUT_VEHROUTES:
        setWished(CORN_VEH_DEPART_TIME);
        setWished(CORN_VEH_SAVEREROUTING);
        setWished(CORN_VEH_DEPART_INFO);
        setWished(CORN_VEH_ARRIVAL_INFO);
        break;
    case CORN_OUT_EMISSIONS:
        setWished(CORN_MEAN_VEH_TRAVELTIME);
        setWished(CORN_MEAN_VEH_WAITINGTIME);
        break;
    case CORN_MEAN_VEH_TRAVELTIME:
        setWished(CORN_VEH_DEPART_TIME);
        break;
    case CORN_MEAN_VEH_WAITINGTIME:
        setWished(CORN_VEH_DEPART_TIME);
        break;
    default:
        break;
    }
}


//car2car
void
MSCORN::saveClusterInfoData(SUMOTime step, int id,
                            const std::string &headID, const std::string &vehs,
                            int quantity)
{
    if (!wished(CORN_OUT_CLUSTER_INFO)) {
        return;
    }
    if (myLastStepClusterInfoOutput!=step) {
        if (myLastStepClusterInfoOutput!=-1) {
            // close the previous step if its not the first one
            OutputDevice::getDeviceByOption("c2x.cluster-info").closeTag();
        }
        OutputDevice::getDeviceByOption("c2x.cluster-info").openTag("timestep")<<"time=\""<<step<<"\">\n";
    }
    myLastStepClusterInfoOutput = step;
    OutputDevice::getDeviceByOption("c2x.cluster-info")
    << "      <cluster id=\"" << id
    << "\" headID=\"" << headID << "\" vehNo=\"" << quantity << "\""
    << ">" << vehs << "</cluster>\n";
}


void
MSCORN::saveSavedInformationData(SUMOTime step, const std::string &veh,
                                 const std::string &edge, const std::string &type,
                                 SUMOTime time, SUMOReal nt)
{
    if (!wished(CORN_OUT_SAVED_INFO)) {
        return;
    }
    if (myLastStepSavedInfoOutput!=step) {
        if (myLastStepSavedInfoOutput!=-1) {
            // close the previous step if its not the first one
            OutputDevice::getDeviceByOption("c2x.saved-info").closeTag();
        }
        OutputDevice::getDeviceByOption("c2x.saved-info").openTag("timestep")<<"time=\""<<step<<"\">\n";
    }
    myLastStepSavedInfoOutput = step;
    // save the current values
    OutputDevice::getDeviceByOption("c2x.saved-info")
    << "      <info veh=\"" << veh <<"\" edge=\"" << edge << "\""<< " type=\"" << type << "\" "
    << "time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\"/>\n";
}

void
MSCORN::saveTransmittedInformationData(SUMOTime step, const std::string &from, const std::string &to,
                                       const std::string &edge, SUMOTime time, SUMOReal nt)
{
    if (!wished(CORN_OUT_TRANS_INFO)) {
        return;
    }
    if (myLastStepTransmittedInfoOutput!=step) {
        if (myLastStepTransmittedInfoOutput!=-1) {
            // close the previous step if its not the first one
            OutputDevice::getDeviceByOption("c2x.transmitted-info").closeTag();
        }
        OutputDevice::getDeviceByOption("c2x.transmitted-info").openTag("timestep")<<"time=\""<<step<<"\">\n";
    }
    myLastStepTransmittedInfoOutput = step;
    OutputDevice::getDeviceByOption("c2x.transmitted-info")
    << "      <info edge=\"" << edge <<"\" from=\"" << from << "\""<< " to=\"" << to << "\" "
    << "time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\"/>\n";
}


void
MSCORN::saveVehicleInRangeData(SUMOTime step, const std::string &veh1, const std::string &veh2,
                               SUMOReal x1, SUMOReal y1, SUMOReal x2 , SUMOReal y2)
{
    if (!wished(MSCORN::CORN_OUT_VEH_IN_RANGE)) {
        return;
    }
    if (myLastStepVehicleInRangeOutput!=step) {
        if (myLastStepVehicleInRangeOutput!=-1) {
            // close the previous step if its not the first one
            OutputDevice::getDeviceByOption("c2x.vehicle-in-range").closeTag();
        }
        OutputDevice::getDeviceByOption("c2x.vehicle-in-range").openTag("timestep")<<"time=\""<<step<<"\">\n";
    }
    myLastStepVehicleInRangeOutput = step;
    OutputDevice::getDeviceByOption("c2x.vehicle-in-range")
    << "      <connection veh1=\"" << veh1<<"\"" <<" x1=\""<< x1 << "\""<< " y1=\"" << y1 << "\" "
    <<"veh2=\"" << veh2<<"\"" <<" x2=\""<< x2 << "\""<< " y2=\"" << y2 << "\" />\n";
}


/****************************************************************************/

