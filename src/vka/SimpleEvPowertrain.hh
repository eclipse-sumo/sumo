/**
 * \file vka/SimpleEvPowertrain.hh
 * \author Lucas Koch (koch_luc@vka.rwth-aachen.de)
 * \author Marius Wegener (wegener_ma@vka.rwth-aachen.de)
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-02
 * \brief This file contains a simple model of an EV powertrain.
 */


#pragma once


#include <vka/CharacteristicMap.hh>

#include <string>




/**
 * \namespace vka
 */
namespace vka
{
  /**
   * \class SimpleEvPowertrain
   * \brief This class models the powertrain of an electric vehicle.
   */
  class SimpleEvPowertrain
  {
  private:
    /// Vehicle mass [kg]
    double m;

    /// Wheel radius [m]
    double r_wheel;

    /// Internal moment of inertia [kg*m^2]
    double Theta;

    /// Mass factor [1]
    double e_i;

    /// Rolling resistance coefficient [1]
    double c_rr;

    /// Drag coefficient [1]
    double c_d;

    /// Cross-sectional area of the front of the car [m^2]
    double A_front;

    /// Gear ratio (i_gear = n_motor/n_wheel) [1]
    double i_gear;

    /// Gear efficiency [1]
    double eta_gear;

    /// Maximum torque of the motor [Nm]
    double M_max;

    /// Maximum motor power [W]
    double P_max;

    /// Maximum recuperation torque [Nm]
    double M_recup_max;

    /// Maximum recuperation power [W]
    double P_recup_max;

    /// Internal battery resistance [Ohm]
    double R_battery;

    /// Nominal battery voltage [V]
    double U_battery_0;

    /// Constant power consumption of ancillary devices (e.g. A/C) [W]
    double P_const;

    /// Power loss map of the EM + inverter
    CharacteristicMap powerLossMap;



  public:
    /**
     * \brief Constructor
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
     * \param[in] ref_powerLossMapString String representation of the power
     *            loss map of the EM + inverter
     */
    SimpleEvPowertrain(double m, double r_wheel, double Theta, double c_rr,
        double c_d, double A_front, double i_gear, double eta_gear,
        double M_max, double P_max, double M_recup_max, double P_recup_max,
        double R_battery, double U_battery_0, double P_const,
        const std::string& ref_powerLossMapString);

    /**
     * \brief Get the vehicle's mass.
     *
     * \returns Vehicle mass [kg]
     */
    double get_m() const;

    /**
     * \brief Set the vehicle mass.
     *
     * This setter is intended for cases where the mass changes due to people
     * getting in and out of the car or when transporting (heavy) objects in
     * the cargo area.
     * \param[in] m The new weight of the vehicle [kg]
     */
    void set_m(double m);

    /**
     * \brief Get the wheel radius.
     *
     * \returns Wheel radius [m]
     */
    double get_r_wheel() const;

    /**
     * \brief Get the vehicle's internal moment of inertia.
     *
     * \returns Moment of inertia [kg*m^2]
     */
    double get_Theta() const;

    /**
     * \brief Get the mass factor.
     *
     * \returns Mass factor [1]
     */
    double get_e_i() const;

    /**
     * \brief Get the rolling resistance coefficient.
     *
     * \returns Rolling resistance coefficient [1]
     */
    double get_c_rr() const;

    /**
     * \brief Get the drag coefficient.
     *
     * \returns Drag coefficient [1]
     */
    double get_c_d() const;

    /**
     * \brief Get the frontal area of the vehicle.
     *
     * \returns Cross-sectional area of the front of the car [m^2]
     */
    double get_A_front() const;

    /**
     * \brief Get the vehicle's gear ratio.
     *
     * \returns Gear ratio [1]
     */
    double get_i_gear() const;

    /**
     * \brief Get the efficiency of the gearbox.
     *
     * \returns Transmission efficiency [1]
     */
    double get_eta_gear() const;

    /**
     * \brief Get the maximum torque the electric motor can generate.
     *
     * \returns Maximum torque [Nm]
     */
    double get_M_max() const;

    /**
     * \brief Get the maximum power output of the electric motor.
     *
     * \returns Maximum power [W]
     */
    double get_P_max() const;

    /**
     * \brief Get the maximum torque that the EM can handle during
     *        recuperation.
     *
     * \returns Maximum recuperation torque [Nm]
     */
    double get_M_recup_max() const;

    /**
     * \brief Get the maximum recuperation power of the electric motor.
     *
     * \returns Maximum recuperation power [W]
     */
    double get_P_recup_max() const;

    /**
     * \brief Get the internal resistance of the battery.
     *
     * \returns Internal battery resistance [Ohm]
     */
    double get_R_battery() const;

    /**
     * \brief Get the battery's nominal voltage.
     *
     * \returns Nominal battery voltage [V]
     */
    double get_U_battery_0() const;

    /**
     * \brief Get the constant power consumption of all ancillary devices
     *        (e.g. A/C).
     *
     * \returns The constant power consumption of ancillary devices [W]
     */
    double get_P_const() const;

    /**
     * \brief Set the constant power consumption.
     *
     * This setter is intended for cases where, for instance, the driver turns
     * on/off the air conditioning system or the stereo.
     * \param[in] P_const The new constant power consumption [W]
     */
    void set_P_const(double P_const);

    /**
     * \brief Get the power loss map of the EM + inverter.
     *
     * \returns The EM's power loss map
     */
    const vka::CharacteristicMap& get_powerLossMap() const;

    /**
     * \brief Compute the power consumption of the EV powertrain in a certain
     *        state.
     *
     * The model assumes that the driver recuperates as much energy as
     * possible, meaning that he perfectly employs the mechanical brakes such
     * that only what lies beyond the motor's capabilities is dissipated.
     * If the new state is invalid, that is the demanded acceleration exceeds
     * what the electric motor can manage or the power loss map is not defined
     * for a given point, the torque and/or power are capped to the motor's
     * limits or the power loss is set to zero.
     * \param[in] dt Simulation timestep [s]
     * \param[in] v Vehicle speed at the end of a timestep [m/s]
     * \param[in] a Constant acceleration during a timestep [m/s^2]
     * \param[in] alpha Constant incline during a timestep [deg]
     * \param[out] ref_powerConsumption Power consumption during the last
     *             timestep [W]
     * \returns true if the new state is valid, else false
     */
    bool calcPowerConsumption(double dt, double v, double a, double alpha,
        double& ref_powerConsumption) const;
  };
}

