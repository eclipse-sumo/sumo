/**
 * \file microsim/output/MSEVPowertrainExport.h
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-03
 * \brief This file contains the class responsible for writing the output of
 *        the VKA EV powertrain device and was heavily inspired by
 *        MSBatteryExport.h.
 */


#pragma once


#include <utils/common/SUMOTime.h>




class OutputDevice;



/**
 * \class MSEVPowertrainExport
 * \brief This class generates the output for the EV powertrain XML file.
 */
class MSEVPowertrainExport
{
private:
  /**
   * \brief Invalidated copy-constructor
   */
  MSEVPowertrainExport(const MSEVPowertrainExport& /* ref_obj */);

  /**
   * \brief Invalidated assignment operator
   */
  MSEVPowertrainExport& operator=(const MSEVPowertrainExport& /* ref_obj */);



public:
  /**
   * \brief Write the current power and energy consumption of the EV powertrain
   *        to an output device.
   *
   * \param[in] ref_outputDevice The device to write to
   * \param[in] timestep The current timestep
   * \param[in] precision Precision of the written output
   */
  static void write(OutputDevice& ref_outputDevice, SUMOTime timestep,
      int precision);
};

