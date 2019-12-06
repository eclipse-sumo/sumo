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

//due to WRITE_WARNING
#include <utils/common/MsgHandler.h>

//due to sqrt of complex
#include <complex>


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

    // E_Bat = E_kin_pot + EnergyLoss;
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
            //
            // Insaf Sagaama, Amine Kchiche, Wassim Trojet, Farouk Kamoun
            // Improving The Accuracy of The Energy Consumption Model for
            // Electric Vehicle in SUMO Considering The Ambient Temperature
            // Effects
            energyDiff *= (1 / exp(param->find(SUMO_ATTR_RECUPERATIONEFFICIENCY_BY_DECELERATION)->second / fabs(a)));
        }
    }

    // convert from [Ws], i.e. [J]  to [Wh] (3600s / 1h):
    return energyDiff / 3600.;
}

double
HelpersEnergy::acceleration(const SUMOEmissionClass /* c */, const PollutantsInterface::EmissionType e, const double v, const double P, const double slope, const std::map<int, double>* param) const {
    if (e != PollutantsInterface::ELEC) {
        return 0.;
    }
        if (param == 0) {
        param = &myDefaultParameter;
    }

    //Inverse formula for the function compute()
    //Computes the achievable acceleartion using the given speed and amount of consumed electric power
    //It does not consider loss through friction by radial force and the recuperation efficiency dependent on |a| (eta_reuperation is considered constant)
    // Prest = const1*a + const2*a^2 + const3*a^3

    const double mass = param->find(SUMO_ATTR_VEHICLEMASS)->second;
    double const1, const2, const3;
    double Prest;
    int numX;
    double x1, x2, x3;

    if (P > 0) {
        // Assumption: Efficiency of myPropulsionEfficiency when accelerating
        Prest = P*3600 * param->find(SUMO_ATTR_PROPULSIONEFFICIENCY)->second;
    }
    else {
        // Assumption: Efficiency of myRecuperationEfficiency when recuperating
        Prest = P*3600 / param->find(SUMO_ATTR_RECUPERATIONEFFICIENCY)->second;
    }

    // calculate power drop due to a potential energy difference
    Prest -= mass * 9.81 * sin(DEG2RAD(slope)) * (v);
    const1 = mass * 9.81 * sin(DEG2RAD(slope)) * (TS);

    // Power loss through Roll resistance [W]
    //                    ... (fabs(veh.getSpeed())>=0.01) = 0, if vehicle isn't moving
    // EnergyLoss,Tire = c_R [-] * F_N [N] * s [m]
    //                    ... with c_R = ~0.012    (car tire on asphalt)
    //                    ... with F_N [N] = myMass [kg] * g [m/s^2]
    Prest -= param->find(SUMO_ATTR_ROLLDRAGCOEFFICIENT)->second * 9.81 * mass * v;
    const1 += param->find(SUMO_ATTR_ROLLDRAGCOEFFICIENT)->second * 9.81 * mass * (TS);
    
    //Constant loads are omitted. We assume P as the max limit for the main traction drive. Constant loads are often covered by an auxiliary drive
    //Power loss through constant loads (e.g. A/C) [W]
    //Prest -= param->find(SUMO_ATTR_CONSTANTPOWERINTAKE)->second / TS;

    // kinetic energy difference of vehicle
    const1 += 0.5 * mass * (2 * v);
    const2 = 0.5 * mass * (TS);

    // add rotational energy diff of internal rotating elements
    const1 += param->find(SUMO_ATTR_INTERNALMOMENTOFINERTIA)->second * (2 * v);
    const2 += param->find(SUMO_ATTR_INTERNALMOMENTOFINERTIA)->second * (TS);

    // Energy loss through Air resistance [Ws]
    // Calculate energy losses:
    // EnergyLoss,Air = 1/2 * rho_air [kg/m^3] * myFrontSurfaceArea [m^2] * myAirDragCoefficient [-] * v_Veh^2 [m/s] * s [m]
    //                    ... with rho_air [kg/m^3] = 1,2041 kg/m^3 (at T = 20C)
    //                    ... with s [m] = v_Veh [m/s] * TS [s]
    Prest -= 0.5 * 1.2041 * param->find(SUMO_ATTR_FRONTSURFACEAREA)->second * param->find(SUMO_ATTR_AIRDRAGCOEFFICIENT)->second * (v * v * v);
    const1 += 0.5 * 1.2041 * param->find(SUMO_ATTR_FRONTSURFACEAREA)->second * param->find(SUMO_ATTR_AIRDRAGCOEFFICIENT)->second * (3 * v * v * TS);
    const2 += 0.5 * 1.2041 * param->find(SUMO_ATTR_FRONTSURFACEAREA)->second * param->find(SUMO_ATTR_AIRDRAGCOEFFICIENT)->second * (3 * v * TS * TS);
    const3 = 0.5 * 1.2041 * param->find(SUMO_ATTR_FRONTSURFACEAREA)->second * param->find(SUMO_ATTR_AIRDRAGCOEFFICIENT)->second * (TS * TS * TS);

    // Prest = const1*a + const2*a^2 + const3*a^3
    // solve cubic equation in a

    std::tie(numX, x1, x2, x3) = cubicSolve(const3,const2,const1,-Prest);
    

    switch (numX) {
    case 1:
        return x1;
    case 2:
        return MAX2(x1, x2);
    case 3:
        return MAX3(x1, x2, x3);
    default: 
        WRITE_ERROR("An acceleration given by the power was not found.");
        return 0;
    }

}


std::tuple<int, double, double>
HelpersEnergy::quadraticSolve(double a, double b, double c) const {
    // ax^2 + bx + c = 0
    // return only real roots
    if (a == 0) {
        //WRITE_WARNING("The coefficient of the quadrat of x is 0. Please use the utility for a FIRST degree linear. No further action taken.");
        if (b == 0) {
            if (c == 0) {
                return std::make_tuple(2, std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
            } else {
                return std::make_tuple(0, NAN, NAN);
            }
        } else {
            return std::make_tuple(1, -c / b, NAN);
        }
    }
    if (c == 0) {
        //WRITE_WARNING("One root is 0. Now divide through by x and use the utility for a FIRST degree linear to solve the resulting equation for the other one root. No further action taken.");
        return std::make_tuple(2, 0, -b / a);
    }
    double disc, x1_real, x2_real ;
    disc = b * b - 4 * a*c;

    if (disc > 0) {
        //two different real roots
        x1_real = (-b + sqrt(disc)) / (2 * a);
        x2_real = (-b - sqrt(disc)) / (2 * a);
        return std::make_tuple(2, x1_real, x2_real);
    } else if (disc == 0) {
        //two real roots are equal
        x1_real = (-b + sqrt(disc)) / (2 * a);
        return std::make_tuple(1, x1_real, NAN);
    } else {
        //all roots are complex
        return std::make_tuple(0, NAN, NAN);
    }
    return std::make_tuple(0, NAN, NAN);
}

std::tuple<int,double, double, double> 
HelpersEnergy::cubicSolve(double a, double b, double c, double d) const {
    // ax^3 + bx^2 + cx + d = 0
    // return only real roots
    double x;
    if (a == 0) {
        //WRITE_WARNING("The coefficient of the cube of x is 0. Please use the utility for a SECOND degree quadratic. No further action taken.");
        int numX;
        double x1, x2;
        std::tie(numX, x1, x2) = quadraticSolve(b, c, d);
        return std::make_tuple(numX, x1, x2,NAN);
    }
    if (d == 0)	{
        //WRITE_WARNING("One root is 0. Now divide through by x and use the utility for a SECOND degree quadratic to solve the resulting equation for the other two roots. No further action taken.");
        int numX;
        double x1, x2;
        std::tie(numX, x1, x2) = quadraticSolve(a, b, c);
        return std::make_tuple(numX+1, 0, x1, x2);
    }

    b /= a;
    c /= a;
    d /= a;

    double disc, q, r, dum1, s, t, term1, r13;
    q = (3.0*c - (b*b)) / 9.0;
    r = -(27.0*d) + b * (9.0*c - 2.0*(b*b));
    r /= 54.0;
    disc = q*q*q + r*r;
    term1 = (b / 3.0);

    double x1_real, x2_real, x3_real;
    //We are interested only in real roots
    //double x2_imag, x3_imag;
    //std::string x2_imag_s, x3_imag_s;
    if (disc > 0)   // One root real, two are complex
    {
        s = r + sqrt(disc);
        s = s < 0 ? -cbrt(-s) : cbrt(s);
        t = r - sqrt(disc);
        t = t < 0 ? -cbrt(-t) : cbrt(t);
        x1_real = -term1 + s + t;
        term1 += (s + t) / 2.0;
        x3_real = x2_real = -term1;
        return std::make_tuple(1, x1_real, NAN, NAN);
        //term1 = sqrt(3.0)*(-t + s) / 2;
        //x2_imag = term1;
        //x3_imag = -term1;
        //x2_imag_s = " + " + std::to_string(x2_imag) + "i";
        //x3_imag_s = " - " + std::to_string(x2_imag) + "i";
    }
    // The remaining options are all real
    else if (disc == 0)  // All roots real, at least two are equal.
    {
        //x3_imag = x2_imag = 0;
        r13 = r < 0 ? -cbrt(-r) : cbrt(r);
        x1_real = -term1 + 2.0*r13;
        x3_real = x2_real = -(r13 + term1);
        return std::make_tuple(2, x1_real, x2_real, NAN);
    }
    // Only option left is that all roots are real and unequal (to get here, q < 0)
    else
    {
        //x3_imag = x2_imag = 0;
        q = -q;
        dum1 = q * q*q;
        dum1 = acos(r / sqrt(dum1));
        r13 = 2.0*sqrt(q);
        x1_real = -term1 + r13 * cos(dum1 / 3.0);
        x2_real = -term1 + r13 * cos((dum1 + 2.0*M_PI) / 3.0);
        x3_real = -term1 + r13 * cos((dum1 + 4.0*M_PI) / 3.0);
        return std::make_tuple(3, x1_real, x2_real, x3_real);
    }

    return std::make_tuple(0, NAN, NAN, NAN);
}
/****************************************************************************/
