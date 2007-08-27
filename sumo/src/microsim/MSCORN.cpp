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
#include "devices/MSDevice_CPhone.h"

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
OutputDevice *MSCORN::myClusterInfoOutput= 0;
OutputDevice *MSCORN::mySavedInfoOutput= 0;
OutputDevice *MSCORN::myTransmittedInfoOutput= 0;
OutputDevice *MSCORN::myVehicleInRangeOutput= 0;

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
    for (int i=0; i<CORN_MAX; i++) {
        myWished[i] = false;
        myFirstCall[i] = true;
    }
    // c2x output files
    myClusterInfoOutput = 0;
    mySavedInfoOutput = 0;
    myTransmittedInfoOutput = 0;
    myVehicleInRangeOutput = 0;
    myLastStepClusterInfoOutput = -1;
    myLastStepSavedInfoOutput = -1;
    myLastStepTransmittedInfoOutput = -1;
    myLastStepVehicleInRangeOutput = -1;
}


void
MSCORN::clear()
{
    // car2car
    delete myClusterInfoOutput;
    delete mySavedInfoOutput;
    delete myTransmittedInfoOutput;
    delete myVehicleInRangeOutput;
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
        setWished(CORN_VEH_REALDEPART);
        break;
    case CORN_OUT_VEHROUTES:
        setWished(CORN_VEH_SAVEREROUTING);
        setWished(CORN_VEH_REALDEPART);
        break;
    case CORN_OUT_EMISSIONS:
        setWished(CORN_VEHCONTROL_WANTS_DEPARTURE_INFO);
        setWished(CORN_MEAN_VEH_TRAVELTIME);
        setWished(CORN_MEAN_VEH_WAITINGTIME);
        break;
    case CORN_VEH_REALDEPART:
        break;
    case CORN_MEAN_VEH_TRAVELTIME:
        setWished(CORN_VEH_REALDEPART);
        break;
    case CORN_MEAN_VEH_WAITINGTIME:
        setWished(CORN_VEH_REALDEPART);
        break;
    case CORN_VEHCONTROL_WANTS_DEPARTURE_INFO:
        break;
    default:
        break;
    }
}


//car2car
void
MSCORN::setClusterInfoOutput(OutputDevice *s)
{
    myClusterInfoOutput = s;
}

void
MSCORN::setSavedInfoOutput(OutputDevice *s)
{
    mySavedInfoOutput = s;
}

void
MSCORN::setTransmittedInfoOutput(OutputDevice *s)
{
    myTransmittedInfoOutput = s;
}

void
MSCORN::setVehicleInRangeOutput(OutputDevice *s)
{
    myVehicleInRangeOutput = s;
}


//car2car
void
MSCORN::saveClusterInfoData(SUMOTime step, int id,
                            const std::string &headID, const std::string &vehs,
                            int quantity)
{
    if (myClusterInfoOutput==0) {
        return;
    }
    if (myLastStepClusterInfoOutput!=step) {
        if (myLastStepClusterInfoOutput!=-1) {
            // close the previous step if its not the first one
            *myClusterInfoOutput << "   </timestep>" << "\n";
        }
        *myClusterInfoOutput << "   <timestep time=\"" << step << "\">"<< "\n";
    }
    myLastStepClusterInfoOutput = step;
    *myClusterInfoOutput
    << "      <cluster id=\"" << id
    << "\" headID=\"" << headID << "\" vehNo=\"" << quantity << "\""
    << ">" << vehs << "</cluster>" << "\n";
}


void
MSCORN::checkCloseClusterInfoData()
{
    if (myLastStepClusterInfoOutput!=-1) {
        *myClusterInfoOutput << "   </timestep>" << "\n";
    }
}


void
MSCORN::saveSavedInformationData(SUMOTime step, const std::string &veh,
                                 const std::string &edge, const std::string &type,
                                 int time, SUMOReal nt)
{
    if (mySavedInfoOutput==0) {
        return;
    }
    if (myLastStepSavedInfoOutput!=step) {
        if (myLastStepSavedInfoOutput!=-1) {
            // close the previous step if its not the first one
            *mySavedInfoOutput << "   </timestep>"<< "\n";
        }
        *mySavedInfoOutput << "   <timestep time=\"" << step << "\">"<< "\n";
    }
    myLastStepSavedInfoOutput = step;
    // save the current values
    *mySavedInfoOutput
    << "      <info veh=\"" << veh <<"\" edge=\"" << edge << "\""<< " type=\"" << type << "\" "
    << "time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\"/>"<<"\n";
}

void
MSCORN::checkCloseSavedInformationData()
{
    if (myLastStepSavedInfoOutput!=-1) {
        *mySavedInfoOutput << "   </timestep>"<< "\n";
    }
}



void
MSCORN::saveTransmittedInformationData(SUMOTime step, const std::string &from, const std::string &to,
                                       const std::string &edge, int time, SUMOReal nt)
{
    if (myTransmittedInfoOutput==0) {
        return;
    }
    if (myLastStepTransmittedInfoOutput!=step) {
        if (myLastStepTransmittedInfoOutput!=-1) {
            // close the previous step if its not the first one
            *myTransmittedInfoOutput << "   </timestep>"<< "\n";
        }
        *myTransmittedInfoOutput << "   <timestep time=\"" << step << "\">"<< "\n";
    }
    myLastStepTransmittedInfoOutput = step;
    *myTransmittedInfoOutput
    << "      <info edge=\"" << edge <<"\" from=\"" << from << "\""<< " to=\"" << to << "\" "
    << "time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\"/>"<<"\n";
}

void
MSCORN::checkCloseTransmittedInformationData()
{
    if (myLastStepTransmittedInfoOutput!=-1) {
        *myTransmittedInfoOutput << "   </timestep>"<< "\n";
    }
}


void
MSCORN::saveVehicleInRangeData(SUMOTime step, const std::string &veh1, const std::string &veh2,
                               SUMOReal x1, SUMOReal y1, SUMOReal x2 , SUMOReal y2)
{
    if (myVehicleInRangeOutput==0) {
        return;
    }
    if (myLastStepVehicleInRangeOutput!=step) {
        if (myLastStepVehicleInRangeOutput!=-1) {
            // close the previous step if its not the first one
            *myVehicleInRangeOutput << "   </timestep>"<< "\n";
        }
        *myVehicleInRangeOutput << "   <timestep time=\"" << step << "\">"<< "\n";
    }
    myLastStepVehicleInRangeOutput = step;
    *myVehicleInRangeOutput
    << "      <connection veh1=\"" << veh1<<"\"" <<" x1=\""<< x1 << "\""<< " y1=\"" << y1 << "\" "
    <<"veh2=\"" << veh2<<"\"" <<" x2=\""<< x2 << "\""<< " y2=\"" << y2 << "\" "
    <<"/>"<<"\n";
}


void
MSCORN::checkCloseVehicleInRangeData()
{
    if (myLastStepVehicleInRangeOutput!=-1) {
        *myVehicleInRangeOutput << "   </timestep>"<< "\n";
    }
}



/****************************************************************************/

