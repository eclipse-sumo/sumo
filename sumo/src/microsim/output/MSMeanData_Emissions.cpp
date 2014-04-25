/****************************************************************************/
/// @file    MSMeanData_Emissions.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Emission data collector for edges/lanes that
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Emissions.h"
#include <utils/emissions/PollutantsInterface.h>
#include <limits>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// MSMeanData_Emissions::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Emissions::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane* const lane,
        const SUMOReal length, const bool doAdd,
        const std::set<std::string>* const vTypes,
        const MSMeanData_Emissions* parent)
    : MSMeanData::MeanDataValues(lane, length, doAdd, vTypes),
      myEmissions(), myParent(parent) {}


MSMeanData_Emissions::MSLaneMeanDataValues::~MSLaneMeanDataValues() {
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::reset(bool) {
    sampleSeconds = 0.;
    travelledDistance = 0.;
    myEmissions = PollutantsInterface::Emissions();
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues& val) const {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.sampleSeconds += sampleSeconds;
    v.travelledDistance += travelledDistance;
    v.myEmissions.addScaled(myEmissions);
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::notifyMoveInternal(SUMOVehicle& veh, SUMOReal timeOnLane, SUMOReal speed) {
    sampleSeconds += timeOnLane;
    travelledDistance += speed * timeOnLane;
    const double a = veh.getAcceleration();
    myEmissions.addScaled(PollutantsInterface::computeAll(veh.getVehicleType().getEmissionClass(), speed, a, veh.getSlope()), timeOnLane);
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::write(OutputDevice& dev, const SUMOTime period,
        const SUMOReal /*numLanes*/, const SUMOReal defaultTravelTime, const int /*numVehicles*/) const {
    const SUMOReal normFactor = SUMOReal(3600. / STEPS2TIME(period) / myLaneLength);
    dev << " CO_abs=\"" << OutputDevice::realString(myEmissions.CO, 6) <<
        "\" CO2_abs=\"" << OutputDevice::realString(myEmissions.CO2, 6) <<
        "\" HC_abs=\"" << OutputDevice::realString(myEmissions.HC, 6) <<
        "\" PMx_abs=\"" << OutputDevice::realString(myEmissions.PMx, 6) <<
        "\" NOx_abs=\"" << OutputDevice::realString(myEmissions.NOx, 6) <<
        "\" fuel_abs=\"" << OutputDevice::realString(myEmissions.fuel, 6) <<
        "\"\n            CO_normed=\"" << OutputDevice::realString(normFactor * myEmissions.CO, 6) <<
        "\" CO2_normed=\"" << OutputDevice::realString(normFactor * myEmissions.CO2, 6) <<
        "\" HC_normed=\"" << OutputDevice::realString(normFactor * myEmissions.HC, 6) <<
        "\" PMx_normed=\"" << OutputDevice::realString(normFactor * myEmissions.PMx, 6) <<
        "\" NOx_normed=\"" << OutputDevice::realString(normFactor * myEmissions.NOx, 6) <<
        "\" fuel_normed=\"" << OutputDevice::realString(normFactor * myEmissions.fuel, 6);
    if (sampleSeconds > myParent->myMinSamples) {
        SUMOReal vehFactor = myParent->myMaxTravelTime / sampleSeconds;
        SUMOReal traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            vehFactor = MIN2(vehFactor, myLaneLength / travelledDistance);
            traveltime = MIN2(traveltime, myLaneLength * sampleSeconds / travelledDistance);
        }
        dev << "\"\n            traveltime=\"" << OutputDevice::realString(traveltime) <<
            "\" CO_perVeh=\"" << OutputDevice::realString(myEmissions.CO * vehFactor, 6) <<
            "\" CO2_perVeh=\"" << OutputDevice::realString(myEmissions.CO2 * vehFactor, 6) <<
            "\" HC_perVeh=\"" << OutputDevice::realString(myEmissions.HC * vehFactor, 6) <<
            "\" PMx_perVeh=\"" << OutputDevice::realString(myEmissions.PMx * vehFactor, 6) <<
            "\" NOx_perVeh=\"" << OutputDevice::realString(myEmissions.NOx * vehFactor, 6) <<
            "\" fuel_perVeh=\"" << OutputDevice::realString(myEmissions.fuel * vehFactor, 6);
    } else if (defaultTravelTime >= 0.) {
        const MSVehicleType* t = MSNet::getInstance()->getVehicleControl().getVType();
        const SUMOReal speed = MIN2(myLaneLength / defaultTravelTime, t->getMaxSpeed());
        dev << "\"\n            traveltime=\"" << OutputDevice::realString(defaultTravelTime) <<
            "\" CO_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::CO, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" CO2_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::CO2, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" HC_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::HC, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" PMx_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::PM_X, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" NOx_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::NO_X, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" fuel_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::FUEL, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6); // @todo: give correct slope
    }
    dev << "\"";
    dev.closeTag();
}



// ---------------------------------------------------------------------------
// MSMeanData_Emissions - methods
// ---------------------------------------------------------------------------
MSMeanData_Emissions::MSMeanData_Emissions(const std::string& id,
        const SUMOTime dumpBegin,
        const SUMOTime dumpEnd,
        const bool useLanes, const bool withEmpty,
        const bool printDefaults,
        const bool withInternal,
        const bool trackVehicles,
        const SUMOReal maxTravelTime,
        const SUMOReal minSamples,
        const std::set<std::string> vTypes)
    : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, printDefaults,
                 withInternal, trackVehicles, maxTravelTime, minSamples, vTypes) {
}


MSMeanData_Emissions::~MSMeanData_Emissions() {}


MSMeanData::MeanDataValues*
MSMeanData_Emissions::createValues(MSLane* const lane, const SUMOReal length, const bool doAdd) const {
    return new MSLaneMeanDataValues(lane, length, doAdd, &myVehicleTypes, this);
}


/****************************************************************************/
