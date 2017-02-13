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
#include <microsim/MSVehicle.h>
#include <utils/geom/GeomHelper.h>
#include "MSCFModel_Rail.h"


#define G  9.80665

MSCFModel_Rail::MSCFModel_Rail(const MSVehicleType *vtype) : MSCFModel(vtype, 1.0, 1.0, 1.0) {
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

    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    if (vars->isNotYetInitialized()) {
        vars->init(veh);
    }



//    VehicleVariables v = veh->

    if (speed >= vars->trainParams.vmax) {
        return vars->trainParams.vmax;
    }


    //TODO signals + stops ...

    SUMOReal targetSpeed = vars->trainParams.vmax;

    SUMOReal res = vars->getResitance(speed); // kN

    SUMOReal slope = veh->getSlope();
    SUMOReal gr = vars->trainParams.weight * G * sin(DEG2RAD(slope)); //kN

    SUMOReal totalRes = res + gr; //kN

    SUMOReal trac = vars->getTraction(speed); //kN

    SUMOReal a;
    if (speed < targetSpeed) {
        a = (trac - totalRes) / vars->trainParams.rotWeight; //kN/t == N/kg
    } else {
        a = 0.;
        if (totalRes > trac) {
            a = (trac - totalRes) / vars->trainParams.rotWeight;//kN/t == N/kg
        }
    }

    SUMOReal maxNextSpeed = speed + a * DELTA_T/1000.;

//    std::cout << veh->getID() << " speed: " << (speed*3.6) << std::endl;

    return maxNextSpeed;
}

SUMOReal MSCFModel_Rail::minNextSpeed(SUMOReal speed, const MSVehicle *const veh) const {
    return speed;
}

SUMOReal MSCFModel_Rail::VehicleVariables::getResitance(SUMOReal speed) const {
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

double MSCFModel_Rail::VehicleVariables::getTraction(double speed) const {
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
}

void MSCFModel_Rail::VehicleVariables::init(const MSVehicle *const veh) {

    std::string trainType = veh->getVehicleType().getParameter().getCFParamString(SUMO_ATTR_TRAIN_TYPE,"NGT400");

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

    notYetInitialized = false;

}

double MSCFModel_Rail::getSpeedAfterMaxDecel(SUMOReal speed) const {

    //TODO grade resistance force
    SUMOReal gr = 0; //trainParams.weight * 9.81 * edge.grade

    //TODO train params
    SUMOReal a = 0;//trainParams.decl - gr/trainParams.rotWeight;

    return speed + a * DELTA_T/1000.;
}

MSCFModel::VehicleVariables *MSCFModel_Rail::createVehicleVariables() const {
    VehicleVariables* ret = new VehicleVariables();
    return ret;
}

//double MSCFModel_Rail::VehicleVariables::getResitance(SUMOReal speed) const {
//    return 0;
//}
