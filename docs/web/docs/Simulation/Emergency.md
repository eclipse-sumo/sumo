---
title: Simulation/Emergency
permalink: /Simulation/Emergency/
---

# Emergency Vehicle simulation

The simulation of emergency vehicles and their special rights is
partially possible with SUMO. Additional capabilities are planned for
the future.

- [Blue light device](../SUMO.md#bluelight_device): Driving in a
  virtual middle lane on a multi-lane road: This is supported by using
  the [sublane model](../Simulation/SublaneModel.md). Other
  traffic participants are forced to form a virtual middle lane
  (rescue lane) to let the emergency vehicle pass if the vehicle has a
  blue light device. A blue light device indicates that the emergency
  vehicle is driving with special rights (using blue flashing light
  and sirene). All vehicles in front of the emergency vehicle in a
  distanc of 25 meters are driving to sides of the edges. Only the
  emergency vehicle is able use this virtual middle lane. After the
  emergency vehicle has passed the vehicles they are driving like a
  normal vehicle again (with the same lateral Alignement).
  Additionally, the emergency vehicle with the bluelight device is
  able to violate red traffic lights.
- Disregarding right-of-way and traffic lights: This is supported by
  using the [TraCI 'speed mode'
  command](../TraCI/Change_Vehicle_State.md#speed_mode_0xb3)
  to disable intersection related safety checks.
- Exceeding the speed limit: This is supported by setting the
  vType-attribute
  [*speedFactor*](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types)
  (a value of 1.5 allows driving with 150% of the road speed limit).
- Overtaking on the right: This is always permitted for vehicles with `vClass="emergency"`

# Visualization

The visualization of emergency vehicles is supported in
[SUMO-GUI](../SUMO-GUI.md). When setting the vType-attribute `guiShape="emergency"` a
white vehicle with the international sign for first aid is drawn.
Furthermore a police car will be drawn with vType-attribute `guiShape="police"` and a
firebrigade with vType-attribute `guiShape="firebrigade"`. When additionally setting the
vType-attribute `vClass="emergency"` a blue flashing light will be drawn also.

![<File:Ev.png>](../images/Ev.png "File:Ev.png")

# References

Bieker, Laura (2015) Traffic safety evaluations for Emergency Vehicles.
Young Researchers Seminar, 17.-19. Juni 2015, Rom, Italien.

Bieker, Laura (2011) Emergency Vehicle Prioritization using
Vehicle-To-Vehicle Communication. Young Researchers Seminar, 8.-10. Jun.
2011, Kopenhagen, Dänemark.