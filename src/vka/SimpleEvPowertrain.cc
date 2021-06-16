/**
 * \file vka/SimpleEvPowertrain.cc
 * \author Lucas Koch (koch_luc@vka.rwth-aachen.de)
 * \author Marius Wegener (wegener_ma@vka.rwth-aachen.de)
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-02
 * \brief This file contains a simple model of an EV powertrain.
 */


#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <vka/SimpleEvPowertrain.hh>
#include <vka/Constants.hh>
#include <vka/Helper.hh>




/**
 * \namespace vka
 */
namespace vka
{
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
  SimpleEvPowertrain::SimpleEvPowertrain(double m, double r_wheel,
      double Theta, double c_rr, double c_d, double A_front, double i_gear,
      double eta_gear, double M_max, double P_max, double M_recup_max,
      double P_recup_max, double R_battery, double U_battery_0, double P_const,
      const std::string& ref_powerLossMapString)
   : m(m),
     r_wheel(r_wheel),
     Theta(Theta),
     e_i(1 + Theta/(m*r_wheel*r_wheel)),
     c_rr(c_rr),
     c_d(c_d),
     A_front(A_front),
     i_gear(i_gear),
     eta_gear(eta_gear),
     M_max(M_max),
     P_max(P_max),
     M_recup_max(std::abs(M_recup_max)),
     P_recup_max(std::abs(P_recup_max)),
     R_battery(R_battery),
     U_battery_0(U_battery_0),
     P_const(P_const),
     powerLossMap(CharacteristicMap(ref_powerLossMapString))
  { }



  /**
   * \brief Get the vehicle's mass.
   *
   * \returns Vehicle mass [kg]
   */
  double SimpleEvPowertrain::get_m() const
  {
    return m;
  }



  /**
   * \brief Set the vehicle mass.
   *
   * This setter is intended for cases where the mass changes due to people
   * getting in and out of the car or when transporting (heavy) objects in
   * the cargo area.
   * \param[in] m The new weight of the vehicle [kg]
   */
  void SimpleEvPowertrain::set_m(double m)
  {
    if(m != 0)
    {
      this->m = std::abs(m);
      e_i = 1 + Theta/(m*r_wheel*r_wheel);
    }
  }



  /**
   * \brief Get the wheel radius.
   *
   * \returns Wheel radius [m]
   */
  double SimpleEvPowertrain::get_r_wheel() const
  {
    return r_wheel;
  }



  /**
   * \brief Get the vehicle's internal moment of inertia.
   *
   * \returns Moment of inertia [kg*m^2]
   */
  double SimpleEvPowertrain::get_Theta() const
  {
    return Theta;
  }



  /**
   * \brief Get the mass factor.
   *
   * \returns Mass factor [1]
   */
  double SimpleEvPowertrain::get_e_i() const
  {
    return e_i;
  }



  /**
   * \brief Get the rolling resistance coefficient.
   *
   * \returns Rolling resistance coefficient [1]
   */
  double SimpleEvPowertrain::get_c_rr() const
  {
    return c_rr;
  }



  /**
   * \brief Get the drag coefficient.
   *
   * \returns Drag coefficient [1]
   */
  double SimpleEvPowertrain::get_c_d() const
  {
    return c_d;
  }



  /**
   * \brief Get the frontal area of the vehicle.
   *
   * \returns Cross-sectional area of the front of the car [m^2]
   */
  double SimpleEvPowertrain::get_A_front() const
  {
    return A_front;
  }



  /**
   * \brief Get the vehicle's gear ratio.
   *
   * \returns Gear ratio [1]
   */
  double SimpleEvPowertrain::get_i_gear() const
  {
    return i_gear;
  }



  /**
   * \brief Get the efficiency of the gearbox.
   *
   * \returns Transmission efficiency [1]
   */
  double SimpleEvPowertrain::get_eta_gear() const
  {
    return eta_gear;
  }



  /**
   * \brief Get the maximum torque the electric motor can generate.
   *
   * \returns Maximum torque [Nm]
   */
  double SimpleEvPowertrain::get_M_max() const
  {
    return M_max;
  }



  /**
   * \brief Get the maximum power output of the electric motor.
   *
   * \returns Maximum power [W]
   */
  double SimpleEvPowertrain::get_P_max() const
  {
    return P_max;
  }



  /**
   * \brief Get the maximum torque that the EM can handle during
   *        recuperation.
   *
   * \returns Maximum recuperation torque [Nm]
   */
  double SimpleEvPowertrain::get_M_recup_max() const
  {
    return M_recup_max;
  }



  /**
   * \brief Get the maximum recuperation power of the electric motor.
   *
   * \returns Maximum recuperation power [W]
   */
  double SimpleEvPowertrain::get_P_recup_max() const
  {
    return P_recup_max;
  }



  /**
   * \brief Get the internal resistance of the battery.
   *
   * \returns Internal battery resistance [Ohm]
   */
  double SimpleEvPowertrain::get_R_battery() const
  {
    return R_battery;
  }



  /**
   * \brief Get the battery's nominal voltage.
   *
   * \returns Nominal battery voltage [V]
   */
  double SimpleEvPowertrain::get_U_battery_0() const
  {
    return U_battery_0;
  }



  /**
   * \brief Get the constant power consumption of all ancillary devices
   *        (e.g. A/C).
   *
   * \returns The constant power consumption of ancillary devices [W]
   */
  double SimpleEvPowertrain::get_P_const() const
  {
    return P_const;
  }



  /**
   * \brief Set the constant power consumption.
   *
   * This setter is intended for cases where, for instance, the driver turns
   * on/off the air conditioning system or the stereo.
   * \param[in] P_const The new constant power consumption [W]
   */
  void SimpleEvPowertrain::set_P_const(double P_const)
  {
    this->P_const = std::abs(P_const);
  }



  /**
   * \brief Get the power loss map of the EM + inverter.
   *
   * \returns The EM's power loss map
   */
  const vka::CharacteristicMap& SimpleEvPowertrain::get_powerLossMap() const
  {
    return powerLossMap;
  }



  /**
   * \brief Compute the power consumption of the EV powertrain in a certain
   *        state.
   *
   * The model assumes that the driver recuperates as much energy as possible,
   * meaning that he perfectly employs the mechanical brakes such that only
   * what lies beyond the motor's capabilities is dissipated. If the new state
   * is invalid, that is the demanded acceleration exceeds what the electric
   * motor can manage or the power loss map is not defined for a given point,
   * the torque and/or power are capped to the motor's limits or the power loss
   * is set to zero.
   * \param[in] dt Simulation timestep [s]
   * \param[in] v Vehicle speed at the end of a timestep [m/s]
   * \param[in] a Constant acceleration during a timestep [m/s^2]
   * \param[in] alpha Constant incline during a timestep [deg]
   * \param[out] ref_powerConsumption Power consumption during the last
   *             timestep [W]
   * \returns true if the new state is valid, else false
   */
  bool SimpleEvPowertrain::calcPowerConsumption(double dt, double v, double a,
      double alpha, double& ref_powerConsumption) const
  {
    bool b_stateValid = true;

    // Average speed during the previous timestep
    const double v_mean = v - 0.5*a*dt;

    // Force required for the desired acceleration [N]
    double F_a = m*a*e_i;
    // Grade resistance [N]
    double F_gr = m * constants::g * std::sin(deg2rad(alpha));
    // Rolling resistance [N]
    double F_rr = m * constants::g * std::cos(deg2rad(alpha)) * c_rr;
    if(std::abs(v_mean) <= constants::eps)
      F_rr = 0;
    // Drag [N]
    double F_d = 0.5 * c_d * A_front * constants::rho_air * v_mean * v_mean;
    // Tractive force [N]
    const double F_tractive = F_a + F_gr + F_rr + F_d;

    // Speed of the motor [rpm]
    const double n_motor = v_mean/(2*M_PI*r_wheel)*60*i_gear;
    // Angular velocity of the motor [1/s]
    double omega_motor = 2*M_PI*n_motor/60;
    if(omega_motor == 0)
      omega_motor = constants::eps;

    // Torque at the motor [Nm]. Please note that this model, like most real
    // EVs, utilizes the EM to hold the vehicle on an incline rather than the
    // brakes.
    double M_motor = F_tractive*r_wheel/i_gear;
    if(F_tractive < 0)
      M_motor *= eta_gear;
    else
      M_motor /= eta_gear;
    // Engine power [W]
    double P_motor = M_motor*omega_motor;

    // Cap torque or power if necessary
    // Accelerating
    if(M_motor >= 0)
    {
      if(M_motor > M_max)
      {
        M_motor = M_max;
        P_motor = M_motor*omega_motor;
        b_stateValid = false;
      }
      if(P_motor > P_max)
      {
        P_motor = P_max;
        M_motor = P_motor/omega_motor;
        b_stateValid = false;
      }
    }
    // Recuperating
    else
    {
      // Even when capping, the state is still valid here because the extra
      // energy is assumed to go to the brakes
      if(M_motor < -M_recup_max)
      {
        M_motor = -M_recup_max;
        P_motor = M_motor*omega_motor;
      }
      if(P_motor < -P_recup_max)
      {
        P_motor = -P_recup_max;
        M_motor = P_motor/omega_motor;
      }
    }

    // Power loss in the electric motor + inverter [W]
    double P_loss_motor =
        powerLossMap.eval(std::vector<double>{n_motor, M_motor})[0];
    if(std::isnan(P_loss_motor))
    {
      P_loss_motor = 0.0;
      b_stateValid = false;
    }

    // Power demand at the battery [W]
    double P_battery = P_motor + P_loss_motor + P_const;
    // Total power demand (including the power loss in the battery) [W]
    double P_total = (U_battery_0 * U_battery_0)/(2.0 * R_battery)
        - U_battery_0 * std::sqrt((U_battery_0 * U_battery_0
        - 4.0 * R_battery * P_battery)/(4.0 * R_battery * R_battery));

    ref_powerConsumption = P_total;
    return b_stateValid;
  }
}

