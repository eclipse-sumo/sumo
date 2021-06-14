/**
 * \file microsim/output/MSEVPowertrainExport.cpp
 * \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
 * \copyright Eclipse Public License v2.0
 *            (https://www.eclipse.org/legal/epl-2.0/)
 * \date 2021-03
 * \brief This file contains the class responsible for writing the output of
 *        the VKA EV powertrain device and was heavily inspired by
 *        MSBatteryExport.cpp.
 */


#include <microsim/output/MSEVPowertrainExport.h>
#include <microsim/devices/MSDevice_EVPowertrain.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>




/**
 * \brief Write the current power and energy consumption of the EV powertrain
 *        to an output device.
 *
 * \param[in] ref_outputDevice The device to write to
 * \param[in] timestep The current timestep
 * \param[in] precision Precision of the written output
 */
void MSEVPowertrainExport::write(OutputDevice& ref_outputDevice,
    SUMOTime timestep, int precision)
{
  ref_outputDevice.openTag(SUMO_TAG_TIMESTEP).writeAttr(SUMO_ATTR_TIME,
      time2string(timestep));
  ref_outputDevice.setPrecision(precision);

  MSVehicleControl& ref_vehicleControl
      = MSNet::getInstance()->getVehicleControl();
  for(auto it = ref_vehicleControl.loadedVehBegin();
      it != ref_vehicleControl.loadedVehEnd(); it++)
  {
    const MSVehicle* ptr_vehicle = static_cast<const MSVehicle*>((*it).second);
    if(!ptr_vehicle->isOnRoad())
      continue;

    if(static_cast<MSDevice_EVPowertrain*>(ptr_vehicle->getDevice(
        typeid(MSDevice_EVPowertrain))) != nullptr)
    {
      MSDevice_EVPowertrain* ptr_evPowertrain
          = dynamic_cast<MSDevice_EVPowertrain*>(ptr_vehicle->getDevice(
          typeid(MSDevice_EVPowertrain)));
      assert(ptr_evPowertrain != nullptr);
      
      ref_outputDevice.openTag(SUMO_TAG_VEHICLE);
      ref_outputDevice.writeAttr(SUMO_ATTR_ID, ptr_vehicle->getID());
      ref_outputDevice.writeAttr(SUMO_ATTR_SPEED, ptr_vehicle->getSpeed());
      ref_outputDevice.writeAttr(SUMO_ATTR_ACCELERATION,
          ptr_vehicle->getAcceleration());
      ref_outputDevice.writeAttr(SUMO_ATTR_SLOPE, ptr_vehicle->getSlope());
      ref_outputDevice.writeAttr(SUMO_ATTR_POWERCONSUMPTION,
          ptr_evPowertrain->get_P());
      ref_outputDevice.writeAttr(SUMO_ATTR_ENERGYCONSUMPTION,
          ptr_evPowertrain->get_P()*TS);
      ref_outputDevice.writeAttr(SUMO_ATTR_STATEVALID,
          toString(ptr_evPowertrain->get_b_stateValid()));
      ref_outputDevice.closeTag();
    }
  }

  ref_outputDevice.closeTag();
}

