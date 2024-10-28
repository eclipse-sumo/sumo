/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    RealisticEngineModel.cpp
/// @author  Michele Segata, Antonio Saverio Valente
/// @date    4 Feb 2015
///
// A detailed engine model
/****************************************************************************/
#include <config.h>

#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <iostream>

#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/cfmodels/CC_Const.h>
#include "RealisticEngineModel.h"


// ===========================================================================
// method definitions
// ===========================================================================
RealisticEngineModel::RealisticEngineModel() {
    className = "RealisticEngineModel";
    dt_s = 0.01;
    xmlFile = "vehicles.xml";
    minSpeed_mps = rpmToSpeed_mps(ep.minRpm, ep.wheelDiameter_m, ep.differentialRatio, ep.gearRatios[0]);
}


RealisticEngineModel::~RealisticEngineModel() {}


double RealisticEngineModel::rpmToSpeed_mps(double rpm, double wheelDiameter_m = 0.94,
        double differentialRatio = 4.6, double gearRatio = 4.5) {
    return rpm * wheelDiameter_m * M_PI / (differentialRatio * gearRatio * 60);
}


double RealisticEngineModel::rpmToSpeed_mps(double rpm) {
    return ep.__rpmToSpeedCoefficient * rpm / ep.gearRatios[currentGear];
}


double RealisticEngineModel::speed_mpsToRpm(double speed_mps, double wheelDiameter_m,
        double differentialRatio, double gearRatio) {
    return speed_mps * differentialRatio * gearRatio * 60 / (wheelDiameter_m * M_PI);
}


double RealisticEngineModel::speed_mpsToRpm(double speed_mps) {
    return ep.__speedToRpmCoefficient * speed_mps * ep.gearRatios[currentGear];
}


double RealisticEngineModel::speed_mpsToRpm(double speed_mps, double gearRatio) {
    return ep.__speedToRpmCoefficient * speed_mps * gearRatio;
}


double RealisticEngineModel::rpmToPower_hp(double rpm, const struct EngineParameters::PolynomialEngineModelRpmToHp* engineMapping) {
    double sum = engineMapping->x[0];
    for (int i = 1; i < engineMapping->degree; i++) {
        sum += engineMapping->x[i] + pow(rpm, i);
    }
    return sum;
}


double RealisticEngineModel::rpmToPower_hp(double rpm) {
    if (rpm >= ep.maxRpm) {
        rpm = ep.maxRpm;
    }
    double sum = ep.engineMapping.x[0];
    for (int i = 1; i < ep.engineMapping.degree; i++) {
        sum += ep.engineMapping.x[i] * pow(rpm, i);
    }
    return sum;
}


double RealisticEngineModel::speed_mpsToPower_hp(double speed_mps,
        const struct EngineParameters::PolynomialEngineModelRpmToHp* engineMapping,
        double wheelDiameter_m, double differentialRatio,
        double gearRatio) {
    double rpm = speed_mpsToRpm(speed_mps, wheelDiameter_m, differentialRatio, gearRatio);
    return rpmToPower_hp(rpm, engineMapping);
}


double RealisticEngineModel::speed_mpsToPower_hp(double speed_mps) {
    return rpmToPower_hp(speed_mpsToRpm(speed_mps));
}


double RealisticEngineModel::speed_mpsToThrust_N(double speed_mps,
        const struct EngineParameters::PolynomialEngineModelRpmToHp* engineMapping,
        double wheelDiameter_m, double differentialRatio,
        double gearRatio, double engineEfficiency) {
    double power_hp = speed_mpsToPower_hp(speed_mps, engineMapping, wheelDiameter_m, differentialRatio, gearRatio);
    return engineEfficiency * power_hp * HP_TO_W / speed_mps;
}


double RealisticEngineModel::speed_mpsToThrust_N(double speed_mps) {
    double power_hp = speed_mpsToPower_hp(speed_mps);
    return ep.__speedToThrustCoefficient * power_hp / speed_mps;
}


double RealisticEngineModel::airDrag_N(double speed_mps, double cAir, double a_m2, double rho_kgpm3) {
    return 0.5 * cAir * a_m2 * rho_kgpm3 * speed_mps * speed_mps;
}


double RealisticEngineModel::airDrag_N(double speed_mps) {
    return ep.__airFrictionCoefficient * speed_mps * speed_mps;
}


double RealisticEngineModel::rollingResistance_N(double speed_mps, double mass_kg, double cr1, double cr2) {
    return mass_kg * GRAVITY_MPS2 * (cr1 + cr2 * speed_mps * speed_mps);
}


double RealisticEngineModel::rollingResistance_N(double speed_mps) {
    return ep.__cr1 + ep.__cr2 * speed_mps * speed_mps;
}


double RealisticEngineModel::gravityForce_N(double mass_kg, double slope = 0) {
    return mass_kg * GRAVITY_MPS2 * sin(slope / 180 * M_PI);
}


double RealisticEngineModel::gravityForce_N() {
    return ep.__gravity;
}


double RealisticEngineModel::opposingForce_N(double speed_mps, double mass_kg, double slope,
        double cAir, double a_m2, double rho_kgpm3,
        double cr1, double cr2) {
    return airDrag_N(speed_mps, cAir, a_m2, rho_kgpm3) +
           rollingResistance_N(speed_mps, mass_kg, cr1, cr2) +
           gravityForce_N(mass_kg, slope);
}


double RealisticEngineModel::opposingForce_N(double speed_mps) {
    return airDrag_N(speed_mps) + rollingResistance_N(speed_mps) + gravityForce_N();
}


double RealisticEngineModel::maxNoSlipAcceleration_mps2(double slope, double frictionCoefficient) {
    return frictionCoefficient * GRAVITY_MPS2 * cos(slope / 180 * M_PI);
}


double RealisticEngineModel::maxNoSlipAcceleration_mps2() {
    return ep.__maxNoSlipAcceleration;
}


double RealisticEngineModel::thrust_NToAcceleration_mps2(double thrust_N) {
    return thrust_N / ep.__maxAccelerationCoefficient;
}


int RealisticEngineModel::performGearShifting(double speed_mps, double acceleration_mps2) {
    int newGear = 0;
    const double delta = acceleration_mps2 >= 0 ? ep.shiftingRule.deltaRpm : -ep.shiftingRule.deltaRpm;
    for (newGear = 0; newGear < ep.nGears - 1; newGear++) {
        const double rpm = speed_mpsToRpm(speed_mps, ep.gearRatios[newGear]);
        if (rpm >= ep.shiftingRule.rpm + delta) {
            continue;
        } else {
            break;
        }
    }
    currentGear = newGear;
    return currentGear;
}


double RealisticEngineModel::maxEngineAcceleration_mps2(double speed_mps) {
    return speed_mpsToThrust_N(speed_mps) / ep.__maxAccelerationCoefficient;
}


double RealisticEngineModel::getEngineTimeConstant_s(double rpm) {
    if (rpm <= 0) {
        return TAU_MAX;
    } else {
        if (ep.fixedTauBurn)
            //in this case, tau_burn is fixed and is within __engineTauDe_s
        {
            return std::min(TAU_MAX, ep.__engineTau2 / rpm + ep.__engineTauDe_s);
        } else
            //in this case, tau_burn is dynamic and is within __engineTau1
        {
            return std::min(TAU_MAX, ep.__engineTau1 / rpm + ep.tauEx_s);
        }
    }
}


double RealisticEngineModel::getRealAcceleration(double speed_mps, double accel_mps2, double reqAccel_mps2, SUMOTime timeStep) {
    double realAccel_mps2;
    //perform gear shifting, if needed
    performGearShifting(speed_mps, accel_mps2);
    //since we consider no clutch (clutch always engaged), 0 speed would mean 0 rpm, and thus
    //0 available power. thus, the car could never start from a complete stop. so we assume
    //a minimum speed of 1 m/s to compute engine power
    double correctedSpeed = std::max(speed_mps, minSpeed_mps);
    if (reqAccel_mps2 >= 0) {
        //the system wants to accelerate
        //compute what is the maximum acceleration that can be delivered by the engine
        double engineAccel = maxEngineAcceleration_mps2(correctedSpeed);
        //the maximum acceleration is given by the maximum acceleration the engine can provide minus friction forces
        double maxAccel = engineAccel - thrust_NToAcceleration_mps2(opposingForce_N(speed_mps));
        //now we need to computed delayed acceleration due to actuation lag
        double tau = getEngineTimeConstant_s(speed_mpsToRpm(correctedSpeed));
        double alpha = ep.dt / (tau + ep.dt);
        //use standard first order lag with time constant depending on engine rpm
        realAccel_mps2 = alpha * reqAccel_mps2 + (1 - alpha) * accel_mps2;
        //limit the actual acceleration by considering friction forces and the maximum acceleration considering tyres
        realAccel_mps2 = std::min(std::min(realAccel_mps2, maxAccel), maxNoSlipAcceleration_mps2());
    } else {
        realAccel_mps2 = getRealBrakingAcceleration(speed_mps, accel_mps2, reqAccel_mps2, timeStep);
    }
    return realAccel_mps2;
}


void RealisticEngineModel::getEngineData(double speed_mps, int& gear, double& rpm) {
    gear = currentGear;
    rpm = speed_mpsToRpm(speed_mps);
}


double RealisticEngineModel::getRealBrakingAcceleration(double speed_mps, double accel_mps2, double reqAccel_mps2, SUMOTime t) {
    UNUSED_PARAMETER(t);
    //compute which part of the deceleration is currently done by frictions
    double frictionDeceleration = thrust_NToAcceleration_mps2(opposingForce_N(speed_mps));
    //the maximum deceleration is what the vehicle can do given the tyres plus the friction forces (friction can make the vehicle brake stronger)
    double maxDeceleration = -maxNoSlipAcceleration_mps2() - frictionDeceleration;
    //compute the new brakes deceleration
    double newBrakesAccel_mps2 = ep.__brakesAlpha * reqAccel_mps2 + ep.__brakesOneMinusAlpha * accel_mps2;
    //keep actual deceleration within physical limits (considering that wind force can help us slowing down)
    return std::max(maxDeceleration, newBrakesAccel_mps2);

}


void RealisticEngineModel::loadParameters() {
    VehicleEngineHandler engineHandler(vehicleType);
    if (!XMLSubSys::runParser(engineHandler, xmlFile)) {
        throw ProcessError();
    }
    //copy loaded parameters into our engine parameters
    ep = engineHandler.getEngineParameters();
    ep.dt = dt_s;
    ep.computeCoefficients();
    //compute "minimum speed" to be used when computing maximum acceleration at speeds close to 0
    minSpeed_mps = rpmToSpeed_mps(ep.minRpm, ep.wheelDiameter_m, ep.differentialRatio, ep.gearRatios[0]);
}


void RealisticEngineModel::setParameter(const std::string parameter, const std::string& value) {
    if (parameter == ENGINE_PAR_XMLFILE) {
        xmlFile = value;
    }
    if (parameter == ENGINE_PAR_VEHICLE) {
        vehicleType = value;
        if (xmlFile != "") {
            loadParameters();
        }
    }
}


void RealisticEngineModel::setParameter(const std::string parameter, double value) {
    if (parameter == ENGINE_PAR_DT) {
        dt_s = value;
    }
}


void RealisticEngineModel::setParameter(const std::string parameter, int value) {
    UNUSED_PARAMETER(parameter);
    UNUSED_PARAMETER(value);
}
