//---------------------------------------------------------------------------//
//                        MSCORN.cpp -
//  A storage for optional things to compute
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.20  2006/11/27 14:08:51  dkrajzew
// added Danilot's current changes
//
// Revision 1.19  2006/11/24 10:34:59  dkrajzew
// added Eric Nicolay's current code
//
// Revision 1.18  2006/11/16 07:02:17  dkrajzew
// warnings removed
//
// Revision 1.17  2006/11/14 06:45:24  dkrajzew
// prettier timestr format for TrafficOnline
//
// Revision 1.16  2006/11/08 16:18:44  ericnicolay
// -change code for ss2-sql-output
//
// Revision 1.15  2006/11/02 11:44:50  dkrajzew
// added Danilo Teta-Boyom's changes to car2car-communication
//
// Revision 1.14  2006/10/19 11:03:12  ericnicolay
// change code for the ss2-sql-output
//
// Revision 1.13  2006/10/12 14:48:43  ericnicolay
// removes bug in the ss2-sql-output
//
// Revision 1.12  2006/09/18 10:05:46  dkrajzew
// patching junction-internal state simulation
//
// Revision 1.11  2006/09/15 09:28:47  ericnicolay
// TO SS2 SQL output added
//
// Revision 1.10  2006/04/05 05:28:49  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.9  2006/03/17 09:04:10  dkrajzew
// beautifying
//
// Revision 1.8  2006/03/16 15:19:35  ericnicolay
// add ss2 interface for cells and LAs
//
// Revision 1.7  2006/02/23 11:31:09  dkrajzew
// TO SS2 output added
//
// Revision 1.6  2006/01/16 13:35:52  dkrajzew
// output formats updated for the next release
//
// Revision 1.5  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/11/23 10:20:09  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.2  2004/08/02 12:13:33  dkrajzew
// output device handling rechecked; tiny documentation added
//
//
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

#include <iostream>
#include "MSVehicle.h"
#include "MSCORN.h"
#include <utils/iodevices/OutputDevice.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
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
// c2x output files
OutputDevice *MSCORN::myClusterInfoOutput= 0;
OutputDevice *MSCORN::myEdgeNearInfoOutput= 0;
OutputDevice *MSCORN::mySavedInfoOutput= 0;
OutputDevice *MSCORN::mySavedInfoOutputFreq = 0;
OutputDevice *MSCORN::myTransmittedInfoOutput= 0;
OutputDevice *MSCORN::myVehicleInRangeOutput= 0;

bool MSCORN::myWished[CORN_MAX];
bool MSCORN::myFirstCall[CORN_MAX];


/* =========================================================================
 * method definitions
 * ======================================================================= */
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
    for(int i=0; i<CORN_MAX; i++) {
        myWished[i] = false;
		myFirstCall[i] = true;
    }
    // c2x output files
    myClusterInfoOutput = 0;
    myEdgeNearInfoOutput = 0;
    mySavedInfoOutput = 0;
    myTransmittedInfoOutput = 0;
    myVehicleInRangeOutput = 0;
	mySavedInfoOutputFreq = 0;
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
    delete myEdgeNearInfoOutput;
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
    switch(f) {
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

//car2car
void
MSCORN::setClusterInfoOutput(OutputDevice *s)
{
    myClusterInfoOutput = s;
}

void
MSCORN::setEdgeNearInfoOutput(OutputDevice *s)
{
    myEdgeNearInfoOutput = s;
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
    SUMOTime realDepart = (SUMOTime) v->getCORNDoubleValue(CORN_VEH_REALDEPART);
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    myTripDurationsOutput->getOStream()
        << "   <tripinfo vehicle_id=\"" << v->getID() << "\" "
        << "start=\"" << realDepart << "\" "
        << "wished=\"" << v->desiredDepart() << "\" "
        << "end=\"" << time << "\" "
        << "duration=\"" << time-realDepart << "\" "
        << "waited=\"" << realDepart-v->desiredDepart() << "\" "
        << "/>" << endl;
}


void
MSCORN::compute_VehicleRouteOutput(MSVehicle *v)
{
	myVehicleRouteOutput->getOStream() <<
		"   <vehicle id=\"" << v->getID() << "\" emitedAt=\""
        << v->getCORNDoubleValue(MSCORN::CORN_VEH_REALDEPART)
        << "\" endedAt=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\">" << endl;
    if(v->hasCORNDoubleValue(CORN_VEH_NUMBERROUTE)) {
    	for(int i=0; i<(int) v->getCORNDoubleValue(CORN_VEH_NUMBERROUTE); i++) {
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
    if(myVehicleDeviceTOSS2Output!=0) {
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
    if(myVehicleDeviceTOSS2SQLOutput!=0) {
		if(!MSCORN::myFirstCall[CORN_OUT_DEVICE_TO_SS2_SQL])
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
    if(myCellTOSS2Output!=0) {
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
	if(myCellTOSS2SQLOutput!=0)
	{
		if(!MSCORN::myFirstCall[CORN_OUT_CELL_TO_SS2_SQL])
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
	if(myLATOSS2SQLOutput!=0)
	{
		if(!MSCORN::myFirstCall[CORN_OUT_LA_TO_SS2_SQL])
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
	if(myLATOSS2Output!=0) {
		std::string timestr="1970-01-01 " + toDateTimeString(time);
        myLATOSS2Output->getOStream()
            << "03;" << ';' << timestr << ';' << position_id << ';' << dir << ';' << sum_changes
            << ';' << quality_id << ';' << intervall << "\n";
    }
}

//car2car
void
MSCORN::saveClusterInfoData(SUMOTime step, int id, const std::string vehs, int quantity, int a)
{
    if(myClusterInfoOutput!=0) {
        if(a==0){
            myClusterInfoOutput->getOStream()
                << "	<timeStep time=\"" << step << "\">"<< "\n";
        }
        if(a==1) {
            myClusterInfoOutput->getOStream()
                << "	</timeStep>"<< "\n";
        }
        if(a==-1) {
            myClusterInfoOutput->getOStream()
                << "		<cluster id=\"" <<id<< "\" "<<" NOfveh =\""<<quantity<<"\""
                <<">"<<vehs<<"</cluster>"<< "\n";
        }
    }
}

void
MSCORN::saveEdgeNearInfoData(const std::string id, const std::string neighbor, int quantity)
{
    if(myEdgeNearInfoOutput!=0) {
        myEdgeNearInfoOutput->getOStream()
            << "	<edge id=\"" << id << "\" "<<" NOfNeighbor =\""<<quantity<<"\""
            <<">"<<neighbor<<" </edge>"<< "\n";
    }
}

void
MSCORN::saveSavedInformationDataFreq(SUMOTime step, const std::string veh, int numberOfInfos)
{
    if(mySavedInfoOutputFreq!=0) {
		mySavedInfoOutputFreq->getOStream()
				<< "	<vehicle id=\"" << veh << "\""<<" timestep=\""<<step<<"\""<<" numberOfInfos=\""<<numberOfInfos<<"\" "<<"/>"<<"\n";
	}
}


void
MSCORN::saveSavedInformationData(SUMOTime step, const std::string veh,
				 const std::string edge, std::string type, int time, SUMOReal nt, int a)
{
    if(mySavedInfoOutput!=0) {
        if(a==0){
            mySavedInfoOutput->getOStream()
                << "	<timeStep time=\"" << step << "\">"<< "\n";
        }
        if(a==1) {
            mySavedInfoOutput->getOStream()
                << "	</timeStep>"<< "\n";
        }
        if(a==-1) {
            mySavedInfoOutput->getOStream()
                << "		<info veh=\"" << veh <<"\" edge=\"" << edge << "\""<< " type=\"" << type << "\" "
                <<" time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\" "<<"/>"<<"\n";
        }
    }
}

void
MSCORN::saveTransmittedInformationData(SUMOTime step, const std::string from, const std::string to,
    const std::string edge, int time, SUMOReal nt, int a)
{
    if(myTransmittedInfoOutput!=0) {
        if(a==0){
            myTransmittedInfoOutput->getOStream()
                << "	<timeStep time=\"" << step << "\">"<< "\n";
        }
        if(a==1) {
            myTransmittedInfoOutput->getOStream()
                << "	</timeStep>"<< "\n";
        }
        if(a==-1) {
            myTransmittedInfoOutput->getOStream()
                << "		<info edge=\"" << edge <<"\" from=\"" << from << "\""<< " to=\"" << to << "\" "
                <<" time=\""<<time<<"\""<<" neededtime=\""<<nt<<"\" "<<"/>"<<"\n";
        }
    }
}

void
MSCORN::saveVehicleInRangeData(SUMOTime step, const std::string veh1, const std::string veh2,
    SUMOReal x1, SUMOReal y1, SUMOReal x2 , SUMOReal y2, int a)
{
    if(myVehicleInRangeOutput!=0) {
        if(a==0){
            myVehicleInRangeOutput->getOStream()
                << "	<timeStep time=\"" << step << "\">"<< "\n";
        }
        if(a==1) {
            myVehicleInRangeOutput->getOStream()
                << "	</timeStep>"<< "\n";
        }
        if(a==-1) {
            myVehicleInRangeOutput->getOStream()
                << "		<connection veh1=\"" << veh1<<"\"" <<" x1=\""<< x1 << "\""<< " y1=\"" << y1 << "\" "
                <<" veh2=\"" << veh2<<"\"" <<" x2=\""<< x2 << "\""<< " y2=\"" << y2 << "\" "
                <<"/>"<<"\n";
        }
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

