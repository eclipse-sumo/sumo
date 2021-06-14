/**
 * \file microsim/devices/MSDevice_EVPowertrain.cpp
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-02
 * \brief This file contains the EV powertrain device that SUMO vehicles can be
 *        equipped with. It is an adaptation of MSDevice_Example.cpp and was
 *        heavily inspired by MSDevice_Battery.cpp.
 */


#include <microsim/devices/MSDevice_EVPowertrain.h>
#include <utils/common/SUMOTime.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSVehicle.h>




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
MSDevice_EVPowertrain::MSDevice_EVPowertrain(SUMOVehicle& ref_holder,
    const std::string& ref_id, double m, double r_wheel, double Theta,
    double c_rr, double c_d, double A_front, double i_gear, double eta_gear,
    double M_max, double P_max, double M_recup_max, double P_recup_max,
    double R_battery, double U_battery_0, double P_const,
    const std::string& ref_powerLossMapString)
 : MSVehicleDevice(ref_holder, ref_id),
   P(0),
   b_stateValid(true),
   model(vka::SimpleEvPowertrain(m, r_wheel, Theta, c_rr, c_d, A_front, i_gear,
       eta_gear, M_max, P_max, M_recup_max, P_recup_max, R_battery,
       U_battery_0, P_const, ref_powerLossMapString))
{ }



/**
 * \brief Destructor
 */
MSDevice_EVPowertrain::~MSDevice_EVPowertrain()
{ }



/**
 * \brief Get the current power consumption.
 *
 * \returns Current power consumption [W]
 */
double MSDevice_EVPowertrain::get_P() const
{
  return P;
}



/**
 * \brief Get the state validity flag.
 *
 * \returns true if the current state of the EV model is valid, else false
 */
bool MSDevice_EVPowertrain::get_b_stateValid() const
{
  return b_stateValid;
}



/**
 * \brief Add the EV powertrain device as an option to an options container.
 *
 * \param[out] ref_optionsContainer An options container that shall hold the
 *             options of this device
 */
void MSDevice_EVPowertrain::insertOptions(OptionsCont& ref_optionsContainer)
{
  insertDefaultAssignmentOptions("ev_powertrain", "EV Powertrain",
      ref_optionsContainer);
}



/**
 * \brief Create the EV powertrain and add it to the list of devices a vehicle
 *        owns.
 *
 * \param[in] ref_vehicle The owner of the powertrain
 * \param[out] ref_into A vector into which the newly created device shall be
 *             pushed
 */
void MSDevice_EVPowertrain::buildVehicleDevices(SUMOVehicle& ref_vehicle,
    std::vector<MSVehicleDevice*>& ref_into)
{
  OptionsCont& ref_optionsContainer = OptionsCont::getOptions();
  if(equippedByDefaultAssignmentOptions(ref_optionsContainer, "ev_powertrain",
      ref_vehicle, false))
  {
    DefaultParams defaultParams;
    const SUMOVTypeParameter& ref_typeParams
        = ref_vehicle.getVehicleType().getParameter();

    // Read the (default) parameters
    const double m = ref_typeParams.getDouble(toString(SUMO_ATTR_VEHICLEMASS),
        defaultParams.m);
    const double r_wheel
        = ref_typeParams.getDouble(toString(SUMO_ATTR_WHEELRADIUS),
        defaultParams.r_wheel);
    const double Theta
        = ref_typeParams.getDouble(toString(SUMO_ATTR_INTERNALMOMENTOFINERTIA),
        defaultParams.Theta);
    const double c_rr
        = ref_typeParams.getDouble(toString(SUMO_ATTR_ROLLDRAGCOEFFICIENT),
        defaultParams.c_rr);
    const double c_d
        = ref_typeParams.getDouble(toString(SUMO_ATTR_AIRDRAGCOEFFICIENT),
        defaultParams.c_d);
    const double A_front
        = ref_typeParams.getDouble(toString(SUMO_ATTR_FRONTSURFACEAREA),
        defaultParams.A_front);
    const double i_gear
        = ref_typeParams.getDouble(toString(SUMO_ATTR_GEARRATIO),
        defaultParams.i_gear);
    const double eta_gear
        = ref_typeParams.getDouble(toString(SUMO_ATTR_GEAREFFICIENCY),
        defaultParams.eta_gear);
    const double M_max
        = ref_typeParams.getDouble(toString(SUMO_ATTR_MAXIMUMTORQUE),
        defaultParams.M_max);
    const double P_max
        = ref_typeParams.getDouble(toString(SUMO_ATTR_MAXIMUMPOWER),
        defaultParams.P_max);
    const double M_recup_max
        = ref_typeParams.getDouble(
        toString(SUMO_ATTR_MAXIMUMRECUPERATIONTORQUE),
        defaultParams.M_recup_max);
    const double P_recup_max
        = ref_typeParams.getDouble(
        toString(SUMO_ATTR_MAXIMUMRECUPERATIONPOWER),
        defaultParams.P_recup_max);
    const double R_battery
        = ref_typeParams.getDouble(
        toString(SUMO_ATTR_INTERNALBATTERYRESISTANCE),
        defaultParams.R_battery);
    const double U_battery_0
        = ref_typeParams.getDouble(toString(SUMO_ATTR_NOMINALBATTERYVOLTAGE),
        defaultParams.U_battery_0);
    const double P_const
        = ref_typeParams.getDouble(toString(SUMO_ATTR_CONSTANTPOWERINTAKE),
        defaultParams.P_const);
    std::string powerLossMapString
        = ref_typeParams.getParameter(toString(SUMO_ATTR_POWERLOSSMAP));
    if(powerLossMapString == "")
      powerLossMapString = defaultParams.powerLossMapString;

    // Create the device and append it to the vehicle's list
    MSDevice_EVPowertrain* ptr_device = new MSDevice_EVPowertrain(ref_vehicle,
        "ev_powertrain_" + ref_vehicle.getID(), m, r_wheel, Theta, c_rr, c_d,
        A_front, i_gear, eta_gear, M_max, P_max, M_recup_max, P_recup_max,
        R_battery, U_battery_0, P_const, powerLossMapString);
    ref_into.push_back(ptr_device);
  }
}



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
bool MSDevice_EVPowertrain::notifyMove(SUMOTrafficObject& ref_trafficObj,
    double /* oldPos */, double /* newPos */, double /* newSpeed */)
{
  if(!ref_trafficObj.isVehicle())
    return true;
  SUMOVehicle& ref_vehicle = static_cast<SUMOVehicle&>(ref_trafficObj);

  b_stateValid = model.calcPowerConsumption(TS, ref_vehicle.getSpeed(),
      ref_vehicle.getAcceleration(), ref_vehicle.getSlope(), P);
  if(!b_stateValid)
  {
    WRITE_WARNING("The EV powertrain of '" + ref_vehicle.getID()
        + "' encountered an invalid state for v = "
        + toString(ref_vehicle.getSpeed()) + " m/s, a = "
        + toString(ref_vehicle.getAcceleration()) + " m/s^2, and a slope of "
        + toString(ref_vehicle.getSlope()) + " deg.")
  }
  return true;
}



/**
 * \brief Get the name of the device.
 *
 * \returns Device name
 */
const std::string MSDevice_EVPowertrain::deviceName() const
{
  return "EV Powertrain";
}



/**
 * \brief Get a parameter of the EV model.
 *
 * \param[in] ref_key A key specifying which parameter one wants
 * \returns A string representation of the requested parameter
 * \throws InvalidArgument
 */
std::string MSDevice_EVPowertrain::getParameter(
    const std::string& ref_key) const
{
  if(ref_key == toString(SUMO_ATTR_VEHICLEMASS))
    return toString(model.get_m());
  else if(ref_key == toString(SUMO_ATTR_WHEELRADIUS))
    return toString(model.get_r_wheel());
  else if(ref_key == toString(SUMO_ATTR_INTERNALMOMENTOFINERTIA))
    return toString(model.get_Theta());
  else if(ref_key == toString(SUMO_ATTR_ROLLDRAGCOEFFICIENT))
    return toString(model.get_c_rr());
  else if(ref_key == toString(SUMO_ATTR_AIRDRAGCOEFFICIENT))
    return toString(model.get_c_d());
  else if(ref_key == toString(SUMO_ATTR_FRONTSURFACEAREA))
    return toString(model.get_A_front());
  else if(ref_key == toString(SUMO_ATTR_GEARRATIO))
    return toString(model.get_i_gear());
  else if(ref_key == toString(SUMO_ATTR_GEAREFFICIENCY))
    return toString(model.get_eta_gear());
  else if(ref_key == toString(SUMO_ATTR_MAXIMUMTORQUE))
    return toString(model.get_M_max());
  else if(ref_key == toString(SUMO_ATTR_MAXIMUMPOWER))
    return toString(model.get_P_max());
  else if(ref_key == toString(SUMO_ATTR_MAXIMUMRECUPERATIONTORQUE))
    return toString(model.get_M_recup_max());
  else if(ref_key == toString(SUMO_ATTR_MAXIMUMRECUPERATIONPOWER))
    return toString(model.get_P_recup_max());
  else if(ref_key == toString(SUMO_ATTR_INTERNALBATTERYRESISTANCE))
    return toString(model.get_R_battery());
  else if(ref_key == toString(SUMO_ATTR_NOMINALBATTERYVOLTAGE))
    return toString(model.get_U_battery_0());
  else if(ref_key == toString(SUMO_ATTR_CONSTANTPOWERINTAKE))
    return toString(model.get_P_const());
  else if(ref_key == toString(SUMO_ATTR_POWERLOSSMAP))
    return model.get_powerLossMap().toString();
  else
  {
    throw InvalidArgument("Parameter '" + ref_key + "' is not supported for"
        + " device of type '" + deviceName() + "'");
  }
}



/**
 * \brief Set a parameter of the EV model.
 *
 * \param[in] ref_key A key specifying which parameter one wants to set
 * \param[in] ref_val A string representation of the value
 * \throws InvalidArgument
 */
void MSDevice_EVPowertrain::setParameter(const std::string& ref_key,
    const std::string& ref_val)
{
  double value;
  try
  {
    value = StringUtils::toDouble(ref_val);
  }
  catch(const NumberFormatException& ref_e)
  {
    throw InvalidArgument("Setting parameter '" + ref_key + "' requires a"
        + " number for device of type '" + deviceName() + "'");
  }

  if(ref_key == toString(SUMO_ATTR_VEHICLEMASS))
    model.set_m(value);
  else if(ref_key == toString(SUMO_ATTR_CONSTANTPOWERINTAKE))
    model.set_P_const(value);
  else
  {
    throw InvalidArgument("Setting parameter '" + ref_key + "' is not"
        + " supported for device of type '" + deviceName() + "'");
  }
}

