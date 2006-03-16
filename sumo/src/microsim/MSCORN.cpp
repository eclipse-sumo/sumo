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
OutputDevice *MSCORN::myTripDurationsOutput = 0;
OutputDevice *MSCORN::myVehicleRouteOutput = 0;
OutputDevice *MSCORN::myVehicleDeviceTOSS2Output = 0;
OutputDevice *MSCORN::myCellTOSS2Output = 0;
OutputDevice *MSCORN::myLATOSS2Output = 0;

bool MSCORN::myWished[CORN_MAX];


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
MSCORN::init()
{
    myTripDurationsOutput = 0;
	myVehicleRouteOutput = 0;
    myVehicleDeviceTOSS2Output = 0;
	myCellTOSS2Output = 0;
	myLATOSS2Output = 0;
    for(int i=0; i<CORN_MAX; i++) {
        myWished[i] = false;
    }
}


void
MSCORN::clear()
{
	delete myTripDurationsOutput;
	delete myVehicleRouteOutput;
    delete myVehicleDeviceTOSS2Output;
	delete myCellTOSS2Output;
	delete myLATOSS2Output;
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
MSCORN::setCellTOSS2Output(OutputDevice *s)
{
    myCellTOSS2Output = s;
}

void
MSCORN::setLATOSS2Output(OutputDevice *s)
{
    myLATOSS2Output = s;
}



void
MSCORN::compute_TripDurationsOutput(MSVehicle *v)
{
    SUMOTime realDepart = (SUMOTime) v->getCORNDoubleValue(CORN_VEH_REALDEPART);
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    myTripDurationsOutput->getOStream()
        << "   <tripinfo vehicle_id=\"" << v->id() << "\" "
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
		"   <vehicle id=\"" << v->id() << "\" emitedAt=\""
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


void
MSCORN::saveTOSS2_CalledPositionData(SUMOTime time, int callID,
                                     const std::string &pos,
                                     int quality)
{
    if(myVehicleDeviceTOSS2Output!=0) {
        myVehicleDeviceTOSS2Output->getOStream()
            << "01;" << time << ';' << callID << ';' << pos << ';' << quality << "\n"; // !!! check <CR><LF>-combination
    }
}
	

void
MSCORN::saveTOSS2_CellStateData(SUMOTime time, 
		int Cell_Id, int Calls_In, int Calls_Out, int Dyn_Calls_In, 
		int Dyn_Calls_Out, int Sum_Calls, int Intervall)
{
	if(myCellTOSS2Output!=0)
	{
		myCellTOSS2Output->getOStream()
			<< "02;" << time << ';' << Cell_Id << ';' << Calls_In << ';' << Calls_Out << ';' << 
			Dyn_Calls_In << ';' << Dyn_Calls_Out << ';' << Sum_Calls << ';' << Intervall << "\n";
	}
}
	
void
MSCORN::saveTOSS2_LA_ChangesData(SUMOTime time, int position_id, 
        int dir, int sum_changes, int quality_id, int intervall)
{
	if(myLATOSS2Output!=0)
	{
		myLATOSS2Output->getOStream() 
			<< "03;" << ';' << time << ';' << position_id << ';' << dir << ';' << sum_changes
			<< ';' << quality_id << ';' << intervall << "\n";
	}
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

