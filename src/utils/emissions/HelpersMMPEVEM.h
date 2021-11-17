/**
 * \file utils/emissions/HelpersMMPEVEM.h
 * \author Kevin Badalian (badalian_k@mmp.rwth-aachen.de)
 *         Teaching and Research Area Mechatronics in Mobile Propulsion (MMP)
 *         RWTH Aachen University
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-10
 * \brief This file contains MMP's emission model for electric vehicles.
 */


#pragma once


#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/EnergyParams.h>

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


public:
  /**
   * \brief Constructor
   */
  HelpersMMPEVEM();

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
        const EnergyParams* ptr_energyParams) const;
};

