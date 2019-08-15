/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    HelpersEnergy.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for HBEFA-based emission computation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include "HelpersEnergy.h"


// ===========================================================================
// method definitions
// ===========================================================================
HelpersEnergy::HelpersEnergy() : PollutantsInterface::Helper("Energy") {
    myEmissionClassStrings.insert("zero", PollutantsInterface::ZERO_EMISSIONS);
    myEmissionClassStrings.insert("unknown", ENERGY_BASE);
    // default values from
    // Kurczveil, T., López, P.Á., & Schnieder, E. (2014). Implementation of an Energy Model and a Charging Infrastructure in SUMO.
    myDefaultParameter[SUMO_ATTR_VEHICLEMASS] = 1000.;
    myDefaultParameter[SUMO_ATTR_FRONTSURFACEAREA] = 5.;
    myDefaultParameter[SUMO_ATTR_AIRDRAGCOEFFICIENT] = 0.6;
    myDefaultParameter[SUMO_ATTR_INTERNALMOMENTOFINERTIA] = 0.01;
    myDefaultParameter[SUMO_ATTR_RADIALDRAGCOEFFICIENT] = 0.5;
    myDefaultParameter[SUMO_ATTR_ROLLDRAGCOEFFICIENT] = 0.01;
    myDefaultParameter[SUMO_ATTR_CONSTANTPOWERINTAKE] = 100.;
    myDefaultParameter[SUMO_ATTR_PROPULSIONEFFICIENCY] = 0.9;
    myDefaultParameter[SUMO_ATTR_RECUPERATIONEFFICIENCY] = 0.8;
    myDefaultParameter[SUMO_ATTR_RECUPERATIONEFFICIENCY_BY_DECELERATION] = 0.0;
    myDefaultParameter[SUMO_ATTR_ANGLE] = 0.;
}


double
HelpersEnergy::compute(const SUMOEmissionClass /* c */, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const std::map<int, double>* param) const {
    if (e != PollutantsInterface::ELEC) {
        return 0.;
    }
    if (param == nullptr) {
        param = &myDefaultParameter;
    }
    //@ToDo: All formulas below work with the logic of the euler update (refs #860).
    //       Approximation order could be improved. Refs. #2592.

    const double lastV = v - ACCEL2SPEED(a);
    const double mass = param->find(SUMO_ATTR_VEHICLEMASS)->second;

    // calculate potential energy difference
    double energyDiff = mass * 9.81 * sin(DEG2RAD(slope)) * SPEED2DIST(v);

    // kinetic energy difference of vehicle
    energyDiff += 0.5 * mass * (v * v - lastV * lastV);

    // add rotational energy diff of internal rotating elements
    energyDiff += param->find(SUMO_ATTR_INTERNALMOMENTOFINERTIA)->second * (v * v - lastV * lastV);

    // Energy loss through Air resistance [Ws]
    // Calculate energy losses:
    // EnergyLoss,Air = 1/2 * rho_air [kg/m^3] * myFrontSurfaceArea [m^2] * myAirDragCoefficient [-] * v_Veh^2 [m/s] * s [m]
    //                    ... with rho_air [kg/m^3] = 1,2041 kg/m^3 (at T = 20C)
    //                    ... with s [m] = v_Veh [m/s] * TS [s]
    energyDiff += 0.5 * 1.2041 * param->find(SUMO_ATTR_FRONTSURFACEAREA)->second * param->find(SUMO_ATTR_AIRDRAGCOEFFICIENT)->second * v * v * SPEED2DIST(v);

    // Energy loss through Roll resistance [Ws]
    //                    ... (fabs(veh.getSpeed())>=0.01) = 0, if vehicle isn't moving
    // EnergyLoss,Tire = c_R [-] * F_N [N] * s [m]
    //                    ... with c_R = ~0.012    (car tire on asphalt)
    //                    ... with F_N [N] = myMass [kg] * g [m/s^2]
    energyDiff += param->find(SUMO_ATTR_ROLLDRAGCOEFFICIENT)->second * 9.81 * mass * SPEED2DIST(v);

    // Energy loss through friction by radial force [Ws]
    // If angle of vehicle was changed
    const double angleDiff = param->find(SUMO_ATTR_ANGLE)->second;
    if (angleDiff != 0.) {
        // Compute new radio
        double radius = SPEED2DIST(v) / fabs(angleDiff);

        // Check if radius is in the interval [0.0001 - 10000] (To avoid overflow and division by zero)
        if (radius < 0.0001) {
            radius = 0.0001;
        } else if (radius > 10000) {
            radius = 10000;
        }
        // EnergyLoss,internalFrictionRadialForce = c [m] * F_rad [N];
        // Energy loss through friction by radial force [Ws]
        energyDiff += param->find(SUMO_ATTR_RADIALDRAGCOEFFICIENT)->second * mass * v * v / radius;
    }

    // EnergyLoss,constantConsumers
    // Energy loss through constant loads (e.g. A/C) [Ws]
    energyDiff += param->find(SUMO_ATTR_CONSTANTPOWERINTAKE)->second;

    //E_Bat = E_kin_pot + EnergyLoss;
    if (energyDiff > 0) {
        // Assumption: Efficiency of myPropulsionEfficiency when accelerating
        energyDiff /= param->find(SUMO_ATTR_PROPULSIONEFFICIENCY)->second;
    } else {
        // Assumption: Efficiency of myRecuperationEfficiency when recuperating
        energyDiff *= param->find(SUMO_ATTR_RECUPERATIONEFFICIENCY)->second;
        if (a != 0) {
            // Fiori, Chiara & Ahn, Kyoungho & Rakha, Hesham. (2016).
            // Power-based electric vehicle energy consumption model: Model
            // development and validation. Applied Energy. 168. 257-268.
            // 10.1016/j.apenergy.2016.01.097.
            energyDiff *= (1 / exp(param->find(SUMO_ATTR_RECUPERATIONEFFICIENCY_BY_DECELERATION)->second / fabs(a)));
        }
    }

    // convert from [Ws] to [Wh] (3600s / 1h):
    return energyDiff / 3600.;
}


/****************************************************************************/
