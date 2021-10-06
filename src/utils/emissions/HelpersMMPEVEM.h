/**
 * \file utils/emissions/HelpersMMPEVEM.h
 * \author Kevin Badalian (badalian_k@mmp.rwth-aachen.de)
 *         Teaching and Research Area Mechatronics in Mobile Propulsion (MMP)
 *         RWTH Aachen University
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-10
 * \brief This file contains a helper class for MMPEVEMs.
 */


#pragma once


#include <utils/emissions/MMPEVEM/MMPEVEM.h>
#include <utils/emissions/PollutantsInterface.h>

#include <map>




/**
 * \class HelpersMMPEVEM
 * \brief This helper class allows the PollutantsInterface to load and use
 *        different MMPEVEMs.
 */
class HelpersMMPEVEM : public PollutantsInterface::Helper
{
private:
  static const int MMPEVEM_BASE = 5 << 16;
  int myIndex;
  std::map<int, MMPEVEM> myModels;


public:
  /**
   * \brief Constructor
   */
  HelpersMMPEVEM();

  /**
   * \brief Get the emission class that is associated with a name.
   *
   * \param[in] ref_eClass Name of an emission class
   * \param[in] vc The vehicle class to use when determining the default class
   */
  SUMOEmissionClass getClassByName(const std::string& ref_eClass,
        const SUMOVehicleClass vc);

  /**
   * \brief Compute the amount of emitted pollutants for an emission class in a
   *        given state.
   *
   * This method returns 0 for all emission types but electric power
   * consumption.
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
  double compute(const SUMOEmissionClass c,
        const PollutantsInterface::EmissionType e, const double v,
        const double a, const double slope,
        const EnergyParams* /* ptr_energyParams */) const;
};

