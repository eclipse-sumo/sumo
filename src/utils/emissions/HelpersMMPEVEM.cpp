/**
 * \file utils/emissions/HelpersMMPEVEM.cpp
 * \author Kevin Badalian (badalian_k@mmp.rwth-aachen.de)
 *         Teaching and Research Area Mechatronics in Mobile Propulsion (MMP)
 *         RWTH Aachen University
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-10
 * \brief This file contains a helper class for MMPEVEMs.
 */


#include <utils/emissions/HelpersMMPEVEM.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/StringUtils.h>

#include <fstream>
#include <exception>
#include <cmath>
#include <cctype>




/**
 * \brief Find a key inside a parameter map and convert its value into a double.
 *
 * \param[in] ref_params A parameter map where both keys and values are strings
 * \param[in] ref_key A key
 *
 * \returns The converted double value
 * \throws std::runtime_error
 */
double extractDoubleFromParams(
    const std::map<std::string, std::string>& ref_params,
    const std::string& ref_key)
{
  auto it = ref_params.find(ref_key);
  if(it == ref_params.end())
    throw std::runtime_error("Couldn't find the key '" + ref_key + "'.");
  try
  {
    return std::stod(it->second);
  }
  catch(const std::exception& ref_e)
  {
    throw std::runtime_error("Couldn't convert the value '" + it->second
        + "' of key '" + ref_key + "' into a double.");
  }
}



/**
 * \brief Find a key inside a parameter map and return its string value.
 *
 * \param[in] ref_params A parameter map where both keys and values are strings
 * \param[in] ref_key A key
 *
 * \returns The value as a string
 * \throws std::runtime_error
 */
std::string extractStringFromParams(
    const std::map<std::string, std::string>& ref_params,
    const std::string& ref_key)
{
  auto it = ref_params.find(ref_key);
  if(it == ref_params.end())
    throw std::runtime_error("Couldn't find the key '" + ref_key + "'.");
  return it->second;
}



/**
 * \brief Parse an MMPEVEM model file and create a model base on its parameters.
 *
 * \param[in] ref_modelFile An MMPEVEM model file
 *
 * \returns An MMPEVEM object
 */
MMPEVEM loadModelFromFile(const std::string& ref_modelFile)
{
  std::map<std::string, std::string> params;

  // Parse the file
  std::ifstream ifs(ref_modelFile);
  for(std::string line; std::getline(ifs, line);)
  {
    // Remove whitespaces
    line.erase(std::remove_if(line.begin(), line.end(),
        [](unsigned char x)
        {
          return std::isspace(x);
        }),
        line.end());
    // Skip comments
    if(line[0] == '#')
      continue;

    // Find the '=' character which delimits keys and values
    std::size_t delim = line.find("=");
    if(delim == std::string::npos)
      continue;
    std::string k = line.substr(0, delim);
    std::string v = line.substr(delim + 1);
    params[k] = v;
  }

  // Extract all required parameters
  // Vehicle mass
  double m = extractDoubleFromParams(params, "m");
  // Wheel radius
  double r_wheel = extractDoubleFromParams(params, "r_wheel");
  // Internal moment of inertia
  double Theta = extractDoubleFromParams(params, "Theta");
  // Rolling resistance coefficient
  double c_rr = extractDoubleFromParams(params, "c_rr");
  // Air drag coefficient
  double c_d = extractDoubleFromParams(params, "c_d");
  // Cross-sectional area of the front of the car
  double A_front = extractDoubleFromParams(params, "A_front");
  // Gear ratio
  double i_gear = extractDoubleFromParams(params, "i_gear");
  // Gearbox efficiency
  double eta_gear = extractDoubleFromParams(params, "eta_gear");
  // Maximum torque
  double M_max = extractDoubleFromParams(params, "M_max");
  // Maximum power
  double P_max = extractDoubleFromParams(params, "P_max");
  // Maximum recuperation torque
  double M_recup_max = extractDoubleFromParams(params, "M_recup_max");
  // Maximum recuperation power
  double P_recup_max = extractDoubleFromParams(params, "P_recup_max");
  // Internal battery resistance
  double R_battery = extractDoubleFromParams(params, "R_battery");
  // Nominal battery voltage
  double U_battery_0 = extractDoubleFromParams(params, "U_battery_0");
  // Constant power consumption
  double P_const = extractDoubleFromParams(params, "P_const");
  // Power loss map
  std::string powerLossMapString = extractStringFromParams(params,
      "powerLossMapString");

  // Create the model
  return MMPEVEM(m, r_wheel, Theta, c_rr, c_d, A_front, i_gear, eta_gear, M_max,
      P_max, M_recup_max, P_recup_max, R_battery, U_battery_0, P_const,
      powerLossMapString);
}



/**
 * \brief Constructor
 */
HelpersMMPEVEM::HelpersMMPEVEM()
 : PollutantsInterface::Helper("MMPEVEM", MMPEVEM_BASE, -1),
   myIndex(MMPEVEM_BASE)
{ }



/**
 * \brief Get the emission class that is associated with a name.
 *
 * \param[in] ref_eClass Name of an emission class
 * \param[in] vc The vehicle class to use when determining the default class
 */
SUMOEmissionClass HelpersMMPEVEM::getClassByName(const std::string& ref_eClass,
    const SUMOVehicleClass vc)
{
  std::string eClass = StringUtils::to_lower_case(ref_eClass);

  if(eClass == "unknown" && !myEmissionClassStrings.hasString("unknown"))
    myEmissionClassStrings.addAlias("unknown", getClassByName("vw_id3", vc));
  if(eClass == "default" && !myEmissionClassStrings.hasString("default"))
    myEmissionClassStrings.addAlias("default", getClassByName("vw_id3", vc));
  if(myEmissionClassStrings.hasString(eClass))
    return myEmissionClassStrings.get(eClass);

  int currentIndex = myIndex;
  myIndex++;

  std::string modelFile = std::string(getenv("SUMO_HOME"))
      + "/data/emissions/MMPEVEM/" + eClass + ".dat";
  myModels.insert({currentIndex, loadModelFromFile(modelFile)});

  myEmissionClassStrings.insert(eClass, currentIndex);
  return currentIndex;
}



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
 *
 * \returns The electric power consumption [Wh/s] or 0 for all other emission
 *          types
 */
double HelpersMMPEVEM::compute(const SUMOEmissionClass c,
    const PollutantsInterface::EmissionType e, const double v, const double a,
    const double slope, const EnergyParams* /* ptr_energyParams */) const
{
  if(e != PollutantsInterface::ELEC)
    return 0.0;

  double P = 0.0;  // [W]
  const MMPEVEM& ref_model = myModels.find(c)->second;
  bool b_stateValid = ref_model.calcPowerConsumption(TS, v, a, slope, P);
  P /= 3600.0;  // [Wh/s]

  if(b_stateValid)
    return P;
  else
    return std::nan("");
}

