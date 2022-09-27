---
title: Emergency
---

# Emergency Vehicle simulation

The simulation of emergency vehicles and their special rights is possible with SUMO.

## Vehicle Class 'emergency'

If a vehicle is assigned a vType with vClass="emergency", the following features are activated:

- the vehicle will receive default shape and size for a rescue vehicle
- the vehicle may drive on lanes that allow "emergency" (but which might disallow normal passenger traffic)
- the vehicle may overtake on the right in all traffic situations
- the vehicle may drive through the [opposite direction edge](OppositeDirectionDriving.md) even when it's own edge has multiple usable lanes
- the vehicle may overtake through the [opposite direction edge](OppositeDirectionDriving.md) even when it's leader vehicle also wants to change left
- the vehicle may overtake through the [opposite direction edge](OppositeDirectionDriving.md) even when it's approaching an intersection without priority
- the vehicle may overtake through the [opposite direction edge](OppositeDirectionDriving.md) even when it must overtake a long column of vehicles (up to 1000m)

## Blue Light Device

A [Blue light device](../sumo.md#bluelight_device) indicates that the emergency
vehicle is driving with special rights (using blue flashing light and sirene).
The device activates the following features:

- the vehicle will ignore red traffic lights
- once the vehicle is jammed (waitingTime > 1s) it will perform lane changing without regard for strategic considerations (it will use any lane that allows to advance along the road)
- if the vehicle is at an intersection but on the wrong turning lane, it will enter the intersection anyway (performing a silent teleport to the closest internal lane that allows continuing it's route

If the simulation is running with the sublane model (**--lateral-resolution** is set to a value > 0), other traffic participants are forced to form a virtual middle lane (rescue lane) to let the emergency vehicle pass.
The following behaviors are activated for surrounding traffic ahead of the device-equipped vehicle

- Vehicles on the leftmost lane move towards the left side (latAlignment="left")
- Vehicles on all other lanes move towards the right side (latAlignment="right")
- Vehicles do not perform any lane changes (in particular, they do not move into the rescue lane)
- After the emergency vehicle has passed the vehicles resume normal driving (with their previous  lateral Alignment).
    
  
Surrounding traffic reacts when within 25m. This distance is configurable by

- option **--device.bluelight.reactiondist**
- traci.vehicle.setParameter(vehID, "device.bluelight.reactiondist", str(value)

## Further Features for driving with special rights
  
- Disregarding right-of-way and traffic lights: 
  - supported by using the [TraCI 'speed mode'
  command](../TraCI/Change_Vehicle_State.md#speed_mode_0xb3) to disable intersection related safety checks.
  - supported by [junction model parameters](Safety.md#junction_model)
- Exceeding the speed limit: This is supported by setting the
  vType-attribute [*speedFactor*](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types)
  (a value of 1.5 allows driving with 150% of the road speed limit).
 

# Example configuration

```
<vType id="rescue" vClass="emergency" speedFactor="1.5">
   <param key="has.bluelight.device" value="true"/>
</vType>

```

# Visualization

The visualization of emergency vehicles is supported in
[sumo-gui](../sumo-gui.md). When setting the vType-attribute `guiShape="emergency"` a
white vehicle with the international sign for first aid is drawn.
Furthermore a police car will be drawn with vType-attribute `guiShape="police"` and a
firebrigade with vType-attribute `guiShape="firebrigade"`. When additionally setting the
vType-attribute `vClass="emergency"` a blue flashing light will be drawn also.

![<File:Ev.png>](../images/Ev.png "File:Ev.png")

# References

Bieker, Laura (2015) Traffic safety evaluations for Emergency Vehicles.
Young Researchers Seminar, 17.-19. June 2015, Rome, Italy.

Bieker, Laura (2011) Emergency Vehicle Prioritization using
Vehicle-To-Vehicle Communication. Young Researchers Seminar, 8.-10. June
2011, Copenhagen, Denmark.
