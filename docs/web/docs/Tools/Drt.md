---
title: DRT
---

# drtOnline.py

The drtOnline.py tool allows you to simulate shared demand responsive transport (DRT).
The tool uses [TraCI](../TraCI.md) and the [taxi device](../Simulation/Taxi.md) to control
the requests and the drt vehicles. Requests arrive dynamically and are handled by 
a dispatcher that seeks to combine multiple requests in order to maximize the number
of requests served while minimizing the mileage of the entire fleet of vehicles.

If you don't need to simulate a shared service, you can directly use the taxi device
in sumo and don't need this tool. See [Taxi](../Simulation/Taxi.md) and the
respective [Taxi service tutorial ](../Tutorials/TaxiService.md).

The tool requires python3 and the python LP (Linear Programming) modeler [PuLP](https://coin-or.github.io/pulp/).

The minimal call is:

```
python tools/drt/drtOnline.py -n <net-file> -r <route-file> -v <vehicle-file>
```

The route file contains the requests, which are defined as persons with ride
elements with the lines of the drt vehicles. For example:

```
<routes>
    <person id="0" depart="0.00">
        <ride from="edgeStart" to="edgeEnd" lines="taxi"/>
    </person>
</routes>
```

The vehicle file contains the drt vehicles, which must be equipped with a taxi
device. The file should looks like:

```
<routes>
    <vType id="taxi" vClass="taxi">
        <param key="has.taxi.device" value="true"/>
    </vType>
    <trip id="taxiA" depart="0.00" type="taxi" personCapacity="12">
        <stop lane="edge0" triggered="person"/>
    </trip>
</routes>
```
You can change the capacity as well as other attributes of the vehicles just like 
in sumo. See [vehicle attributes](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#available_vtype_attributes).

As output a tripinfo.xml file is generated with information about each request or person
and vehicle. Persons who have as result the attribute ```<vehicle="NULL">```, represent orders
that could not be served and therefore were rejected.

The tool has different options to adapt the drt service. The most 
relevant are: the **--max-wait** option which defines the maximum waiting time for 
pick-up the requests (default 900 s). With **--sim-step** the step time to collect
new reservations is defined (default 30 seconds). The **--drf** option defines the 
direct route factor that is used as an auxiliary to set the maximum travel time with 
the drt service that a person can accept. The maximum travel time with the drt service
cannot be greater than the direct travel time multiplied by this factor, which by 
default has a value equal to 2.

# rtvAlgorithm.py

This is an auxiliary tool used by the drtOnline.py that contains the algorithms
available for the routing of the drt vehicles.