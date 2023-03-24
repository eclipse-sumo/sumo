/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
/// @file    HelpersMMPEVEM.cpp
/// @author  Kevin Badalian (badalian_k@mmp.rwth-aachen.de)
/// @date    2021-10
///
// The MMP's emission model for electric vehicles.
// If you use this model for academic research, you are highly encouraged to
// cite our paper "Accurate physics-based modeling of electric vehicle energy
// consumption in the SUMO traffic microsimulator"
// (DOI: 10.1109/ITSC48978.2021.9564463).
// Teaching and Research Area Mechatronics in Mobile Propulsion (MMP), RWTH Aachen
/****************************************************************************/
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeomHelper.h>
#include <utils/emissions/CharacteristicMap.h>
#include <utils/emissions/HelpersMMPEVEM.h>


// ===========================================================================
// method definitions
// ===========================================================================
/**
 * \brief Compute the power consumption of an EV powertrain in a certain state.
 *
 * The model assumes that the driver recuperates as much energy as possible,
 * meaning that he perfectly employs the mechanical brakes such that only what
 * lies beyond the motor's capabilities is dissipated. If the new state is
 * invalid, that is the demanded acceleration exceeds what the electric motor
 * can manage or the power loss map is not defined for a given point, the torque
 * and/or power are capped to the motor's limits or the power loss is set to
 * zero.
 *
 * \param[in] m Vehicle mass [kg]
 * \param[in] r_wheel Wheel radius [m]
 * \param[in] Theta Moment of inertia [kg*m^2]
 * \param[in] c_rr Rolling resistance coefficient [1]
 * \param[in] c_d Drag coefficient [1]
 * \param[in] A_front Cross-sectional area of the front of the car [m^2]
 * \param[in] i_gear Gear ratio (i_gear = n_motor/n_wheel) [1]
 * \param[in] eta_gear Gear efficiency [1]
 * \param[in] M_max Maximum torque of the EM [Nm]
 * \param[in] P_max Maximum power of the EM [W]
 * \param[in] M_recup_max Maximum recuperation torque [Nm]
 * \param[in] P_recup_max Maximum recuperation power [W]
 * \param[in] R_battery Internal battery resistance [Ohm]
 * \param[in] U_battery_0 Nominal battery voltage [V]
 * \param[in] P_const Constant power consumption of ancillary devices [W]
 * \param[in] ref_powerLossMap Power loss map of the EM + inverter
 * \param[in] dt Simulation timestep [s]
 * \param[in] v Vehicle speed at the end of a timestep [m/s]
 * \param[in] a Constant acceleration during a timestep [m/s^2]
 * \param[in] alpha Constant incline during a timestep [deg]
 * \param[out] ref_powerConsumption Power consumption during the last timestep
 *             [W]
 * \returns true if the new state is valid, else false
 */
bool calcPowerConsumption(double m, double r_wheel, double Theta, double c_rr,
                          double c_d, double A_front, double i_gear, double eta_gear, double M_max,
                          double P_max, double M_recup_max, double P_recup_max, double R_battery,
                          double U_battery_0, double P_const,
                          const CharacteristicMap& ref_powerLossMap, double dt, double v, double a,
                          double alpha, double& ref_powerConsumption) {
    const double EPS = 1e-6;
    const double RHO_AIR = 1.204;  // Air density [kg/m^3]
    bool b_stateValid = true;

    // Mass factor [1]
    const double e_i = 1.0 + Theta / (m * r_wheel * r_wheel);
    // Average speed during the previous timestep
    const double v_mean = v - 0.5 * a * dt;

    // Force required for the desired acceleration [N]
    double F_a = m * a * e_i;
    // Grade resistance [N]
    double F_gr = m * GRAVITY * std::sin(DEG2RAD(alpha));
    // Rolling resistance [N]
    double F_rr = m * GRAVITY * std::cos(DEG2RAD(alpha)) * c_rr;
    if (std::abs(v_mean) <= EPS) {
        F_rr = 0;
    }
    // Drag [N]
    double F_d = 0.5 * c_d * A_front * RHO_AIR * v_mean * v_mean;
    // Tractive force [N]
    const double F_tractive = F_a + F_gr + F_rr + F_d;

    // Speed of the motor [rpm]
    const double n_motor = v_mean / (2 * M_PI * r_wheel) * 60 * i_gear;
    // Angular velocity of the motor [1/s]
    double omega_motor = 2 * M_PI * n_motor / 60;
    if (omega_motor == 0) {
        omega_motor = EPS;
    }

    // Torque at the motor [Nm]. Please note that this model, like most real EVs,
    // utilizes the EM to hold the vehicle on an incline rather than the brakes.
    double M_motor = F_tractive * r_wheel / i_gear;
    if (F_tractive < 0) {
        M_motor *= eta_gear;
    } else {
        M_motor /= eta_gear;
    }
    // Engine power [W]
    double P_motor = M_motor * omega_motor;

    // Cap torque or power if necessary
    // Accelerating
    if (M_motor >= 0) {
        if (M_motor > M_max) {
            M_motor = M_max;
            P_motor = M_motor * omega_motor;
            b_stateValid = false;
        }
        if (P_motor > P_max) {
            P_motor = P_max;
            M_motor = P_motor / omega_motor;
            b_stateValid = false;
        }
    }
    // Recuperating
    else {
        // Even when capping, the state is still valid here because the extra energy
        // is assumed to go to the brakes
        if (M_motor < -M_recup_max) {
            M_motor = -M_recup_max;
            P_motor = M_motor * omega_motor;
        }
        if (P_motor < -P_recup_max) {
            P_motor = -P_recup_max;
            M_motor = P_motor / omega_motor;
        }
    }

    // Power loss in the electric motor + inverter [W]
    double P_loss_motor =
        ref_powerLossMap.eval(std::vector<double> {n_motor, M_motor})[0];
    if (std::isnan(P_loss_motor)) {
        P_loss_motor = 0.0;
        b_stateValid = false;
    }

    // Power demand at the battery [W]
    double P_battery = P_motor + P_loss_motor + P_const;
    // Total power demand (including the power loss in the battery) [W]
    double P_total = (U_battery_0 * U_battery_0) / (2.0 * R_battery)
                     - U_battery_0 * std::sqrt((U_battery_0 * U_battery_0
                             - 4.0 * R_battery * P_battery) / (4.0 * R_battery * R_battery));

    ref_powerConsumption = P_total;
    return b_stateValid;
}




/**
 * \brief Constructor
 */
HelpersMMPEVEM::HelpersMMPEVEM()
    : PollutantsInterface::Helper("MMPEVEM", MMPEVEM_BASE, MMPEVEM_BASE + 1)
{ }





/**
 * \brief Compute the amount of emitted pollutants for an emission class in a
 *        given state.
 *
 * This method returns 0 for all emission types but electric power consumption.
 *
 * \param[in] c An emission class
 * \param[in] e An emission type
 * \param[in] v Current vehicle velocity [m/s]
 * \param[in] a Current acceleration of the vehicle [m/s^2]
 * \param[in] slope Slope of the road at the vehicle's current position [deg]
 * \param[in] ptr_energyParams Vehicle parameters
 *
 * \returns The electric power consumption [Wh/s] or 0 for all other emission
 *          types
 */
double HelpersMMPEVEM::compute(const SUMOEmissionClass /* c */,
                               const PollutantsInterface::EmissionType e, const double v, const double a,
                               const double slope, const EnergyParams* ptr_energyParams) const {
    if (e != PollutantsInterface::ELEC) {
        return 0.0;
    }

    // Extract all required parameters
    // Vehicle mass
    const double m = ptr_energyParams->getDouble(SUMO_ATTR_VEHICLEMASS);
    // Wheel radius
    const double r_wheel = ptr_energyParams->getDouble(SUMO_ATTR_WHEELRADIUS);
    // Internal moment of inertia
    const double Theta
        = ptr_energyParams->getDouble(SUMO_ATTR_INTERNALMOMENTOFINERTIA);
    // Rolling resistance coefficient
    const double c_rr
        = ptr_energyParams->getDouble(SUMO_ATTR_ROLLDRAGCOEFFICIENT);
    // Air drag coefficient
    const double c_d = ptr_energyParams->getDouble(SUMO_ATTR_AIRDRAGCOEFFICIENT);
    // Cross-sectional area of the front of the car
    const double A_front
        = ptr_energyParams->getDouble(SUMO_ATTR_FRONTSURFACEAREA);
    // Gear ratio
    const double i_gear = ptr_energyParams->getDouble(SUMO_ATTR_GEARRATIO);
    // Gearbox efficiency
    const double eta_gear = ptr_energyParams->getDouble(SUMO_ATTR_GEAREFFICIENCY);
    // Maximum torque
    const double M_max = ptr_energyParams->getDouble(SUMO_ATTR_MAXIMUMTORQUE);
    // Maximum power
    const double P_max = ptr_energyParams->getDouble(SUMO_ATTR_MAXIMUMPOWER);
    // Maximum recuperation torque
    const double M_recup_max
        = ptr_energyParams->getDouble(SUMO_ATTR_MAXIMUMRECUPERATIONTORQUE);
    // Maximum recuperation power
    const double P_recup_max
        = ptr_energyParams->getDouble(SUMO_ATTR_MAXIMUMRECUPERATIONPOWER);
    // Internal battery resistance
    const double R_battery
        = ptr_energyParams->getDouble(SUMO_ATTR_INTERNALBATTERYRESISTANCE);
    // Nominal battery voltage
    const double U_battery_0
        = ptr_energyParams->getDouble(SUMO_ATTR_NOMINALBATTERYVOLTAGE);
    // Constant power consumption
    const double P_const
        = ptr_energyParams->getDouble(SUMO_ATTR_CONSTANTPOWERINTAKE);
    // Power loss map
    const CharacteristicMap& ref_powerLossMap
        = ptr_energyParams->getCharacteristicMap(SUMO_ATTR_POWERLOSSMAP);

    double P = 0.0;  // [W]
    bool b_stateValid = calcPowerConsumption(m, r_wheel, Theta, c_rr, c_d,
                        A_front, i_gear, eta_gear, M_max, P_max, M_recup_max, P_recup_max,
                        R_battery, U_battery_0, P_const, ref_powerLossMap, TS, v, a, slope, P);
    P /= 3600.0;  // [Wh/s]

    if (b_stateValid) {
        return P;
    } else {
        return std::nan("");
    }
}
