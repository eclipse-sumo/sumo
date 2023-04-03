---
title: Collisions
---

## Instantiating within the Simulation

Collision output is activated by setting the simulation option **--collision-output** {{DT_FILE}} on the
command line or in a *.sumocfg* file. This output contains the
information about each [collision](../Safety.md#collisions) during the simulation.

## Generated Output

The format is the following:

```xml
<collisions>
    <collision ''ATTRIBUTES''...\>
    ...
</collisions>
```

The following output attributes are generated:

| Name               | Type              | Description                                                           |
| ------------------ | ----------------- | --------------------------------------------------------------------- |
| time               | float or hh:mm:ss | The time of the collision                                             |
| type               | string            | The [type](#collision_types) of collision                             | 
| lane               | (lane) id         | The name of the lane on which the collision took place                |
| pos                | m                 | The position along the the lane where the collision took place        |
| collider           | (vehicle) id      | The name of the vehicle that was (more) responsible for the collision |
| victim             | (vehicle) id      | The name of the vehicle or person that was hit                        |
| colliderType       | (type) id         | The type of the collider                                              |
| victimType         | (type) id         | The type of the victim                                                |
| colliderSpeed      | m/s               | The speed of the collider                                             |
| victimSpeed        | ms/               | The speed of the victim                                               |

## Collision Types
The following collision types are written

- collision: rear collision (leader vehicle is the victim)
- frontal: collision will driving through the opposite direction lane (the vehicle using the opposite direction lane is the collider)
- junction: collision between vehicles on a junction. Collider and Victim are assigned arbitrarily.
- sharedLane: collision between vehicle and person on the same lane. The person is always the victim
- crossing: collision between vehicle and person on a pedestrian crossing. The person is always the victim
- walkingarea: collision between vehicle and person on a walkingarea. The person is always the victim
- junctionPedestrian: other collision between vehicle and person on a junction. The person is always the victim
