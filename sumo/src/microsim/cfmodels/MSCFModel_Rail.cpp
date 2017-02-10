/****************************************************************************/
/// @file    MSCFModel_Rail.cpp
/// @author  Gregor L\"ammel
/// @date    Tue, 08 Feb 2017
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include <iostream>
#include <utils/common/MsgHandler.h>
#include "MSCFModel_Rail.h"

MSCFModel_Rail::MSCFModel_Rail(const MSVehicleType *vtype, std::string trainType) : MSCFModel(vtype, 1.0, 1.0, 1.0) {

    if (trainType.compare("RB425") == 0) {
        traction = initRB425Traction();
        resistance = initRB425Resistance();
        trainParams = initRB425Params();

    } else if (trainType.compare("NGT400") == 0) {
        traction = initNGT400Traction();
        resistance = initNGT400Resistance();
        trainParams = initNGT400Params();
    }else {
        WRITE_ERROR("Unknown train type: " + trainType);
        throw ProcessError();
    }



}

MSCFModel_Rail::~MSCFModel_Rail() {

}

SUMOReal MSCFModel_Rail::followSpeed(const MSVehicle *const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed,
                                     SUMOReal predMaxDecel) const {
    return speed;
}

SUMOReal MSCFModel_Rail::stopSpeed(const MSVehicle *const veh, const SUMOReal speed, SUMOReal gap) const {
    return speed;
}

int MSCFModel_Rail::getModelID() const {
    return SUMO_TAG_CF_RAIL;
}

MSCFModel *MSCFModel_Rail::duplicate(const MSVehicleType *vtype) const {
    return nullptr;
}

SUMOReal MSCFModel_Rail::maxNextSpeed(SUMOReal speed, const MSVehicle *const veh) const {
//    //TODO edge vmax
//    SUMOReal permissibleSpd = trainParams.vmax; //MIN2(trainParams.vmax,edge.vmax);
//
////    if (speed >= trainParams.vmax) {
////        return  trainParams.vmax;
////    }
//
//
//    SUMOReal targetSpd = permissibleSpd;
//    //TODO delay + reserve

    if (speed >= trainParams.vmax) {
        return trainParams.vmax;
    }



    //TODO signals + stops ...

    SUMOReal targetSpeed = trainParams.vmax;

    SUMOReal res = getResitance(speed);

    //TODO grade resistance force
    SUMOReal gr = 0; //trainParams.weight * 9.81 * edge.grade

    SUMOReal totalRes = res + gr;

    SUMOReal trac = getTraction(speed);

    SUMOReal a;
    if (speed < targetSpeed) {
        a = (trac - totalRes) / trainParams.rotWeight;
    } else {
        a = 0.;
        if (totalRes > trac) {
            a = (trac - totalRes) / trainParams.rotWeight;
        }
    }

    SUMOReal maxNextSpeed = speed + a * DELTA_T/1000.;

    std::cout << "maxNextSpeed: " << maxNextSpeed << std::endl;

    return maxNextSpeed;
}

SUMOReal MSCFModel_Rail::minNextSpeed(SUMOReal speed, const MSVehicle *const veh) const {
    return 0.;
}

SUMOReal MSCFModel_Rail::getResitance(SUMOReal speed) const {
    std::map<SUMOReal, SUMOReal>::const_iterator low, prev;
    low = resistance.lower_bound(speed);

    if (low == resistance.end()) { //speed > max speed
        return (*resistance.rbegin()).second;
    }

    if (low == resistance.begin()) {
        return low->second;
    }

    prev = low;
    --prev;

    SUMOReal range = low->first - prev->first;
    SUMOReal dist = speed - prev->first;
    assert(range > 0);
    assert(dist > 0);

    SUMOReal weight = dist / range;

    SUMOReal res = (1 - weight) * prev->second + weight * low->second;

    return res;

}

double MSCFModel_Rail::getTraction(double speed) const {
    std::map<SUMOReal, SUMOReal>::const_iterator low, prev;
    low = traction.lower_bound(speed);

    if (low == traction.end()) { //speed > max speed
        return (*traction.rbegin()).second;
    }

    if (low == traction.begin()) {
        return low->second;
    }

    prev = low;
    --prev;

    SUMOReal range = low->first - prev->first;
    SUMOReal dist = speed - prev->first;
    assert(range > 0);
    assert(dist > 0);

    SUMOReal weight = dist / range;

    SUMOReal trac = (1 - weight) * prev->second + weight * low->second;

    return trac;

    return 0;
}
