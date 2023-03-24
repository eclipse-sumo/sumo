/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    HelpersEnergy.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
///
// Helper methods for HBEFA-based emission computation
/****************************************************************************/
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/common/PolySolver.h>
#include "HelpersEnergy.h"

//due to WRITE_WARNING
#include <utils/common/MsgHandler.h>

//due to sqrt of complex
#include <complex>


// ===========================================================================
// method definitions
// ===========================================================================
HelpersEnergy::HelpersEnergy():
    PollutantsInterface::Helper("Energy", ENERGY_BASE, ENERGY_BASE)
{ }


double
HelpersEnergy::compute(const SUMOEmissionClass /* c */, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const EnergyParams* param) const {
    if (e != PollutantsInterface::ELEC) {
        return 0.;
    }
    if (param == nullptr) {
        param = EnergyParams::getDefault();
    }
    //@ToDo: All formulas below work with the logic of the euler update (refs #860).
    //       Approximation order could be improved. Refs. #2592.

    const double lastV = v - ACCEL2SPEED(a);
    const double mass = param->getDouble(SUMO_ATTR_VEHICLEMASS);

    // calculate power needed for potential energy difference
    double power = mass * GRAVITY * sin(DEG2RAD(slope)) * v;

    // power needed for kinetic energy difference of vehicle
    power += 0.5 * mass * (v * v - lastV * lastV) / TS;

    // add power needed for rotational energy diff of internal rotating elements
    power += 0.5 * param->getDouble(SUMO_ATTR_INTERNALMOMENTOFINERTIA) * (v * v - lastV * lastV) / TS;

    // power needed for Energy loss through Air resistance [Ws]
    // Calculate energy losses:
    // EnergyLoss,Air = 1/2 * rho_air [kg/m^3] * myFrontSurfaceArea [m^2] * myAirDragCoefficient [-] * v_Veh^2 [m/s] * s [m]
    //                    ... with rho_air [kg/m^3] = 1,2041 kg/m^3 (at T = 20C)
    //                    ... with s [m] = v_Veh [m/s] * TS [s]
    // divided by TS to get power instead of energy
    power += 0.5 * 1.2041 * param->getDouble(SUMO_ATTR_FRONTSURFACEAREA) * param->getDouble(SUMO_ATTR_AIRDRAGCOEFFICIENT) * v * v * v;

    // power needed for Energy loss through Roll resistance [Ws]
    //                    ... (fabs(veh.getSpeed())>=0.01) = 0, if vehicle isn't moving
    // EnergyLoss,Tire = c_R [-] * F_N [N] * s [m]
    //                    ... with c_R = ~0.012    (car tire on asphalt)
    //                    ... with F_N [N] = myMass [kg] * g [m/s^2]
    // divided by TS to get power instead of energy
    power += param->getDouble(SUMO_ATTR_ROLLDRAGCOEFFICIENT) * GRAVITY * mass * v;

    // Energy loss through friction by radial force [Ws]
    // If angle of vehicle was changed
    const double angleDiff = param->getDouble(SUMO_ATTR_ANGLE);
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
        // divided by TS to get power instead of energy
        power += param->getDouble(SUMO_ATTR_RADIALDRAGCOEFFICIENT) * mass * v * v / radius * v;
    }

    // EnergyLoss,constantConsumers
    // Energy loss through constant loads (e.g. A/C) [W]
    power += param->getDouble(SUMO_ATTR_CONSTANTPOWERINTAKE);

    // E_Bat = E_kin_pot + EnergyLoss;
    if (power > 0) {
        // Assumption: Efficiency of myPropulsionEfficiency when accelerating
        power /= param->getDouble(SUMO_ATTR_PROPULSIONEFFICIENCY);
    } else {
        // Assumption: Efficiency of myRecuperationEfficiency when recuperating
        power *= param->getDouble(SUMO_ATTR_RECUPERATIONEFFICIENCY);
        if (a != 0) {
            // Fiori, Chiara & Ahn, Kyoungho & Rakha, Hesham. (2016).
            // Power-based electric vehicle energy consumption model: Model
            // development and validation. Applied Energy. 168. 257-268.
            // 10.1016/j.apenergy.2016.01.097.
            //
            // Insaf Sagaama, Amine Kchiche, Wassim Trojet, Farouk Kamoun
            // Improving The Accuracy of The Energy Consumption Model for
            // Electric Vehicle in SUMO Considering The Ambient Temperature
            // Effects
            power *= (1 / exp(param->getDouble(SUMO_ATTR_RECUPERATIONEFFICIENCY_BY_DECELERATION) / fabs(a)));
        }
    }

    // convert from [W], to [Wh/s] (3600s / 1h):
    return power / 3600.;
}


double
HelpersEnergy::acceleration(const SUMOEmissionClass /* c */, const PollutantsInterface::EmissionType e, const double v, const double P, const double slope, const EnergyParams* param) const {
    if (e != PollutantsInterface::ELEC) {
        return 0.;
    }

    if (param == nullptr) {
        param = EnergyParams::getDefault();
    }

    // Inverse formula for the function compute()

    // Computes the achievable acceleration using the given speed and drive power in [Wh/s]
    // It does not consider friction losses by radial force and the acceleration-dependent
    // recuperation efficiency (eta_recuperation is considered constant)
    //
    // The power `Prest` used for acceleration is given by a cubic polynomial in acceleration,
    // i.e.the equation
    //
    //   Prest = const1*a + const2*a^2 + const3*a^3
    //
    // and we need to find `a` given `Prest`.
    //
    // The solutions of `a(P)` is stored in variables `x1`, `x2`, and `x3` returned by
    // the method `PolySolver::cubicSolve()`, see below.
    //
    // Power used for accelerating, `Prest`, is the total used power minus power wasted by running resistances.

    const double mass = param->getDouble(SUMO_ATTR_VEHICLEMASS);
    double const1, const2, const3;
    double Prest;
    int numX;
    double x1, x2, x3;

    // Power delivered by the drive
    if (P > 0) {
        // Assumption: Efficiency of myPropulsionEfficiency when accelerating
        Prest = P * 3600 * param->getDouble(SUMO_ATTR_PROPULSIONEFFICIENCY);
    } else {
        // Assumption: Efficiency of myRecuperationEfficiency when recuperating
        Prest = P * 3600 / param->getDouble(SUMO_ATTR_RECUPERATIONEFFICIENCY);
    }

    // calculate power drop due to a potential energy difference
    // in inverse original formula:      power = mass * GRAVITY * sin(DEG2RAD(slope)) * v;
    // i.e. in terms of 'lastV' and 'a': power = mass * GRAVITY * sin(DEG2RAD(slope)) * (lastV +  TS * a);
    Prest -= mass * GRAVITY * sin(DEG2RAD(slope)) * (v);
    const1 = mass * GRAVITY * sin(DEG2RAD(slope)) * (TS);

    // update coefficients of a(P) equation considering power loss through Roll resistance
    // in inverse original formula:      power += param->getDouble(SUMO_ATTR_ROLLDRAGCOEFFICIENT) * GRAVITY * mass * v;
    // i.e. in terms of 'lastV' and 'a': power += param->getDouble(SUMO_ATTR_ROLLDRAGCOEFFICIENT) * GRAVITY * mass * (lastV +  TS * a);
    Prest -= param->getDouble(SUMO_ATTR_ROLLDRAGCOEFFICIENT) * GRAVITY * mass * v;
    const1 += param->getDouble(SUMO_ATTR_ROLLDRAGCOEFFICIENT) * GRAVITY * mass * (TS);

    //Constant loads are omitted. We assume P as the max limit for the main traction drive. Constant loads are often covered by an auxiliary drive
    //Power loss through constant loads (e.g. A/C) [W]
    //Prest -= param->getDouble(SUMO_ATTR_CONSTANTPOWERINTAKE) / TS;

    // update coefficients of a(P) equation considering kinetic energy difference of vehicle
    // in inverse original formula:      power += 0.5 * mass * (v * v - lastV * lastV) / TS;
    // i.e. in terms of 'lastV' and 'a': power += 0.5 * mass * (2 * lastV * a + TS * a * a);
    const1 += 0.5 * mass * (2 * v);
    const2 = 0.5 * mass * (TS);

    // update coefficients of a(P) equation considering rotational energy diff of internal rotating elements
    // in inverse original formula:      power += 0.5 * param->getDouble(SUMO_ATTR_INTERNALMOMENTOFINERTIA) * (v * v - lastV * lastV) / TS;
    // i.e. in terms of 'lastV' and 'a': power += 0.5 * param->getDouble(SUMO_ATTR_INTERNALMOMENTOFINERTIA) * (2 * lastV * a + TS * a * a);
    const1 += 0.5 * param->getDouble(SUMO_ATTR_INTERNALMOMENTOFINERTIA) * (2 * v);
    const2 += 0.5 * param->getDouble(SUMO_ATTR_INTERNALMOMENTOFINERTIA) * (TS);

    // update coefficients of a(P) equation considering energy loss through Air resistance
    // in inverse original formula:      power += 0.5 * 1.2041 * param->getDouble(SUMO_ATTR_FRONTSURFACEAREA) * param->getDouble(SUMO_ATTR_AIRDRAGCOEFFICIENT) * v * v * v;
    // i.e. in terms of 'lastV' and 'a': power += 0.5 * param->getDouble(SUMO_ATTR_INTERNALMOMENTOFINERTIA) * (lastV^3 + 3* lastV^2 * TS * a +  3* lastV * TS^2 *a^2 + TS^3 * a^3);
    Prest -= 0.5 * 1.2041 * param->getDouble(SUMO_ATTR_FRONTSURFACEAREA) * param->getDouble(SUMO_ATTR_AIRDRAGCOEFFICIENT) * (v * v * v);
    const1 += 0.5 * 1.2041 * param->getDouble(SUMO_ATTR_FRONTSURFACEAREA) * param->getDouble(SUMO_ATTR_AIRDRAGCOEFFICIENT) * (3 * v * v * TS);
    const2 += 0.5 * 1.2041 * param->getDouble(SUMO_ATTR_FRONTSURFACEAREA) * param->getDouble(SUMO_ATTR_AIRDRAGCOEFFICIENT) * (3 * v * TS * TS);
    const3 = 0.5 * 1.2041 * param->getDouble(SUMO_ATTR_FRONTSURFACEAREA) * param->getDouble(SUMO_ATTR_AIRDRAGCOEFFICIENT) * (TS * TS * TS);

    // Prest = const1*a + const2*a^2 + const3*a^3
    // Solve cubic equation in `a` for real roots, and return the number of roots in `numX`
    // and the roots in `x1`, `x2`, and `x3`.
    std::tie(numX, x1, x2, x3) = PolySolver::cubicSolve(const3, const2, const1, -Prest);


    switch (numX) {
        case 1:
            return x1;
        case 2:
            return MAX2(x1, x2);
        case 3:
            return MAX3(x1, x2, x3);
        default:
            WRITE_ERROR(TL("An acceleration given by the power was not found."));
            return 0;
    }

}


/****************************************************************************/
