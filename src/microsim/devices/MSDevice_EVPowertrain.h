/**
 * \file microsim/devices/MSDevice_EVPowertrain.h
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-02
 * \brief This file contains the EV powertrain device that SUMO vehicles can be
 *        equipped with. It is an adaptation of MSDevice_Example.h and was
 *        heavily inspired by MSDevice_Battery.h.
 */


#pragma once


#include <vka/SimpleEvPowertrain.hh>
#include <microsim/devices/MSVehicleDevice.h>




/**
 * \class MSDevice_EVPowertrain
 * \brief This device encapsulates the VKA EV powertrain model and can be
 *        equipped by SUMO vehicles.
 *
 * The vType-parameters it expects are mostly identical to the battery device,
 * though some, like for instance the power loss map, are new additions.
 */
class MSDevice_EVPowertrain : public MSVehicleDevice
{
public:
  struct DefaultParams
  {
    /// Vehicle mass [kg]
    const double m = 1417.0;
    /// Wheel radius [m]
    const double r_wheel = 0.3498;
    /// Moment of inertia [kg*m^2]
    const double Theta = 12.5;
    /// Rolling resistance coefficient [1]
    const double c_rr = 0.007;
    /// Drag coefficient [1]
    const double c_d = 0.29;
    /// Cross-sectional area of the front of the car [m^2]
    const double A_front = 2.38;
    /// Gear ratio (i_gear = n_motor/n_wheel) [1]
    const double i_gear = 9.665;
    /// Gear efficiency [1]
    const double eta_gear = 0.96;
    /// Maximum torque [Nm]
    const double M_max = 250.0;
    /// Maximum power [W]
    const double P_max = 125000.0;
    /// Maximum recuperation torque [Nm]
    const double M_recup_max = 77.0;
    /// Maximum recuperation power [W]
    const double P_recup_max = 50000.0;
    /// Internal battery resistance [Ohm]
    const double R_battery = 0.0768;
    /// Nominal battery voltage [V]
    const double U_battery_0 = 370.0;
    /// Constant power consumption [W]
    const double P_const = 360.0;
    /// String encoding of a map that results in no losses in the EM
    const std::string powerLossMapString
        = "2,1|-1e6,1e6;-1e6,1e6|0.0,0.0,0.0,0.0";
  };



private:
  /// Current total power consumption [W]
  double P;
  /// Indicates whether the current state is valid
  bool b_stateValid;
  /// VKA EV powertrain model
  vka::SimpleEvPowertrain model;

  /**
   * \brief Constructor
   *
   * \param[in] ref_holder Holder of the device
   * \param[in] ref_id Device ID
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
   * \param[in] ref_powerLossMapString String representation of the power loss
   *            map of the EM + inverter
   */
  MSDevice_EVPowertrain(SUMOVehicle& ref_holder, const std::string& ref_id,
      double m, double r_wheel, double Theta, double c_rr, double c_d,
      double A_front, double i_gear, double eta_gear, double M_max,
      double P_max, double M_recup_max, double P_recup_max, double R_battery,
      double U_battery_0, double P_const,
      const std::string& ref_powerLossMapString);

  /**
   * \brief Invalidated copy-constructor
   */
  MSDevice_EVPowertrain(const MSDevice_EVPowertrain& /* ref_obj */);

  /**
   * \brief Invalidated assignment operator
   */
  MSDevice_EVPowertrain& operator=(const MSDevice_EVPowertrain /* ref_obj */);



public:
  /**
   * \brief Destructor
   */
  ~MSDevice_EVPowertrain();

  /**
   * \brief Get the current power consumption.
   *
   * \returns Current power consumption [W]
   */
  double get_P() const;

  /**
   * \brief Get the state validity flag.
   *
   * \returns true if the current state of the EV model is valid, else false
   */
  bool get_b_stateValid() const;

  /**
   * \brief Add the EV powertrain device as an option to an options container.
   *
   * \param[out] ref_optionsContainer An options container that shall hold the
   *             options of this device
   */
  static void insertOptions(OptionsCont& ref_optionsContainer);

  /**
   * \brief Create the EV powertrain and add it to the list of devices a
   *        vehicle owns.
   *
   * \param[in] ref_vehicle The owner of the powertrain
   * \param[out] ref_into A vector into which the newly created device shall be
   *             pushed
   */
  static void buildVehicleDevices(SUMOVehicle& ref_vehicle,
      std::vector<MSVehicleDevice*>& ref_into);

  /**
   * \brief Callback function which updates the vehicle's current power
   *        consumption every time it moves.
   *
   * \param[in] ref_trafficObj The object this device belongs to
   * \param[in] oldPos Previous vehicle position [m]
   * \param[in] newPos New vehicle position [m]
   * \param[in] newSpeed New velocity [m/s]
   * \returns true
   */
  bool notifyMove(SUMOTrafficObject& ref_trafficObj, double /* oldPos */,
      double /* newPos */, double /* newSpeed */);
  
  /**
   * \brief Get the name of the device.
   *
   * \returns Device name
   */
  const std::string deviceName() const;

  /**
   * \brief Get a parameter of the EV model.
   *
   * \param[in] ref_key A key specifying which parameter one wants
   * \returns A string representation of the requested parameter
   * \throws InvalidArgument
   */
  std::string getParameter(const std::string& ref_key) const;

  /**
   * \brief Set a parameter of the EV model.
   *
   * \param[in] ref_key A key specifying which parameter one wants to set
   * \param[in] ref_val A string representation of the value
   * \throws InvalidArgument
   */
  void setParameter(const std::string& ref_key, const std::string& ref_val);
};

