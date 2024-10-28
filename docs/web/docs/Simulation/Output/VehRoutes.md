---
title: VehRoutes
---

The vehicle routes output contains information about which route a
vehicle took and if his route was replaced at any time by a new one,
each of the previous routes together with the edge at the time their
replacement took place is reported. Furthermore, the times the vehicle
has entered and has left the network are stored herein.

In normal conditions, when all vehicles use predefined routes, the
output does not contain any information that could not be retrieved from
the routes and the tripinfo output. But as soon as you reroute your
vehicles within the simulation, f.e. using rerouters, it will contain
new information.

## Instantiating within the Simulation

This output is enabled for a simulation using the option **--vehroute-output** {{DT_FILE}} or **--vehroutes** {{DT_FILE}}. Further
options affecting this output are listed at
[SUMO\#Output](../../sumo.md#output).

## Generated Output

The generated file look like this:

```xml
<routes>
    <vehicle id="<VEHICLE_ID>" [type="<TYPE_ID>"] depart="<INSERTION_TIME>" arrival="<ARRIVAL_TIME>" [routeLength="<LENGTH>"]>
        <routeDistribution>
            <route replacedOnEdge="<EDGE_ID>" replacedAtTime="<TIME>" probability="0" edges="<PREVIOUS_ROUTE>"/>

            ... further replaced routes ...

            <route edges="<LAST_ROUTE>" [exitTimes="<EXIT_TIMES>"]/>
        <routeDistribution>
    </vehicle>

    <person id="<PERSON_ID>" depart="<INSERTION_TIME>" arrival="<ARRIVAL_TIME>">
        <ride from="..." to="..." lines="..." [started="<START_TIME>" ended="<END_TIME>"]/>
        <walk edges="..." speed="..." [exitTimes="<EXIT_TIMES>" started="<START_TIME>" ended="<END_TIME>"]/>
    </person>

    ... information about further vehicles and persons ...

</routes>
```

| Name              | Type/Unit       | Description                                                                                 |
| ----------------- | --------------- | ------------------------------------------------------------------------------------------- |
| id                | (vehicle) id    | The id of the vehicle this entry describes                                                  |
| type              | vehicle type id | The id of the vehicle type if different from the default                                    |
| depart            | s               | The time the vehicle was emitted into the network                                           |
| arrival           | s               | The time the vehicle was removed from the simulation (due to arriving at the route end)     |
| routeLength       | m               | The total length of the vehicle's route (if activated by the vehroutes.route-length option) |
| replacedOnEdge    | (edge) id       | The edge the vehicle was on when the described route was replaced                           |
| replacedAtTime    | s               | The time step of this replacement                                                           |
| edges             | \[(edge) id\]   | The replaced route                                                                          |
| route edges       | \[(edge) id\]   | The final vehicle route                                                                     |
| exitTimes         | \[time in s\]   | The leave time for every edge in the route or walk, if enabled with the option **--vehroute-output.exit-times**  |
| ended             | s               | The arrival time for the walk or ride (or tranship/transport for containers), if enabled with the option **--vehroute-output.exit-times**  |
| started           | s               | The departure time for the walk or the time when the ride vehicle has been entered and starts to drive again (also for tranship/transport with containers), if enabled with the option **--vehroute-output.exit-times**  |

!!! note
    Additional attributes of the vehicle are also included if they were set.

!!! note
    Only finished trips are saved in the vehroute file as default. With the option **--vehroute-output.write-unfinished** unfinished trips can be saved as well. They can be identified in the file when (1) the option **--vehroute-output.exit-times** is set to true: the started and ended values are -1.; or (2) the option **--vehroute-output.route-length** is set to true: the routeLength value = -1; If following walks exist, their routeLengths will be still reported.

Both the previous and the final routes are complete, that means that
they contain all the edges the vehicle was meant to pass as long as the
route was not replaced, yet. The information replacedOnEdge and
replacedAtTime are available only for routes which were replaced. The
probability field is only given to make the file directly usable as
input to the simulation which lets the vehicle travel its final route.
For proper simulation input the file should be also sorted by departure
time. This can be accomplished by also setting option **--vehroute-output.sorted**.

## Output only for selected vehicles or vehicle types
By default all vehicles will generate vehroute-output. By [assigning a vehroute device](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#assignment_by_generic_parameters) to selected vehicles or vehicle types, this can be changed.
As an example, the following definition enables the device for the vehicle type **t1**:

```xml
<vType id="t1">
  <param key="has.vehroute.device" value="true"/>
</vType>
```

Alternatively, you can use the SUMO option **--device.vehroute.probability** to set the probability for a vehicle to generate the vehroute-output.
For example, setting the option **--device.vehroute.probability 0.25** will equip about a quarter of the vehicles with a vehroute device (each vehicle determines this randomly with 25% probability).

## Further Options

- **--personroute-output FILE**: Writes output for persons/containers to a separate FILE
- **--vehroute-output.exit-times**: Write the exit times for all edges, the attributes 'started' and 'ended' for stops and include attribute 'ended' for rides.
- **--vehroute-output.last-route**: Only write the final route (if the vehicle was rerouted during the simulation)
- **--vehroute-output.sorted**:     Sorts the written vehicles by departure time
- **--vehroute-output.dua**:        Write the output in the duarouter-alternatives format
- **--vehroute-output.cost**:       Write attribute 'cost' for all routes
- **--vehroute-output.intended-depart**: Use the intended departure time rather than the actual departure time (in case of departure delays)
- **--vehroute-output.route-length**: Write total route length as attribute 'length'
- **--vehroute-output.write-unfinished**:   Write vehroute output for vehicles and persons which have not reached their destination at the end of the simulation
- **--vehroute-output.skip-ptlines**:  Skip vehroute output for public transport vehicles (vehicles that have a 'line' attribute)
- **--vehroute-output.incomplete**:   Include invalid routes and route stubs (from-to) in the vehroute output
- **--vehroute-output.stop-edges**:   Include information about edges between stops
- **--vehroute-output.speedfactor**:   Include information vehicle specific speedFactor in output (defaults to 'true' if the vehicle had its departSpeed set)
- **--vehroute-output.internal**:   Include internal edges in the output. Note, that such routes are not suitable for being loaded by sumo or duarouter!

## Visualization example

### departure times versus arrival times

<img src="../../images/vehroute_output.png" width="500px"/>

Generated with [plotXMLAttributes.py](../../Tools/Visualization.md#departure_times_versus_arrival_times).
