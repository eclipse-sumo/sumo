---
title: AmitranOutput
---

The Amitran trajectories output contains information about type, current
speed and acceleration of each vehicle according to the schema at
<https://sumo.dlr.de/xsd/amitran/trajectories.xsd>

## Instantiating within the Simulation

The simulation is forced to generate this output using the option **--amitran-output** {{DT_FILE}}. {{DT_FILE}} is
the name of the file the output will be written to. Any other file with
this name will be overwritten, the destination folder must exist.

## Generated Output

The generated XML file looks like this:

```xml
<trajectories xmlns:xsi="https://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="https://sumo.dlr.de/xsd/amitran/trajectories.xsd" timeStepSize="1000">
    <actorConfig id="0" vehicleClass="Passenger" fuel="Gasoline" emissionClass="Euro0" ref="DEFAULT_VEHTYPE"/>
    <vehicle id="0" actorConfig="0" startTime="100000" ref="always_left.0"/>
    <motionState vehicle="0" speed="0" time="100000" acceleration="0"/>
    <vehicle id="1" actorConfig="0" startTime="100000" ref="always_right.0"/>
    <motionState vehicle="1" speed="0" time="100000" acceleration="0"/>
    <vehicle id="2" actorConfig="0" startTime="100000" ref="horizontal.0"/>
    <motionState vehicle="2" speed="0" time="100000" acceleration="0"/>
    <motionState vehicle="0" speed="144" time="101000" acceleration="1442"/>
    ...
</trajectories>
```

The actorConfig elements are in direct correspondence to SUMO's vehicle
types while the vehicle elements refer directly to SUMO's vehicles. The
actorConfig for each vehicle will be defined before the vehicle is
written. The same is true for vehicles and their motionStates.

!!! note
    All numbers and ids are integers, where all the times are in milliseconds. Speeds are given in cm/s and acceleration values in mm/s^2. For details and further information on the types please see the schema https://sumo.dlr.de/xsd/amitran/trajectories.xsd>.

<div style="border:1px solid #909090; min-height: 35px;" align="right">
<span style="float: right; margin-top: -5px;"><a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm"><img src="../../images/FP7-small.gif" alt="Seventh Framework Programme"></a>
<a href="https://web.archive.org/web/20180309093847/https://amitran.eu/"><img src="../../images/AMITRAN-small.png" alt="AMITRAN project"></a></span>
<span style="">This part of SUMO was developed, reworked, or extended within the project
<a href="https://web.archive.org/web/20180309093847/https://amitran.eu/">"AMITRAN"</a>, co-funded by the European Commission within the <a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm">Seventh Framework Programme</a>.</span></div>
