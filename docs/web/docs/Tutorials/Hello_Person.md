---
title: HelloPerson
---

Maya walks to a station, waits for a train and rides to the next station. She then walks from the platform to her destination. The example uses two pedestrian paths and a railway.

## Run the example

Open `docs/examples/sumo/hello_person/data/hello_person.sumocfg` from your SUMO installation in [sumo-gui](../sumo-gui.md). Start the simulation. The view shows both stations, the train and Maya's walk to the west platform. The train waits there until simulation time 75 seconds. Maya boards at West Station and steps off at East Station.

You can also run the same example from the `data` directory.

```sh
sumo -c hello_person.sumocfg --tripinfo-output trips.xml
```

Open `trips.xml` and find the `personinfo` element whose `id` is `maya`. Its `ride` entry identifies the vehicle `regional`. The two `access` entries show the time spent moving between a pedestrian path and a platform. The last `walk` shows when Maya reaches her destination.

## Read the person plan

The `person` element in `hello_person.rou.xml` defines the journey in order.

```xml
<person id="maya" depart="0">
    <walk from="approach" busStop="west"/>
    <ride busStop="east" lines="R1"/>
    <walk to="exit" arrivalPos="60"/>
</person>
```

The first walk starts on the `approach` edge and ends at the stop `west`. The ride starts where the walk ends. The `lines` attribute lets Maya board a vehicle assigned to line `R1`. Its destination is the stop `east`. The final walk ends 60 metres along the `exit` edge. Both train stops are referenced through the `busStop` attribute used by the person plan.

The stops in `hello_person.add.xml` lie on railway lanes. Their `access` elements connect the platforms to the pedestrian lanes. Each `pos` measures distance from the start of its lane. Each `length` gives the distance from the lane to the platform. The vehicle stops in `hello_person.rou.xml` define where the train stops and how long it stays there. The vehicle type uses `guiShape="rail"` to draw the train in the GUI.

## Rebuild the network

The example includes its compiled network. Run this command from the `data` directory after editing the plain node or edge file.

```sh
netconvert --node-files hello_person.nod.xml --edge-files hello_person.edg.xml --offset.disable-normalization --output-file hello_person.net.xml
```

See the [person plan reference](../Specification/Persons.md) and [railway stop access](../Simulation/Public_Transport.md#access_lanes) for the available stage and stop attributes.
