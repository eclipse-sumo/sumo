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
// Revision 1.2  2004/08/02 12:13:33  dkrajzew
// output device handling rechecked; tiny documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "MSVehicle.h"
#include "MSCORN.h"
#include <utils/iodevices/OutputDevice.h>

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
OutputDevice *MSCORN::myTripInfoOut = 0;
bool MSCORN::myWished[CORN_MAX];


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
MSCORN::init()
{
    myTripInfoOut = 0;
    for(int i=0; i<CORN_MAX; i++) {
        myWished[i] = false;
    }
}


void
MSCORN::clear()
{
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
    case CORN_OUT_TRIPOUTPUT:
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
MSCORN::setTripInfoOutput(OutputDevice *s)
{
    myTripInfoOut = s;
}


void
MSCORN::compute_TripInfoOutput(MSVehicle *v)
{
    myTripInfoOut->getOStream()
        << "<tripinfo id=\"" << v->id() << "\" "
        << "start=\"" << v->getCORNDoubleValue(CORN_VEH_REALDEPART) << "\" "
        << "wished=\"" << v->desiredDepart() << "\" "
        << "end=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\"/>" << endl;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

