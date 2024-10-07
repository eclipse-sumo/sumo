---
title: Cadyts
---

## Introduction

Cadyts is a tool to overcome the gap between the real traffic flows,
which you should have, in form of induction loop data and the simulated
traffic flows produced by SUMO. It adapts the flows by using the script
cadytsIterate.py.

Cadyts is orginally designed to deal with people with different trip plans,
i.e. each person/agent has different trip plans, including different departure
times, activities and duration for each activity etc. In this sense, each
person's departure time can be adjusted by choosing different trip plan for
matching given traffic measurements.

In the SUMO-Cadyts coupling, each person
has only one trip plan with single origin and destination as well as fixed
departure time. So, departure time will not be adjusted. But, when the scale
option is used, the whole demand will be scaled, i.e. the original trips
will be "cloned". Then the scaled trips with the depature times, which
can result in a better matching to traffic measurements for each given
interval, would be selected. The un-selected cloned trips will be treated
as "stay at home" and not be used. So, using the scale factor does not mean
that the whole scaled demand will be considered in the simulation. The scale
factor should not be too large. Otherweise, the network could be overloaded,
and the overall O-D matrix structure would be twisted.

For further information please consult the [Cadyts home page](https://github.com/gunnarfloetteroed/java), [CaDyTS: Calibration of Dynamic Traffic Simulations
] (https://api-depositonce.tu-berlin.de/server/api/core/bitstreams/76ff1f2b-1e6f-4caa-ba56-7f34956a3700/content) and [Cadyts – a free calibration tool for dynamic traffic simulations](https://core.ac.uk/reader/147962730).

## Usage

The Cadyts tool has to be started via cadytsIterate.py, which is a
command line application. It has the following parameters:

| Option | Description |
|--------|-------------|
| -r | route alternatives file from sumo (comma separated list) (in Cadyts: -choicesetfile)|
|-d| adapt to the traffic flows on the edges defined in a given file (in Cadyts: -measfile)|
|-c| classpath for the calibrator, default=os.path.join(os.path.dirname(sys.argv\[0\]), "..","contributed","calibration","cadytsSumoController.jar")|
|-s| last step of the calibration, default=100|
|-S| scaled demand, optional, default=2 (in Cadyts: -demandscale); If -M is defined, this parameter will be used in the calibration.|
|-F| define the number of iterations for stabilizing the results in the DTA-calibration, default= 85 (in Cadyts: - freezeit)|
|-V| define variance of the measured traffic flows for the DTA-calibration, default=1, (in Cadyts: varscale)|
|-P| number of preparatory iterations, default = 5 (in Cadyts: PREPITS)|
|-W| prefix of flow evaluation files; only for the calibration with use of detector data, optional (in Cadyts: -flowfile)|
|-Y| fit the traffic counts as accurate as possible, default = False, (in Cadyts: -bruteforce)|
|-Z| minimal traffic count standard deviation"), default = 25 (in Cadyts: -mincountstddev)|
|-O| override depart times according to updated link travel times, default= False (in Cadyts: -overridett)|
|-M| prefix of OD matrix files in VISUM format (in Cadyts: - fmaprefix)|
|-N| postfix attached to clone ids, default='-CLONE' (in Cadyts: -clonepostfix)|
|-E| No summary information is written by the simulation, default=False|
|-T| No tripinfos are written by the simulation, default=False|

In additions, the simulation-related parameters in the duaIterate.py can
also be defined and applied in cadytsIterate.py, such as a network file,
the begin time and the end time of a simulation/routing, main weights
aggregation period (default=900, in Cadyts: -binsize).

The script cadytsIterate.py expects at least three parameters: The
<NETFILE\> (-n), the <ROUTEALTERATIVESFILE\> (-r), the <REALFLOWSFILE\>
(-d).

Example for using the cadytsIterate.py:

```
    cadytsIterate.py -n <NETFILE> -r <ROUTEALTERNATIVESFILE> -d <REALFLOWSFILE>
    -a <INT> (sets main weights aggregation period) -c <CADTYS-MODEL-PATH>
    -b <INT> (sets simulation begin) -e <INT> (sets simulation end)
```

The <REALFLOWS\>-file has to be a XML-file in form of:

```
    <measurements>
        <singlelink link="1to21" start="25200" end="32400" value="750" stddev="8" type="COUNT_VEH"/>
        <singlelink link="1to22" start="25200" end="32400" value="250" stddev="8" type="COUNT_VEH"/>
    </measurements>
```
The definition of each attribute is:

•	link: edge ID

•	start: the begin of the interval

•	end: the end of the interval

•	value: the measurement

•	stddev: standard deviation of the given data (measurement error); The larger this value is, the less one should believe this value.

•	type: measurement type (here: COUNT_VEH für counting data)

The < ROUTEALTERNATIVESFILE\> has to be a XML-file in form of:

```
    <route-alternatives>
        <vehicle id="830" depart="25208.00" departlane="free" departspeed="max" fromtaz="1" totaz="2">
            <routeDistribution last="1">
                <route cost="154.29" probability="0.57623629" edges="91to1 1to21 out" exitTimes="25240.19 25337.05 25362.30"/>
                <route cost="153.69" probability="0.42376371" edges="91to1 1to22 out" exitTimes="25240.19 25336.44 25361.69"/>
            </routeDistribution>
        </vehicle>
    </route-alternatives>
```

An easy way to get a route-alternatives file is to use the duaIterate.py
or the duarouter application.
