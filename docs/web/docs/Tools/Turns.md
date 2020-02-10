---
title: Tools/Turns
permalink: /Tools/Turns/
---

# jtcrouter.py
The **J**unction**T**urn**C**ountRouter generates vehicle routes from turn-count data.
It does so by converting the turn counts into into flows and turn-ratio files that are suitable as [JTRROUTER](../JTRROUTER.md) input.
Then it calls JTRROUTER in the background. The turn

```
<SUMO_HOME>/tools/jtcrouter.py -n <net-file> -t <turn-file> -o <output-file>
```
There are three basic styles of converting turn-counts to routes:

- Flows start at all turn-count locations in the network but end when reaching the next count location
- Flows start at all turn-count locations in the network and are discounted when reaching the next count location (**--discount-sources**)
- Flows only start on the fringe of the network (**--fringe-flows**)

## Turn count data format
The turn-count data must be provided in the format:
```
<turns>
   <interval begin="0" end="3600">
      <fromEdge id="myEdge0">
         <toEdge id="myEdge1" probability="25"/>
         <toEdge id="myEdge2" probability="100"/>
         <toEdge id="myEdge3" probability="42"/>
      </fromEdge>

      ... any other edges ...

   </interval>

   ... some further intervals ...

</turns>
```

!!! note
    The attribute 'probability' can replaced with any other attribute name by setting the option **--turn-attribute <attrname>**. The default value of 'probability' is used for consistency with [JTRROUTER](../JTRROUTER.md) and [generateTurnRatios.py](#generateturnratiospy).


# routeSampler.py
The script generates routes from turn-count data. It requires a route file as
input that defines possible routes. Routes are sampled (heuristically) from the input so that
the resulting routes fullfill the turn-count data.

```
<SUMO_HOME>/tools/routeSampler.py -r <input-route-file> -t <turn-file. -o <output-file>
```

The turn-count data format is the same as as described [above](#turncountdataformat).
In addition to loading a turn-count file, routeSampler can also load an [edgeData
file](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md) using option **--edgedata-file**.
The attributes for reading the counts from the turn-data file and edgedata-file
can be set with options

- **--turn-attribute** (default 'probability')
- **--edgedata-attribute** (default 'entered')
 
# generateTurnRatios.py

This script is to calculate the turn ratios or turn movements from a
link to its downstream links with a given route file. The output file
can be directly used as input in jtrrouter, where the time interval will
be set for one day. The call is

```
<SUMO_HOME>/tools/generateTurnRatios.py -r <route-file>
```

The standard output is the traffic volumes. With the option -p turning
ratios will be calculated and outputted.


# generateTurnDefs.py

This script allows generation of the turn definitions based on the
number of lanes allowing particular turns. The basic functionality
distributes the traffic uniformly, that is:

1.  distribute the incoming traffic uniformly across the lanes forming
    the road
2.  distribute the amount of traffic assigned to each lane uniformly
    among the destinations that the lane allows turns to.
3.  sum up the values for each of the destinations that the road being
    processed allows turning to.

Example use

```
<SUMO_HOME>/tools/turn-defs/generateTurnDefs.py --connections-file connections.con.xml --turn-definitions-file output.turndefs.xml
```

The script allows to be extended with new traffic distribution policies
(for example, based on Gaussian distribution) easily. See the
*DestinationWeightCalculator* class for details.

The script processes the connections given in the provided *\*.con.xml*
file. For usage details, execute the *generateTurnDefs.py* script with
*--help* option.

!!! note
    You can generate a connections file with all the connections in the network using [NETCONVERT](../NETCONVERT.md) - see the *--plain-output-prefix* option.

# turnCount2EdgeCount.py

This script converts turn-count data into [edgeData](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md).

```
<SUMO_HOME>/tools/turn-defs/turnCount2EdgeCount.py -t <turn-file> -o <output-file>
```
