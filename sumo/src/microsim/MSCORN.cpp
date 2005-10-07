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
bool MSCORN::myWished[CORN_MAX];


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
MSCORN::init()
{
    myTripDurationsOutput = 0;
	myVehicleRouteOutput = 0;
    for(int i=0; i<CORN_MAX; i++) {
        myWished[i] = false;
    }
}


void
MSCORN::clear()
{
	delete myTripDurationsOutput;
	delete myVehicleRouteOutput;
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
MSCORN::compute_TripDurationsOutput(MSVehicle *v)
{
    SUMOTime realDepart = (SUMOTime) v->getCORNDoubleValue(CORN_VEH_REALDEPART);
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    myTripDurationsOutput->getOStream()
        << "<tripinfo id=\"" << v->id() << "\" "
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
	for(int i=0; i<(int) v->getCORNDoubleValue(CORN_VEH_NUMBERROUTE); i++) {
		v->writeXMLRoute(myVehicleRouteOutput->getOStream(), i);
        myVehicleRouteOutput->getOStream() << endl;
	}
	v->writeXMLRoute(myVehicleRouteOutput->getOStream());
	myVehicleRouteOutput->getOStream() <<
		"   </vehicle>" << endl << endl;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

