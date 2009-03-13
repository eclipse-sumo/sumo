/****************************************************************************/
/// @file    MSDevice_HBEFA.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 30.01.2009
/// @version $Id: MSDevice_HBEFA.cpp 6619 2009-01-13 13:34:48Z behrisch $
///
// A device which collects vehicular emissions (using HBEFA-reformulation)
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

#include "MSDevice_HBEFA.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/StaticCommand.h>
#include <utils/common/SUMODijkstraRouter.h>
#include <microsim/output/HelpersHBEFA.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


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
    oc.addDescription("device.hbefa.probability", "Routing", "The probability for a vehicle to have an emission logging device");

    oc.doRegister("device.hbefa.knownveh", new Option_String());//!!! describe
    oc.addDescription("device.hbefa.knownveh", "Routing", "Assign a device to named vehicles");

    oc.doRegister("device.hbefa.deterministic", new Option_Bool(false)); //!!! describe
    oc.addDescription("device.hbefa.deterministic", "Routing", "The devices are set deterministic using a fraction of 1000");

    myVehicleIndex = 0;
}


void
MSDevice_HBEFA::buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    if (oc.getFloat("device.hbefa.probability")==0&&!oc.isSet("device.hbefa.knownveh")) {
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
MSDevice_HBEFA::MSDevice_HBEFA(MSVehicle &holder, const std::string &id) throw()
        : MSDevice(holder, id), myComputeAndCollectCommand(0),
        myCO2(0), myCO(0), myHC(0), myPMx(0), myNOx(0), myFuel(0) {
}


MSDevice_HBEFA::~MSDevice_HBEFA() throw() {
    // make the rerouting command invalid
    myComputeAndCollectCommand->deschedule();
}


void
MSDevice_HBEFA::enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &) {
    myComputeAndCollectCommand = new WrappingCommand< MSDevice_HBEFA >(this, &MSDevice_HBEFA::wrappedComputeCommandExecute);
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(
        myComputeAndCollectCommand, MSNet::getInstance()->getCurrentTimeStep(),
        MSEventControl::ADAPT_AFTER_EXECUTION);
}


SUMOTime
MSDevice_HBEFA::wrappedComputeCommandExecute(SUMOTime currentTime) throw(ProcessError) {
    SUMOEmissionClass c = getHolder().getVehicleType().getEmissionClass();
    SUMOReal v = getHolder().getSpeed();
    SUMOReal a = getHolder().getPreDawdleAcceleration();
    myCO2 += HelpersHBEFA::computeCO2(c, v, a);
    myCO += HelpersHBEFA::computeCO(c, v, a);
    myHC += HelpersHBEFA::computeHC(c, v, a);
    myPMx += HelpersHBEFA::computePMx(c, v, a);
    myNOx += HelpersHBEFA::computeNOx(c, v, a);
    myFuel += HelpersHBEFA::computeFuel(c, v, a);
    return 1;
}


bool
MSDevice_HBEFA::tripInfoOutput(OutputDevice &os, const std::string &intend) const throw(IOError) {
    os << intend <<
    "<emissions CO_abs=\"" << myCO <<
    "\" CO2_abs=\"" << myCO2 <<
    "\" HC_abs=\"" << myHC <<
    "\" PMx_abs=\""<< myPMx <<
    "\" NOx_abs=\""<< myNOx <<
    "\" fuel_abs=\""<< myFuel <<
    "\"/>";
    return true;
}



/****************************************************************************/

