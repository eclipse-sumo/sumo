/****************************************************************************/
/// @file    MSDevice_HBEFA.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects vehicular emissions (using HBEFA-reformulation)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include "MSDevice_HBEFA.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/HelpersHBEFA.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
int MSDevice_HBEFA::myVehicleIndex = 0;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_HBEFA::insertOptions() throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addOptionSubTopic("Emissions");

    oc.doRegister("device.hbefa.probability", new Option_Float(0.));//!!! describe
    oc.addDescription("device.hbefa.probability", "Emissions", "The probability for a vehicle to have an emission logging device");

    oc.doRegister("device.hbefa.knownveh", new Option_String());//!!! describe
    oc.addDescription("device.hbefa.knownveh", "Emissions", "Assign a device to named vehicles");

    oc.doRegister("device.hbefa.deterministic", new Option_Bool(false)); //!!! describe
    oc.addDescription("device.hbefa.deterministic", "Emissions", "The devices are set deterministic using a fraction of 1000");

    myVehicleIndex = 0;
}


void
MSDevice_HBEFA::buildVehicleDevices(SUMOVehicle &v, std::vector<MSDevice*> &into) throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    if (oc.getFloat("device.hbefa.probability")==0 && !oc.isSet("device.hbefa.knownveh")) {
        // no route computation is modelled
        return;
    }
    // route computation is enabled
    bool haveByNumber = false;
    if (oc.getBool("device.hbefa.deterministic")) {
        haveByNumber = ((myVehicleIndex%1000) < (int)(oc.getFloat("device.hbefa.probability")*1000.));
    } else {
        haveByNumber = RandHelper::rand()<=oc.getFloat("device.hbefa.probability");
    }
    bool haveByName = oc.isSet("device.hbefa.knownveh") && OptionsCont::getOptions().isInStringVector("device.hbefa.knownveh", v.getID());
    if (haveByNumber||haveByName) {
        // build the device
        MSDevice_HBEFA* device = new MSDevice_HBEFA(v, "hbefa_" + v.getID());
        into.push_back(device);
    }
    myVehicleIndex++;
}


// ---------------------------------------------------------------------------
// MSDevice_HBEFA-methods
// ---------------------------------------------------------------------------
MSDevice_HBEFA::MSDevice_HBEFA(SUMOVehicle &holder, const std::string &id) throw()
        : MSDevice(holder, id),
        myCO2(0), myCO(0), myHC(0), myPMx(0), myNOx(0), myFuel(0) {
}


MSDevice_HBEFA::~MSDevice_HBEFA() throw() {
}


bool
MSDevice_HBEFA::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/, SUMOReal /*newPos*/, SUMOReal newSpeed) throw() {
    SUMOEmissionClass c = veh.getVehicleType().getEmissionClass();
    SUMOReal a = veh.getPreDawdleAcceleration();
    myCO2 += HelpersHBEFA::computeCO2(c, newSpeed, a);
    myCO += HelpersHBEFA::computeCO(c, newSpeed, a);
    myHC += HelpersHBEFA::computeHC(c, newSpeed, a);
    myPMx += HelpersHBEFA::computePMx(c, newSpeed, a);
    myNOx += HelpersHBEFA::computeNOx(c, newSpeed, a);
    myFuel += HelpersHBEFA::computeFuel(c, newSpeed, a);
    return true;
}


void
MSDevice_HBEFA::generateOutput() const throw(IOError) {
    OutputDevice &os = OutputDevice::getDeviceByOption("tripinfo-output");
    os << resetiosflags(std::ios::floatfield);
    (os.openTag("emissions") <<
     " CO_abs=\"" << myCO <<
     "\" CO2_abs=\"" << myCO2 <<
     "\" HC_abs=\"" << myHC <<
     "\" PMx_abs=\""<< myPMx <<
     "\" NOx_abs=\""<< myNOx <<
     "\" fuel_abs=\""<< myFuel <<
     "\"").closeTag(true);
    os<<setiosflags(std::ios::fixed);
}



/****************************************************************************/

