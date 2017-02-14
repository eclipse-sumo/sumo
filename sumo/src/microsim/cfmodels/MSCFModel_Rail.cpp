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

    trainParams["RB425"] = initRB425Params();
    trainParams["RB628"] = initRB628Params();
    trainParams["NGT400"] = initNGT400Params();
    trainParams["NGT400_16"] = initNGT400_16Params();
    trainParams["ICE1"] = initICE1Params();
    trainParams["ICE3"] = initICE3Params();
    trainParams["REDosto7"] = initREDosto7Params();
    trainParams["Freight"] = initFreightParams();
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

    MSCFModel_Rail::VehicleVariables *vars = (MSCFModel_Rail::VehicleVariables *) veh->getCarFollowVariables();
    if (vars->isNotYetInitialized()) {
        initVehicleVariables(veh, vars);
    }

    TrainParams myTrainParams = trainParams.find(vars->getTrainType())->second;


//    VehicleVariables v = veh->

    if (speed >= myTrainParams.vmax) {
        return myTrainParams.vmax;
    }


    //TODO signals + stops ...

    SUMOReal targetSpeed = myTrainParams.vmax;

    SUMOReal res = getResitance(speed, &myTrainParams); // kN

    SUMOReal slope = veh->getSlope();
    SUMOReal gr = myTrainParams.weight * G * sin(DEG2RAD(slope)); //kN

    SUMOReal totalRes = res + gr; //kN

    SUMOReal trac = getTraction(speed, &myTrainParams); //kN

    SUMOReal a;
    if (speed < targetSpeed) {
        a = (trac - totalRes) / myTrainParams.rotWeight; //kN/t == N/kg
    } else {
        a = 0.;
        if (totalRes > trac) {
            a = (trac - totalRes) / myTrainParams.rotWeight;//kN/t == N/kg
        }
    }

    SUMOReal maxNextSpeed = speed + a * DELTA_T / 1000.;

//    std::cout << veh->getID() << " speed: " << (speed*3.6) << std::endl;

    return maxNextSpeed;
}

SUMOReal MSCFModel_Rail::minNextSpeed(SUMOReal speed, const MSVehicle *const veh) const {
    return speed;
}

SUMOReal MSCFModel_Rail::getResitance(SUMOReal speed, TrainParams *params) const {
    std::map<SUMOReal, SUMOReal>::const_iterator low, prev;
    low = params->resistance.lower_bound(speed);

    if (low == params->resistance.end()) { //speed > max speed
        return (params->resistance.rbegin())->second;
    }

    if (low == params->resistance.begin()) {
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

double MSCFModel_Rail::getTraction(double speed, TrainParams *params) const {
    std::map<SUMOReal, SUMOReal>::const_iterator low, prev;
    low = params->traction.lower_bound(speed);

    if (low == params->traction.end()) { //speed > max speed
        return params->traction.rbegin()->second;
    }

    if (low == params->traction.begin()) {
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

void
MSCFModel_Rail::initVehicleVariables(const MSVehicle *const veh, MSCFModel_Rail::VehicleVariables *pVariables) const {

    std::string trainType = veh->getVehicleType().getParameter().getCFParamString(SUMO_ATTR_TRAIN_TYPE, "NGT400");

    if (trainParams.find(trainType) == trainParams.end()) {
        WRITE_ERROR("Unknown train type: " + trainType);
        throw ProcessError();
    }
    pVariables->setTrainType(trainType);
    pVariables->setInitialized();

}

double MSCFModel_Rail::getSpeedAfterMaxDecel(SUMOReal speed) const {

    //TODO grade resistance force
    SUMOReal gr = 0; //trainParams.weight * 9.81 * edge.grade

    //TODO train params
    SUMOReal a = 0;//trainParams.decl - gr/trainParams.rotWeight;

    return speed + a * DELTA_T / 1000.;
}

MSCFModel::VehicleVariables *MSCFModel_Rail::createVehicleVariables() const {
    VehicleVariables *ret = new VehicleVariables();
    return ret;
}

//double MSCFModel_Rail::VehicleVariables::getResitance(SUMOReal speed) const {
//    return 0;
//}
