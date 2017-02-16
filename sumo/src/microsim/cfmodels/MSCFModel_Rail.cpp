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
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>



#define G  9.80665


MSCFModel_Rail::MSCFModel_Rail(const MSVehicleType *vtype) : MSCFModel(vtype, -1, -1, -1) {

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
//TODO: impossible to answer unless the pred vehicle is known [Gregor Feb '17]
    return 0;
}

SUMOReal MSCFModel_Rail::stopSpeed(const MSVehicle *const veh, const SUMOReal speed, SUMOReal gap) const {

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return MIN2(maximumSafeStopSpeed(veh,gap,speed,false,TS), maxNextSpeed(speed, veh));
    } else {
        WRITE_ERROR("Anything else then semi implicit euler update is not yet implemented. Exiting!");
        throw ProcessError();
    }
}

SUMOReal MSCFModel_Rail::maximumSafeStopSpeed(const MSVehicle *const veh, SUMOReal gap, const SUMOReal spd, bool insertion,
                                         SUMOReal deltaT) const {

    MSCFModel_Rail::VehicleVariables *vars = (MSCFModel_Rail::VehicleVariables *) veh->getCarFollowVariables();
    if (vars->isNotYetInitialized()) {
        initVehicleVariables(veh, vars);
    }

    TrainParams myTrainParams = trainParams.find(vars->getTrainType())->second;

    gap -= NUMERICAL_EPS; // lots of code relies on some slack XXX: it shouldn't...
    if (gap <= 0) {
        return 0;
    } else if (gap <= ACCEL2SPEED(myTrainParams.decl)) {
        // workaround for #2310
        return MIN2(ACCEL2SPEED(myTrainParams.decl), DIST2SPEED(gap));
    }
    const SUMOReal g = gap;
    const SUMOReal b = ACCEL2SPEED(myTrainParams.decl);
    const SUMOReal t = myHeadwayTime;
    const SUMOReal s = TS;


    // h = the distance that would be covered if it were possible to stop
    // exactly after gap and decelerate with b every simulation step
    // h = 0.5 * n * (n-1) * b * s + n * b * t (solve for n)
    //n = ((1.0/2.0) - ((t + (pow(((s*s) + (4.0*((s*((2.0*h/b) - t)) + (t*t)))), (1.0/2.0))*sign/2.0))/s));
    const SUMOReal n = floor(.5 - ((t + (sqrt(((s * s) + (4.0 * ((s * (2.0 * g / b - t)) + (t * t))))) * -0.5)) / s));
    const SUMOReal h = 0.5 * n * (n - 1) * b * s + n * b * t;
    assert(h <= g + NUMERICAL_EPS);
    // compute the additional speed that must be used during deceleration to fix
    // the discrepancy between g and h
    const SUMOReal r = (g - h) / (n * s + t);
    const SUMOReal x = n * b + r;
    assert(x >= 0);
    return x;
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

    if (speed >= myTrainParams.vmax) {
        return myTrainParams.vmax;
    }

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

    std::cout << veh->getID() << " speed: " << (speed*3.6) << std::endl;

    return maxNextSpeed;
}

SUMOReal MSCFModel_Rail::minNextSpeed(SUMOReal speed, const MSVehicle *const veh) const {

    MSCFModel_Rail::VehicleVariables *vars = (MSCFModel_Rail::VehicleVariables *) veh->getCarFollowVariables();
    if (vars->isNotYetInitialized()) {
        initVehicleVariables(veh, vars);
    }

    TrainParams myTrainParams = trainParams.find(vars->getTrainType())->second;

    SUMOReal slope = veh->getSlope();
    SUMOReal gr = myTrainParams.weight * G * sin(DEG2RAD(slope)); //kN
    SUMOReal res = getResitance(speed, &myTrainParams); // kN
    SUMOReal totalRes = res + gr; //kN

    SUMOReal a = (myTrainParams.decl + totalRes)/myTrainParams.rotWeight;

    return speed - a * DELTA_T / 1000.;

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

SUMOReal MSCFModel_Rail::getSpeedAfterMaxDecel(SUMOReal speed) const {

//    //TODO neither vehicle nor train is known here, so spd aftr mx decl cannot be calculated! [Gregor Feb '17]
//    SUMOReal gr = 0; //trainParams.weight * 9.81 * edge.grade
//
//    SUMOReal a = 0;//trainParams.decl - gr/trainParams.rotWeight;
//
//    return speed + a * DELTA_T / 1000.;
    WRITE_ERROR("function call not allowd for rail model. Exiting!");
    throw ProcessError();
}


MSCFModel::VehicleVariables *MSCFModel_Rail::createVehicleVariables() const {
    VehicleVariables *ret = new VehicleVariables();
    return ret;
}


//mostly c 'n p from MSCFModel
SUMOReal MSCFModel_Rail::moveHelper(MSVehicle *const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    SUMOReal vMin, vNext;
    const SUMOReal vMax = MIN3(veh->getMaxSpeedOnLane(), maxNextSpeed(oldV, veh), vSafe);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // we cannot rely on never braking harder than maxDecel because TraCI or strange cf models may decide to do so
        vMin = MIN2(minNextSpeed(oldV, veh), vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
    } else {
        // for ballistic update, negative vnext must be allowed to
        // indicate a stop within the coming timestep (i.e., to attain negative values)
        vMin =  MIN2(minNextSpeed(oldV, veh), vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
        // (Leo) moveHelper() is responsible for assuring that the next
        // velocity is chosen in accordance with maximal decelerations.
        // At this point vNext may also be negative indicating a stop within next step.
        // Moreover, because maximumSafeStopSpeed() does not consider deceleration bounds
        // vNext can be a large negative value at this point. We cap vNext here.
        vNext = MAX2(vNext, vMin);
    }

    return vNext;
}

double MSCFModel_Rail::freeSpeed(const MSVehicle *const veh, SUMOReal speed, SUMOReal dist, SUMOReal targetSpeed,
                                 const bool onInsertion) const {

    MSCFModel_Rail::VehicleVariables *vars = (MSCFModel_Rail::VehicleVariables *) veh->getCarFollowVariables();
    if (vars->isNotYetInitialized()) {
        initVehicleVariables(veh, vars);
    }

    TrainParams myTrainParams = trainParams.find(vars->getTrainType())->second;

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // adapt speed to succeeding lane, no reaction time is involved
        // when breaking for y steps the following distance g is covered
        // (drive with v in the final step)
        // g = (y^2 + y) * 0.5 * b + y * v
        // y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
        const SUMOReal v = SPEED2DIST(targetSpeed);
        if (dist < v) {
            return targetSpeed;
        }
        const SUMOReal b = ACCEL2DIST(myTrainParams.decl);
        const SUMOReal y = MAX2(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
        const SUMOReal yFull = floor(y);
        const SUMOReal exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
        const SUMOReal fullSpeedGain = (yFull + (onInsertion ? 1. : 0.)) * ACCEL2SPEED(myTrainParams.decl);
        return DIST2SPEED(MAX2((SUMOReal)0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + targetSpeed;
    } else {
        WRITE_ERROR("Anything else then semi implicit euler update is not yet implemented. Exiting!");
        throw ProcessError();
    }
}
