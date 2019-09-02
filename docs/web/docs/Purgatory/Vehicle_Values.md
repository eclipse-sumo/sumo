---
title: Purgatory/Vehicle Values
permalink: /Purgatory/Vehicle_Values/
---

## Angle

A vehicle's direction of drive is computed using the positions of the
vehicle's front and back:

```
p1 = vehicle's front position
p2 = vehicle's back position
if p1!=p2:
    angle = atan2(p1.x()-p2.x(), p2.y()-p1.y())*180./PI;
else:
    angle = lane's angle at vehicle's front
```

The check for `p1!=p2` is necessary as the lane's used length may differ
from the lane's geometrical length. Not regarding this caused vehicles
to change the direction of drive to 0°, reported as
[ticket\#430](http://sourceforge.net/apps/trac/sumo/ticket/430).

The following image shows how the resulting encoding.

![Image:vehicle_angle.png](../images/Vehicle_angle.png "Image:vehicle_angle.png")