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

#include <iostream>
#include "MSVehicle.h"
#include "MSCORN.h"
#include <utils/iodevices/OutputDevice.h>

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
// standard output files
OutputDevice *MSCORN::myTripDurationsOutput = 0;
OutputDevice *MSCORN::myVehicleRouteOutput = 0;
// TrafficOnline output files
OutputDevice *MSCORN::myVehicleDeviceTOSS2Output = 0;
OutputDevice *MSCORN::myCellTOSS2Output = 0;
OutputDevice *MSCORN::myLATOSS2Output = 0;
OutputDevice *MSCORN::myVehicleDeviceTOSS2SQLOutput = 0;
OutputDevice *MSCORN::myCellTOSS2SQLOutput = 0;
OutputDevice *MSCORN::myLATOSS2SQLOutput = 0;
OutputDevice *MSCORN::myCELLPHONEDUMPOutput = 0;
// c2x output files
OutputDevice *MSCORN::myClusterInfoOutput= 0;
OutputDevice *MSCORN::mySavedInfoOutput= 0;
OutputDevice *MSCORN::mySavedInfoOutputFreq = 0;
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
    // standard output files
    myTripDurationsOutput = 0;
    myVehicleRouteOutput = 0;
    // TrafficOnline output files & settings
    myVehicleDeviceTOSS2Output = 0;
    myCellTOSS2Output = 0;
    myLATOSS2Output = 0;
    myVehicleDeviceTOSS2SQLOutput = 0;
    myCellTOSS2SQLOutput = 0;
    myLATOSS2SQLOutput = 0;
    for (int i=0; i<CORN_MAX; i++) {
        myWished[i] = false;
        myFirstCall[i] = true;
    }
    // c2x output files
    myClusterInfoOutput = 0;
    mySavedInfoOutput = 0;
    myTransmittedInfoOutput = 0;
    myVehicleInRangeOutput = 0;
    mySavedInfoOutputFreq = 0;
    myLastStepClusterInfoOutput = -1;
    myLastStepSavedInfoOutput = -1;
    myLastStepTransmittedInfoOutput = -1;
    myLastStepVehicleInRangeOutput = -1;
}


void
MSCORN::clear()
{
    // standard
    delete myTripDurationsOutput;
    delete myVehicleRouteOutput;
    // TrafficOnline
    delete myVehicleDeviceTOSS2Output;
    delete myCellTOSS2Output;
    delete myLATOSS2Output;
    delete myVehicleDeviceTOSS2SQLOutput;
    delete myCellTOSS2SQLOutput;
    delete myLATOSS2SQLOutput;
    // car2car
    delete myClusterInfoOutput;
    delete mySavedInfoOutput;
    delete myTransmittedInfoOutput;
    delete myVehicleInRangeOutput;
    delete mySavedInfoOutputFreq;
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


void
MSCORN::setTripDurationsOutput(OutputDevice *s)
{
    myTripDurationsOutput = s;
}


void
MSCORN::setVehicleRouteOutput(OutputDevice *s)
{
    myVehicleRouteOutput = s;
}


void
MSCORN::setVehicleDeviceTOSS2Output(OutputDevice *s)
{
    myVehicleDeviceTOSS2Output = s;
}

void
MSCORN::setVehicleDeviceTOSS2SQLOutput(OutputDevice *s)
{
    myVehicleDeviceTOSS2SQLOutput = s;
}

void
MSCORN::setCellTOSS2Output(OutputDevice *s)
{
    myCellTOSS2Output = s;
}

void
MSCORN::setCellTOSS2SQLOutput(OutputDevice *s)
{
    myCellTOSS2SQLOutput = s;
}

void
MSCORN::setLATOSS2Output(OutputDevice *s)
{
    myLATOSS2Output = s;
}

void
MSCORN::setLATOSS2SQLOutput(OutputDevice *s)
{
    myLATOSS2SQLOutput = s;
}

void
MSCORN::setCELLPHONEDUMPOutput(OutputDevice *s)
{
    myCELLPHONEDUMPOutput = s;
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
MSCORN::setSavedInfoOutputFreq(OutputDevice *s)
{
    mySavedInfoOutputFreq = s;
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


void
MSCORN::compute_TripDurationsOutput(MSVehicle *v)
{
    SUMOTime realDepart = (SUMOTime) v->getCORNIntValue(CORN_VEH_REALDEPART);
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    myTripDurationsOutput->getOStream()
    << "   <tripinfo vehicle_id=\"" << v->getID() << "\" "
    << "start=\"" << realDepart << "\" "
    << "wished=\"" << v->desiredDepart() << "\" "
    << "end=\"" << time << "\" "
    << "duration=\"" << time-realDepart << "\" "
    << "waited=\"" << realDepart-v->desiredDepart() << "\" "
    // write reroutes
    << "reroutes=\"";
    if (v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
        myTripDurationsOutput->getOStream()
        << v->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
    } else {
        myTripDurationsOutput->getOStream() << '0';
    }
    myTripDurationsOutput->getOStream() << "\" ";
    // write devices
    myTripDurationsOutput->getOStream() << "devices=\"";
    bool addSem = false;
    if (v->hasCORNIntValue(MSCORN::CORN_VEH_DEV_NO_CPHONE)) {
        int noCPhones = (int) v->getCORNIntValue(MSCORN::CORN_VEH_DEV_NO_CPHONE);
        if(noCPhones!=0) {
            myTripDurationsOutput->getOStream() << "cphones=" << noCPhones;
            addSem = true;
        }
    }
    if (v->isEquipped()) {
        if (addSem) {
            myTripDurationsOutput->getOStream() << ';';
        }
        myTripDurationsOutput->getOStream() << "c2c";
        addSem = true;
    }
    myTripDurationsOutput->getOStream() << "\" vtype=\"" << v->getVehicleType().getID() << "\"/>" << endl;
}


void
MSCORN::compute_VehicleRouteOutput(MSVehicle *v)
{
    myVehicleRouteOutput->getOStream() <<
    "   <vehicle id=\"" << v->getID() << "\" emittedAt=\""
    << v->getCORNIntValue(MSCORN::CORN_VEH_REALDEPART)
    << "\" endedAt=\"" << MSNet::getInstance()->getCurrentTimeStep()
    << "\">" << endl;
    if (v->hasCORNIntValue(CORN_VEH_NUMBERROUTE)) {
        int noReroutes = v->getCORNIntValue(CORN_VEH_NUMBERROUTE);
        for (int i=0; i<noReroutes; i++) {
            v->writeXMLRoute(myVehicleRouteOutput->getOStream(), i);
            myVehicleRouteOutput->getOStream() << endl;
        }
    }
    v->writeXMLRoute(myVehicleRouteOutput->getOStream());
    myVehicleRouteOutput->getOStream() << "   </vehicle>" << endl << endl;
}



inline
std::string
toDateTimeString(SUMOTime time)
{
    std::ostringstream oss;
    char buffer[4];
    sprintf(buffer, "%02i:",(time/3600));
    oss << buffer;
    time=time%3600;
    sprintf(buffer, "%02i:",(time/60));
    oss << buffer;
    time=time%60;
    sprintf(buffer, "%02i", time);
    oss << buffer;
    return oss.str();
}

void
MSCORN::saveTOSS2_CalledPositionData(SUMOTime time, int callID,
                                     const std::string &pos,
                                     int quality)
{
    if (myVehicleDeviceTOSS2Output!=0) {
        std::string timestr="1970-01-01 " + toDateTimeString(time);
        myVehicleDeviceTOSS2Output->getOStream()
        << "01;'" << timestr << "';" << callID << ';' << pos << ';' << quality << "\n"; // !!! check <CR><LF>-combination
    }
}

void
MSCORN::saveTOSS2SQL_CalledPositionData(SUMOTime time, int callID,
                                        const std::string &pos,
                                        int quality)
{
    if (myVehicleDeviceTOSS2SQLOutput!=0) {
        if (!MSCORN::myFirstCall[CORN_OUT_DEVICE_TO_SS2_SQL])
            myVehicleDeviceTOSS2SQLOutput->getOStream() << "," << endl;
        else
            MSCORN::myFirstCall[CORN_OUT_DEVICE_TO_SS2_SQL] = false;
        std::string timestr="1970-01-01 " + toDateTimeString(time);
        myVehicleDeviceTOSS2SQLOutput->getOStream()
        << "(NULL, NULL, '" << timestr << "', " << pos << ", " << callID
        << ", " << quality << ")";
        //<< "(NULL, NULL, '" << timestr << "', NULL , NULL, '1;"
        //	<< timestr << ';' << callID << ';' << pos << ';' << quality << "',1)";
    }
}


void
MSCORN::saveTOSS2_CellStateData(SUMOTime time,
                                int Cell_Id, int Calls_In, int Calls_Out, int Dyn_Calls_In,
                                int Dyn_Calls_Out, int Sum_Calls, int Intervall)
{
    if (myCellTOSS2Output!=0) {
        std::string timestr="1970-01-01 " + toDateTimeString(time);
        myCellTOSS2Output->getOStream()
        << "02;" << timestr << ';' << Cell_Id << ';' << Calls_In << ';' << Calls_Out << ';' <<
        Dyn_Calls_In << ';' << Dyn_Calls_Out << ';' << Sum_Calls << ';' << Intervall << "\n";
    }
}

void
MSCORN::saveTOSS2SQL_CellStateData(SUMOTime time,
                                   int Cell_Id, int Calls_In, int Calls_Out, int Dyn_Calls_In,
                                   int Dyn_Calls_Out, int Sum_Calls, int Intervall)
{
    if (myCellTOSS2SQLOutput!=0) {
        if (!MSCORN::myFirstCall[CORN_OUT_CELL_TO_SS2_SQL])
            myCellTOSS2SQLOutput->getOStream() << "," << endl;
        else
            MSCORN::myFirstCall[CORN_OUT_CELL_TO_SS2_SQL] = false;
        std::string timestr="1970-01-01 " + toDateTimeString(time);
        myCellTOSS2SQLOutput->getOStream()
        << "(NULL, \' \', '" << timestr << "'," << Cell_Id << ',' << Calls_In << ',' << Calls_Out << ','
        << Dyn_Calls_In << ',' << Dyn_Calls_Out << ',' << Sum_Calls << ',' << Intervall << ")";
    }
}

void
MSCORN::saveTOSS2SQL_LA_ChangesData(SUMOTime time, int position_id,
                                    int dir, int sum_changes, int quality_id, int intervall)
{
    if (myLATOSS2SQLOutput!=0) {
        if (!MSCORN::myFirstCall[CORN_OUT_LA_TO_SS2_SQL])
            myLATOSS2SQLOutput->getOStream() << "," << endl;
        else
            MSCORN::myFirstCall[CORN_OUT_LA_TO_SS2_SQL] = false;
        std::string timestr="1970-01-01 " + toDateTimeString(time);
        myLATOSS2SQLOutput->getOStream()
        << "(NULL, \' \', '" << timestr << "'," << position_id << ',' << dir << ',' << sum_changes << ','
        << quality_id << ',' << intervall << ")";
    }
}

void
MSCORN::saveTOSS2_LA_ChangesData(SUMOTime time, int position_id,
                                 int dir, int sum_changes, int quality_id, int intervall)
{
    if (myLATOSS2Output!=0) {
        std::string timestr="1970-01-01 " + toDateTimeString(time);
        myLATOSS2Output->getOStream()
        << "03;" << ';' << timestr << ';' << position_id << ';' << dir << ';' << sum_changes
        << ';' << quality_id << ';' << intervall << "\n";
    }
}

void
MSCORN::saveCELLPHONEDUMP(SUMOTime time, int cell_id, int call_id, int event_type)
{/*event_type describes if a connection begins(0), changecell(1) or ends(2) */
    if (myCELLPHONEDUMPOutput != 0) {
        myCELLPHONEDUMPOutput->getOStream()
        << time << ';' << call_id << ';' << cell_id << ';' << event_type << "\n";
    }
}


//car2car
void
MSCORN::saveClusterInfoData(SUMOTime step, int id, 
                            const std::string &headID, const std::string &vehs,
                            int quantity)
{
    if(myClusterInfoOutput==0) {
        return;
    }
    if(myLastStepClusterInfoOutput!=step) {
        if(myLastStepClusterInfoOutput!=-1) {
            // close the previous step if its not the first one
            myClusterInfoOutput->getOStream() << "   </timestep>" << endl;
        }
        myClusterInfoOutput->getOStream() << "   <timestep time=\"" << step << "\">"<< endl;
    }
    myLastStepClusterInfoOutput = step;
    myClusterInfoOutput->getOStream()
    << "      <cluster id=\"" << id 
    << "\" headID=\"" << headID << "\" vehNo=\"" << quantity << "\""
    << ">" << vehs << "</cluster>" << endl;
}


void
MSCORN::checkCloseClusterInfoData()
{
    if(myLastStepClusterInfoOutput!=-1) {
        myClusterInfoOutput->getOStream() << "   </timestep>" << endl;
    }
}


void
MSCORN::saveSavedInformationDataFreq(SUMOTime step, const MSVehicle &veh)
{
    if (mySavedInfoOutputFreq!=0) {
        int noReroutes = veh.hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)
                         ? veh.getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE) : 0;
        mySavedInfoOutputFreq->getOStream()
        << "	<vehicle id=\"" << veh.getID()
        << "\" timestep=\"" << step
        << "\" numberOfInfos=\"" << veh.getTotalInformationNumber()
        << "\" numberRelevant=\"" << veh.getNoGotRelevant()
        << "\" got=\"" << veh.getNoGot()
        << "\" sent=\"" << veh.getNoSent()
        << "\" reroutes=\"" << noReroutes
        << "\"/>"<<endl;
    }
}


void
MSCORN::saveSavedInformationData(SUMOTime step, const std::string &veh,
                                 const std::string &edge, const std::string &type, 
                                 int time, SUMOReal nt)
{
    if(mySavedInfoOutput==0) {
        return;
    }
    if(myLastStepSavedInfoOutput!=step) {
        if(myLastStepSavedInfoOutput!=-1) {
            // close the previous step if its not the first one
            mySavedInfoOutput->getOStream() << "   </timestep>"<< endl;
        }
        mySavedInfoOutput->getOStream() << "   <timestep time=\"" << step << "\">"<< endl;
    }
    myLastStepSavedInfoOutput = step;
    // save the current values
    mySavedInfoOutput->getOStream()
        << "      <info veh=\"" << veh <<"\" edge=\"" << edge << "\""<< " type=\"" << type << "\" "
        << "time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\"/>"<<endl;
}

void
MSCORN::checkCloseSavedInformationData()
{
    if(myLastStepSavedInfoOutput!=-1) {
        mySavedInfoOutput->getOStream() << "   </timestep>"<< endl;
    }
}



void
MSCORN::saveTransmittedInformationData(SUMOTime step, const std::string &from, const std::string &to,
                                       const std::string &edge, int time, SUMOReal nt)
{
    if (myTransmittedInfoOutput==0) {
        return;
    }
    if(myLastStepTransmittedInfoOutput!=step) {
        if(myLastStepTransmittedInfoOutput!=-1) {
            // close the previous step if its not the first one
            myTransmittedInfoOutput->getOStream() << "   </timestep>"<< endl;
        }
        myTransmittedInfoOutput->getOStream() << "   <timestep time=\"" << step << "\">"<< endl;
    }
    myLastStepTransmittedInfoOutput = step;
    myTransmittedInfoOutput->getOStream()
        << "      <info edge=\"" << edge <<"\" from=\"" << from << "\""<< " to=\"" << to << "\" "
        << "time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\"/>"<<endl;
}

void
MSCORN::checkCloseTransmittedInformationData()
{
    if(myLastStepTransmittedInfoOutput!=-1) {
        myTransmittedInfoOutput->getOStream() << "   </timestep>"<< endl;
    }
}


void
MSCORN::saveVehicleInRangeData(SUMOTime step, const std::string &veh1, const std::string &veh2,
                               SUMOReal x1, SUMOReal y1, SUMOReal x2 , SUMOReal y2)
{
    if (myVehicleInRangeOutput==0) {
        return;
    }
    if(myLastStepVehicleInRangeOutput!=step) {
        if(myLastStepVehicleInRangeOutput!=-1) {
            // close the previous step if its not the first one
            myVehicleInRangeOutput->getOStream() << "   </timestep>"<< endl;
        }
        myVehicleInRangeOutput->getOStream() << "   <timestep time=\"" << step << "\">"<< endl;
    }
    myLastStepVehicleInRangeOutput = step;
    myVehicleInRangeOutput->getOStream()
    << "      <connection veh1=\"" << veh1<<"\"" <<" x1=\""<< x1 << "\""<< " y1=\"" << y1 << "\" "
    <<"veh2=\"" << veh2<<"\"" <<" x2=\""<< x2 << "\""<< " y2=\"" << y2 << "\" "
    <<"/>"<<endl;
}


void
MSCORN::checkCloseVehicleInRangeData()
{
    if(myLastStepVehicleInRangeOutput!=-1) {
        myVehicleInRangeOutput->getOStream() << "   </timestep>"<< endl;
    }
}



/****************************************************************************/

