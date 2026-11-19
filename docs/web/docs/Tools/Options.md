---
title: Tools Options Overview
---

# Python Tools Options Overview

The command-line Python tools listed below use `sumolib.options.ArgumentParser` and
therefore also support configuration files via **-c** / **--configuration-file** {{DT_FILE}},
**-C** / **--save-configuration** {{DT_FILE}}, **--save-template** {{DT_FILE}}, and **-h** / **--help**.

## General (`tools/`)

### [averageTripStatistics.py]({{Source}}tools/averageTripStatistics.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_INT}}<br> **--num-runs** {{DT_INT}} | repeat simulation INT times; *default:* **10** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | initial seed for generation random seeds; *default:* **42** |
| **--sumo** {{DT_STR}} | override sumo binary |
| **config** {{DT_STR}} | sumo config file |

### [countEdgeUsage.py]({{Source}}tools/countEdgeUsage.py)

count edge usage by vehicles

| Option | Description |
|--------|-------------|
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | name of output file |
| **--subpart** {{DT_STR}} | Restrict counts to routes that contain the given consecutive edge sequence |
| **--subpart-file** {{DT_FILE}} | Restrict counts to routes that contain one of the consecutive edge sequences in the given input file (one sequence per line) |
| **--subpart.via** {{DT_BOOL}} | Use subpart as via-edges (permit gaps); *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--intermediate** {{DT_BOOL}} | count all edges of a route; *default:* **False** |
| **--taz** {{DT_BOOL}} | use fromTaz and toTaz instead of from and to; *default:* **False** |
| **--elements** {{DT_STR}} | include edges for the given elements in output; *default:* **trip,route,walk** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | collect departures after begin time; *default:* **0** |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | collect departures up to end time (default unlimited) |
| **--period** {{DT_STR}} | create data intervals of the given period duration |
| **-m** {{DT_INT}}<br> **--min-count** {{DT_INT}} | include only values above the minimum; *default:* **0** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | parse net for geo locations of the edges |
| **-p** {{DT_FILE}}<br> **--poi-file** {{DT_FILE}} | write geo POIs |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **routefiles** {{DT_Routes}} | Set one or more input route files |

### [createScreenshotSequence.py]({{Source}}tools/createScreenshotSequence.py)

Run SUMO simulation and take screenshots of chosen time intervals

| Option | Description |
|--------|-------------|
| **--sumocfg** {{DT_STR}} | define the sumocfg to be loaded |
| **-o** {{DT_STR}}<br> **--output-dir** {{DT_STR}} | define the output directory to write the screenshots to; *default:* **screenshots** |
| **--begin** {{DT_FLOAT}} | simulation time the recording begins |
| **--end** {{DT_FLOAT}} | simulation time the recording ends |
| **--view** {{DT_STR}} | View ID to use for the screenshots; *default:* **View #0** |
| **-p** {{DT_STR}}<br> **--prefix** {{DT_STR}} | define a prefix of the screenshot filename |
| **--relative** {{DT_BOOL}} | whether to interpret animation commands relative to the previous one / the initial view settings; *default:* **False** |
| **--zoom** {{DT_STR}} | linear interpolation of zoom values given the key frames syntax t1:v1[;t2:v2 ...] |
| **--rotate** {{DT_STR}} | linear interpolation to rotation values in degrees (around Z axis) given the key frames syntax t1:v1[;t2:v2 ...] |
| **--translate** {{DT_STR}} | linear interpolation to the given view center points with the key frames syntax t1:p1[;t2:p2 ...] where p1 is "x,y" |
| **--include-time** {{DT_BOOL}} | whether to include the system time at simulation begin in the file name; *default:* **False** |
| **--image-format** {{DT_STR}} | image format to use; *default:* **png** |

### [createVehTypeDistribution.py]({{Source}}tools/createVehTypeDistribution.py)

| Option | Description |
|--------|-------------|
| **configFile** {{DT_STR}} | file path of the config file which defines the car-following parameter distributions |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | file path of the output file(if the file already exists, the script tries to insert the distribution node into it); *default:* **vTypeDistributions.add.xml** |
| **-n** {{DT_STR}}<br> **--name** {{DT_STR}} | alphanumerical ID used for the created vehicle type distribution; *default:* **vehDist** |
| **-s** {{DT_INT}}<br> **--size** {{DT_INT}} | number of vTypes in the distribution; *default:* **100** |
| **-d** {{DT_INT}}<br> **--decimal-places** {{DT_INT}} | number of decimal places for numeric attribute values; *default:* **3** |
| **--resampling** {{DT_INT}} | number of attempts to resample a value until it lies in the specified bounds; *default:* **100** |
| **--seed** {{DT_INT}} | random seed; *default:* **42** |

### [distributeChargingStations.py]({{Source}}tools/distributeChargingStations.py)

Add charging stations on parkings and rebuild parkings if necessary

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | define the net file (mandatory); *required* |
| **-a** {{DT_STR}}<br> **--add-files** {{DT_STR}} | define the base parking areas and charging stations; *required* |
| **--selection-file** {{DT_STR}} | optionally restrict the parking area to the selected net part |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | define the output filename for charging station definitions (and by default parking areas); *default:* **parkingCharging.add.xml** |
| **--output-parking-file** {{DT_STR}} | define the output filename for the separate parking additional file |
| **--separate-unused-parkings** {{DT_BOOL}} | Write parkings with charging stations to the main output file and other parkings to the separate one for parkings; *default:* **False** |
| **-p** {{DT_FLOAT}}<br> **--probability** {{DT_FLOAT}} | Probability for an edge to receive a charging station; *default:* **1** |
| **-d** {{DT_FLOAT}}<br> **--density** {{DT_FLOAT}} | Share of parking spaces on the edge which should get charging points; *default:* **0.1** |
| **--power** {{DT_FLOAT}} | Charging power of the charging station; *default:* **22000** |
| **--efficiency** {{DT_FLOAT}} | Charging efficiency; *default:* **0.95** |
| **--min** {{DT_INT}} | Minimum number of charging points per parking; *default:* **1** |
| **--max** {{DT_INT}} | Maximum number of charging points per parking; *default:* **100** |
| **--prefix** {{DT_STR}} | prefix for the charging station IDs; *default:* **cs** |
| **--suffix** {{DT_STR}} | suffix for ID of splitted parkingArea; *default:* **_shift** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **--vclass** {{DT_STR}} | only use edges which permit the given vehicle class; *default:* **passenger** |
| **--entire-parkings** {{DT_BOOL}} | If set, parkings are not divided if the number of charging stations is smaller than the parking capacity; *default:* **False** |
| **--include-existing** {{DT_BOOL}} | If set, loaded charging stations from input files will contribute to the density; *default:* **False** |
| **--skip-equipped-edges** {{DT_BOOL}} | If set, edges where a charging station already exists are skipped; *default:* **False** |
| **--only-roadside** {{DT_BOOL}} | Only use roadside parking for charging points; *default:* **False** |
| **--only-parking-lot** {{DT_BOOL}} | Only use parking lots off the road for charging points; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [edgesInDistricts.py]({{Source}}tools/edgesInDistricts.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **--complete** {{DT_BOOL}} | assign edges only if they are not in more than one district; *default:* **False** |
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | read SUMO network from FILE (mandatory) |
| **-t** {{DT_STR}}<br> **--taz-files** {{DT_STR}} | read districts from FILEs |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | write results to FILE; *default:* **districts.taz.xml** |
| **-x** {{DT_FLOAT}}<br> **--max-speed** {{DT_FLOAT}} | use lanes where speed is not greater than this (m/s); *default:* **1000.0** |
| **-m** {{DT_FLOAT}}<br> **--min-speed** {{DT_FLOAT}} | use lanes where speed is greater than this (m/s); *default:* **0.0** |
| **-w** {{DT_BOOL}}<br> **--weighted** {{DT_BOOL}} | Weights sources/sinks by lane number and length; *default:* **False** |
| **-f** {{DT_BOOL}}<br> **--assign-from** {{DT_BOOL}} | Assign the edge always to the district where the 'from' node is located; *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--internal** {{DT_BOOL}} | Include internal edges in output; *default:* **False** |
| **-l** {{DT_STR}}<br> **--vclass** {{DT_STR}} | Include only edges allowing VCLASS |
| **-s** {{DT_BOOL}}<br> **--shapeinfo** {{DT_BOOL}} | write also the shape info in the file; *default:* **False** |
| **--merge-separator** {{DT_STR}} | merge edge lists of taz starting with the same string up to the given separator |
| **--merge-param** {{DT_STR}} | merge edge lists of taz/polygons having the same value for the given parameter |
| **--min-length** {{DT_FLOAT}} | use edges where length is greater than this (m); *default:* **0.0** |

### [fcdReplay.py]({{Source}}tools/fcdReplay.py)

| Option | Description |
|--------|-------------|
| **-k** {{DT_STR}}<br> **--sumo-config** {{DT_STR}} | sumo config file; *default:* **sumo.sumocfg** |
| **-f** {{DT_STR}}<br> **--fcd-files** {{DT_STR}} | the fcd files to replay |
| **--geo** {{DT_BOOL}} | use fcd data in lon,lat format; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **sumo_args** {{DT_STR_LIST}} | additional sumo arguments |

### [findAllRoutes.py]({{Source}}tools/findAllRoutes.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | the SUMO net filename |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | the route output filename |
| **-s** {{DT_STR}}<br> **--source-edges** {{DT_STR}} | comma separated list of source edge ids |
| **-t** {{DT_STR}}<br> **--target-edges** {{DT_STR}} | comma separated list of target edge ids |

### [generateBidiDistricts.py]({{Source}}tools/generateBidiDistricts.py)

| Option | Description |
|--------|-------------|
| **-o** {{DT_STR}}<br> **--outfile** {{DT_STR}} | name of output file |
| **-r** {{DT_FLOAT}}<br> **--radius** {{DT_FLOAT}} | maximum air distance around the edge; *default:* **10.0** |
| **-t** {{DT_FLOAT}}<br> **--travel-distance** {{DT_FLOAT}} | maximum travel distance in the graph |
| **--symmetrical** {{DT_BOOL}} | extend the bidi-relationship to be symmetrical; *default:* **False** |
| **net** {{DT_STR}} | SUMO network file |

### [generateContinuousRerouters.py]({{Source}}tools/generateContinuousRerouters.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the net file (mandatory); *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output rerouter filename; *default:* **rerouters.xml** |
| **-T** {{DT_STR}}<br> **--turn-defaults** {{DT_STR}} | Use STR[] as default turn probabilities [right,straight,left[,turn]]; *default:* **30,50,20** |
| **-l** {{DT_BOOL}}<br> **--long-routes** {{DT_BOOL}} | place rerouters further upstream (after the previous decision point) to increase overlap of routes when rerouting and thereby improve anticipation of intersections; *default:* **False** |
| **--vclass** {{DT_STR}} | only create routes which permit the given vehicle class |
| **-s** {{DT_FILE}}<br> **--stop-file** {{DT_FILE}} | Stop at any stopping places loaded from file |
| **--stop.duration** {{DT_INT}} | Set duration for added stops; *default:* **60** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | begin time; *default:* **0** |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | end time (default 3600); *default:* **3600** |

### [generateLandmarks.py]({{Source}}tools/generateLandmarks.py)

Generate random boundary landmarks for a given network

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | define the net file (mandatory); *required* |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | define the output filename; *default:* **landmarks.txt** |
| **--selection-output** {{DT_STR}} | Write a selection file for visualization |
| **-N** {{DT_INT}}<br> **--number** {{DT_INT}} | Define number of landmarks to generate; *default:* **10** |
| **--geo** {{DT_BOOL}} | Store landmarks as geo-coordinates instead of edge IDs; *default:* **False** |
| **-d** {{DT_BOOL}}<br> **--no-deadends** {{DT_BOOL}} | Only use edges that have at least one follower and one successor; *default:* **False** |
| **-p** {{DT_INT}}<br> **--min-priority** {{DT_INT}} | Only use edges with priority of at least INT; *default:* **-10** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **--random** {{DT_BOOL}} | use a random seed to initialize the random number generator; *default:* **False** |
| **--vclass** {{DT_STR}} | only use edges which permit the given vehicle class; *default:* **passenger** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [generateParkingAreaRerouters.py]({{Source}}tools/generateParkingAreaRerouters.py)

Generate parking area rerouters from the parking area definition.

| Option | Description |
|--------|-------------|
| **-a** {{DT_FILE}}<br> **--parking-areas** {{DT_FILE}} | SUMO parkingArea definition.; *required* |
| **-n** {{DT_NET}}<br> **--sumo-net** {{DT_NET}} | SUMO network definition.; *required* |
| **-b** {{DT_FLOAT}}<br> **--begin** {{DT_FLOAT}} | Rerouter interval begin; *default:* **0.0** |
| **-e** {{DT_FLOAT}}<br> **--end** {{DT_FLOAT}} | Rerouter interval end; *default:* **86400.0** |
| **--max-number-alternatives** {{DT_INT}} | Rerouter: max number of alternatives.; *default:* **10** |
| **--max-distance-alternatives** {{DT_FLOAT}} | Rerouter: max distance for the alternatives.; *default:* **500.0** |
| **--min-capacity-visibility-true** {{DT_INT}} | Rerouter: parking capacity for the visibility threshold.; *default:* **25** |
| **--max-distance-visibility-true** {{DT_FLOAT}} | Rerouter: parking distance for the visibility threshold.; *default:* **250.0** |
| **--opposite-visible** {{DT_BOOL}} | ParkingArea on the opposite side of the road is always visible; *default:* **False** |
| **--prefer-visible** {{DT_BOOL}} | ParkingAreas which are visible are preferentially; *default:* **False** |
| **--min-capacity** {{DT_INT}} | Do no reroute to parkingAreas with less than min-capacity; *default:* **1** |
| **--distribute** {{DT_STR}} | Distribute alternatives by distance according to the given weights. 3,1 means that 75 percent of the alternatives are below the median distance of allalternatives in range and 25 percent are above the median distance |
| **--visible-ids** {{DT_STR}} | set list of parkingArea ids as always visible |
| **--processes** {{DT_INT}} | Number of processes spawned to compute the distance between parking areas.; *default:* **1** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Name for the output file.; *required* |
| **--tqdm** {{DT_BOOL}} | Enable TQDM feature.; *default:* **False** |

### [generateParkingAreas.py]({{Source}}tools/generateParkingAreas.py)

Generate parking areas along the edges

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | define the net file (mandatory) |
| **--selection-file** {{DT_STR}} | optionally restrict the parking area to the selected net part |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | define the output filename; *default:* **parkingareas.add.xml** |
| **-p** {{DT_FLOAT}}<br> **--probability** {{DT_FLOAT}} | Probability for an edge to receive a parkingArea; *default:* **1** |
| **-L** {{DT_FLOAT}}<br> **--length** {{DT_FLOAT}} | Length required per parking space; *default:* **6** |
| **-l** {{DT_FLOAT}}<br> **--space-length** {{DT_FLOAT}} | visual length of each parking space; *default:* **5** |
| **-w** {{DT_FLOAT}}<br> **--width** {{DT_FLOAT}} | visual width of each parking space |
| **-r** {{DT_BOOL}}<br> **--random-capacity** {{DT_BOOL}} | Randomize roadsideCapacity; *default:* **False** |
| **--min** {{DT_INT}} | Minimum capacity for parkingAreas; *default:* **0** |
| **--max** {{DT_INT}} | Maximum capacity for parkingAreas; *default:* **1000000000** |
| **--edge-type.keep** {{DT_STR}} | Optional list of edge types to keep exclusively |
| **--edge-type.remove** {{DT_STR}} | Optional list of edge types to exclude |
| **--keep-all** {{DT_BOOL}} | whether to keep parkingAreas with 0 capacity; *default:* **False** |
| **--lefthand** {{DT_BOOL}} | whether to place parkingareas on the left of the road; *default:* **False** |
| **-a** {{DT_FLOAT}}<br> **--angle** {{DT_FLOAT}} | parking area angle |
| **--on-road** {{DT_BOOL}} | whether to place parkingareas directly on the road; *default:* **False** |
| **--on-road.lane-offset** {{DT_INT}} | lane index to place on-road parking spaces on (use negative value to use all lanes); *default:* **0** |
| **--prefix** {{DT_STR}} | prefix for the parkingArea ids; *default:* **pa** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **--random** {{DT_BOOL}} | use a random seed to initialize the random number generator; *default:* **False** |
| **--vclass** {{DT_STR}} | only use edges which permit the given vehicle class; *default:* **passenger** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [generateRailSignalConstraints.py]({{Source}}tools/generateRailSignalConstraints.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | Input network file |
| **-a** {{DT_STR}}<br> **--additional-file** {{DT_STR}} | Input additional file (busStops) |
| **-t** {{DT_STR}}<br> **--trip-file** {{DT_STR}} | Input trip file (will be processed into a route file) |
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | Input route file (must contain routed vehicles rather than trips) |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | Output additional file; *default:* **constraints.add.xml** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | ignore vehicles departing before the given begin time (seconds or H:M:S); *default:* **0** |
| **--until-from-duration** {{DT_BOOL}} | Use stop arrival+duration instead of 'until' to compute insertion constraints; *default:* **False** |
| **-d** {{DT_STR}}<br> **--delay** {{DT_STR}} | Assume given maximum delay when computing the number of intermediate vehicles that pass a given signal (for setting limit); *default:* **0** |
| **-l** {{DT_INT}}<br> **--limit** {{DT_INT}} | Increases the limit value for tracking passed vehicles by the given amount; *default:* **0** |
| **--abort-unordered** {{DT_BOOL}} | Abort generation of constraints for a stop once the ordering of vehicles by 'arrival' differs from the ordering by 'until'; *default:* **False** |
| **--abort-unordered.keep-actual** {{DT_BOOL}} | Keep constraints for a stop with 'started' and 'ended' value even after the ordering of vehicles by 'arrival' differs from the ordering by 'until'; *default:* **False** |
| **--premature-threshold** {{DT_STR}} | Ignore schedule if a train leaves a station ahead of schedule by more than the threshold value; *default:* **600** |
| **--write-inactive** {{DT_BOOL}} | Export aborted constraints as inactive; *default:* **False** |
| **--all-inactive** {{DT_BOOL}} | Export all constraints as inactive; *default:* **False** |
| **-p** {{DT_BOOL}}<br> **--ignore-parking** {{DT_BOOL}} | Ignore unordered timing if the vehicle which arrives first is parking; *default:* **False** |
| **-P** {{DT_BOOL}}<br> **--skip-parking** {{DT_BOOL}} | Do not generate constraints for a vehicle that parks at the next stop; *default:* **False** |
| **--redundant** {{DT_STR}} | Add redundant constraint within given time range (reduces impact of modifying constraints at runtime); *default:* **-1** |
| **--bidi-max-range** {{DT_FLOAT}} | Find bidiStops on sequential edges within the given range in m; *default:* **1** |
| **--bidi-conflicts** {{DT_BOOL}} | Write bidiPredecessor constraints; *default:* **False** |
| **--comment.line** {{DT_BOOL}} | add lines of involved trains in comment; *default:* **False** |
| **--comment.id** {{DT_BOOL}} | add ids of involved trains in comment (when different from tripId); *default:* **False** |
| **--comment.switch** {{DT_BOOL}} | add id of the merging switch that prompted the constraint; *default:* **False** |
| **--comment.stop** {{DT_BOOL}} | add busStop id that was used to determine the train ordering for the constraint; *default:* **False** |
| **--comment.time** {{DT_BOOL}} | add timing information for the constraint; *default:* **False** |
| **--comment.all** {{DT_BOOL}} | add all comments; *default:* **False** |
| **--params** {{DT_BOOL}} | stores comments as params; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **--debug-switch** {{DT_STR}} | print debug information for the given merge-switch edge |
| **--debug-signal** {{DT_STR}} | print debug information for the given signal id |
| **--debug-stop** {{DT_STR}} | print debug information for the given busStop id |
| **--debug-vehicle** {{DT_STR}} | print debug information for the given vehicle id |
| **--debug-foe-vehicle** {{DT_STR}} | print debug information for the given (foe) vehicle id |
| **--debug-edge** {{DT_STR}} | print debug information for the given edge id |

### [generateRerouters.py]({{Source}}tools/generateRerouters.py)

Generate rerouter definition for closed edges

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the net file (mandatory); *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output rerouter filename; *default:* **rerouters.xml** |
| **-x** {{DT_STR_LIST}}<br> **--closed-edges** {{DT_STR_LIST}} | provide a comma-separated list of edges to close |
| **-f** {{DT_FILE}}<br> **--closed-edges.input-file** {{DT_FILE}} | provide a selection file with edges to close |
| **-i** {{DT_STR}}<br> **--id-prefix** {{DT_STR}} | id prefix for generated rerouters; *default:* **rr** |
| **--vclass** {{DT_STR}} | only consider necessary detours for the given vehicle class (default passenger); *default:* **passenger** |
| **--allow** {{DT_STR}} | vClasses that shall be permitted on the closed edge; *default:* **authority** |
| **--disallow** {{DT_STR}} | vClasses that shall be prohibited on the closed edge |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin time for the closing; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | end time for the closing (default 86400); *default:* **86400** |
| **-t** {{DT_BOOL}}<br> **--terminate-unreachable** {{DT_BOOL}} | Let vehicles that cannot reach their destination terminate their route at the notification edge; *default:* **False** |

### [jtcrouter.py]({{Source}}tools/jtcrouter.py)

Route by turn counts

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | Input net file |
| **-t** {{DT_STR}}<br> **--turn-file** {{DT_STR}} | Input turn-count file |
| **--additional-files** {{DT_STR}} | Input additional files to pass to jtrrouter |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | Output route file; *default:* **out.rou.xml** |
| **--turn-output** {{DT_STR}} | Intermediate turn-ratio-file; *default:* **turns.tmp.xml** |
| **--flow-output** {{DT_STR}} | Intermediate flow file; *default:* **flows.tmp.xml** |
| **--turn-attribute** {{DT_STR}} | Read turning counts from the given attribute; *default:* **count** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | begin time; *default:* **0** |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | end time (default 3600); *default:* **3600** |
| **-p** {{DT_STR}}<br> **--count-param** {{DT_STR}} | the connection parameter to use as count; *default:* **count** |
| **--fringe-flows** {{DT_BOOL}} | Avoid overlapping flows (start only on the outside of the network); *default:* **False** |
| **-D** {{DT_BOOL}}<br> **--discount-sources** {{DT_BOOL}} | passes option --discount-sources to jtrrouter; *default:* **False** |
| **--prefix** {{DT_STR}} | prefix for the flow ids |
| **-a** {{DT_STR}}<br> **--attributes** {{DT_STR}} | additional flow attributes |

### [osmGet.py]({{Source}}tools/osmGet.py)

Get network from OpenStreetMap

| Option | Description |
|--------|-------------|
| **-p** {{DT_STR}}<br> **--prefix** {{DT_STR}} | for output file; *default:* **osm** |
| **-b** {{DT_STR}}<br> **--bbox** {{DT_STR}} | bounding box to retrieve in geo coordinates west,south,east,north |
| **-t** {{DT_INT}}<br> **--tiles** {{DT_INT}} | number of tiles the output gets split into; *default:* **1** |
| **-d** {{DT_STR}}<br> **--output-dir** {{DT_STR}} | optional output directory (must already exist) |
| **-a** {{DT_INT}}<br> **--area** {{DT_INT}} | area id to retrieve |
| **-x** {{DT_STR}}<br> **--polygon** {{DT_STR}} | calculate bounding box from polygon data in file |
| **-u** {{DT_STR}}<br> **--url** {{DT_STR}} | Download from the given Overpass server(s); *default:* **dressel,dressel-na,hpi,oapi,pcoffee** |
| **-w** {{DT_BOOL}}<br> **--wikidata** {{DT_BOOL}} | get the corresponding wikidata; *default:* **False** |
| **-r** {{DT_STR}}<br> **--road-types** {{DT_STR}} | only delivers osm data to the specified road-types |
| **-s** {{DT_BOOL}}<br> **--shapes** {{DT_BOOL}} | determines if polygon data (buildings, areas, etc.) is downloaded; *default:* **False** |
| **-z** {{DT_BOOL}}<br> **--gzip** {{DT_BOOL}} | save gzipped output; *default:* **False** |
| **-q** {{DT_STR}}<br> **--query-output** {{DT_STR}} | write query to the given FILE |
| **--config-output** {{DT_STR}} | write configuration to the given FILE and not as comment into the output file |
| **--retries** {{DT_INT}} | How many attempts to download if a 504 is encountered; *default:* **5** |
| **--timeout** {{DT_INT}} | Set request timeout; *default:* **250** |
| **--retry-delay** {{DT_INT}} | Delay between download attempts; *default:* **3** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [plot_trajectories.py]({{Source}}tools/plot_trajectories.py)

| Option | Description |
|--------|-------------|
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |
| **-t** {{DT_STR}}<br> **--trajectory-type** {{DT_STR}} | select two letters from [t, s, d, a, i, x, y, k, g] to plot Time, Speed, Distance, Acceleration, Angle, x-Position, y-Position, Kilometrage, leaderGap. Default 'ds' plots Distance vs. Speed; *default:* **ds** |
| **--persons** {{DT_BOOL}} | plot person trajectories; *default:* **False** |
| **--meso** {{DT_BOOL}} | plot meso trajectories; *default:* **False** |
| **-s** {{DT_BOOL}}<br> **--show** {{DT_BOOL}} | show plot directly; *default:* **False** |
| **--csv-output** {{DT_STR}} | write plot as csv |
| **-b** {{DT_BOOL}}<br> **--ballistic** {{DT_BOOL}} | perform ballistic integration of distance; *default:* **False** |
| **--filter-route** {{DT_STR}} | only export trajectories that pass the given list of edges (regardless of gaps) |
| **--filter-edges** {{DT_STR}} | only consider data for the given list of edges |
| **--filter-ids** {{DT_STR}} | only consider data for the given list of vehicle (or person) ids |
| **-p** {{DT_FLOAT}}<br> **--pick-distance** {{DT_FLOAT}} | pick lines within the given distance in interactive plot mode; *default:* **1** |
| **-i** {{DT_FLOAT}}<br> **--invert-distance-angle** {{DT_FLOAT}} | invert distance for trajectories with a average angle near FLOAT |
| **--label** {{DT_STR}} | plot label (default input file name |
| **--invert-yaxis** {{DT_BOOL}} | Invert the Y-Axis; *default:* **False** |
| **--legend** {{DT_BOOL}} | Add legend; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **fcdfiles** {{DT_FILE}} | FCD input file(s) |

### [ptlines2flows.py]({{Source}}tools/ptlines2flows.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | network file; *required* |
| **-l** {{DT_STR}}<br> **--ptlines-file** {{DT_STR}} | public transit lines file; *required* |
| **-s** {{DT_STR}}<br> **--ptstops-file** {{DT_STR}} | public transit stops file; *required* |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | output flows file; *default:* **flows.rou.xml** |
| **-i** {{DT_STR}}<br> **--stopinfos-file** {{DT_STR}} | file from '--stop-output'; *default:* **stopinfos.xml** |
| **-r** {{DT_STR}}<br> **--routes-file** {{DT_STR}} | file from '--vehroute-output'; *default:* **vehroutes.xml** |
| **-t** {{DT_STR}}<br> **--trips-file** {{DT_STR}} | output trips file; *default:* **trips.trips.xml** |
| **-p** {{DT_TIME}}<br> **--period** {{DT_TIME}} | the default service period (in seconds) to use if none is specified in the ptlines file; *default:* **600** |
| **--period-aerialway** {{DT_TIME}} | the default service period (in seconds) to use for aerialways if none is specified in the ptlines file; *default:* **60** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | start time; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | end time; *default:* **3600** |
| **-j** {{DT_TIME}}<br> **--jump-duration** {{DT_TIME}} | The time to add for each missing stop (when joining opposite lines); *default:* **180** |
| **-T** {{DT_TIME}}<br> **--turnaround-duration** {{DT_TIME}} | The extra stopping time at terminal stops; *default:* **300** |
| **--join** {{DT_BOOL}} | Join opposite lines at the terminals; *default:* **False** |
| **--join-threshold** {{DT_FLOAT}} | maximum distance for terminal stops when joining lines; *default:* **100** |
| **--multistart** {{DT_BOOL}} | Insert multiple vehicles per line at different offsets along the route to avoid warmup; *default:* **False** |
| **--min-stops** {{DT_INT}} | only import lines with at least this number of stops; *default:* **2** |
| **-f** {{DT_STR}}<br> **--flow-attributes** {{DT_STR}} | additional flow attributes |
| **--use-osm-routes** {{DT_BOOL}} | use osm routes; *default:* **False** |
| **--extend-to-fringe** {{DT_BOOL}} | let routes of incomplete lines start/end at the network border if the route edges are known; *default:* **False** |
| **--random-begin** {{DT_BOOL}} | randomize begin times within period; *default:* **False** |
| **--seed** {{DT_INT}} | random seed |
| **--ignore-errors** {{DT_BOOL}} | ignore problems with the input data; *default:* **False** |
| **--no-vtypes** {{DT_BOOL}} | do not write vtypes for generated flows; *default:* **False** |
| **--types** {{DT_STR}} | only export the given list of types (using OSM nomenclature) |
| **--bus.parking** {{DT_BOOL}} | let busses clear the road while stopping; *default:* **False** |
| **--vtype-prefix** {{DT_STR}} | prefix for vtype ids |
| **-d** {{DT_FLOAT}}<br> **--stop-duration** {{DT_FLOAT}} | Configure the minimum stopping duration; *default:* **20** |
| **--stop-duration-slack** {{DT_FLOAT}} | Stopping time reserve in the schedule; *default:* **10** |
| **--speedfactor.bus** {{DT_FLOAT}} | Assumed bus relative travel speed; *default:* **0.95** |
| **--speedfactor.tram** {{DT_FLOAT}} | Assumed tram relative travel speed; *default:* **1.0** |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | write times as h:m:s; *default:* **False** |
| **--night** {{DT_BOOL}} | Export night service lines; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [randomTrips.py]({{Source}}tools/randomTrips.py)

Generate trips between random locations

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the net file (mandatory); *required* |
| **-a** {{DT_FILE}}<br> **--additional-files** {{DT_FILE}} | define additional files to be loaded by the router |
| **--weights-prefix** {{DT_FILE}} | loads probabilities for being source, destination and via-edge from the files named 'prefix'.src.xml, 'prefix'.dst.xml and 'prefix'.via.xml |
| **--edge-type-file** {{DT_STR}} | Load a file that defines probability factors for specific edge types (each line with 'TYPE FLOAT') |
| **-o** {{DT_Routes}}<br> **--output-trip-file** {{DT_Routes}} | define the output trip filename; *default:* **trips.trips.xml** |
| **-r** {{DT_Routes}}<br> **--route-file** {{DT_Routes}} | generates route file with duarouter |
| **--vtype-output** {{DT_FILE}} | Store generated vehicle types in a separate file |
| **--weights-output-prefix** {{DT_FILE}} | generates weights files for visualisation |
| **--error-log** {{DT_FILE}} | record routing errors |
| **--persons** {{DT_BOOL}} | create a person file with person trips instead of vehicle trips; *default:* **False** |
| **--personrides** {{DT_STR}} | create a person file with rides using STR as lines attribute |
| **--persontrip.modes** {{DT_STR}} | Use any comma-separated combination of 'bicycle', 'car', 'public' and 'taxi' |
| **--persontrip.transfer.car-walk** {{DT_STR}} | Where are mode changes from car to walking allowed (possible values: 'ptStops', 'allJunctions' and combinations) |
| **--persontrip.walkfactor** {{DT_FLOAT}} | Use FLOAT as a factor on pedestrian maximum speed during intermodal routing |
| **--persontrip.walk-opposite-factor** {{DT_FLOAT}} | Use FLOAT as a factor on pedestrian maximum speed against vehicle traffic direction |
| **--from-stops** {{DT_STR}} | Create trips that start at stopping places of the indicated type(s). i.e. 'busStop' |
| **--to-stops** {{DT_STR}} | Create trips that end at stopping places of the indicated type(s). i.e. 'busStop' |
| **--prefix** {{DT_STR}} | prefix for the trip ids |
| **-t** {{DT_STR}}<br> **--trip-attributes** {{DT_STR}} | additional trip attributes. When generating pedestrians, attributes for 'person' and 'walk' are supported. |
| **--fringe-start-attributes** {{DT_STR}} | additional trip attributes when starting on a fringe. |
| **--vehicle-class** {{DT_STR}} | The vehicle class assigned to the generated trips (adds a standard vType definition to the output file). |
| **--random-departpos** {{DT_BOOL}} | Randomly choose a position on the starting edge of the trip; *default:* **False** |
| **--random-arrivalpos** {{DT_BOOL}} | Randomly choose a position on the ending edge of the trip; *default:* **False** |
| **--junction-taz** {{DT_BOOL}} | Write trips with fromJunction and toJunction; *default:* **False** |
| **-l** {{DT_BOOL}}<br> **--length** {{DT_BOOL}} | weight edge probability by length; *default:* **False** |
| **-L** {{DT_BOOL}}<br> **--lanes** {{DT_BOOL}} | weight edge probability by number of lanes; *default:* **False** |
| **--edge-param** {{DT_STR}} | use the given edge parameter as factor for edge |
| **--speed-exponent** {{DT_FLOAT}} | weight edge probability by speed^'FLOAT' (default 0); *default:* **0.0** |
| **--fringe-speed-exponent** {{DT_STR}} | weight fringe edge probability by speed^'FLOAT' (default: speed exponent) |
| **--angle** {{DT_FLOAT}} | weight edge probability by angle [0-360] relative to the network center; *default:* **90.0** |
| **--angle-factor** {{DT_FLOAT}} | maximum weight factor for angle; *default:* **1.0** |
| **--random-factor** {{DT_FLOAT}} | edge weights are dynamically disturbed by a random factor drawn uniformly from [1,FLOAT]; *default:* **1.0** |
| **--fringe-factor** {{DT_STR}} | multiply weight of fringe edges by 'FLOAT' (default 1) or set value 'max' to force all traffic to start/end at the fringe.; *default:* **1.0** |
| **--fringe-threshold** {{DT_FLOAT}} | only consider edges with speed above 'FLOAT' as fringe edges (default 0); *default:* **0.0** |
| **--allow-fringe** {{DT_BOOL}} | Allow departing on edges that leave the network and arriving on edges that enter the network (via turnarounds or as 1-edge trips; *default:* **False** |
| **--allow-fringe.min-length** {{DT_FLOAT}} | Allow departing on edges that leave the network and arriving on edges that enter the network, if they have at least the given length |
| **--fringe-junctions** {{DT_BOOL}} | Determine fringe edges based on junction attribute 'fringe'; *default:* **False** |
| **--edge-permission** {{DT_STR}} | only from and to edges which permit the given vehicle class |
| **--via-edge-types** {{DT_STR}} | Set list of edge types that cannot be used for departure or arrival (unless being on the fringe) |
| **--allow-roundabouts** {{DT_BOOL}} | Permit trips that start or end inside a roundabout; *default:* **False** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **--random** {{DT_BOOL}} | use a random seed to initialize the random number generator; *default:* **False** |
| **--min-distance** {{DT_FLOAT}} | require start and end edges for each trip to be at least 'FLOAT' m apart; *default:* **0.0** |
| **--min-distance.fringe** {{DT_FLOAT}} | require start and end edges for each fringe to fringe trip to be at least 'FLOAT' m apart |
| **--max-distance** {{DT_FLOAT}} | require start and end edges for each trip to be at most 'FLOAT' m apart (default 0 which disables any checks) |
| **-i** {{DT_INT}}<br> **--intermediate** {{DT_INT}} | generates the given number of intermediate way points; *default:* **0** |
| **--return-to-origin** {{DT_BOOL}} | end each trip on the opposite-direction edge of its source edge if available; *default:* **False** |
| **--jtrrouter** {{DT_BOOL}} | Create flows without destination as input for jtrrouter; *default:* **False** |
| **--maxtries** {{DT_INT}} | number of attemps for finding a trip which meets the distance constraints; *default:* **100** |
| **--remove-loops** {{DT_BOOL}} | Remove loops at route start and end; *default:* **False** |
| **--random-routing-factor** {{DT_FLOAT}} | Edge weights for routing are dynamically disturbed by a random factor drawn uniformly from [1,FLOAT); *default:* **1** |
| **--marouter** {{DT_BOOL}} | Compute routes with marouter instead of duarouter; *default:* **False** |
| **--validate** {{DT_BOOL}} | Whether to produce trip output that is already checked for connectivity; *default:* **True** |
| **--no-validate** {{DT_BOOL}} | *default:* **True** |
| **--threads** {{DT_INT}} | Uses INT threads when validating trips (defaults to half the available CPUs |
| **--min-success-rate** {{DT_FLOAT}} | Minimum ratio of valid trips to retry sampling if some trips are invalid; *default:* **0.1** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin time; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | end time (default 3600); *default:* **3600** |
| **-p** {{DT_STR_LIST}}<br> **--period** {{DT_STR_LIST}} | Generate vehicles with equidistant departure times and period=FLOAT (default 1.0). If option --binomial is used, the expected arrival rate is set to 1/period. |
| **--insertion-rate** {{DT_STR_LIST}} | How many vehicles arrive in the simulation per hour (alternative to the period option). |
| **--insertion-density** {{DT_STR_LIST}} | How many vehicles arrive in the simulation per hour per kilometer of road (alternative to the period option). |
| **--flows** {{DT_INT}} | generates INT flows that together output vehicles with the specified period; *default:* **0** |
| **--poisson** {{DT_BOOL}} | Flows will use poisson distributed departures; *default:* **False** |
| **--random-depart** {{DT_BOOL}} | Distribute departures randomly between begin and end; *default:* **False** |
| **--binomial** {{DT_INT}} | If this is set, the number of departures per second will be drawn from a binomial distribution with n=N and p=PERIOD/N where PERIOD is the argument given to --period |

### [routeSampler.py]({{Source}}tools/routeSampler.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-r** {{DT_Routes}}<br> **--route-files** {{DT_Routes}} | Input route file; *required* |
| **-t** {{DT_FILE}}<br> **--turn-files** {{DT_FILE}} | Input turn-count file |
| **-T** {{DT_FILE}}<br> **--turn-ratio-files** {{DT_FILE}} | Input turn-ratio file |
| **-d** {{DT_FILE}}<br> **--edgedata-files** {{DT_FILE}} | Input edgeData file (for counts) |
| **-O** {{DT_FILE}}<br> **--od-files** {{DT_FILE}} | Input edgeRelation and tazRelation files for origin-destination counts |
| **--taz-files** {{DT_FILE}} | Input TAZ (district) definitions for interpreting tazRelation files |
| **--edgedata-attribute** {{DT_STR}} | Read edgeData counts from the given attribute; *default:* **entered** |
| **--arrival-attribute** {{DT_STR}} | Read arrival counts from the given edgeData file attribute |
| **--depart-attribute** {{DT_STR}} | Read departure counts from the given edgeData file attribute |
| **--turn-attribute** {{DT_STR}} | Read turning counts and origin-destination counts from the given attribute; *default:* **count** |
| **--turn-ratio-attribute** {{DT_STR}} | Read turning ratios from the given attribute; *default:* **probability** |
| **-o** {{DT_Routes}}<br> **--output-file** {{DT_Routes}} | Output route file; *default:* **out.rou.xml** |
| **--mismatch-output** {{DT_STR}} | write cout-data with overflow/underflow information to FILE |
| **--precision** {{DT_INT}} | Number of decimal digits in output; *default:* **2** |
| **--keep-stops** {{DT_BOOL}} | Preserve stops from the input routes; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-V** {{DT_BOOL}}<br> **--verbose.histograms** {{DT_BOOL}} | print histograms of edge numbers and detector passing count; *default:* **False** |
| **--verbose.timing** {{DT_BOOL}} | print time performance information; *default:* **False** |
| **--prefix** {{DT_STR}} | prefix for the vehicle ids |
| **-a** {{DT_STR}}<br> **--attributes** {{DT_STR}} | additional vehicle attributes |
| **-f** {{DT_STR}}<br> **--write-flows** {{DT_STR}} | write flows with the give style instead of vehicles [number,probability,poisson] |
| **-I** {{DT_BOOL}}<br> **--write-route-ids** {{DT_BOOL}} | write routes with ids; *default:* **False** |
| **-u** {{DT_STR}}<br> **--write-route-distribution** {{DT_STR}} | write routeDistribution with the given ID instead of individual routes |
| **--pedestrians** {{DT_BOOL}} | write person walks instead of vehicle routes; *default:* **False** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | custom begin time (seconds or H:M:S) |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | custom end time (seconds or H:M:S) |
| **-i** {{DT_STR}}<br> **--interval** {{DT_STR}} | custom aggregation interval (seconds or H:M:S) |
| **--depart-distribution** {{DT_STR}} | load list of densities that cover [begin, end] to customize departure time probabilities |
| **--timeline** {{DT_STR}} | load shares of intervals when splitting |
| **--turn-max-gap** {{DT_INT}} | Allow at most a gap of INT edges between from-edge and to-edge; *default:* **0** |
| **--total-count** {{DT_STR}} | Set a total count that should be reached (either as single value that is split proportionally among all intervals or as a list of counts per interval). Setting the value 'input' preserves input vehicle counts in each interval. |
| **--extra-od** {{DT_BOOL}} | Permit traffic between OD-pairs that did not occur in the input; *default:* **False** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **--weighted** {{DT_BOOL}} | Sample routes according to their probability (or count); *default:* **False** |
| **--optimize** {{DT_STR}} | set optimization method level (full, INT boundary) |
| **--optimize-input** {{DT_BOOL}} | Skip resampling and run optimize directly on the input routes; *default:* **False** |
| **--keep-attributes** {{DT_BOOL}} | use loaded vehicle attributes when writing used routes; *default:* **False** |
| **--init-input** {{DT_BOOL}} | use loaded routes as initialization for the used routes; *default:* **False** |
| **--init-input.remove-overflow** {{DT_BOOL}} | use loaded routes as initialization but remove those that are responsible for overflow; *default:* **False** |
| **--no-sampling** {{DT_BOOL}} | Skip sampling of routes; *default:* **False** |
| **--min-count** {{DT_INT}} | Set minimum number of counting locations that a route must visit; *default:* **1** |
| **--minimize-vehicles** {{DT_FLOAT}} | Set optimization factor from [0, 1[ for reducing the number of vehicles(prefer routes that pass multiple counting locations over routes that pass fewer); *default:* **0** |
| **--geh-ok** {{DT_FLOAT}} | threshold for acceptable GEH values; *default:* **5** |
| **--geh-scale** {{DT_FLOAT}} | Should be set to 0.1 when loading traffic for a full day (estimating peak hour traffic as 1/10 of daily traffic) |
| **--turn-ratio-total** {{DT_FLOAT}} | Set value for normalizing turning ratios (default 1); *default:* **1** |
| **--turn-ratio-tolerance** {{DT_FLOAT}} | Set tolerance for error in resulting ratios (relative to turn-ratio-total) |
| **--turn-ratio-abs-tolerance** {{DT_INT}} | Set tolerance for error in resulting turning ratios as absolute count; *default:* **1** |
| **--threads** {{DT_INT}} | If parallelization is desired, enter the number of CPUs to use. Set to a value &gt;&gt; then your machines CPUs if you want to utilize all CPUs (Default is 1); *default:* **1** |
| **--merge-strategy** {{DT_STR}} | how to merge different data points for the same location and interval; *default:* **sum** |

### [runSeeds.py]({{Source}}tools/runSeeds.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-k** {{DT_FILE}}<br> **--configuration** {{DT_FILE}} | configuration to run or comma-separated list of configurations; *required* |
| **-a** {{DT_STR}}<br> **--application** {{DT_STR}} | application to run or comma-separated list of applications |
| **-p** {{DT_STR}}<br> **--output-prefix** {{DT_STR}} | output prefix; *default:* **SEED.** |
| **--no-folders** {{DT_BOOL}} | do not create folders to distinguish multiple configurations or applications but use prefixes instead; *default:* **False** |
| **--seeds** {{DT_STR}} | which seeds to run; *default:* **0:10** |
| **--threads** {{DT_INT}} | number of parallel processes; *default:* **1** |

### [stateReplay.py]({{Source}}tools/stateReplay.py)

| Option | Description |
|--------|-------------|
| **--sumo-config** {{DT_STR}} | sumo config file; *default:* **sumo.sumocfg** |
| **--state-prefix** {{DT_STR}} | prefix for synchronized state files; *default:* **state** |
| **--src** {{DT_STR}} | the remote directory to sync |
| **--dst** {{DT_STR}} | the subdirectory for the synced files; *default:* **states** |
| **--delay** {{DT_FLOAT}} | the delay between simulation states; *default:* **1** |
| **--iterations** {{DT_INT}} | the number of iterations to run (mainly useful for testing) |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [tileGet.py]({{Source}}tools/tileGet.py)

| Option | Description |
|--------|-------------|
| **-p** {{DT_STR}}<br> **--prefix** {{DT_STR}} | for output file; *default:* **tile** |
| **-b** {{DT_STR}}<br> **--bbox** {{DT_STR}} | bounding box to retrieve in geo coordinates west,south,east,north |
| **-t** {{DT_INT}}<br> **--tiles** {{DT_INT}} | maximum number of tiles the output gets split into; *default:* **1** |
| **-d** {{DT_STR}}<br> **--output-dir** {{DT_STR}} | optional output directory (must already exist); *default:* **.** |
| **-s** {{DT_STR}}<br> **--decals-file** {{DT_STR}} | name of decals settings file; *default:* **settings.xml** |
| **-l** {{DT_INT}}<br> **--layer** {{DT_INT}} | (int) layer at which the image will appear, default 0; *default:* **0** |
| **-x** {{DT_STR}}<br> **--polygon** {{DT_STR}} | calculate bounding box from polygon data in file |
| **-n** {{DT_STR}}<br> **--net** {{DT_STR}} | get bounding box from net file |
| **-k** {{DT_STR}}<br> **--key** {{DT_STR}} | API key to use |
| **-m** {{DT_STR}}<br> **--maptype** {{DT_STR}} | map type (roadmap, satellite, hybrid, terrain); *default:* **satellite** |
| **-u** {{DT_STR}}<br> **--url** {{DT_STR}} | Download from the given tile server; *default:* **arcgis** |
| **-a** {{DT_STR}}<br> **--user-agent** {{DT_STR}} | user agent string to be used when downloading tiles |
| **-f** {{DT_INT}}<br> **--min-file-size** {{DT_INT}} | maximum number of tiles the output gets split into; *default:* **3000** |
| **--simulate** {{DT_BOOL}} | print download urls and filenames instead of requesting from tile server; *default:* **False** |
| **-z** {{DT_INT}}<br> **--max-zoom** {{DT_INT}} | restrict maximum zoom level; *default:* **19** |
| **--zoom** {{DT_INT}} | use this zoom level directly (overrides --tiles based selection, capped by --max-zoom) |
| **--resolution** {{DT_FLOAT}} | target resolution in meters per pixel; the closest zoom level is used (overrides --tiles based selection, capped by --max-zoom) |
| **-j** {{DT_INT}}<br> **--parallel-jobs** {{DT_INT}} | Number of parallel jobs to run when downloading tiles. 0 means no parallelism.; *default:* **0** |
| **-r** {{DT_BOOL}}<br> **--retina** {{DT_BOOL}} | set 'true' for double resolution tiles (applies to cartodb only).; *default:* **False** |
| **--reproject** {{DT_BOOL}} | reproject tiles from EPSG:3857 (Web Mercator) to the network projection; *default:* **False** |
| **--crop-margin** {{DT_STR}} | when using --reproject, shrink the output image by this many meters on each side (either one value or west,south,east,north) |
| **--background-factor** {{DT_FLOAT}} | reduce saturation, brightness and contrast of the reprojected image by this factor (requires Pillow); 1.0 is unchanged, 0.8 looks fine.; *default:* **1.0** |

### [tlsCoordinator.py]({{Source}}tools/tlsCoordinator.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the net file (mandatory); *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output filename; *default:* **tlsOffsets.add.xml** |
| **-r** {{DT_Routes}}<br> **--route-file** {{DT_Routes}} | define the input route file (mandatory); *required* |
| **-a** {{DT_FILE}}<br> **--additional-file** {{DT_FILE}} | define replacement tls plans to be coordinated |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--ignore-priority** {{DT_BOOL}} | ignore road priority when sorting TLS pairs; *default:* **False** |
| **--speed-factor** {{DT_FLOAT}} | avg ratio of vehicle speed in relation to the speed limit; *default:* **0.8** |
| **-e** {{DT_BOOL}}<br> **--evaluate** {{DT_BOOL}} | run the scenario and print duration statistics; *default:* **False** |

### [tlsCycleAdaptation.py]({{Source}}tools/tlsCycleAdaptation.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | define the net file (mandatory); *required* |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | define the output filename; *default:* **tlsAdaptation.add.xml** |
| **-r** {{DT_STR}}<br> **--route-files** {{DT_STR}} | define the route file separated by comma (mandatory); *required* |
| **-b** {{DT_FLOAT}}<br> **--begin** {{DT_FLOAT}} | begin time of the optimization period with unit second (mandatory) |
| **-y** {{DT_INT}}<br> **--yellow-time** {{DT_INT}} | yellow time; *default:* **4** |
| **-a** {{DT_INT}}<br> **--all-red** {{DT_INT}} | all-red time per cycle; *default:* **0** |
| **-l** {{DT_INT}}<br> **--lost-time** {{DT_INT}} | lost time for start-up and clearance in each phase; *default:* **4** |
| **-g** {{DT_INT}}<br> **--min-green** {{DT_INT}} | minimal green time when there is no traffic volume; *default:* **4** |
| **--green-filter-time** {{DT_INT}} | when computing critical flows, do not count phases with a green time below INT; *default:* **0** |
| **--min-cycle** {{DT_INT}} | minimal cycle length; *default:* **20** |
| **--max-cycle** {{DT_INT}} | maximal cycle length; *default:* **120** |
| **-e** {{DT_BOOL}}<br> **--existing-cycle** {{DT_BOOL}} | use the existing cycle length; *default:* **False** |
| **--write-critical-flows** {{DT_BOOL}} | print critical flows for each tls and phase; *default:* **False** |
| **-p** {{DT_STR}}<br> **--program** {{DT_STR}} | save new definitions with this program id; *default:* **a** |
| **-H** {{DT_FLOAT}}<br> **--saturation-headway** {{DT_FLOAT}} | saturation headway in seconds for calculating hourly saturation flows; *default:* **2** |
| **-R** {{DT_BOOL}}<br> **--restrict-cyclelength** {{DT_BOOL}} | restrict the max. cycle length as the given one; *default:* **False** |
| **-u** {{DT_BOOL}}<br> **--unified-cycle** {{DT_BOOL}} | use the calculated max cycle length as the cycle length for all intersections; *default:* **False** |
| **--sorted** {{DT_BOOL}} | assume the route file is sorted (aborts reading earlier); *default:* **False** |
| **--skip** {{DT_STR}} | the tls ids, which are skipped and seperated by comma |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [traceExporter.py]({{Source}}tools/traceExporter.py)

Converts the given fcd file (generated by sumo --fcd-output) into the selected output format. Optionally the output can be sampled, filtered and distorted.

| Option | Description |
|--------|-------------|
| **-i** {{DT_STR}}<br> **--fcd-input** {{DT_STR}} | Defines the FCD-output file to use as input (numeric value is interpreted as port to listen on); *required* |
| **-n** {{DT_STR}}<br> **--net-input** {{DT_STR}} | Defines the network file to use as input |
| **-p** {{DT_FLOAT}}<br> **--penetration** {{DT_FLOAT}} | Defines the percentage (0-1) of vehicles to export; *default:* **1.0** |
| **-b** {{DT_FLOAT}}<br> **--begin** {{DT_FLOAT}} | Defines the first step to export |
| **-e** {{DT_FLOAT}}<br> **--end** {{DT_FLOAT}} | Defines the first step not longer to export |
| **-d** {{DT_FLOAT}}<br> **--delta-t** {{DT_FLOAT}} | Defines the export step length |
| **--gps-blur** {{DT_FLOAT}} | Defines the GPS blur; *default:* **0** |
| **--boundary** {{DT_STR}} | Defines the bounding box as 'xmin,ymin,xmax,ymax' |
| **-s** {{DT_FLOAT}}<br> **--seed** {{DT_FLOAT}} | Defines the randomizer seed; *default:* **42** |
| **--base-date** {{DT_INT}} | Defines the base date; *default:* **-1** |
| **--orig-ids** {{DT_BOOL}} | Write original vehicle IDs instead of a running index; *default:* **False** |
| **--ignore-gaps** {{DT_BOOL}} | Ignore steps where a vehicle is not in the network; *default:* **False** |
| **--persons** {{DT_BOOL}} | Include person data; *default:* **False** |
| **--dri-output** {{DT_STR}} | Defines the name of the PHEM .dri-file to generate |
| **--str-output** {{DT_STR}} | Defines the name of the PHEM .str-file to generate |
| **--fzp-output** {{DT_STR}} | Defines the name of the PHEM .fzp-file to generate |
| **--flt-output** {{DT_STR}} | Defines the name of the PHEM .flt-file to generate |
| **--omnet-output** {{DT_STR}} | Defines the name of the OMNET file to generate |
| **--shawn-output** {{DT_STR}} | Defines the name of the Shawn file to generate |
| **--ns2activity-output** {{DT_STR}} | Defines the name of the ns2 file to generate |
| **--ns2config-output** {{DT_STR}} | Defines the name of the ns2 file to generate |
| **--ns2mobility-output** {{DT_STR}} | Defines the name of the ns2 file to generate |
| **--gpsdat-output** {{DT_STR}} | Defines the name of the gpsdat file to generate |
| **--kml-output** {{DT_STR}} | Defines the name of the kml file to generate |
| **--gpx-output** {{DT_STR}} | Defines the name of the gpx file to generate |
| **--poi-output** {{DT_STR}} | Defines the name of the poi file to generate |
| **--fcd-filter** {{DT_STR}} | Defines the name of the filter definition file |
| **--keplerjson-output** {{DT_STR}} | Defines the name of the kelper.gl JSON file to generate |
| **--fcd-filter-comment** {{DT_STR}} | Extra comments to include in fcd file |
| **--fcd-filter-type** {{DT_STR}} | vehicle type to include in fcd file |
| **--shift** {{DT_STR}} | shift coordinates to positive values only |
| **--ipg-output** {{DT_STR}} | Defines the name of the ipg trace file to generate |
| **--trj-output** {{DT_STR}} | Defines the name of the trj file to generate |
| **--trj-veh-width** {{DT_FLOAT}} | Defines the assumed vehicle width; *default:* **1.7** |
| **--trj-veh-length** {{DT_FLOAT}} | Defines the assumed vehicle length; *default:* **4.8** |
| **--timestep** {{DT_FLOAT}} | Used time step duration; *default:* **1.0** |

## `tools/assign`

### [costFunctionChecker.py]({{Source}}tools/assign/costFunctionChecker.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **--continue-on-unbuild** {{DT_BOOL}} | continues on unbuild routes; *default:* **False** |
| **-w** {{DT_BOOL}}<br> **--disable-warnings** {{DT_BOOL}} | disables warnings; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | SUMO network (mandatory) |
| **-t** {{DT_Routes}}<br> **--trips** {{DT_Routes}} | trips in step 0 (this or flows is mandatory) |
| **-F** {{DT_Routes}}<br> **--flows** {{DT_Routes}} | flows in step 0 (this or trips is mandatory) |
| **-+** {{DT_FILE}}<br> **--additional** {{DT_FILE}} | Additional files |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | Set simulation/routing begin [default: 0] |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | Set simulation/routing end [default: None] |
| **-R** {{DT_INT}}<br> **--route-steps** {{DT_INT}} | Set simulation route steps [default: 200] |
| **-a** {{DT_TIME}}<br> **--aggregation** {{DT_TIME}} | Set main weights aggregation period [default: 900] |
| **-A** {{DT_FLOAT}}<br> **--gA** {{DT_FLOAT}} | Sets Gawron's Alpha [default: 0.5] |
| **-B** {{DT_FLOAT}}<br> **--gBeta** {{DT_FLOAT}} | Sets Gawron's Beta [default: 0.9] |
| **-f** {{DT_INT}}<br> **--first-step** {{DT_INT}} | First DUA step [default: 0] |
| **-l** {{DT_INT}}<br> **--last-step** {{DT_INT}} | Last DUA step [default: 50] |
| **-p** {{DT_STR}}<br> **--path** {{DT_STR}} | Path to binaries |
| **-y** {{DT_BOOL}}<br> **--absrand** {{DT_BOOL}} | use current time to generate random number; *default:* **False** |
| **--cost-function** {{DT_STR}} | (python) function to use as cost function; *default:* **identity** |

### [duaIterate.py]({{Source}}tools/assign/duaIterate.py)

Any options of the form sumo--long-option-name will be passed to sumo. These must be given after all the other options example: sumo--step-length 0.5 will add the option --step-length 0.5 to sumo.

| Option | Description |
|--------|-------------|
| **-w** {{DT_BOOL}}<br> **--disable-warnings** {{DT_BOOL}} | disables warnings; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | SUMO network (mandatory) |
| **-+** {{DT_FILE}}<br> **--additional** {{DT_FILE}} | Additional files |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | Set simulation/routing begin; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | Set simulation/routing end |
| **-R** {{DT_INT}}<br> **--route-steps** {{DT_INT}} | Set simulation route steps; *default:* **200** |
| **-a** {{DT_TIME}}<br> **--aggregation** {{DT_TIME}} | Set main weights aggregation period; *default:* **900** |
| **-m** {{DT_BOOL}}<br> **--mesosim** {{DT_BOOL}} | Whether mesosim shall be used; *default:* **False** |
| **-p** {{DT_STR}}<br> **--path** {{DT_STR}} | Path to binaries |
| **-y** {{DT_BOOL}}<br> **--absrand** {{DT_BOOL}} | use current time to generate random number; *default:* **False** |
| **-I** {{DT_BOOL}}<br> **--nointernal-link** {{DT_BOOL}} | not to simulate internal link: true or false; *default:* **False** |
| **-j** {{DT_BOOL}}<br> **--meso-junctioncontrol** {{DT_BOOL}} | Enable mesoscopic traffic light and priority junction handling; *default:* **False** |
| **-L** {{DT_BOOL}}<br> **--meso-junctioncontrollimited** {{DT_BOOL}} | Enable mesoscopic traffic light and priority junction handling for saturated links; *default:* **False** |
| **-q** {{DT_BOOL}}<br> **--meso-multiqueue** {{DT_BOOL}} | Enable multiple queues at edge ends; *default:* **False** |
| **--meso-recheck** {{DT_INT}} | Delay before checking whether a jam is gone. (higher values can lead to a big speed increase); *default:* **0** |
| **--meso-tls-penalty** {{DT_FLOAT}} | Apply scaled time penalties when driving across tls controlled junctions |
| **--meso-minor-penalty** {{DT_INT}} | Apply fixed time penalty when driving across a minor link; do not use together with --meso-junctioncontrollimited |
| **-Q** {{DT_STR}}<br> **--eco-measure** {{DT_STR}} | define the applied eco measure, e.g. fuel, CO2, noise |
| **--eager-insert** {{DT_BOOL}} | eager insertion tests (may slow down the sim considerably); *default:* **False** |
| **--time-to-teleport** {{DT_TIME}} | Delay before blocked vehicles are teleported (negative value disables teleporting); *default:* **300** |
| **--time-to-teleport.highways** {{DT_TIME}} | Delay before blocked vehicles are teleported on wrong highway lanes; *default:* **0** |
| **--measure-vtypes** {{DT_STR}} | Restrict edgeData measurements to the given vehicle types |
| **-7** {{DT_BOOL}}<br> **--zip** {{DT_BOOL}} | zip old iterations (non-functional, kept for backwards compatibility); *default:* **False** |
| **-s** {{DT_BOOL}}<br> **--method-of-successive-average** {{DT_BOOL}} | apply the method of successive average as the swapping algorithm; *default:* **False** |
| **--continue-on-unbuild** {{DT_BOOL}} | continues on unbuild routes; *default:* **False** |
| **-t** {{DT_STR}}<br> **--trips** {{DT_STR}} | trips in step 0 (either trips, flows, or routes have to be supplied) |
| **-r** {{DT_STR}}<br> **--routes** {{DT_STR}} | routes in step 0 (either trips, flows, or routes have to be supplied) |
| **-F** {{DT_STR}}<br> **--flows** {{DT_STR}} | flows in step 0 (either trips, flows, or routes have to be supplied) |
| **-A** {{DT_FLOAT}}<br> **--gA** {{DT_FLOAT}} | Sets Gawron's Alpha; *default:* **0.5** |
| **-B** {{DT_FLOAT}}<br> **--gBeta** {{DT_FLOAT}} | Sets Gawron's Beta; *default:* **0.9** |
| **-E** {{DT_BOOL}}<br> **--disable-summary** {{DT_BOOL}} | No summaries are written by the simulation; *default:* **False** |
| **-T** {{DT_BOOL}}<br> **--disable-tripinfos** {{DT_BOOL}} | No tripinfos are written by the simulation; *default:* **False** |
| **--tripinfo-filter** {{DT_STR}} | filter tripinfo attributes |
| **--inc-start** {{DT_FLOAT}} | Start for incrementing scale; *default:* **0** |
| **--inc-max** {{DT_FLOAT}} | Maximum for incrementing scale; *default:* **1** |
| **--inc-base** {{DT_INT}} | Give the incrementation base. Negative values disable incremental scaling; *default:* **-1** |
| **--incrementation** {{DT_INT}} | Give the incrementation; *default:* **1** |
| **--time-inc** {{DT_INT}} | Give the time incrementation; *default:* **0** |
| **--max-teleport-ratio** {{DT_FLOAT}} | Hold incremental scaling constant while teleports/inserted exceeds FLOAT (negative values disable dynamic scaling); *default:* **-1** |
| **-f** {{DT_INT}}<br> **--first-step** {{DT_INT}} | First DUA step; *default:* **0** |
| **-l** {{DT_INT}}<br> **--last-step** {{DT_INT}} | Last DUA step; *default:* **50** |
| **--convergence-iterations** {{DT_INT}} | Number of iterations to use for convergence calculation; *default:* **10** |
| **--max-convergence-deviation** {{DT_FLOAT}} | Maximum relative standard deviation in travel times |
| **-D** {{DT_STR}}<br> **--districts** {{DT_STR}} | use districts as sources and targets |
| **-x** {{DT_STR}}<br> **--vehroute-file** {{DT_STR}} | choose the format of the route file; *default:* **None** |
| **-z** {{DT_BOOL}}<br> **--output-lastRoute** {{DT_BOOL}} | output the last routes; *default:* **False** |
| **-K** {{DT_BOOL}}<br> **--keep-allroutes** {{DT_BOOL}} | save routes with near zero probability; *default:* **False** |
| **--routing-algorithm** {{DT_STR}} | select the routing algorithm; *default:* **dijkstra** |
| **--max-alternatives** {{DT_STR}} | prune the number of alternatives to INT; *default:* **5** |
| **--skip-first-routing** {{DT_BOOL}} | run simulation with demands before first routing; *default:* **False** |
| **--logit** {{DT_BOOL}} | use the logit model for route choice; *default:* **False** |
| **-g** {{DT_FLOAT}}<br> **--logitbeta** {{DT_FLOAT}} | use the c-logit model for route choice; logit model when beta = 0; *default:* **0.15** |
| **-i** {{DT_FLOAT}}<br> **--logitgamma** {{DT_FLOAT}} | use the c-logit model for route choice; *default:* **1.0** |
| **-G** {{DT_FLOAT}}<br> **--logittheta** {{DT_FLOAT}} | parameter to adapt the cost unit |
| **-J** {{DT_STR}}<br> **--addweights** {{DT_STR}} | Additional weights for duarouter |
| **--convergence-steps** {{DT_INT}} | Given x, if x &gt; 0 reduce probability to change route by 1/x per step (Probabilistic Swapping (PSwap)). If x &lt; 0 set probability of rerouting to 1/step after step ,x, |
| **--addweights.once** {{DT_BOOL}} | use added weights only on the first iteration; *default:* **False** |
| **--router-verbose** {{DT_BOOL}} | let duarouter print some statistics; *default:* **False** |
| **--weight-memory** {{DT_BOOL}} | smooth edge weights across iterations; *default:* **False** |
| **--pessimism** {{DT_FLOAT}} | give traffic jams a higher weight when using option --weight-memory; *default:* **1** |
| **--clean-alt** {{DT_BOOL}} | Whether old rou.alt.xml files shall be removed; *default:* **False** |
| **--gzip** {{DT_BOOL}} | writing intermediate and resulting route files in gzipped format (the default); *default:* **True** |
| **--no-gzip** {{DT_BOOL}} | *default:* **True** |
| **--dualog** {{DT_STR}} | log file path (default 'dua.log'); *default:* **dua.log** |
| **--log** {{DT_STR}} | stdout log file path (default 'stdout.log'); *default:* **stdout.log** |
| **--marginal-cost** {{DT_BOOL}} | use marginal cost to perform system optimal traffic assignment; *default:* **False** |
| **--marginal-cost.exp** {{DT_FLOAT}} | apply the given exponent on the current traffic count when computing marginal cost; *default:* **0** |
| **remaining_args** {{DT_STR_LIST}} |  |

### [duaIterate_analysis.py]({{Source}}tools/assign/duaIterate_analysis.py)

| Option | Description |
|--------|-------------|
| **--stdout** {{DT_FILE}} | also parse the given FILE containing stdout of duaIterate |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | output prefix for plotting with gnuplot; *default:* **plot** |
| **-l** {{DT_INT}}<br> **--label-size** {{DT_INT}} | limit length of the plot label to this size; *default:* **40** |
| **--limit** {{DT_INT}} | only parse the first INT number of iterations |
| **--teleports** {{DT_STR}} | output prefix for plotting teleport-prone edges; *default:* **teleplot** |
| **--mpl** {{DT_STR}} | output prefix for matplotlib plots or SHOW for plotting to the display |
| **dualog** {{DT_FILE}} | file path to dua log file |

### [duaIterate_reroutingAnalysis.py]({{Source}}tools/assign/duaIterate_reroutingAnalysis.py)

Extract fraction of vehicles that changed their route per iteration

| Option | Description |
|--------|-------------|
| **-d** {{DT_STR}}<br> **--base-directory** {{DT_STR}} | Base directory of duaIterate run; *default:* **.** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more details for every iteration; *default:* **False** |

### [duaIterateMix.py]({{Source}}tools/assign/duaIterateMix.py)

Any options of the form sumo--long-option-name will be passed to sumo. These must be given after all the other options example: sumo--step-length 0.5 will add the option --step-length 0.5 to sumo.

| Option | Description |
|--------|-------------|
| **-w** {{DT_BOOL}}<br> **--disable-warnings** {{DT_BOOL}} | disables warnings; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | SUMO network (mandatory) |
| **-+** {{DT_FILE}}<br> **--additional** {{DT_FILE}} | Additional files |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | Set simulation/routing begin; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | Set simulation/routing end |
| **-R** {{DT_INT}}<br> **--route-steps** {{DT_INT}} | Set simulation route steps; *default:* **200** |
| **-a** {{DT_TIME}}<br> **--aggregation** {{DT_TIME}} | Set main weights aggregation period; *default:* **900** |
| **-m** {{DT_BOOL}}<br> **--mesosim** {{DT_BOOL}} | Whether mesosim shall be used; *default:* **False** |
| **-p** {{DT_STR}}<br> **--path** {{DT_STR}} | Path to binaries |
| **-y** {{DT_BOOL}}<br> **--absrand** {{DT_BOOL}} | use current time to generate random number; *default:* **False** |
| **-I** {{DT_BOOL}}<br> **--nointernal-link** {{DT_BOOL}} | not to simulate internal link: true or false; *default:* **False** |
| **-j** {{DT_BOOL}}<br> **--meso-junctioncontrol** {{DT_BOOL}} | Enable mesoscopic traffic light and priority junction handling; *default:* **False** |
| **-L** {{DT_BOOL}}<br> **--meso-junctioncontrollimited** {{DT_BOOL}} | Enable mesoscopic traffic light and priority junction handling for saturated links; *default:* **False** |
| **-q** {{DT_BOOL}}<br> **--meso-multiqueue** {{DT_BOOL}} | Enable multiple queues at edge ends; *default:* **False** |
| **--meso-recheck** {{DT_INT}} | Delay before checking whether a jam is gone. (higher values can lead to a big speed increase); *default:* **0** |
| **--meso-tls-penalty** {{DT_FLOAT}} | Apply scaled time penalties when driving across tls controlled junctions |
| **--meso-minor-penalty** {{DT_INT}} | Apply fixed time penalty when driving across a minor link; do not use together with --meso-junctioncontrollimited |
| **-Q** {{DT_STR}}<br> **--eco-measure** {{DT_STR}} | define the applied eco measure, e.g. fuel, CO2, noise |
| **--eager-insert** {{DT_BOOL}} | eager insertion tests (may slow down the sim considerably); *default:* **False** |
| **--time-to-teleport** {{DT_TIME}} | Delay before blocked vehicles are teleported (negative value disables teleporting); *default:* **300** |
| **--time-to-teleport.highways** {{DT_TIME}} | Delay before blocked vehicles are teleported on wrong highway lanes; *default:* **0** |
| **--measure-vtypes** {{DT_STR}} | Restrict edgeData measurements to the given vehicle types |
| **-7** {{DT_BOOL}}<br> **--zip** {{DT_BOOL}} | zip old iterations (non-functional, kept for backwards compatibility); *default:* **False** |
| **-s** {{DT_BOOL}}<br> **--method-of-successive-average** {{DT_BOOL}} | apply the method of successive average as the swapping algorithm; *default:* **False** |
| **--continue-on-unbuild** {{DT_BOOL}} | continues on unbuild routes; *default:* **False** |
| **-t** {{DT_Routes}}<br> **--trips** {{DT_Routes}} | trips in step 0 (either trips, flows, or routes have to be supplied) |
| **-r** {{DT_Routes}}<br> **--routes** {{DT_Routes}} | routes in step 0 (either trips, flows, or routes have to be supplied) |
| **-F** {{DT_Routes}}<br> **--flows** {{DT_Routes}} | flows in step 0 (either trips, flows, or routes have to be supplied) |
| **-A** {{DT_FLOAT}}<br> **--gA** {{DT_FLOAT}} | Sets Gawron's Alpha; *default:* **0.5** |
| **-B** {{DT_FLOAT}}<br> **--gBeta** {{DT_FLOAT}} | Sets Gawron's Beta; *default:* **0.9** |
| **-E** {{DT_BOOL}}<br> **--disable-summary** {{DT_BOOL}} | No summaries are written by the simulation; *default:* **False** |
| **-T** {{DT_BOOL}}<br> **--disable-tripinfos** {{DT_BOOL}} | No tripinfos are written by the simulation; *default:* **False** |
| **--tripinfo-filter** {{DT_STR}} | filter tripinfo attributes |
| **--inc-start** {{DT_FLOAT}} | Start for incrementing scale; *default:* **0** |
| **--inc-max** {{DT_FLOAT}} | Maximum for incrementing scale; *default:* **1** |
| **--inc-base** {{DT_INT}} | Give the incrementation base. Negative values disable incremental scaling; *default:* **-1** |
| **--incrementation** {{DT_INT}} | Give the incrementation; *default:* **1** |
| **--time-inc** {{DT_INT}} | Give the time incrementation; *default:* **0** |
| **-f** {{DT_INT}}<br> **--first-step** {{DT_INT}} | First DUA step; *default:* **0** |
| **-l** {{DT_INT}}<br> **--last-step** {{DT_INT}} | Last DUA step; *default:* **50** |
| **--convergence-iterations** {{DT_INT}} | Number of iterations to use for convergence calculation; *default:* **10** |
| **--max-convergence-deviation** {{DT_FLOAT}} | Maximum relative standard deviation in travel times |
| **-D** {{DT_FILE}}<br> **--districts** {{DT_FILE}} | use districts as sources and targets |
| **-x** {{DT_STR}}<br> **--vehroute-file** {{DT_STR}} | choose the format of the route file; *default:* **None** |
| **-z** {{DT_BOOL}}<br> **--output-lastRoute** {{DT_BOOL}} | output the last routes; *default:* **False** |
| **-K** {{DT_BOOL}}<br> **--keep-allroutes** {{DT_BOOL}} | save routes with near zero probability; *default:* **False** |
| **--routing-algorithm** {{DT_STR}} | select the routing algorithm; *default:* **dijkstra** |
| **--max-alternatives-HDV** {{DT_INT}} | prune the number of alternatives to INT for HDV; *default:* **5** |
| **--max-alternatives-CAV** {{DT_INT}} | prune the number of alternatives to INT for CAV; *default:* **5** |
| **--skip-first-routing** {{DT_BOOL}} | run simulation with demands before first routing; *default:* **False** |
| **--logit** {{DT_BOOL}} | use the logit model for route choice; *default:* **False** |
| **-g** {{DT_FLOAT}}<br> **--logitbeta** {{DT_FLOAT}} | use the c-logit model for route choice; logit model when beta = 0; *default:* **0.15** |
| **-i** {{DT_FLOAT}}<br> **--logitgamma** {{DT_FLOAT}} | use the c-logit model for route choice; *default:* **1.0** |
| **-G** {{DT_FLOAT}}<br> **--logittheta** {{DT_FLOAT}} | parameter to adapt the cost unit |
| **-J** {{DT_STR}}<br> **--addweights** {{DT_STR}} | Additional weights for duarouter |
| **--convergence-steps** {{DT_INT}} | Given x, if x &gt; 0 reduce probability to change route by 1/x per step (Probabilistic Swapping (PSwap)). If x &lt; 0 set probability of rerouting to 1/step after step ,x, |
| **--addweights.once** {{DT_BOOL}} | use added weights only on the first iteration; *default:* **False** |
| **--router-verbose** {{DT_BOOL}} | let duarouter print some statistics; *default:* **False** |
| **--weight-memory** {{DT_BOOL}} | smooth edge weights across iterations; *default:* **False** |
| **--pessimism** {{DT_FLOAT}} | give traffic jams a higher weight when using option --weight-memory; *default:* **1** |
| **--clean-alt** {{DT_BOOL}} | Whether old rou.alt.xml files shall be removed; *default:* **False** |
| **--binary** {{DT_BOOL}} | alias for --gzip; *default:* **False** |
| **--gzip** {{DT_BOOL}} | writing intermediate and resulting route files in gzipped format; *default:* **False** |
| **--dualog** {{DT_FILE}} | log file path (default 'dua.log'); *default:* **dua.log** |
| **--log** {{DT_FILE}} | stdout log file path (default 'stdout.log'); *default:* **stdout.log** |
| **--marginal-cost** {{DT_BOOL}} | use marginal cost to perform system optimal traffic assignment; *default:* **False** |
| **--marginal-cost-reverse** {{DT_BOOL}} | use with mix option for reversing the marginal cost with original travel times; *default:* **False** |
| **--mix** {{DT_BOOL}} | performing mix traffic assignment; *default:* **False** |
| **--marginal-cost.exp** {{DT_FLOAT}} | apply the given exponent on the current traffic count when computing marginal cost; *default:* **0** |
| **remaining_args** {{DT_STR_LIST}} |  |

### [one-shot.py]({{Source}}tools/assign/one-shot.py)

| Option | Description |
|--------|-------------|
| **-W** {{DT_BOOL}}<br> **--with-warnings** {{DT_BOOL}} | enables warnings; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | SUMO network (mandatory) |
| **-t** {{DT_Routes}}<br> **--trips** {{DT_Routes}} | trips in step 0 |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | Set simulation/routing begin; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | Set simulation/routing end |
| **-R** {{DT_INT}}<br> **--route-steps** {{DT_INT}} | Set simulation route steps; *default:* **200** |
| **-a** {{DT_TIME}}<br> **--aggregation** {{DT_TIME}} | Set main weights aggregation period; *default:* **900** |
| **-f** {{DT_STR}}<br> **--frequencies** {{DT_STR}} | Set the frequencies to iterate over; *default:* **-1,1800,300,15** |
| **-i** {{DT_TIME}}<br> **--adaptation-interval** {{DT_TIME}} | Set edge weight adaptation interval; *default:* **1** |
| **-E** {{DT_BOOL}}<br> **--disable-summary** {{DT_BOOL}} | No summaries are written by the simulation; *default:* **False** |
| **-T** {{DT_BOOL}}<br> **--disable-tripinfos** {{DT_BOOL}} | No tripinfos are written by the simulation; *default:* **False** |
| **-m** {{DT_BOOL}}<br> **--mesosim** {{DT_BOOL}} | Whether mesosim shall be used; *default:* **False** |
| **-w** {{DT_BOOL}}<br> **--with-taz** {{DT_BOOL}} | Whether districts shall be used; *default:* **False** |
| **-+** {{DT_FILE}}<br> **--additional** {{DT_FILE}} | Additional files |
| **-L** {{DT_BOOL}}<br> **--lastRoutes** {{DT_BOOL}} | only save the last routes in the vehroute-output; *default:* **False** |
| **-F** {{DT_FILE}}<br> **--weight-files** {{DT_FILE}} | Load edge/lane weights from FILE |
| **-A** {{DT_STR}}<br> **--routing-algorithm** {{DT_STR}} | type of routing algorithm [default: astar] |
| **-r** {{DT_STR}}<br> **--rerouting-explicit** {{DT_STR}} | define the ids of the vehicles that should be re-routed. |
| **-x** {{DT_BOOL}}<br> **--with-exittime** {{DT_BOOL}} | Write the exit times for all edges; *default:* **False** |
| **-s** {{DT_BOOL}}<br> **--route-sorted** {{DT_BOOL}} | sorts the output by departure time; *default:* **False** |
| **-p** {{DT_FILE}}<br> **--path** {{DT_FILE}} | Path to binaries |
| **--cost-modifier** {{DT_STR}} | Whether to modify link travel costs of the given routes; *default:* **None** |

## `tools/detector`

### [aggregateFlows.py]({{Source}}tools/detector/aggregateFlows.py)

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--det-file** {{DT_FILE}} | read detectors of interest from FILE |
| **flowFiles** {{DT_FILE}} | one or more flow input files |

### [edgeDataFromFlow.py]({{Source}}tools/detector/edgeDataFromFlow.py)

Convert detector flow file to edgeData format

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | read detectors from FILE |
| **-f** {{DT_FILE}}<br> **--detector-flow-file** {{DT_FILE}} | read detector flows to compare to from FILE (mandatory) |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | read network to determine detectors that provide turn counts |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | output edgeData FILE (mandatory) |
| **-t** {{DT_FILE}}<br> **--turn-output** {{DT_FILE}} | output edgeData FILE (mandatory) |
| **--id-column** {{DT_STR}} | Read detector ids from the given column; *default:* **Detector** |
| **--time-column** {{DT_STR}} | Read detector time from the given column; *default:* **Time** |
| **--time-scale** {{DT_INT}} | Interpretation of time units in seconds (default 60); *default:* **60** |
| **--time-format** {{DT_STR}} | Format string for interpreting custom time values |
| **--time-offset** {{DT_STR}} | time value to subbtract from parsed times |
| **-q** {{DT_STR}}<br> **--flow-columns** {{DT_STR}} | which columns contains flows (specified via column header); *default:* **qPKW,qLKW** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | custom begin time (minutes or H:M:S); *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | custom end time (minutes or H:M:S); *default:* **1440** |
| **-i** {{DT_TIME}}<br> **--interval** {{DT_TIME}} | custom aggregation interval (minutes or H:M:S); *default:* **1440** |
| **-s** {{DT_BOOL}}<br> **--skip-incomplete** {{DT_BOOL}} | Only write edge data if all vehicular lanes have a detector; *default:* **False** |
| **--cadyts** {{DT_BOOL}} | generate output in cadyts format; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [filterFlows.py]({{Source}}tools/detector/filterFlows.py)

| Option | Description |
|--------|-------------|
| **-f** {{DT_FILE}}<br> **--flows** {{DT_FILE}} | read detector flows from FILE(s) (mandatory) |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | filtered file |
| **-d** {{DT_STR}}<br> **--detectors** {{DT_STR}} | list of detectors to keep (defaults to *all*) |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin interval in minutes (inclusive); *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | end interval in minutes (exclusive); *default:* **1440** |

### [flowFromEdgeData.py]({{Source}}tools/detector/flowFromEdgeData.py)

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | read detectors from FILE (mandatory) |
| **-e** {{DT_FILE}}<br> **--edgedata-file** {{DT_FILE}} | read edgeData from FILE (mandatory) |
| **-f** {{DT_FILE}}<br> **--detector-flow-file** {{DT_FILE}} | read detector flows to compare to from FILE (mandatory) |
| **--flow-column** {{DT_STR}} | which column contains flows; *default:* **qPKW** |
| **-z** {{DT_BOOL}}<br> **--respect-zero** {{DT_BOOL}} | respect detectors without data (or with permanent zero) with zero flow; *default:* **False** |
| **-i** {{DT_TIME}}<br> **--interval** {{DT_TIME}} | aggregation interval in minutes; *default:* **1440** |
| **--long-names** {{DT_BOOL}} | do not use abbreviated names for detector groups; *default:* **False** |
| **--edge-names** {{DT_BOOL}} | include detector group edge name in output; *default:* **False** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin time in minutes; *default:* **0** |
| **--end** {{DT_TIME}} | end time in minutes |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | write output to file instead of printing it to console |
| **--flow-output** {{DT_FILE}} | write output in flowfile format to FILE |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [flowFromRoutes.py]({{Source}}tools/detector/flowFromRoutes.py)

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | read detectors from FILE (mandatory) |
| **-r** {{DT_Routes}}<br> **--routes** {{DT_Routes}} | read routes from FILE (mandatory) |
| **-e** {{DT_FILE}}<br> **--emitters** {{DT_FILE}} | read emitters from FILE (mandatory) |
| **-f** {{DT_FILE}}<br> **--detector-flow-file** {{DT_FILE}} | read detector flows to compare to from FILE |
| **--flow-column** {{DT_STR}} | which column contains flows; *default:* **qPKW** |
| **-z** {{DT_BOOL}}<br> **--respect-zero** {{DT_BOOL}} | respect detectors without data (or with permanent zero) with zero flow; *default:* **False** |
| **-D** {{DT_BOOL}}<br> **--dfrouter-style** {{DT_BOOL}} | emitter files in dfrouter style (explicit routes); *default:* **False** |
| **-i** {{DT_TIME}}<br> **--interval** {{DT_TIME}} | aggregation interval in minutes |
| **--long-names** {{DT_BOOL}} | do not use abbreviated names for detector groups; *default:* **False** |
| **--first-name** {{DT_BOOL}} | use first id in group as representative; *default:* **False** |
| **--edge-names** {{DT_BOOL}} | include detector group edge name in output; *default:* **False** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin time in minutes; *default:* **0** |
| **--end** {{DT_TIME}} | end time in minutes |
| **--geh** {{DT_BOOL}} | compare flows using GEH measure; *default:* **False** |
| **--geh-threshold** {{DT_FLOAT}} | report percentage of detectors below threshold; *default:* **5** |
| **--write-time** {{DT_BOOL}} | write time in output; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [flowrouter.py]({{Source}}tools/detector/flowrouter.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | read SUMO network from FILE (mandatory) |
| **-d** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | read detectors from FILE (mandatory) |
| **--revalidate-detectors** {{DT_BOOL}} | ignore source and sink information in detector file; *default:* **False** |
| **-f** {{DT_FILE}}<br> **--detector-flow-files** {{DT_FILE}} | read detector flows from FILE(s) (mandatory) |
| **--flow-column** {{DT_STR}} | which column contains flows; *default:* **qPKW** |
| **-o** {{DT_Routes}}<br> **--routes-output** {{DT_Routes}} | write routes to FILE |
| **-e** {{DT_FILE}}<br> **--emitters-output** {{DT_FILE}} | write emitters to FILE and create files per emitter (needs -o) |
| **-y** {{DT_STR}}<br> **--params** {{DT_STR}} | vehicle / flow params to use (vType, departPos etc.); *default:* **departSpeed="max" departPos="last" departLane="best"** |
| **-t** {{DT_FILE}}<br> **--trimmed-output** {{DT_FILE}} | write edges of trimmed network to FILE |
| **-p** {{DT_FILE}}<br> **--flow-poi-output** {{DT_FILE}} | write resulting flows as SUMO POIs to FILE |
| **--source-sink-output** {{DT_FILE}} | write sources and sinks in detector format to FILE |
| **-m** {{DT_FLOAT}}<br> **--min-speed** {{DT_FLOAT}} | only consider edges where the fastest lane allows at least this maxspeed (m/s); *default:* **0.0** |
| **-M** {{DT_INT}}<br> **--max-flow** {{DT_INT}} | limit the number of vehicles per lane and hour to this value |
| **--max-turn-flow** {{DT_INT}} | limit the number of vehicles per turn-around connection and hour to this value |
| **-r** {{DT_FILE}}<br> **--flow-restrictions** {{DT_FILE}} | read edge and route restrictions from FILEs (each line starts with '&lt;maxHourlyFlow&gt; ' followed by &lt;edgeID&gt; or '&lt;originEdgeID&gt; &lt;destEdgeID&gt;' or '&lt;e1&gt; &lt;e2&gt; ... &lt;en&gt;') |
| **-s** {{DT_FILE}}<br> **--synthetic-flows** {{DT_FILE}} | read artificial detector values from FILE (lines of the form '&lt;dailyFlow&gt; &lt;edgeID&gt;') |
| **--timeline** {{DT_STR}} | use time line for artificial detector values; *default:* **0.9,0.5,0.2,0.2,0.5,1.3,7.0,9.3,6.7,4.2,4.0,3.8,4.1,4.6,5.0,6.7,9.6,9.2,7.1,4.8,3.5,2.7,2.2,1.9** |
| **-D** {{DT_BOOL}}<br> **--keep-det** {{DT_BOOL}} | keep edges with detectors when deleting "slow" edges; *default:* **False** |
| **-z** {{DT_BOOL}}<br> **--respect-zero** {{DT_BOOL}} | respect detectors without data (or with permanent zero) with zero flow; *default:* **False** |
| **-l** {{DT_BOOL}}<br> **--lane-based** {{DT_BOOL}} | do not aggregate detector data and connections to edges; *default:* **False** |
| **-i** {{DT_TIME}}<br> **--interval** {{DT_TIME}} | aggregation interval in minutes |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin time in minutes |
| **--pedestrians** {{DT_BOOL}} | write pedestrian flows instead of vehicles flows; *default:* **False** |
| **--limit** {{DT_INT}} | limit the amount of flow assigned in a single step |
| **--vclass** {{DT_STR}} | only consider lanes that allow the given vehicle class (default 'passenger'); *default:* **passenger** |
| **-q** {{DT_BOOL}}<br> **--quiet** {{DT_BOOL}} | suppress warnings; *default:* **False** |
| **--random** {{DT_BOOL}} | write route distributions instead of separate flows; *default:* **False** |
| **--via-detectors** {{DT_BOOL}} | set used detectors as via-edges for generated flows; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **--debug** {{DT_BOOL}} | tell me what you are doing in high detail; *default:* **False** |

### [mapDetectors.py]({{Source}}tools/detector/mapDetectors.py)

Map detector locations to a network and write inductionLoop-definitions

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the net file (mandatory) |
| **-d** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | csv input file with detector ids and coordinates |
| **--delimiter** {{DT_STR}} | the field separator of the detector input file; *default:* **;** |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output file for generated mapped detectors |
| **--poi-output** {{DT_FILE}} | file to write the input stop coordinates to |
| **-i** {{DT_STR}}<br> **--id-column** {{DT_STR}} | Read detector ids from the given column; *default:* **id** |
| **-x** {{DT_STR}}<br> **--longitude-column** {{DT_STR}} | Read detector x-coordinate (lon) from the given column; *default:* **lon** |
| **-y** {{DT_STR}}<br> **--latitude-column** {{DT_STR}} | Read detector y-coordinate (lat) from the given column; *default:* **lat** |
| **--max-radius** {{DT_FLOAT}} | specify maximum distance error when mapping coordinates; *default:* **100** |
| **--vclass** {{DT_STR}} | only consider edges that permit the given vClass; *default:* **passenger** |
| **--det-output-file** {{DT_FILE}} | Define the output file that generated detectors shall write to; *default:* **detector.out.xml** |
| **--interval** {{DT_TIME}} | Define the aggregation interval of generated detectors; *default:* **3600** |
| **--write-params** {{DT_BOOL}} | Write additional columns as detector parameters; *default:* **False** |
| **--all-lanes** {{DT_BOOL}} | If set, an induction loop is placed on each lane of the target edges.; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [plotFlows.py]({{Source}}tools/detector/plotFlows.py)

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | read detectors from FILE (mandatory) |
| **--flow-column** {{DT_STR}} | which column contains flows; *default:* **qPKW** |
| **-z** {{DT_BOOL}}<br> **--respect-zero** {{DT_BOOL}} | respect detectors without data (or with permanent zero) with zero flow; *default:* **False** |
| **-i** {{DT_TIME}}<br> **--interval** {{DT_TIME}} | aggregation interval in minutes; *default:* **60** |
| **--long-names** {{DT_BOOL}} | do not use abbreviated names for detector groups; *default:* **False** |
| **--edge-names** {{DT_BOOL}} | include detector group edge name in output; *default:* **False** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin time in minutes; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | end time in minutes |
| **-o** {{DT_STR}}<br> **--csv-output** {{DT_STR}} | write plot data with prefix |
| **--extension** {{DT_STR}} | extension for saving plots; *default:* **png** |
| **-s** {{DT_BOOL}}<br> **--show** {{DT_BOOL}} | show plot directly; *default:* **False** |
| **-g** {{DT_STR}}<br> **--group-by** {{DT_STR}} | group detectors (all, none, type); *default:* **all** |
| **-t** {{DT_STR}}<br> **--type-filter** {{DT_STR}} | only show selected types |
| **-r** {{DT_FILE}}<br> **--reference-flow** {{DT_FILE}} | reference flow file that should not be grouped |
| **--id-filter** {{DT_STR}} | filter detector ids |
| **--single-plot** {{DT_BOOL}} | put averything in a single plot; *default:* **False** |
| **-m** {{DT_INT}}<br> **--max-files** {{DT_INT}} | limit number of input files |
| **-n** {{DT_BOOL}}<br> **--no-legend** {{DT_BOOL}} | dont draw legend; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **flowfiles** {{DT_FILE}} | read flows from FILE(s) (mandatory) |

### [routeUsage.py]({{Source}}tools/detector/routeUsage.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **--threshold** {{DT_INT}} | Output routes that are used less than the threshold value; *default:* **0** |
| **--unused-output** {{DT_FILE}} | Output route ids that are used less than the threshold value to file |
| **-r** {{DT_FILE}}<br> **--flow-restrictions** {{DT_FILE}} | Output route ids that are used more often than the threshold value given in file |
| **emitters** {{DT_FILE}} | file path to emitter file |
| **routes** {{DT_FILE}} | file path to route file |

### [validate.py]({{Source}}tools/detector/validate.py)

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--detectorfile** {{DT_FILE}} | read detector list from file |
| **-v** {{DT_FILE}}<br> **--validation** {{DT_FILE}} | read validation data from file |
| **-i** {{DT_TIME}}<br> **--interval** {{DT_TIME}} | aggregation interval in minutes (default: 15) |
| **-l** {{DT_BOOL}}<br> **--legacy** {{DT_BOOL}} | legacy style, input file is whitespace separated, detector_definition; *default:* **False** |
| **inputFlows** {{DT_FILE}} | csv file with flow input |

## `tools/district`

### [aggregateAndSplitMatrices.py]({{Source}}tools/district/aggregateAndSplitMatrices.py)

| Option | Description |
|--------|-------------|
| **-m** {{DT_FILE}}<br> **--matrix-file** {{DT_FILE}} | read OD matrix for passenger vehicles(long dist.) from FILE (mandatory); *required* |
| **-z** {{DT_FILE}}<br> **--districts-file** {{DT_FILE}} | read connecting links from FILE (mandatory); *required* |
| **-t** {{DT_FILE}}<br> **--timeSeries-file** {{DT_FILE}} | read hourly traffic demand rate from FILE |
| **-d** {{DT_STR}}<br> **--dir** {{DT_STR}} | Directory to store the output files. Default: ./input/; *default:* **./input/** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [countConnectionsInDistricts.py]({{Source}}tools/district/countConnectionsInDistricts.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | read SUMO network(s) from FILE(s) (mandatory); *required* |
| **-o** {{DT_NET}}<br> **--output** {{DT_NET}} | read SUMO network(s) from FILE(s) (mandatory); *required* |

### [districtMapper.py]({{Source}}tools/district/districtMapper.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-1** {{DT_NET}}<br> **--net-file1** {{DT_NET}} | read first SUMO network from FILE (mandatory); *required* |
| **-2** {{DT_NET}}<br> **--net-file2** {{DT_NET}} | read second SUMO network from FILE (mandatory); *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | write resulting districts to FILE (default: districts.add.xml) |
| **-p** {{DT_FILE}}<br> **--polyoutput** {{DT_FILE}} | write districts as polygons to FILE |
| **-a** {{DT_STR}}<br> **--junctions1** {{DT_STR}} | list of junction ids to use from first network (mandatory); *required* |
| **-b** {{DT_STR}}<br> **--junctions2** {{DT_STR}} | list of junction ids to use from second network (mandatory); *required* |
| **--color** {{DT_STR}} | Assign this color to districts (default: 1,0,0) |

### [filterDistricts.py]({{Source}}tools/district/filterDistricts.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | the network to read lane and edge permissions; *required* |
| **-t** {{DT_FILE}}<br> **--taz-file** {{DT_FILE}} | the district file to be filtered; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | write filtered districts to FILE (default: taz_filtered.add.xml) |
| **--vclass** {{DT_STR}} | filter taz edges that allow the given vehicle class |
| **--min-connections** {{DT_INT}} | Minimum number of successors for sources and predecessors for sinks; *default:* **0** |
| **--remove-ids** {{DT_STR}} | Remove the given ids from all TAZ |
| **--remove-ids-file** {{DT_FILE}} | Remove ids listed in FILE from all TAZ |

### [gridDistricts.py]({{Source}}tools/district/gridDistricts.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | the network to read lane and edge permissions; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | output taz file; *required* |
| **-w** {{DT_FLOAT}}<br> **--grid-width** {{DT_FLOAT}} | width of gride cells in m; *default:* **100.0** |
| **--vclass** {{DT_STR}} | Include only edges allowing VCLASS |
| **-u** {{DT_STR}}<br> **--hue** {{DT_STR}} | hue for taz (float from [0,1] or 'random'); *default:* **random** |
| **-s** {{DT_STR}}<br> **--saturation** {{DT_STR}} | saturation for taz (float from [0,1] or 'random'); *default:* **1** |
| **-b** {{DT_STR}}<br> **--brightness** {{DT_STR}} | brightness for taz (float from [0,1] or 'random'); *default:* **1** |
| **--seed** {{DT_INT}} | random seed; *default:* **42** |

### [stationDistricts.py]({{Source}}tools/district/stationDistricts.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | the network to read lane and edge permissions; *required* |
| **-s** {{DT_FILE}}<br> **--stop-file** {{DT_FILE}} | the additional file with stops; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | output taz file; *required* |
| **--split-output** {{DT_FILE}} | generate splits for edges assigned to multiple stations |
| **--poi-output** {{DT_FILE}} | generate a point of interest for every station |
| **--vclasses** {{DT_STR}} | Include only edges allowing VCLASS; *default:* **rail,rail_urban,subway** |
| **--parallel-radius** {{DT_FLOAT}} | search radius for finding parallel edges; *default:* **100** |
| **--merge** {{DT_BOOL}} | merge stations that have a common edge; *default:* **False** |
| **--hue** {{DT_STR}} | hue for taz (float from [0,1] or 'random'); *default:* **random** |
| **--saturation** {{DT_STR}} | saturation for taz (float from [0,1] or 'random'); *default:* **1** |
| **--brightness** {{DT_STR}} | brightness for taz (float from [0,1] or 'random'); *default:* **1** |
| **--seed** {{DT_INT}} | random seed; *default:* **42** |

### [tazRel2POI.py]({{Source}}tools/district/tazRel2POI.py)

Generate trips between random locations

| Option | Description |
|--------|-------------|
| **-t** {{DT_FILE}}<br> **--taz-file** {{DT_FILE}} | define taz file to be loaded; *required* |
| **-d** {{DT_FILE}}<br> **--taz-relation-file** {{DT_FILE}} | define taz file to be loaded; *required* |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define net file for determining taz centers |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output poi filename; *required* |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | define the tazRel attribute to read; *default:* **count** |
| **-s** {{DT_STR}}<br> **--size-metric** {{DT_STR}} | define the metric to visualize ('in', 'out', 'all'); *default:* **all** |
| **-r** {{DT_STR}}<br> **--reference-taz** {{DT_STR}} | Only include relations that start or end at the given taz id |

## `tools/drt`

### [drtOnline.py]({{Source}}tools/drt/drtOnline.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--network** {{DT_NET}} | SUMO network file |
| **-r** {{DT_FILE}}<br> **--reservations** {{DT_FILE}} | File with reservations (persons) |
| **--taxis** {{DT_FILE}} | File with drt vehicles |
| **--sumocfg** {{DT_FILE}} | Sumo configuration file |
| **-s** {{DT_STR}} | Run with sumo (default) or sumo-gui; *default:* **sumo** |
| **-g** {{DT_NET}} | Load visualization settings from FILE |
| **-o** {{DT_NET}} | Name of output file; *default:* **tripinfos.xml** |
| **--darp-solver** {{DT_STR}} | Method to solve the DARP problem. Available: exhaustive_search and simple_rerouting; *default:* **exhaustive_search** |
| **--rtv-time** {{DT_FLOAT}} | Timeout for exhaustive search (default 5 seconds); *default:* **5** |
| **--ilp-time** {{DT_FLOAT}} | Timeout for ILP solver (default 5 seconds); *default:* **5** |
| **--c-ko** {{DT_INT}} | Cost of ignoring a reservation; *default:* **1000000000** |
| **--cost-per-trip** {{DT_INT}} | Cost to avoid using multiple vehicles if the travel time of trips is similar (default 600 seconds); *default:* **600** |
| **--drf** {{DT_FLOAT}} | Factor by which the DRT travel time should not exceed the one of a direct connection (default 2); *default:* **2** |
| **--drf-min** {{DT_TIME}} | Minimum time difference allowed between DRT travel time and direct connection for the cases of short trips (default 600 seconds); *default:* **600** |
| **--max-wait** {{DT_TIME}} | Maximum waiting time for pickup (default 900 seconds); *default:* **900** |
| **--max-processing** {{DT_INT}} | Maximum number of attempts to process a request (default unlimited) |
| **--sim-step** {{DT_INT}} | Step time to collect new reservations (default 30 seconds); *default:* **30** |
| **--end-time** {{DT_TIME}} | Maximum simulation time to close Traci (default 90000 sec - 25h); *default:* **90000** |
| **--routing-algorithm** {{DT_STR}} | Algorithm for shortest path routing. Support: dijkstra (default), astar, CH and CHWrapper; *default:* **dijkstra** |
| **--routing-mode** {{DT_INT}} | Mode for shortest path routing. Support: 0 (default) for routing with loaded or default speeds and 1 for routing with averaged historical speeds; *default:* **0** |
| **--dua-times** {{DT_BOOL}} | Calculate travel time between edges with duarouter; *default:* **False** |
| **--tracefile** {{DT_FILE}} | log traci commands to the given FILE |
| **--tracegetters** {{DT_BOOL}} | include get-methods in tracefile; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | *default:* **False** |
| **--seed** {{DT_INT}} | Set a random seed for the ILP solver |

### [drtOrtools.py]({{Source}}tools/drt/drtOrtools.py)

| Option | Description |
|--------|-------------|
| **-s** {{DT_FILE}}<br> **--sumo-config** {{DT_FILE}} | sumo config file to run; *required* |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | time step to end simulation at |
| **-i** {{DT_TIME}}<br> **--interval** {{DT_TIME}} | dispatching interval in s; *default:* **30** |
| **-n** {{DT_BOOL}}<br> **--nogui** {{DT_BOOL}} | run the commandline version of sumo; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | print debug information; *default:* **False** |
| **-t** {{DT_TIME}}<br> **--time-limit** {{DT_TIME}} | time limit for solver in s; *default:* **10** |
| **-d** {{DT_STR}}<br> **--cost-type** {{DT_STR}} | type of costs to minimize (distance or time); *default:* **distance** |
| **-f** {{DT_FLOAT}}<br> **--drf** {{DT_FLOAT}} | direct route factor to calculate maximum cost for a single dropoff-pickup route (set to -1, if you do not need it); *default:* **1.5** |
| **-a** {{DT_BOOL}}<br> **--fix-allocation** {{DT_BOOL}} | if true: after first solution the allocation of reservations to vehiclesdoes not change anymore; *default:* **False** |
| **-w** {{DT_TIME}}<br> **--waiting-time** {{DT_TIME}} | maximum waiting time to serve a request in s; *default:* **900** |
| **-p** {{DT_STR}}<br> **--penalty-factor** {{DT_STR}} | factor on penalty for rejecting requests, must be 'dynamic' or an integer (e.g. 100000); *default:* **dynamic** |
| **--trace-file** {{DT_FILE}} | log file for TraCI debugging |

## `tools/emissions`

### [hbefa2sumo.py]({{Source}}tools/emissions/hbefa2sumo.py)

Generate cpp file and docs from HBEFA4 csv data

| Option | Description |
|--------|-------------|
| **csv** {{DT_STR}} | define the input data file; *default:* **HBEFACoefs_ms.csv.gz** |
| **-o** {{DT_STR}}<br> **--output-basename** {{DT_STR}} | define the base name of all output files; *default:* **HBEFA4** |
| **-p** {{DT_STR}}<br> **--pm-mode** {{DT_STR}} | how to handle particular matter values; *default:* **sum** |

## `tools/import/citybrain`

### [citybrain_flow.py]({{Source}}tools/import/citybrain/citybrain_flow.py)

Import citybrains traffic demand

| Option | Description |
|--------|-------------|
| **-f** {{DT_STR}}<br> **--flow-file** {{DT_STR}} | citybrains flow file to import |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | define the output sumo route filename |
| **-p** {{DT_STR}}<br> **--prefix** {{DT_STR}} | prefix generated flow ids |

### [citybrain_infostep.py]({{Source}}tools/import/citybrain/citybrain_infostep.py)

Import citybrains step info

| Option | Description |
|--------|-------------|
| **-i** {{DT_STR}}<br> **--info-file** {{DT_STR}} | citybrains step info file to import |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | define the output sumo route filename |
| **-l** {{DT_BOOL}}<br> **--lastpos** {{DT_BOOL}} | use departPos 'last' to include more vehicles; *default:* **False** |
| **--length** {{DT_STR}} | default vehicle length; *default:* **4** |
| **--mingap** {{DT_STR}} | default vehicle mingap; *default:* **1** |

### [citybrain_road.py]({{Source}}tools/import/citybrain/citybrain_road.py)

Import citybrains network

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | citybrains network file to import |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | define the output sumo network filename; *default:* **net.net.xml** |
| **-p** {{DT_STR}}<br> **--prefix** {{DT_STR}} | prefix for plain xml files; *default:* **net** |
| **-j** {{DT_STR}}<br> **--junction-type** {{DT_STR}} | the default type for junctions without traffic light; *default:* **allway_stop** |
| **-t** {{DT_STR}}<br> **--temp-network** {{DT_STR}} | intermediate network file; *default:* **tmp.net.xml** |
| **-x** {{DT_BOOL}}<br> **--ignore-connections** {{DT_BOOL}} | use connections guessed by netconvert instead of the specified connections; *default:* **False** |

## `tools/import/dxf`

### [dxf2jupedsim.py]({{Source}}tools/import/dxf/dxf2jupedsim.py)

| Option | Description |
|--------|-------------|
| **file** {{DT_FILE}} | The DXF file to read from |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | Name of the polygon output file |
| **--test** {{DT_BOOL}} | Write DXF test file and exit; *default:* **False** |
| **--walkable-layer** {{DT_STR}} | Name of the DXF layer containing walkable areas; *default:* **walkable_areas** |
| **--obstacle-layer** {{DT_STR}} | Name of the DXF layer containing obstacles; *default:* **obstacles** |
| **--walkable-color** {{DT_STR}} | Color of the polygons defining walkable areas; *default:* **179,217,255** |
| **--obstacle-color** {{DT_STR}} | Color of the polygons defining obstacles; *default:* **255,204,204** |
| **--sumo-layer** {{DT_STR}} | SUMO layer used to render polygons defining walkable areas; *default:* **0** |
| **--projection** {{DT_STR}} | EPSG code or projection string used to convert back to geocoordinates; *default:* **EPSG:32633** |

## `tools/import/gtfs`

### [gtfs2fcd.py]({{Source}}tools/import/gtfs/gtfs2fcd.py)

converts GTFS data into separate fcd traces for every distinct trip

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--region** {{DT_STR}} | define the region to process; *default:* **gtfs** |
| **--gtfs** {{DT_FILE}} | define gtfs zip file to load (mandatory) |
| **--merged-csv** {{DT_FILE}} | define csv file for loading merged data (instead of gtfs data) |
| **--merged-csv-output** {{DT_FILE}} | define csv file for saving merged GTFS data |
| **--date** {{DT_STR}} | define the day to import, format: 'YYYYMMDD' |
| **--fcd** {{DT_FILE}} | directory to write / read the generated FCD files to / from |
| **--gpsdat** {{DT_FILE}} | directory to write / read the generated gpsdat files to / from |
| **--modes** {{DT_STR}} | comma separated list of modes to import (bus, train, tram, light_rail, monorail, subway, aerialway, funicular, ferry, aeroway, taxi, trolleybus) |
| **--sbahn-is-light-rail** {{DT_BOOL}} | interpret GTFS mode 109 (S-Bahn) as light_rail instead of train (Berlin, Hamburg); *default:* **False** |
| **--vtype-output** {{DT_FILE}} | file to write the generated vehicle types to; *default:* **vtypes.xml** |
| **--write-terminals** {{DT_BOOL}} | Write vehicle parameters that describe terminal stops and times; *default:* **False** |
| **--original-lines** {{DT_BOOL}} | Do not distinguish line ids that have distinct stop sequences; *default:* **False** |
| **--join-blocks** {{DT_BOOL}} | Do not concatenate trips by block_id; *default:* **False** |
| **--coupling** {{DT_STR}} | Set procedure for handling coupled trips. Possible values: [remove] |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | write times as h:m:s; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | Defines the begin time to export; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | Defines the end time for the export; *default:* **86400** |
| **--bbox** {{DT_STR}} | define the bounding box to filter the gtfs data, format: W,S,E,N |

### [gtfs2pt.py]({{Source}}tools/import/gtfs/gtfs2pt.py)

converts GTFS data into separate fcd traces for every distinct trip

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--region** {{DT_STR}} | define the region to process; *default:* **gtfs** |
| **--gtfs** {{DT_FILE}} | define gtfs zip file to load (mandatory) |
| **--merged-csv** {{DT_FILE}} | define csv file for loading merged data (instead of gtfs data) |
| **--merged-csv-output** {{DT_FILE}} | define csv file for saving merged GTFS data |
| **--date** {{DT_STR}} | define the day to import, format: 'YYYYMMDD' |
| **--fcd** {{DT_FILE}} | directory to write / read the generated FCD files to / from |
| **--gpsdat** {{DT_FILE}} | directory to write / read the generated gpsdat files to / from |
| **--modes** {{DT_STR}} | comma separated list of modes to import (bus, train, tram, light_rail, monorail, subway, aerialway, funicular, ferry, aeroway, taxi, trolleybus) |
| **--sbahn-is-light-rail** {{DT_BOOL}} | interpret GTFS mode 109 (S-Bahn) as light_rail instead of train (Berlin, Hamburg); *default:* **False** |
| **--vtype-output** {{DT_FILE}} | file to write the generated vehicle types to; *default:* **vtypes.xml** |
| **--write-terminals** {{DT_BOOL}} | Write vehicle parameters that describe terminal stops and times; *default:* **False** |
| **--original-lines** {{DT_BOOL}} | Do not distinguish line ids that have distinct stop sequences; *default:* **False** |
| **--join-blocks** {{DT_BOOL}} | Do not concatenate trips by block_id; *default:* **False** |
| **--coupling** {{DT_STR}} | Set procedure for handling coupled trips. Possible values: [remove] |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | write times as h:m:s; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | Defines the begin time to export; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | Defines the end time for the export; *default:* **86400** |
| **--bbox** {{DT_STR}} | define the bounding box to filter the gtfs data, format: W,S,E,N |
| **-n** {{DT_NET}}<br> **--network** {{DT_NET}} | sumo network to use; *required* |
| **--route-output** {{DT_Routes}} | file to write the generated public transport vehicles to |
| **--additional-output** {{DT_FILE}} | file to write the generated public transport stops and routes to |
| **--duration** {{DT_INT}} | minimum time to wait on a stop; *default:* **10** |
| **--write-arrival** {{DT_BOOL}} | write stop arrival times (estimated from duration if GTFS gives arrival_time equal to departure_time); *default:* **False** |
| **--bus-parking** {{DT_BOOL}} | set parking to true for bus mode; *default:* **False** |
| **--bus-stop-length** {{DT_FLOAT}} | length for a bus stop; *default:* **13** |
| **--train-stop-length** {{DT_FLOAT}} | length for a train stop; *default:* **110** |
| **--tram-stop-length** {{DT_FLOAT}} | length for a tram stop; *default:* **60** |
| **--center-stops** {{DT_BOOL}} | use stop position as center not as front; *default:* **False** |
| **--overtake-right** {{DT_BOOL}} | allow right lane overtaking at bus / train stops which are not at the outermost lane; *default:* **False** |
| **--skip-access** {{DT_BOOL}} | do not create access links; *default:* **False** |
| **--access-radius** {{DT_FLOAT}} | maximum radius for finding pedestrian access; *default:* **100** |
| **--sort** {{DT_BOOL}} | sorting the output-file; *default:* **False** |
| **--stops** {{DT_FILE}} | files with candidate stops (selected by proxmity) |
| **--patched-stops** {{DT_FILE}} | file with replacement stops (based on stop ids) |
| **--rail-priority-factor** {{DT_FLOAT}} | Take into account edge routingType values scaled by FLOAT (routingTypes must correspond to integers in [0,4]) |
| **--radius** {{DT_FLOAT}} | maximum matching radius for candidate edges and stops; *default:* **150** |
| **--distance-penalty** {{DT_FLOAT}} | Raise the distance between mapped location and input location to FLOAT power and add as penalty when comparing path costs.Defaults to '1' when setting option --stops and '1.2' otherwise. |
| **--warn-detour-factor** {{DT_FLOAT}} | Warn about detours where path distance exceeds airline distance by factor FLOAT; *default:* **5** |
| **--remove-detour-factor** {{DT_FLOAT}} | Disable trips with implausible routes (path distance exceeds airline distance by factor FLOAT); *default:* **0** |
| **--use-gtfs-stopids** {{DT_BOOL}} | use stop identifiers from GTFS for readability; *default:* **False** |
| **--network-split** {{DT_STR}} | directory to write generated networks to |
| **--network-split-vclass** {{DT_BOOL}} | use the allowed vclass instead of the edge type to split the network (always active, option kept for backward compatibility; *default:* **True** |
| **--warn-unmapped** {{DT_BOOL}} | warn about unmapped routes; *default:* **False** |
| **--mapperlib** {{DT_STR}} | mapping library to use (obsolete) |
| **--map-output** {{DT_STR}} | directory to write the generated mapping files to |
| **--map-output-config** {{DT_FILE}} | output configuration template for the mapper library; *default:* **conf/output_configuration_template.xml** |
| **--map-input-config** {{DT_FILE}} | input configuration template for the mapper library; *default:* **conf/input_configuration_template.xml** |
| **--map-parameter** {{DT_FILE}} | parameter template for the mapper library; *default:* **conf/parameters_template.xml** |
| **--poly-output** {{DT_FILE}} | file to write the generated polygon files to |
| **--poi-output** {{DT_FILE}} | file to write the input stop coordinates to |
| **--bulk-prefix** {{DT_STR}} | Speed up routing by calling duarouter to precompute candidate routes between stops; *default:* **bulkprefix** |
| **--bulk-threads** {{DT_INT}} | Speed up routing by calling duarouter to precompute candidate routes between stops; *default:* **11** |
| **--parking-threshold** {{DT_FLOAT}} | If set, trips with consecutive stops in the same spot for more than FLOAT seconds are parked |
| **--parking-length** {{DT_FLOAT}} | If parking-threshold is set, stopping locations where vehicles park are artificially enlarged to accept more vehicles; *default:* **1000** |
| **--fill-gaps** {{DT_FLOAT}} | maximum distance between stops; *default:* **5000** |
| **--reversal-penalty** {{DT_FLOAT}} | travel time penalty for reversal; *default:* **1000** |
| **--skip-fcd** {{DT_BOOL}} | skip generating fcd data; *default:* **False** |
| **--skip-map** {{DT_BOOL}} | skip network mapping; *default:* **False** |
| **--osm-routes** {{DT_Routes}} | osm routes file |
| **--warning-output** {{DT_FILE}} | file to write the unmapped elements from gtfs |
| **--dua-repair-output** {{DT_FILE}} | file to write the osm routes with errors |
| **--repair** {{DT_BOOL}} | repair osm routes; *default:* **False** |
| **--min-stops** {{DT_INT}} | minimum number of stops a public transport line must have to be imported; *default:* **2** |
| **--maxcache** {{DT_INT}} | Set maximum cache size for route computation; *default:* **1000** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed for coloring of pois and polygons; *default:* **42** |

## `tools/import/matsim`

### [matsim_importPlans.py]({{Source}}tools/import/matsim/matsim_importPlans.py)

| Option | Description |
|--------|-------------|
| **-p** {{DT_FILE}}<br> **--plan-file** {{DT_FILE}} | MATSim plan file (mandatory); *required* |
| **-o** {{DT_Routes}}<br> **--output-file** {{DT_Routes}} | SUMO output route file (mandatory); *required* |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | SUMO net file (mandatory for --repair or --remove-loops) |
| **--vehicles-only** {{DT_BOOL}} | Import only vehicles instead of persons; *default:* **False** |
| **--no-bikes** {{DT_BOOL}} | do not import bike trips; *default:* **False** |
| **--no-rides** {{DT_BOOL}} | do not import ride trips; *default:* **False** |
| **--repair** {{DT_BOOL}} | Repair routes after import (needs a SUMO net); *default:* **False** |
| **--remove-loops** {{DT_BOOL}} | Remove loops in routes after import (needs a SUMO net); *default:* **False** |
| **--prefer-coordinate** {{DT_BOOL}} | Use coordinates instead of link ids if both are given; *default:* **False** |
| **--default-start** {{DT_STR}} | default start time for the first activity; *default:* **0:0:0** |
| **--default-end** {{DT_STR}} | default end time for the last activity; *default:* **24:0:0** |
| **--default-dur** {{DT_STR}} | default duration for activities; *default:* **1:0:0** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

## `tools/import/opendrive`

### [signal_POIs_from_xodr.py]({{Source}}tools/import/opendrive/signal_POIs_from_xodr.py)

| Option | Description |
|--------|-------------|
| **xodr_file** {{DT_STR}} | file path of open drive file |
| **net_file** {{DT_STR}} | file path of net file |

## `tools/import/osm`

### [osmTaxiStop.py]({{Source}}tools/import/osm/osmTaxiStop.py)

| Option | Description |
|--------|-------------|
| **-s** {{DT_FILE}}<br> **--osm-file** {{DT_FILE}} | read OSM file from FILE (mandatory); *required* |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | read SUMO net from FILE |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | write stopping places to the output FILE; *required*; *default:* **stopping_places.add.xml** |
| **-f** {{DT_FILE}}<br> **--fleet-file** {{DT_FILE}} | write taxi fleet to the output FILE |
| **--fleet-size** {{DT_FLOAT}} | relative (0 &lt; NUM &lt; 1) or absolute number of vehicles (NUM &gt;= 1) to generate |
| **-a** {{DT_STR}}<br> **--amenity** {{DT_STR}} | amenity type to read; *default:* **taxi** |
| **-t** {{DT_STR}}<br> **--type** {{DT_STR}} | stopping place type; *default:* **chargingStation** |
| **-r** {{DT_FLOAT}}<br> **--radius** {{DT_FLOAT}} | radius for edge finding; *default:* **20.0** |
| **-l** {{DT_FLOAT}}<br> **--length** {{DT_FLOAT}} | (minimum) length of the stopping place; *default:* **20.0** |
| **--vclass** {{DT_STR}} | which vehicle class should be allowed; *default:* **passenger** |

## `tools/import/vissim`

### [convert_detectors2SUMO.py]({{Source}}tools/import/vissim/convert_detectors2SUMO.py)

detector conversion utility (VISSIM.inpx to SUMO)

| Option | Description |
|--------|-------------|
| **-V** {{DT_FILE}}<br> **--vissim-input** {{DT_FILE}} | VISSIM inpx file path; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | output file name; *required* |
| **-S** {{DT_NET}}<br> **--SUMO-net** {{DT_NET}} | SUMO net file path; *required* |

### [convert_vissimXML_flows_statRoutes.py]({{Source}}tools/import/vissim/convert_vissimXML_flows_statRoutes.py)

road network conversion utility for static route flows (VISSIM.inpx to SUMO); generates SUMO routes definition file from given inpx and derived (by netconvert) SUMO net.

| Option | Description |
|--------|-------------|
| **-o** {{DT_Routes}}<br> **--output-file** {{DT_Routes}} | output file name (default: routes.rou.xml) |
| **-V** {{DT_FILE}}<br> **--vissim-file** {{DT_FILE}} | VISSIM inpx file path; *required* |
| **-n** {{DT_NET}}<br> **--sumo-net-file** {{DT_NET}} | SUMO net file path; *required* |

### [tls_vissimXML2SUMOnet_update.py]({{Source}}tools/import/vissim/tls_vissimXML2SUMOnet_update.py)

TLS conversion utility (VISSIM.inpx to SUMO)

| Option | Description |
|--------|-------------|
| **-V** {{DT_FILE}}<br> **--vissim-input** {{DT_FILE}} | VISSIM inpx file path; *required* |
| **-S** {{DT_NET}}<br> **--SUMO-net** {{DT_NET}} | SUMO net file path; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | output file name; *required* |

### [vissim2poly.py]({{Source}}tools/import/vissim/vissim2poly.py)

| Option | Description |
|--------|-------------|
| **vissimNet** {{DT_FILE}} | provide the vissim file path |
| **outputFile** {{DT_FILE}} | provide the output file path |

### [vissim_parseBusStops.py]({{Source}}tools/import/vissim/vissim_parseBusStops.py)

| Option | Description |
|--------|-------------|
| **vissimNet** {{DT_FILE}} | provide the vissim file path |
| **outputPrefix** {{DT_STR}} | provide the output prefix |

### [vissim_parseRoutes.py]({{Source}}tools/import/vissim/vissim_parseRoutes.py)

parse routes from a VISSIM network

| Option | Description |
|--------|-------------|
| **vissimNet** {{DT_FILE}} | the VISSIM network file |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | output filename prefix; *default:* **out** |
| **-e** {{DT_STR_LIST}}<br> **--edgemap** {{DT_STR_LIST}} | mapping of edge names for renamed edges (orig1:renamed1,orig2:renamed2,...) |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |

## `tools/import/visum`

### [visum_convertEdgeTypes.py]({{Source}}tools/import/visum/visum_convertEdgeTypes.py)

Convert VISUM edge type defintions into sumo edge type

| Option | Description |
|--------|-------------|
| **visumNet** {{DT_FILE}} | the VISUM network file |
| **output** {{DT_FILE}} | the output xml file |

### [visum_convertRoutes.py]({{Source}}tools/import/visum/visum_convertRoutes.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | SUMO net file to work with; *required* |
| **-r** {{DT_FILE}}<br> **--visum-routes** {{DT_FILE}} | The VISUM-routes files to parse; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Name of the file to write; *required* |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | The begin time of the routes to generate; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | The end time (+1) of the routes to generate; *default:* **3600** |
| **-p** {{DT_STR}}<br> **--prefix** {{DT_STR}} | ID prefix to use |
| **-t** {{DT_STR}}<br> **--type** {{DT_STR}} | The type to use for vehicles |
| **-u** {{DT_BOOL}}<br> **--uniform** {{DT_BOOL}} | Whether departures shall be distributed uniform in each interval; *default:* **False** |
| **-l** {{DT_STR}}<br> **--timeline** {{DT_STR}} | Percentages over a day |
| **-a** {{DT_BOOL}}<br> **--tabs** {{DT_BOOL}} | tab separated route file; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-2** {{DT_NET}}<br> **--net2** {{DT_NET}} | immediately match routes to a second network |
| **-s** {{DT_FLOAT}}<br> **--step** {{DT_FLOAT}} | distance between successive trace points; *default:* **10** |
| **-d** {{DT_FLOAT}}<br> **--delta** {{DT_FLOAT}} | maximum distance between edge and trace points when matching to the second net; *default:* **1** |
| **-i** {{DT_BOOL}}<br> **--distribution** {{DT_BOOL}} | write route distributions only; *default:* **False** |
| **--cutoff** {{DT_INT}} | Keep only one route when less than CUTOFF vehicles drive the OD; *default:* **0** |

### [visum_convertTurnPercentages.py]({{Source}}tools/import/visum/visum_convertTurnPercentages.py)

| Option | Description |
|--------|-------------|
| **sumoNet** {{DT_NET}} | provide the SUMO net file including the path |
| **vissumTurnings** {{DT_FILE}} | provide the file containing vissum turnings |
| **output** {{DT_FILE}} | provide the output file name including the path |

### [visum_convertXMLRoutes.py]({{Source}}tools/import/visum/visum_convertXMLRoutes.py)

Import VISUM route file

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the SUMO net file (mandatory); *required* |
| **-r** {{DT_Routes}}<br> **--route-file** {{DT_Routes}} | define the input anm_route.xml file (mandatory); *required* |
| **-o** {{DT_Routes}}<br> **--output-trip-file** {{DT_Routes}} | define the output route file; *required* |
| **--scale** {{DT_FLOAT}} | Scale volume by the given value (i.e. 24 when volume denotes hourly rather than daily traffic); *default:* **1** |
| **--vclass** {{DT_STR}} | Only include routes for the given vclass |
| **-a** {{DT_STR}}<br> **--attributes** {{DT_STR}} | additional flow attributes. |
| **--trips** {{DT_BOOL}} | write unvalidated trips instead of routes; *default:* **False** |

### [visum_parseZaehlstelle.py]({{Source}}tools/import/visum/visum_parseZaehlstelle.py)

| Option | Description |
|--------|-------------|
| **sumoNet** {{DT_FILE}} | provide the SUMO net file including the path |
| **vissumNet** {{DT_FILE}} | provide the vissum net file including the path |
| **output** {{DT_FILE}} | provide the output file name including the path |

## `tools/net`

### [abstractRail.py]({{Source}}tools/net/abstractRail.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | the network to read lane and edge permissions; *required* |
| **-s** {{DT_FILE}}<br> **--stop-file** {{DT_FILE}} | the additional file with stops; *required* |
| **-a** {{DT_FILE}}<br> **--region-file** {{DT_FILE}} | Load network regions from additional file (as taz elements) |
| **-o** {{DT_FILE}}<br> **--output-prefix** {{DT_FILE}} | output prefix for patch files; *required* |
| **--split** {{DT_BOOL}} | automatically create a region file from the loaded stops, automatically split the network if needed; *default:* **False** |
| **--filter-regions** {{DT_STR}} | filter regions by name or id |
| **--main-stops** {{DT_STR}} | determine main direction from stops names or ids |
| **--keep-all** {{DT_BOOL}} | keep original regions outside the filtered regions; *default:* **False** |
| **--horizontal** {{DT_BOOL}} | output shapes roughly aligned along the horizontal; *default:* **False** |
| **--track-offset** {{DT_FLOAT}} | default distance between parallel tracks; *default:* **20** |
| **--track-length** {{DT_FLOAT}} | maximum length of track pieces; *default:* **20** |
| **--time-limit** {{DT_FLOAT}} | time limit per region |
| **--max-iter** {{DT_INT}} | maximum number of solver iterations per region |
| **--skip-large** {{DT_INT}} | skip regions require more than the given number of constraints |
| **--skip-yopt** {{DT_BOOL}} | do not optimize the track offsets; *default:* **False** |
| **--skip-building** {{DT_BOOL}} | do not call netconvert with the patch files; *default:* **False** |
| **--extra-verbose** {{DT_BOOL}} | tell me more about what you are doing; *default:* **False** |

### [buildFullGraph.py]({{Source}}tools/net/buildFullGraph.py)

| Option | Description |
|--------|-------------|
| **net** {{DT_NET}} | the network to patch |
| **-o** {{DT_NET}}<br> **--output-file** {{DT_NET}} | the output network file |
| **--vclass** {{DT_STR}} | class for which the fully connected graph should be built |
| **--speed** {{DT_FLOAT}} | speed of added edges; *default:* **27.77777777777778** |
| **--numlanes** {{DT_INT}} | number of lanes for added edges; *default:* **1** |
| **--width** {{DT_FLOAT}} | width of added edges |

### [createRoundaboutConnections.py]({{Source}}tools/net/createRoundaboutConnections.py)

Create connections in roundabout

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | Input file name; *required* |

### [cut_net.py]({{Source}}tools/net/cut_net.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--input-net-file** {{DT_NET}} | The .net.xml file to cut; *required* |
| **-j** {{DT_FILE}}<br> **--geojson** {{DT_FILE}} | GeoJSON input file; *required* |
| **-k** {{DT_STR}}<br> **--netconvert-configuration** {{DT_STR}} | Additional netconvert parameter to use |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | The network output file name |
| **-p** {{DT_FILE}}<br> **--polygon-output-file** {{DT_FILE}} | The polygon output file name |

### [generateStationEdges.py]({{Source}}tools/net/generateStationEdges.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | input network file; *required* |
| **-s** {{DT_FILE}}<br> **--stop-file** {{DT_FILE}} | input network file; *required* |
| **-o** {{DT_FILE}}<br> **--output-prefix** {{DT_FILE}} | prefix of output files; *required* |
| **-l** {{DT_FLOAT}}<br> **--length** {{DT_FLOAT}} | length of generated solitary edges; *default:* **10.0** |
| **-w** {{DT_FLOAT}}<br> **--width** {{DT_FLOAT}} | width of generated solitary edges; *default:* **8.0** |
| **-j** {{DT_BOOL}}<br> **--join-stations** {{DT_BOOL}} | Create a single edge for all stops with the same name; *default:* **False** |
| **-b** {{DT_BOOL}}<br> **--build** {{DT_BOOL}} | Build a new network and stop file; *default:* **False** |
| **-f** {{DT_FLOAT}}<br> **--access-factor** {{DT_FLOAT}} | Declare factor that computes access distance as a factor of airline-distance (default 0); *default:* **0** |
| **--access-radius** {{DT_FLOAT}} | Overrides access search distance |

### [net2geojson.py]({{Source}}tools/net/net2geojson.py)

net to geojson

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | The .net.xml file to convert; *required* |
| **-d** {{DT_FILE}}<br> **--edgedata-file** {{DT_FILE}} | Optional edgeData to include in the output |
| **-p** {{DT_FILE}}<br> **--ptline-file** {{DT_FILE}} | Optional ptline information to include in the output |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | The geojson output file name; *required* |
| **-l** {{DT_BOOL}}<br> **--lanes** {{DT_BOOL}} | Export lane geometries; *default:* **False** |
| **-e** {{DT_BOOL}}<br> **--edges** {{DT_BOOL}} | Export edge geometries; *default:* **False** |
| **--junctions** {{DT_BOOL}} | Export junction geometries; *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--internal** {{DT_BOOL}} | Export internal geometries; *default:* **False** |
| **-j** {{DT_BOOL}}<br> **--junction-coordinates** {{DT_BOOL}} | Append junction coordinates to edge shapes; *default:* **False** |
| **-b** {{DT_BOOL}}<br> **--boundary** {{DT_BOOL}} | Export boundary shapes instead of center-lines; *default:* **False** |
| **-t** {{DT_BOOL}}<br> **--traffic-lights** {{DT_BOOL}} | Export traffic light geometries; *default:* **False** |
| **--edgedata-timeline** {{DT_BOOL}} | Exports all time intervals (by default only the first is exported); *default:* **False** |
| **-x** {{DT_BOOL}}<br> **--extra-attributes** {{DT_BOOL}} | Exports extra attributes from edge and lane (such as max speed, number of lanes and allowed vehicles); *default:* **False** |

### [net2jpsgeometry.py]({{Source}}tools/net/net2jpsgeometry.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | The .net.xml file to convert |
| **-s** {{DT_STR}}<br> **--selected-objects-file** {{DT_STR}} | The txt file including the line-by-line list of objects to select |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | The JuPedSim dxf output file name |
| **-a** {{DT_BOOL}}<br> **--write-additional-file** {{DT_BOOL}} | Export boundaries and doors to XML additional file; *default:* **False** |
| **--no-exclusive-sidewalks** {{DT_BOOL}} | Choose all lanes allowing pedestrians in case of ambiguities (experimental); *default:* **False** |
| **--allow-junctions** {{DT_BOOL}} | Allow junctions as feasible pedestrian areas for JuPedSim (experimental); *default:* **False** |
| **--fixed-metadata** {{DT_BOOL}} | Write fixed metadata (creation date etc.) for stable tests; *default:* **False** |

### [net2kml.py]({{Source}}tools/net/net2kml.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | The .net.xml file to convert |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | The KML output file name |
| **-l** {{DT_BOOL}}<br> **--lanes** {{DT_BOOL}} | Export lane geometries instead of edge geometries; *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--internal** {{DT_BOOL}} | Export internal geometries; *default:* **False** |
| **--color** {{DT_STR}} | Color for normal edges; *default:* **0f0000ff** |
| **--internal-color** {{DT_STR}} | Color for internal edges; *default:* **5f0000ff** |

### [net2poly.py]({{Source}}tools/net/net2poly.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | The .net.xml file to convert; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | The polygon output file name |
| **-l** {{DT_BOOL}}<br> **--lanes** {{DT_BOOL}} | Export lane geometries instead of edge geometries; *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--internal** {{DT_BOOL}} | Export internal geometries; *default:* **False** |
| **--layer** {{DT_STR}} | Layer for normal edges; *default:* **10** |
| **--color** {{DT_STR}} | Color for normal edges; *default:* **red** |
| **--internal-color** {{DT_STR}} | Color for internal edges; *default:* **orange** |

### [net2sel.py]({{Source}}tools/net/net2sel.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | The .net.xml file to convert; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | The polygon output file name; *required* |
| **-l** {{DT_BOOL}}<br> **--lanes** {{DT_BOOL}} | Write lanes to selection instead of edges; *default:* **False** |

### [netcheck.py]({{Source}}tools/net/netcheck.py)

| Option | Description |
|--------|-------------|
| **net** {{DT_NET}} | The network file to be checked |
| **-s** {{DT_STR_LIST}}<br> **--source** {{DT_STR_LIST}} | List edges reachable from the source; *default:* **False** |
| **-d** {{DT_STR}}<br> **--destination** {{DT_STR}} | List edges which can reach the destination; *default:* **False** |
| **-w** {{DT_BOOL}}<br> **--right-of-way** {{DT_BOOL}} | Check for problems with right-of-way rules; *default:* **False** |
| **--short-tls-edges** {{DT_BOOL}} | Check for problems with right-of-way rules; *default:* **False** |
| **-o** {{DT_FILE}}<br> **--selection-output** {{DT_FILE}} | Write output to file(s) as a loadable selection |
| **--ignore-connections** {{DT_BOOL}} | Assume full connectivity at each node when computing all connected components; *default:* **False** |
| **-l** {{DT_STR}}<br> **--vclass** {{DT_STR}} | Include only edges allowing vClass |
| **--component-output** {{DT_FILE}} | Write components of disconnected network to file - not compatible with --source or --destination options |
| **-r** {{DT_FILE}}<br> **--results-output** {{DT_FILE}} | Write results summary of disconnected network to file - not compatible with --source or --destination options |
| **-t** {{DT_BOOL}}<br> **--print-types** {{DT_BOOL}} | Print edge types used in the component; *default:* **False** |

### [netdiff.py]({{Source}}tools/net/netdiff.py)

| Option | Description |
|--------|-------------|
| **source** {{DT_NET}} | original network |
| **dest** {{DT_NET}} | modified network |
| **outprefix** {{DT_FILE}} | prefix for the diff files |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **-p** {{DT_BOOL}}<br> **--use-prefix** {{DT_BOOL}} | interpret source and dest as plain-xml prefix instead of network names; *default:* **False** |
| **-d** {{DT_BOOL}}<br> **--direct** {{DT_BOOL}} | compare source and dest files directly; *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--patch-on-import** {{DT_BOOL}} | generate patch that can be applied during initial network import (exports additional connection elements); *default:* **False** |
| **--copy** {{DT_STR}} | comma-separated list of element names to copy (if they are unchanged) |
| **--ignore-attributes** {{DT_STR}} | comma-separated list of attribute names to ignore |
| **--path** {{DT_STR}} | Path to binaries |
| **--remove-plain** {{DT_BOOL}} | avoid saving plain xml files of source and destination networks; *default:* **False** |
| **-l** {{DT_BOOL}}<br> **--write-selections** {{DT_BOOL}} | Write selection files for created, deleted and changed elements; *default:* **False** |
| **-s** {{DT_BOOL}}<br> **--write-shapes** {{DT_BOOL}} | Write shape files for created, deleted and changed elements; *default:* **False** |
| **-g** {{DT_BOOL}}<br> **--plain-geo** {{DT_BOOL}} | Write geo coordinates instead of network coordinates; *default:* **False** |

### [netmatch.py]({{Source}}tools/net/netmatch.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-1** {{DT_NET}}<br> **--net1** {{DT_NET}} | first SUMO network to use (mandatory); *required* |
| **-2** {{DT_NET}}<br> **--net2** {{DT_NET}} | second SUMO network to use (mandatory); *required* |
| **-d** {{DT_FLOAT}}<br> **--delta** {{DT_FLOAT}} | maximum distance between end points; *default:* **1** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | (base) name for the output; *required* |
| **--edges1** {{DT_FILE}} | matched edges in net 1 |
| **--edges2** {{DT_FILE}} | matched edges in net 2 |

### [network_statistics.py]({{Source}}tools/net/network_statistics.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | The .net.xml file to read |
| **-w** {{DT_BOOL}}<br> **--write-html** {{DT_BOOL}} | Write statistics to HTML output file; *default:* **True** |
| **-p** {{DT_BOOL}}<br> **--show-png** {{DT_BOOL}} | Render PNG barplot with edge statistics; *default:* **True** |

### [patchRailConflicts.py]({{Source}}tools/net/patchRailConflicts.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | the network to read lane and edge permissions; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | output network file; *required* |
| **--vclass** {{DT_STR}} | the vehicle class that restricts which junctions are considered; *default:* **tram** |
| **-t** {{DT_STR}}<br> **--junction-type** {{DT_STR}} | the new junction type for rail/rail crossings; *default:* **rail_signal** |
| **-k** {{DT_STR}}<br> **--keep-junction-type** {{DT_STR}} | the new junction type for rail/rail crossings; *default:* **traffic_light,traffic_light_unregulated** |
| **-e** {{DT_FLOAT}}<br> **--end-offset** {{DT_FLOAT}} | move back the stop line from the crossing; *default:* **0** |
| **-j** {{DT_FLOAT}}<br> **--join-distance** {{DT_FLOAT}} | The distance for joining clusters which are guarded only from the outside; *default:* **200** |
| **-s** {{DT_FLOAT}}<br> **--split-offset** {{DT_FLOAT}} | If set to positive value, split edges FLOAT m before traffic lights if they are part of a rail conflict (requires repairing routes).; *default:* **0** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [patchRailPriorities.py]({{Source}}tools/net/patchRailPriorities.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | the network to read lane and edge permissions; *required* |
| **-r** {{DT_Routes}}<br> **--route-files** {{DT_Routes}} | The file to road routes from (can also be a ptline-file); *required* |
| **-s** {{DT_FILE}}<br> **--stop-file** {{DT_FILE}} | The file to road stops from |
| **-o** {{DT_NET}}<br> **--output-file** {{DT_NET}} | output network file; *required* |
| **-p** {{DT_STR}}<br> **--patchfile-prefix** {{DT_STR}} | prefix for generated patch files |
| **-O** {{DT_FILE}}<br> **--stop-output** {{DT_FILE}} | stop output file (requires stop-file) |
| **--rt-use** {{DT_STR}} | The routing type for edges that should be used (default '4'); *default:* **4** |
| **--rt-avoid** {{DT_STR}} | The routing type for edges that should be avoided (default '0'); *default:* **0** |
| **--vclass** {{DT_STR}} | only consider edges which permit the given vehicle class; *default:* **rail** |
| **--min-length** {{DT_FLOAT}} | minimum siding length; *default:* **100.0** |
| **--max-length** {{DT_FLOAT}} | maximum siding length |
| **--max-detour-factor** {{DT_FLOAT}} | Maximum factor by which the siding may be longer than the main path; *default:* **2** |
| **--min-priority** {{DT_INT}} | Minimum edge priority value to be eligible as siding |
| **-x** {{DT_INT}}<br> **--exclude-usage** {{DT_INT}} | Exclude all edges that are used at least INT times by loaded routes |
| **-X** {{DT_INT}}<br> **--exclude-reverse-usage** {{DT_INT}} | Exclude all edges that are used at least INT times in reverse by loaded routes; *default:* **1** |
| **--reversal-penalty** {{DT_FLOAT}} | Set penalty for reversals, by default sidings with reversals are forbidden; *default:* **-1** |
| **--use-left** {{DT_BOOL}} | Drive on the left instead of on the right; *default:* **False** |
| **--add-stop-signals** {{DT_BOOL}} | If a siding has a stop on the mainline, add signals in case they are missing; *default:* **False** |
| **--add-signals** {{DT_BOOL}} | Add missing signals regardless of stopping; *default:* **False** |
| **--routing-threads** {{DT_INT}} | Perform routing on multiple threads; *default:* **11** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [patchVClasses.py]({{Source}}tools/net/patchVClasses.py)

| Option | Description |
|--------|-------------|
| **net** {{DT_NET}} | the network to patch |
| **-o** {{DT_NET}}<br> **--output-file** {{DT_NET}} | the output network file |
| **-d** {{DT_STR}}<br> **--delete-classes** {{DT_STR}} | list of classes to remove from both allow and disallow (for downward compatibility), default: subway,cable_car,aircraft,wheelchair,scooter,drone,container |
| **-D** {{DT_STR}}<br> **--disallow** {{DT_STR}} | disallow the list of classes (remove from 'allow' or add to 'disallow' |

### [reduceLanes.py]({{Source}}tools/net/reduceLanes.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--network** {{DT_NET}} | sumo network to use; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | The output edge patch file name; *default:* **reduced.edg.xml** |
| **--too-short-output** {{DT_FILE}} | The output for edges which were ignored because of length |
| **--roundabout-output** {{DT_FILE}} | The output for edges which were ignored because of roundabouts |
| **--min-length** {{DT_FLOAT}} | the minimum edge length to process; *default:* **60.0** |
| **--min-lane-number** {{DT_INT}} | the minimum number of lanes to process; *default:* **1** |
| **--junction-distance** {{DT_FLOAT}} | where to perform the edge split near the junction; *default:* **20.0** |
| **--max-priority** {{DT_INT}} | the maximum priority; *default:* **13** |

### [remap_additionals.py]({{Source}}tools/net/remap_additionals.py)

Remap infrastructure from one network to another

| Option | Description |
|--------|-------------|
| **--orig-net** {{DT_NET}} | SUMO network for loading infrastructure; *required* |
| **--target-net** {{DT_NET}} | SUMO network for writing infrastructure; *required* |
| **-a** {{DT_FILE}}<br> **--additional-file** {{DT_FILE}} | File for reading infrastructure; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | File for writing infrastructure; *required* |
| **--radius** {{DT_FLOAT}} | radius for finding candidate edges; *default:* **20** |
| **--shapecut** {{DT_FLOAT}} | Shorten polygon and edge shapes to FLOAT to increase robustness of angular comparison; *default:* **40** |
| **--angle-tolerance** {{DT_FLOAT}} | Match polygons and edges if their angle differs by no more than DEGREES; *default:* **20** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [remap_attributes.py]({{Source}}tools/net/remap_attributes.py)

Remap edge attributes from one network to another

| Option | Description |
|--------|-------------|
| **--orig-net** {{DT_NET}} | SUMO network for reading attributes; *required* |
| **--target-net** {{DT_NET}} | SUMO network for receiving attributes; *required* |
| **-o** {{DT_NET}}<br> **--output-file** {{DT_NET}} | File for writing the patched network; *required* |
| **-p** {{DT_STR}}<br> **--patch-file-prefix** {{DT_STR}} | prefix for patch files; *default:* **patch** |
| **-a** {{DT_STR}}<br> **--attributes** {{DT_STR}} | the list of edge attributes that shall be transferred; *required* |
| **--osm.origid** {{DT_BOOL}} | match objects based on OSM ids (stored in params from --output.original-names); *default:* **False** |
| **--origid** {{DT_BOOL}} | match objects based on origIDs after renaming; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [remap_network.py]({{Source}}tools/net/remap_network.py)

Identify which edges in the target network correspond to edges in the original network

| Option | Description |
|--------|-------------|
| **--orig-net** {{DT_NET}} | SUMO network with edges of interest; *required* |
| **--target-net** {{DT_NET}} | SUMO network with edge ids that should be used to represent the original edges; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | File for writing mapping information; *required* |
| **-s** {{DT_FILE}}<br> **--success-output** {{DT_FILE}} | File for writing mapping success information |
| **--radius** {{DT_FLOAT}} | radius for finding candidate edges; *default:* **1.6** |
| **--min-common** {{DT_FLOAT}} | minimum common length in meters; *default:* **0.1** |
| **--filter-ids** {{DT_STR}} | only handle the given edges |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [remap_renamed.py]({{Source}}tools/net/remap_renamed.py)

Remap routes, infrastructure or selections from one network to another based on the 'origId' attribute

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | SUMO network with renamed ids; *required* |
| **-r** {{DT_Routes}}<br> **--route-file** {{DT_Routes}} | File for reading routes |
| **-a** {{DT_Routes}}<br> **--additional-file** {{DT_Routes}} | File for reading additional elements |
| **-s** {{DT_FILE}}<br> **--selection-file** {{DT_FILE}} | File for reading a selection file with edges, lanes or junctions |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | File for writing output; *required* |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [split_at_stops.py]({{Source}}tools/net/split_at_stops.py)

| Option | Description |
|--------|-------------|
| **stopfile** {{DT_STR_LIST}} | stop files to process |
| **-n** {{DT_NET}}<br> **--network** {{DT_NET}} | validate positions against this network |
| **-r** {{DT_Routes}}<br> **--routes** {{DT_Routes}} | route file to adapt |
| **--split-output** {{DT_FILE}} | split file to generate; *default:* **splits.edg.xml** |
| **-o** {{DT_NET}}<br> **--output** {{DT_NET}} | net file to generate; *default:* **net.net.xml** |
| **--stop-output** {{DT_FILE}} | stop file to generate; *default:* **stops.add.xml** |
| **--route-output** {{DT_Routes}} | route file to generate; *default:* **routes.rou.xml** |
| **--stop-type** {{DT_STR}} | which stop types to use; *default:* **trainStop** |

## `tools/output`

### [aggregateBatteryOutput.py]({{Source}}tools/output/aggregateBatteryOutput.py)

| Option | Description |
|--------|-------------|
| **-i** {{DT_FILE}}<br> **--input** {{DT_FILE}} | battery input file |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | battery merged output file |
| **-t** {{DT_TIME}}<br> **--time** {{DT_TIME}} | time to merge |

### [analyze_pedestrian_jam.py]({{Source}}tools/output/analyze_pedestrian_jam.py)

| Option | Description |
|--------|-------------|
| **logfile** {{DT_FILE}} | log file |
| **-a** {{DT_TIME}}<br> **--aggregation** {{DT_TIME}} | define the time aggregation in seconds; *default:* **3600** |
| **-p** {{DT_FILE}}<br> **--gnuplot-output** {{DT_FILE}} | define the gnuplot output file |
| **-e** {{DT_FILE}}<br> **--edgedata-output** {{DT_FILE}} | define the edgedata output file |

### [analyze_teleports.py]({{Source}}tools/output/analyze_teleports.py)

| Option | Description |
|--------|-------------|
| **logfile** {{DT_FILE}} | log file |

### [attributeCompare.py]({{Source}}tools/output/attributeCompare.py)

| Option | Description |
|--------|-------------|
| **datafiles** {{DT_FILE}} | XML files to analyze |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **-e** {{DT_STR}}<br> **--element** {{DT_STR}} | element to analyze |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to analyze |
| **-i** {{DT_STR}}<br> **--id-attribute** {{DT_STR}} | extra attribute to distinguish values |
| **-l** {{DT_STR}}<br> **--label-attribute** {{DT_STR}} | extra attribute to label values |
| **-o** {{DT_STR}}<br> **--xml-output** {{DT_STR}} | write differences to xml file |
| **-p** {{DT_INT}}<br> **--precision** {{DT_INT}} | Set output precision; *default:* **2** |

### [attributeDiff.py]({{Source}}tools/output/attributeDiff.py)

| Option | Description |
|--------|-------------|
| **old** {{DT_STR}} | the first xml file |
| **new** {{DT_STR}} | the second xml file |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **-e** {{DT_STR}}<br> **--element** {{DT_STR}} | element to analyze |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to analyze |
| **-i** {{DT_STR}}<br> **--id-attribute** {{DT_STR}} | extra attribute to distinguish values |
| **-o** {{DT_STR}}<br> **--xml-output** {{DT_STR}} | write differences to xml file |
| **-m** {{DT_STR}}<br> **--only-first-output** {{DT_STR}} | write statistic on elements that occur only in the first input file to the given FILE |
| **-M** {{DT_STR}}<br> **--only-second-output** {{DT_STR}} | write statistic on elements that occur only in the second input file to the given FILE |
| **-p** {{DT_INT}}<br> **--precision** {{DT_INT}} | Set output precision; *default:* **2** |

### [attributeStats.py]({{Source}}tools/output/attributeStats.py)

| Option | Description |
|--------|-------------|
| **datafiles** {{DT_FILE}} | XML files to analyze |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **-e** {{DT_STR}}<br> **--element** {{DT_STR}} | element to analyze |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to analyze |
| **-i** {{DT_STR}}<br> **--id-attribute** {{DT_STR}} | attribute to identify data elements; *default:* **id** |
| **-b** {{DT_FLOAT}}<br> **--binwidth** {{DT_FLOAT}} | binning width of result histogram; *default:* **50** |
| **--hist-output** {{DT_STR}} | output file for histogram (gnuplot compatible) |
| **-o** {{DT_STR}}<br> **--full-output** {{DT_STR}} | output file for full data dump |
| **-x** {{DT_FILE}}<br> **--xml-output** {{DT_FILE}} | output statistic to xml file |
| **--xml-output.flat** {{DT_BOOL}} | legacy xml output; *default:* **False** |
| **-q** {{DT_BOOL}}<br> **--fast** {{DT_BOOL}} | use fast parser (does not track missing data); *default:* **False** |
| **-p** {{DT_INT}}<br> **--precision** {{DT_INT}} | Set output precision; *default:* **2** |
| **--abs** {{DT_BOOL}} | include statistics on absolute values; *default:* **False** |
| **--sum** {{DT_BOOL}} | include sum of values; *default:* **False** |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | interpret values as times and write them as h:m:s; *default:* **False** |

### [computeCoordination.py]({{Source}}tools/output/computeCoordination.py)

| Option | Description |
|--------|-------------|
| **-f** {{DT_STR}}<br> **--fcd-file** {{DT_STR}} | Input fcd file (mandatory) |
| **-m** {{DT_FLOAT}}<br> **--min-speed** {{DT_FLOAT}} | Minimum speed to consider vehicle undelayed; *default:* **5** |
| **--filter-route** {{DT_STR}} | only consider vehicles that passed the given list of edges in order (regardless of gaps) |
| **--entry** {{DT_STR}} | detect delay after vehicles have passend one of the entry edges (defaults to the first edge of the route) |
| **--exit** {{DT_STR}} | detect delay until vehicles have passend one of the exit edges (defaults to the last edge of the route) |
| **--full-output** {{DT_STR}} | For each vehicle that applies, write the time when it entered the corridor and the time when it was first delayed (-1 for undelayed) |

### [computePassengerCounts.py]({{Source}}tools/output/computePassengerCounts.py)

Compute Stopping Place usage

| Option | Description |
|--------|-------------|
| **-s** {{DT_STR}}<br> **--stop-output-file** {{DT_STR}} | simulation stop-output file |
| **-t** {{DT_STR}}<br> **--stopping-place** {{DT_STR}} | stoppingPlace type (busStop, parkingArea...); *default:* **busStop** |
| **--csv** {{DT_BOOL}} | write in CSV format; *default:* **False** |
| **--containers** {{DT_BOOL}} | count containers instead of persons; *default:* **False** |
| **--only-changes** {{DT_BOOL}} | write output only for steps where the occupancy changes; *default:* **False** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | begin time (when writting all steps) |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | end time (when writting all steps) |

### [computeStoppingPlaceUsage.py]({{Source}}tools/output/computeStoppingPlaceUsage.py)

Compute Stopping Place usage

| Option | Description |
|--------|-------------|
| **-s** {{DT_STR}}<br> **--stop-output-file** {{DT_STR}} | simulation stop-output file |
| **-t** {{DT_STR}}<br> **--stopping-place** {{DT_STR}} | stoppingPlace type (busStop, parkingArea...); *default:* **parkingArea** |
| **--csv** {{DT_BOOL}} | write in CSV format; *default:* **False** |
| **--only-changes** {{DT_BOOL}} | write output only for steps where the occupancy changes; *default:* **False** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | begin time (when writting all steps) |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | end time (when writting all steps) |

### [countLaneChanges.py]({{Source}}tools/output/countLaneChanges.py)

| Option | Description |
|--------|-------------|
| **dumpfile** {{DT_STR}} | dump file path |

### [edgeDataDiff.py]({{Source}}tools/output/edgeDataDiff.py)

| Option | Description |
|--------|-------------|
| **orig** {{DT_STR}} | original data file |
| **new** {{DT_STR}} | modified data file |
| **out** {{DT_STR}} | diff output file |
| **--relative** {{DT_BOOL}} | write relative instead of absolute differences; *default:* **False** |
| **--geh** {{DT_BOOL}} | write geh value instead of absolute differences; *default:* **False** |
| **--geh-scale** {{DT_FLOAT}} | Should be set to 0.1 when loading traffic for a full day (estimating peak hour traffic as 1/10 of daily traffic) |
| **--undefined** {{DT_FLOAT}} | value to use if the difference is undefined; *default:* **-1001** |
| **--attributes** {{DT_STR}} | compare list of custom attributes (A1,A2,B1,B2,C1,C2,...) |
| **--no-statistics** {{DT_BOOL}} | otherwise: handle attributes starting with 'std_' as standarddeviation and calculate propagated error; *default:* **False** |

### [edgeDepartDelay.py]({{Source}}tools/output/edgeDepartDelay.py)

| Option | Description |
|--------|-------------|
| **-t** {{DT_STR}}<br> **--tripinfo-file** {{DT_STR}} | tripinfo file written by the simulation |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | the output file |

### [fcdDiff.py]({{Source}}tools/output/fcdDiff.py)

Compare route-file stop timing with stop-output

| Option | Description |
|--------|-------------|
| **old** {{DT_STR}} | the first fcd file |
| **new** {{DT_STR}} | the second fcd file |
| **-o** {{DT_STR}}<br> **--xml-output** {{DT_STR}} | xml output file |
| **--csv-output** {{DT_STR}} | write diff as csv |
| **--filter-ids** {{DT_STR}} | only include data points from the given list of ids |
| **--exclude-ids** {{DT_STR}} | skip data points from the given list of ids |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | Write time values as hour:minute:second or day:hour:minute:second rathern than seconds; *default:* **False** |
| **-t** {{DT_BOOL}}<br> **--tripid** {{DT_BOOL}} | use attr tripId for matching instead of vehicle id; *default:* **False** |
| **-i** {{DT_FLOAT}}<br> **--histogram** {{DT_FLOAT}} | histogram bin size |
| **--grouped** {{DT_BOOL}} | provide statistics grouped by id; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [generateDetectors.py]({{Source}}tools/output/generateDetectors.py)

Generate detectors on selected network edges

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | Network file to work with. Mandatory.; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | The name of the file to write the detectors to; *required* |
| **--period** {{DT_INT}} | Aggregation period in s; *default:* **60** |
| **--prefix** {{DT_STR}} | prefix for the detectors ids |
| **-r** {{DT_FILE}}<br> **--results-file** {{DT_FILE}} | The name of the file the detectors write their output into. Defaults to det.out.xml.; *default:* **det.out.xml** |
| **--relpos** {{DT_STR}} | relative detector position along the edge [0,1] or 'random'; *default:* **0.5** |
| **--probability** {{DT_FLOAT}} | build detector with the given probability ]0, 1] (lanewise); *default:* **1** |
| **--edge-probability** {{DT_FLOAT}} | build detector with the given probability ]0, 1] (edgewise); *default:* **1** |
| **-t** {{DT_STR}}<br> **--detector-type** {{DT_STR}} | one of ['inductionLoop', 'instantInductionLoop', 'laneAreaDetector'] or the corresponding shortcut ['E1', 'E1I', 'E2']; *default:* **inductionLoop** |
| **--vclass** {{DT_STR}} | only place detectors on lanes that permit the given vehicle class; *default:* **passenger** |
| **--length** {{DT_FLOAT}} | Set length for detector types that support it |
| **--next** {{DT_BOOL}} | generate nextEdges attribute and additional detectors for lanes with multiple targets; *default:* **False** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [generateMeanDataDefinitions.py]({{Source}}tools/output/generateMeanDataDefinitions.py)

| Option | Description |
|--------|-------------|
| **-d** {{DT_STR}}<br> **--detector-file** {{DT_STR}} | Input detector FILE. Mandatory.; *required* |
| **-t** {{DT_STR}}<br> **--detector-type** {{DT_STR}} | Type of detectors defined in the input. Allowed values: e1, e2, e3. Mandatory.; *required* |
| **-f** {{DT_STR}}<br> **--frequency** {{DT_STR}} | The aggregation period the values the detector collects shall be summed up. If not given, the whole time interval from begin to end is aggregated, which is the default. If specified, must be a positive integer (seconds) representing time range length. |
| **-l** {{DT_BOOL}}<br> **--lane-based-dump** {{DT_BOOL}} | Generate lane based dump instead of edge-based dump.; *default:* **True** |
| **-e** {{DT_BOOL}}<br> **--edge-based-dump** {{DT_BOOL}} | Generate edge-based dump instead of lane-based dump. This is the default.; *default:* **True** |
| **-p** {{DT_STR}}<br> **--output-suffix** {{DT_STR}} | Suffix to append to aggregated detector output. For each detector, the detector's aggregated results file with have the name build from the detector's ID and this suffix, with '.xml' extension. Defaults to -results-aggregated.; *default:* **-results-aggregated** |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | Output to write the mean data definition to. Defaults to stdout. |

### [generateTLSE2Detectors.py]({{Source}}tools/output/generateTLSE2Detectors.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--net-file** {{DT_STR}} | Network file to work with. Mandatory. |
| **-l** {{DT_INT}}<br> **--detector-length** {{DT_INT}} | Length of the detector in meters (-1 for maximal length).; *default:* **250** |
| **-d** {{DT_FLOAT}}<br> **--distance-to-TLS** {{DT_FLOAT}} | Distance of the detector to the traffic light in meters. Defaults to 0.1m.; *default:* **0.1** |
| **-f** {{DT_INT}}<br> **--frequency** {{DT_INT}} | Detector's frequency. Defaults to 60.; *default:* **60** |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | The name of the file to write the detector definitions into. Defaults to e2.add.xml.; *default:* **e2.add.xml** |
| **-r** {{DT_STR}}<br> **--results-file** {{DT_STR}} | The name of the file the detectors write their output into. Defaults to e2output.xml.; *default:* **e2output.xml** |
| **--tl-coupled** {{DT_BOOL}} | Couple output frequency to traffic light phase; *default:* **False** |

### [instantOutDiff.py]({{Source}}tools/output/instantOutDiff.py)

| Option | Description |
|--------|-------------|
| **orig** {{DT_STR}} | the first tripinfo file |
| **new** {{DT_STR}} | the second tripinfo file |
| **output** {{DT_STR}} | the output file |
| **--event-type** {{DT_STR}} | Which type of detection event to compare (enter, stay, leave); *default:* **leave** |
| **--combine-lanes** {{DT_BOOL}} | do not distinguish detectors by lane id; *default:* **False** |
| **--filter-ids** {{DT_STR}} | only use detector ids from the given list |

### [instantOutToEdgeCounts.py]({{Source}}tools/output/instantOutToEdgeCounts.py)

| Option | Description |
|--------|-------------|
| **-i** {{DT_FILE}}<br> **--detector-data** {{DT_FILE}} | the detector data to read; *required* |
| **-d** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | the detector definitions to read |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output filename |
| **--poi-output** {{DT_FILE}} | file to write the individual detector counts as poi parameters |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | begin time |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | end time |
| **-p** {{DT_TIME}}<br> **--period** {{DT_TIME}} | The interval duration for grouping counts |
| **-t** {{DT_STR}}<br> **--event-type** {{DT_STR}} | The event type to read (default 'leave'); *default:* **leave** |

### [parkingSearchTraffic.py]({{Source}}tools/output/parkingSearchTraffic.py)

| Option | Description |
|--------|-------------|
| **net** {{DT_STR}} | net file |
| **routes** {{DT_STR}} | route file |

### [scheduleStats.py]({{Source}}tools/output/scheduleStats.py)

Compare route-file stop timing with stop-output

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | Input route file |
| **-s** {{DT_STR}}<br> **--stop-file** {{DT_STR}} | Input stop-output file |
| **-o** {{DT_STR}}<br> **--xml-output** {{DT_STR}} | xml output file |
| **-t** {{DT_STR}}<br> **--statistic-type** {{DT_STR}} | Code for statistic type from dict_keys(['d', 'a', 'de', 'as', 's', 't', 'T']); *default:* **d** |
| **-g** {{DT_STR}}<br> **--group-by** {{DT_STR}} | Code for grouping results |
| **-i** {{DT_FLOAT}}<br> **--histogram** {{DT_FLOAT}} | histogram bin size |
| **-I** {{DT_FLOAT}}<br> **--group-histogram** {{DT_FLOAT}} | group histogram bin size |
| **-T** {{DT_STR}}<br> **--group-statistic-type** {{DT_STR}} | attribute for group statistic from dict_keys(['mean', 'median', 'min', 'max']); *default:* **mean** |
| **-p** {{DT_INT}}<br> **--precision** {{DT_INT}} | output precision; *default:* **1** |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | Write time values as hour:minute:second or day:hour:minute:second rathern than seconds; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [stopOrder.py]({{Source}}tools/output/stopOrder.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | Input route file |
| **-s** {{DT_STR}}<br> **--stop-file** {{DT_STR}} | Input stop-output file |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | Write time values as hour:minute:second or day:hour:minute:second rathern than seconds; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [tripinfoByTAZ.py]({{Source}}tools/output/tripinfoByTAZ.py)

| Option | Description |
|--------|-------------|
| **-t** {{DT_STR}}<br> **--tripinfo-file** {{DT_STR}} | tripinfo file written by the simulation |
| **-r** {{DT_STR}}<br> **--route-files** {{DT_STR}} | demand input (trips, flows, vehicles) for reading fromTaz and toTaz info |
| **-z** {{DT_STR}}<br> **--taz-files** {{DT_STR}} | taz definitions to assign trips to taz based on departure and arrival edge |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to use for statistics; *default:* **duration** |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | the output file |

### [tripinfoByType.py]({{Source}}tools/output/tripinfoByType.py)

| Option | Description |
|--------|-------------|
| **-t** {{DT_STR}}<br> **--tripinfo-file** {{DT_STR}} | tripinfo file written by the simulation |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to use for statistics; *default:* **duration** |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | the output file |
| **-i** {{DT_STR}}<br> **--interval** {{DT_STR}} | custom aggregation interval (seconds or H:M:S) |
| **--by-arrivals** {{DT_BOOL}} | When using --interval, aggregated by arrival time instead of depart time; *default:* **False** |

### [tripinfoDiff.py]({{Source}}tools/output/tripinfoDiff.py)

| Option | Description |
|--------|-------------|
| **orig** {{DT_STR}} | the first tripinfo file |
| **new** {{DT_STR}} | the second tripinfo file |
| **output** {{DT_STR}} | the output file |
| **--filter-ids** {{DT_STR}} | only use trip ids with the given substring |
| **--persons** {{DT_BOOL}} | compute personinfo differences; *default:* **False** |
| **--histogram-scale** {{DT_FLOAT}} | compute data histogram with the FLOAT granularity |

### [vehLanes.py]({{Source}}tools/output/vehLanes.py)

| Option | Description |
|--------|-------------|
| **netstate** {{DT_STR}} | Netstate Dump File |
| **out** {{DT_STR}} | Output file |

### [vehroute2amitranOD.py]({{Source}}tools/output/vehroute2amitranOD.py)

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--routes** {{DT_STR}} | name of the amitran route file output [default: routes.xml] |
| **-o** {{DT_STR}}<br> **--od-file** {{DT_STR}} | name of the amitran O/D file output [default: od.xml] |
| **-i** {{DT_INT}}<br> **--interval** {{DT_INT}} | aggregation interval in seconds [default: 3600] |

### [vehrouteCountValidation.py]({{Source}}tools/output/vehrouteCountValidation.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-files** {{DT_STR}} | Input route file (vehroute-output) |
| **-t** {{DT_STR}}<br> **--turn-files** {{DT_STR}} | Input turn-count file |
| **-T** {{DT_FILE}}<br> **--turn-ratio-files** {{DT_FILE}} | Input turn-ratio file |
| **-d** {{DT_STR}}<br> **--edgedata-files** {{DT_STR}} | Input edgeData file (for counts) |
| **-O** {{DT_FILE}}<br> **--od-files** {{DT_FILE}} | Input edgeRelation and tazRelation files for origin-destination counts |
| **--taz-files** {{DT_FILE}} | Input TAZ (district) definitions for interpreting tazRelation files |
| **--edgedata-attribute** {{DT_STR}} | Read edgeData counts from the given attribute; *default:* **entered** |
| **--arrival-attribute** {{DT_STR}} | Read arrival counts from the given edgeData file attribute |
| **--depart-attribute** {{DT_STR}} | Read departure counts from the given edgeData file attribute |
| **--turn-attribute** {{DT_STR}} | Read turning counts from the given attribute; *default:* **count** |
| **--turn-ratio-attribute** {{DT_STR}} | Read turning ratios from the given attribute; *default:* **probability** |
| **--turn-ratio-total** {{DT_FLOAT}} | Set value for normalizing turning ratios (default 1); *default:* **1** |
| **--turn-max-gap** {{DT_INT}} | Allow at most a gap of INT edges between from-edge and to-edge; *default:* **0** |
| **-m** {{DT_STR}}<br> **--mismatch-files** {{DT_STR}} | Input mismatch file (as computed by routeSampler.py) to compensate sampling errors |
| **--prefix** {{DT_STR}} | prefix for the vehicle ids |
| **--type** {{DT_STR}} | vehiclet type to filter by |
| **--mismatch-output** {{DT_STR}} | write count-data with overflow/underflow information to FILE; *default:* **mismatch.xml** |
| **--geh-ok** {{DT_FLOAT}} | threshold for acceptable GEH values; *default:* **5** |
| **--pedestrians** {{DT_BOOL}} | compare person walks instead of vehicle routes; *default:* **False** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | custom begin time (seconds or H:M:S) |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | custom end time (seconds or H:M:S) |
| **-i** {{DT_STR}}<br> **--interval** {{DT_STR}} | custom aggregation interval (seconds or H:M:S) |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-V** {{DT_BOOL}}<br> **--verbose.histograms** {{DT_BOOL}} | print histograms of edge numbers and detector passing count; *default:* **False** |

### [vehrouteDiff.py]({{Source}}tools/output/vehrouteDiff.py)

| Option | Description |
|--------|-------------|
| **orig** {{DT_STR}} | original routes file |
| **new** {{DT_STR}} | new routes file |
| **out** {{DT_STR}} | output file |
| **--earliest** {{DT_STR}} | write time of the first diff per vehicle to FILE |

### [walkFactor.py]({{Source}}tools/output/walkFactor.py)

| Option | Description |
|--------|-------------|
| **tripinfos** {{DT_FILE}} | Trip Info file |
| **--length** {{DT_FLOAT}} | length threshold (default: 0.1); *default:* **0.1** |

## `tools/route`

### [addParkingAreaStops2Routes.py]({{Source}}tools/route/addParkingAreaStops2Routes.py)

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | define the route file |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | output route file including parking |
| **-p** {{DT_STR}}<br> **--parking-areas** {{DT_STR}} | define the parking areas separated by comma |
| **-d** {{DT_STR}}<br> **--parking-duration** {{DT_STR}} | define the parking duration (in seconds); *default:* **3600** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [addParkingAreaStops2Trips.py]({{Source}}tools/route/addParkingAreaStops2Trips.py)

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | define the input route file with trips; *required* |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | output route file with trips with parking stops |
| **-p** {{DT_STR}}<br> **--parking-areas** {{DT_STR}} | define the parking areas separated by comma; *required* |
| **-d** {{DT_STR}}<br> **--parking-duration** {{DT_STR}} | define the parking duration (in seconds); *default:* **3600** |
| **-u** {{DT_STR}}<br> **--parking-until** {{DT_STR}} | define the parking until duration (in seconds) |
| **-l** {{DT_STR}}<br> **--parking-untilend** {{DT_STR}} | define the parking until end variable duration (in seconds) |
| **-b** {{DT_STR}}<br> **--parking-duration-begin** {{DT_STR}} | define the minimum parking duration (in seconds) |
| **-e** {{DT_STR}}<br> **--parking-duration-end** {{DT_STR}} | define the maximum parking duration (in seconds) |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **--random** {{DT_BOOL}} | use a random seed to initialize the random number generator; *default:* **False** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |

### [addStopDelay.py]({{Source}}tools/route/addStopDelay.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | Input route file |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | Output route file; *default:* **out.rou.xml** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **-p** {{DT_FLOAT}}<br> **--probability** {{DT_FLOAT}} | apply delay with the given probability ]0, 1]; *default:* **1** |
| **--min** {{DT_FLOAT}} | minimum delay value (default 0); *default:* **0** |
| **--max** {{DT_FLOAT}} | maximum delay value (default 3600); *default:* **3600** |
| **--mean** {{DT_FLOAT}} | mean delay (default 0); *default:* **0** |
| **--dev** {{DT_FLOAT}} | mean standard deviation of delay (default 60); *default:* **60** |

### [addStops2Routes.py]({{Source}}tools/route/addStops2Routes.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the net filename (mandatory); *required* |
| **-r** {{DT_Routes}}<br> **--route-files** {{DT_Routes}} | define the route files separated by comma (mandatory) |
| **-o** {{DT_Routes}}<br> **--output-file** {{DT_Routes}} | define the output filename |
| **-t** {{DT_STR}}<br> **--typesfile** {{DT_STR}} | Give a typesfile |
| **-d** {{DT_STR}}<br> **--duration** {{DT_STR}} | Define duration of vehicle stop (setting 'X-Y' picks randomly from [X,Y[) |
| **-u** {{DT_STR}}<br> **--until** {{DT_STR}} | Define end time of vehicle stop |
| **--speed** {{DT_STR}} | Define a waypoint with the given maximum speed |
| **-p** {{DT_BOOL}}<br> **--parking** {{DT_BOOL}} | Let the vehicle stop beside the road; *default:* **False** |
| **--length** {{DT_FLOAT}} | length of the stopping position |
| **--relpos** {{DT_STR}} | relative stopping position along the edge [0,1] or 'random' |
| **--edges** {{DT_STR}} | comma separated list of edges to consider for stopping |
| **--color** {{DT_STR}} | change color of stopping vehicles |
| **--lane** {{DT_STR}} | set index of stop lane or 'random' (unusable lanes are not counted); *default:* **0** |
| **--reledge** {{DT_STR}} | relative stopping position along the route [0,1] or 'random' (1 indicates the last edge); *default:* **1** |
| **--via-index** {{DT_STR}} | index of stop edge along the route (0-based, negative allowed) or 'random' |
| **--probability** {{DT_FLOAT}} | add stop with the given probability ]0, 1]; *default:* **1** |
| **--parking-areas** {{DT_FILE}} | load parkingarea definitions and stop at parkingarea on the arrival edge if possible; *default:* **False** |
| **--start-at-stop** {{DT_BOOL}} | shorten route so it starts at stop; *default:* **False** |
| **--rel-occupancy** {{DT_FLOAT}} | fill all parkingAreas to relative occupancy [0,1] |
| **--abs-occupancy** {{DT_INT}} | fill all parkingAreas to absolute occupancy; *default:* **1** |
| **--abs-free** {{DT_INT}} | fill all parkingAreas to absolute remaining capacity |
| **-D** {{DT_STR}}<br> **--person-duration** {{DT_STR}} | Define duration of person stop (setting 'X-Y' picks randomly from [X,Y[) |
| **-U** {{DT_STR}}<br> **--person-until** {{DT_STR}} | Define end time of person stop |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [addTAZ.py]({{Source}}tools/route/addTAZ.py)

Add fromTaz and toTaz to a route file

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | define the input route file (mandatory) |
| **-a** {{DT_STR}} | define the files to load TAZ (districts) from (mandatory) |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | define the output filename (mandatory) |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |

### [analyzePersonPlans.py]({{Source}}tools/route/analyzePersonPlans.py)

Analyze person plans

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-files** {{DT_STR}} | Input route files |
| **-w** {{DT_BOOL}}<br> **--merge-walks** {{DT_BOOL}} | merge subsequent walks; *default:* **False** |
| **-p** {{DT_STR}}<br> **--public-prefixes** {{DT_STR}} | Distinguish public transport modes by prefix |
| **-i** {{DT_INT}}<br> **--ids** {{DT_INT}} | List the given number of person ids for each type of plan; *default:* **0** |

### [checkReversals.py]({{Source}}tools/route/checkReversals.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--network** {{DT_NET}} | sumo network to use; *required* |
| **-r** {{DT_STR}}<br> **--route-files** {{DT_STR}} | Input route files; *required* |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | Output file for info on each reversing vehicle |

### [checkStopOrder.py]({{Source}}tools/route/checkStopOrder.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-files** {{DT_STR}} | Input route file |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | Write time values as hour:minute:second or day:hour:minute:second rathern than seconds; *default:* **False** |
| **-p** {{DT_BOOL}}<br> **--ignore-parking** {{DT_BOOL}} | Do not report conflicts with parking vehicles; *default:* **False** |
| **--until-from-duration** {{DT_BOOL}} | Use stop arrival+duration instead of 'until' to compute overtaking; *default:* **False** |
| **--filter-ids** {{DT_STR}} | only consider stops for vehicles in the given list of ids |
| **--stop-table** {{DT_STR}} | Print timetable information for the given list of busStops |

### [cutRoutes.py]({{Source}}tools/route/cutRoutes.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **--trips-output** {{DT_STR}} | output trip file |
| **--pt-input** {{DT_STR}} | read public transport flows from file |
| **--pt-output** {{DT_STR}} | write reduced public transport flows to file |
| **--min-length** {{DT_INT}} | minimum route length in the subnetwork (in #edges); *default:* **0** |
| **--min-air-dist** {{DT_FLOAT}} | minimum route length in the subnetwork (in meters); *default:* **0.0** |
| **-o** {{DT_STR}}<br> **--routes-output** {{DT_STR}} | output route file |
| **--stops-output** {{DT_STR}} | output filtered stop file |
| **-a** {{DT_STR}}<br> **--additional-input** {{DT_STR}} | additional file (for bus stop locations) |
| **--speed-factor** {{DT_FLOAT}} | Factor for modifying maximum edge speeds when extrapolating new departure times (default 1.0); *default:* **1.0** |
| **--default.stop-duration** {{DT_FLOAT}} | default duration for stops in stand-alone routes; *default:* **0.0** |
| **--default.departLane** {{DT_STR}} | default departure lane for cut routes; *default:* **best** |
| **--default.departSpeed** {{DT_STR}} | default departure speed for cut routes; *default:* **max** |
| **--orig-net** {{DT_STR}} | complete network for retrieving edge lengths |
| **-b** {{DT_BOOL}}<br> **--big** {{DT_BOOL}} | Perform out-of-memory sort using module sort_routes (slower but more memory efficient); *default:* **False** |
| **-d** {{DT_STR}}<br> **--disconnected-action** {{DT_STR}} | How to deal with routes that are disconnected in the subnetwork. If 'keep' is chosen a disconnected route generates several routes in the subnetwork corresponding to its parts.; *default:* **discard** |
| **-e** {{DT_BOOL}}<br> **--heterogeneous** {{DT_BOOL}} | this option has no effect and only exists for backward compatibility; *default:* **False** |
| **--missing-edges** {{DT_INT}} | print N most missing edges; *default:* **0** |
| **--discard-exit-times** {{DT_BOOL}} | do not use exit times; *default:* **False** |
| **network** {{DT_STR}} | Provide an input network |
| **routeFiles** {{DT_STR_LIST}} | If the given routes contain exit times, these will be used to compute new departure times |

### [cutTrips.py]({{Source}}tools/route/cutTrips.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **-o** {{DT_STR}}<br> **--trips-output** {{DT_STR}} | output trip file |
| **-a** {{DT_STR}}<br> **--additional-input** {{DT_STR}} | additional file for taz (must already be cut) |
| **-b** {{DT_BOOL}}<br> **--big** {{DT_BOOL}} | Perform out-of-memory sort using module sort_routes (slower but more memory efficient); *default:* **False** |
| **network** {{DT_STR}} | Provide an input network |
| **routeFiles** {{DT_STR_LIST}} | If the given routes contain exit times, these will be used to compute new departure times |

### [driveways2poly.py]({{Source}}tools/route/driveways2poly.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--netfile** {{DT_STR}} | name of input network file; *required* |
| **-d** {{DT_STR}}<br> **--driveways** {{DT_STR}} | name of input railsignal block file; *required* |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | name of output file; *required* |
| **-u** {{DT_STR}}<br> **--hue** {{DT_STR}} | hue for polygons (float from [0,1] or 'random'); *default:* **random** |
| **-s** {{DT_STR}}<br> **--saturation** {{DT_STR}} | saturation for polygons (float from [0,1] or 'random'); *default:* **1** |
| **-b** {{DT_STR}}<br> **--brightness** {{DT_STR}} | brightness for polygons (float from [0,1] or 'random'); *default:* **1** |
| **-l** {{DT_STR}}<br> **--layer** {{DT_STR}} | layer for generated polygons; *default:* **100** |
| **--geo** {{DT_BOOL}} | write polygons with geo-coordinates; *default:* **False** |
| **--internal** {{DT_BOOL}} | include internal edges in generated shapes; *default:* **False** |
| **--spread** {{DT_FLOAT}} | spread polygons laterally to avoid overlap |
| **--blur** {{DT_FLOAT}} | maximum random disturbance to route geometry; *default:* **0** |
| **--scale-width** {{DT_FLOAT}} | group similar routes and scale width by group size multiplied with the given factor (in m) |
| **--filter-signals** {{DT_STR}} | only write output for driveways of the given signals |
| **--filter-driveways** {{DT_STR}} | only write output for the given driveways |
| **--filter-foes** {{DT_STR}} | only write output for the foes of the given driveways |
| **--seed** {{DT_INT}} | random seed |

### [geoTrip2POI.py]({{Source}}tools/route/geoTrip2POI.py)

convert geoTrips to POIs

| Option | Description |
|--------|-------------|
| **routeFiles** {{DT_FILE}} | trip files to analyze |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | output poi file |
| **-l** {{DT_STR}}<br> **--layer** {{DT_STR}} | layer for generated polygons; *default:* **100** |
| **--scale-width** {{DT_FLOAT}} | Scale with of POIs by factor; *default:* **10** |
| **--filter-output.file** {{DT_STR}} | only write output for edges in the given selection file |
| **--seed** {{DT_INT}} | random seed |

### [implausibleRoutes.py]({{Source}}tools/route/implausibleRoutes.py)

| Option | Description |
|--------|-------------|
| **-a** {{DT_STR}}<br> **--additional-files** {{DT_STR}} | additional files to pass through duarouter e.g. vehicle type definitions |
| **--unsorted-input** {{DT_BOOL}} | If the provided route file has unsorted departure times; *default:* **False** |
| **--ignore-errors** {{DT_BOOL}} | Ignore errors when calling duarouter; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **--threshold** {{DT_FLOAT}} | Routes with an implausibility-score above threshold are reported; *default:* **2.5** |
| **--airdist-ratio-factor** {{DT_FLOAT}} | Implausibility factor for the ratio of routeDist/airDist; *default:* **1** |
| **--detour-ratio-factor** {{DT_FLOAT}} | Implausibility factor for the ratio of routeDuration/shortestDuration; *default:* **1** |
| **--detour-factor** {{DT_FLOAT}} | Implausibility factor for the absolute detour time in (routeDuration-shortestDuration) in seconds; *default:* **0.01** |
| **--min-dist** {{DT_FLOAT}} | Minimum shortest-path distance below which routes are implausible; *default:* **0** |
| **--min-air-dist** {{DT_FLOAT}} | Minimum air distance below which routes are implausible; *default:* **0** |
| **--standalone** {{DT_BOOL}} | Parse stand-alone routes that are not define as child-element of a vehicle; *default:* **False** |
| **--blur** {{DT_FLOAT}} | maximum random disturbance to output polygon geometry; *default:* **0** |
| **--ignore-routes** {{DT_STR}} | List of route IDs (one per line) that are filtered when generating polygons and command line output (they will still be added to restrictions-output) |
| **-o** {{DT_STR}}<br> **--xml-output** {{DT_STR}} | Write implausibility scores and routes to xml FILE |
| **--restriction-output** {{DT_STR}} | Write flow-restriction output suitable for passing to flowrouter.py to FILE |
| **--od-restrictions** {{DT_BOOL}} | Write restrictions for origin-destination relations rather than whole routes; *default:* **False** |
| **--edge-loops** {{DT_BOOL}} | report routes which use edges twice; *default:* **False** |
| **--node-loops** {{DT_BOOL}} | report routes which use junctions twice; *default:* **False** |
| **--threads** {{DT_INT}} | number of threads to use for duarouter; *default:* **1** |
| **--min-edges** {{DT_INT}} | number of edges a route needs to have to be analyzed; *default:* **2** |
| **--heterogeneous** {{DT_BOOL}} | Use slow parsing for route files with different formats in one file; *default:* **False** |
| **--reuse-routing** {{DT_BOOL}} | do not run duarouter again if output file exists; *default:* **False** |
| **network** {{DT_STR}} | network file to use |
| **routeFiles** {{DT_STR_LIST}} | route files to use |

### [route2alts.py]({{Source}}tools/route/route2alts.py)

| Option | Description |
|--------|-------------|
| **input** {{DT_STR}} | Provide an input network |
| **output** {{DT_STR}} | Provide an output name |

### [route2OD.py]({{Source}}tools/route/route2OD.py)

Add fromTaz and toTaz to a route file

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | define the input route file (mandatory) |
| **-a** {{DT_STR}}<br> **--taz-files** {{DT_STR}} | define the files to load TAZ (districts); it is mandatory when generating taz-based OD file |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | define the output filename (mandatory) |
| **-i** {{DT_STR}}<br> **--interval** {{DT_STR}} | define the output aggregation interval |
| **--id** {{DT_STR}} | define the output aggregation interval; *default:* **DEFAULT_VEHTYPE** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **--edge-relations** {{DT_BOOL}} | generate edgeRelations instead of tazRelations; *default:* **False** |

### [route2poly.py]({{Source}}tools/route/route2poly.py)

generates a polygon for every loaded route

| Option | Description |
|--------|-------------|
| **net** {{DT_NET}} | input net file |
| **routefiles** {{DT_FILE}} | input route files |
| **-o** {{DT_FILE}}<br> **--outfile** {{DT_FILE}} | name of output file |
| **-u** {{DT_STR}}<br> **--hue** {{DT_STR}} | hue for polygons (float from [0,1] or 'random'); *default:* **random** |
| **-s** {{DT_STR}}<br> **--saturation** {{DT_STR}} | saturation for polygons (float from [0,1] or 'random'); *default:* **1** |
| **-b** {{DT_STR}}<br> **--brightness** {{DT_STR}} | brightness for polygons (float from [0,1] or 'random'); *default:* **1** |
| **-l** {{DT_STR}}<br> **--layer** {{DT_STR}} | layer for generated polygons; *default:* **100** |
| **--geo** {{DT_BOOL}} | write polygons with geo-coordinates; *default:* **False** |
| **--internal** {{DT_BOOL}} | include internal edges in generated shapes; *default:* **False** |
| **--spread** {{DT_FLOAT}} | spread polygons laterally to avoid overlap |
| **--blur** {{DT_FLOAT}} | maximum random disturbance to route geometry; *default:* **0** |
| **--scale-width** {{DT_FLOAT}} | group similar routes and scale width by group size or route probability,multiplied with the given factor (in m) |
| **--filter-count** {{DT_FLOAT}} | only include routes that occur at least INT times |
| **--standalone** {{DT_BOOL}} | Parse stand-alone routes that are not define as child-element of a vehicle; *default:* **False** |
| **--filter-output.file** {{DT_STR}} | only write output for edges in the given selection file |
| **--seed** {{DT_INT}} | random seed |

### [route2sel.py]({{Source}}tools/route/route2sel.py)

| Option | Description |
|--------|-------------|
| **routefiles** {{DT_FILE}} | route files |
| **-o** {{DT_FILE}}<br> **--outfile** {{DT_FILE}} | name of output file |

### [route_1htoDay.py]({{Source}}tools/route/route_1htoDay.py)

| Option | Description |
|--------|-------------|
| **routes** {{DT_STR}} | Provide an input route file for 1h |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | Set an output file to contain all routes |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [route_departOffset.py]({{Source}}tools/route/route_departOffset.py)

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--input-file** {{DT_STR}} | the input route file (mandatory) |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | the output route file (mandatory) |
| **-d** {{DT_TIME}}<br> **--depart-offset** {{DT_TIME}} | the depart offset to apply (negative values are allowed, departures are clamped to 0) |
| **--random-depart-offset** {{DT_STR}} | two comma separated values MIN,MAX; shift every departure by a random value uniformly chosen from [MIN,MAX[ (departures are clamped to 0) |
| **--seed** {{DT_INT}} | random seed for --random-depart-offset; *default:* **42** |
| **-i** {{DT_STR}}<br> **--depart-interval** {{DT_STR}} | time intervals a,b,c,d where all vehicles departing in the interval[a,b[ are mapped to the interval [c,d[ |
| **--modify-ids** {{DT_BOOL}} | whether ids should be modified as well; *default:* **False** |
| **--heterogeneous** {{DT_BOOL}} | whether heterogeneous objects shall be parsed (i.e. vehicles with embedded and referenced routes); *default:* **False** |
| **--cumulative** {{DT_BOOL}} | multiply offset by the number of preceeding vehicles; *default:* **False** |
| **--depart-edges** {{DT_STR}} | only modify departure times of vehicles departing on the given edges |
| **--depart-edges.file** {{DT_STR}} | only modify departure times of vehicles departing on edges or lanes in the given selection file |
| **--arrival-edges** {{DT_STR}} | only modify departure times of vehicles arriving on the given edges |
| **--arrival-edges.file** {{DT_STR}} | only modify departure times of vehicles arriving on edges or lanes in the given selection file |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | interpret values as times and write them as h:m:s; *default:* **False** |

### [routecheck.py]({{Source}}tools/route/routecheck.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-f** {{DT_BOOL}}<br> **--fix** {{DT_BOOL}} | fix errors into '.fixed' file; *default:* **False** |
| **-l** {{DT_BOOL}}<br> **--fix-length** {{DT_BOOL}} | fix vehicle type's length and guiOffset attributes; *default:* **False** |
| **-i** {{DT_BOOL}}<br> **--inplace** {{DT_BOOL}} | replace original files; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | network to check connectivity |
| **routes** {{DT_FILE}} | route files to check |

### [routecompare.py]({{Source}}tools/route/routecompare.py)

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--districts-file** {{DT_FILE}} | read districts from FILE |
| **-s** {{DT_BOOL}}<br> **--simple-match** {{DT_BOOL}} | use simple matching algorithm; *default:* **False** |
| **-p** {{DT_BOOL}}<br> **--print-matching** {{DT_BOOL}} | print the resulting matching; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | print more info; *default:* **False** |
| **routes1** {{DT_Routes}} | first route file of comparison |
| **routes2** {{DT_Routes}} | second route file of comparison |

### [routeStats.py]({{Source}}tools/route/routeStats.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Give more output; *default:* **False** |
| **-f** {{DT_BOOL}}<br> **--fast** {{DT_BOOL}} | Use fast xml parser (does not work with length and numEdges); *default:* **False** |
| **-n** {{DT_STR}}<br> **--network** {{DT_STR}} | The network file to use with attribute 'length' |
| **-e** {{DT_STR}}<br> **--edges-file** {{DT_STR}} | The edges file (.edg.xml or edgedata) for retrieving relevant edge ids |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to analyze [length,depart,numEdges,duration,routeLength,speed,speedKmh]; *default:* **length** |
| **-b** {{DT_FLOAT}}<br> **--binwidth** {{DT_FLOAT}} | binning width of result histogram; *default:* **500** |
| **-l** {{DT_FLOAT}}<br> **--minlength** {{DT_FLOAT}} | only use routes with a minimum route length; *default:* **0** |
| **--hist-output** {{DT_STR}} | output file for histogram (gnuplot compatible) |
| **--full-output** {{DT_STR}} | output file for full data dump |

### [scaleTimeLine.py]({{Source}}tools/route/scaleTimeLine.py)

| Option | Description |
|--------|-------------|
| **-r** {{DT_FILE}}<br> **--route-files** {{DT_FILE}} | define the route file separated by comma (mandatory); *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output filename |
| **--timeline-list** {{DT_STR}} | Define the interval duration and then the scaled percentage for each interval; e.g. 200 percent of the current demand; *default:* **3600,8,5,4,3,4,12,45,74,66,52,50,50,52,53,56,67,84,86,74,50,39,30,21,16** |
| **--timeline-pair** {{DT_STR}} | Define the timeline pairs (duration, scacled percentage) |
| **--random** {{DT_BOOL}} | use a random seed to initialize the random number generator; *default:* **False** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [sort_routes.py]({{Source}}tools/route/sort_routes.py)

| Option | Description |
|--------|-------------|
| **-o** {{DT_FILE}}<br> **--outfile** {{DT_FILE}} | name of output file |
| **-b** {{DT_BOOL}}<br> **--big** {{DT_BOOL}} | Use alternative sorting strategy for large files (slower but more memory efficient); *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Tell me what you are doing; *default:* **False** |
| **routefile** {{DT_FILE}} | route file whose routes should be sorted |

### [splitRandom.py]({{Source}}tools/route/splitRandom.py)

| Option | Description |
|--------|-------------|
| **-r** {{DT_Routes}}<br> **--route-file** {{DT_Routes}} | define the input route file with trips or vehicles |
| **-n** {{DT_INT}}<br> **--number** {{DT_INT}} | number of trips/vehicles to split (not together with percent) |
| **-p** {{DT_FLOAT}}<br> **--percent** {{DT_FLOAT}} | percent of trips/vehicles to split (not together with number |
| **-a** {{DT_Routes}}<br> **--output-file-a** {{DT_Routes}} | define the first output route file; *default:* **tripsA.rou.xml** |
| **-b** {{DT_Routes}}<br> **--output-file-b** {{DT_Routes}} | define the second output route file; *default:* **tripsB.rou.xml** |
| **--random** {{DT_BOOL}} | use a random seed to initialize the random number generator; *default:* **False** |
| **-s** {{DT_INT}}<br> **--seed** {{DT_INT}} | random seed; *default:* **42** |

### [splitRouteFiles.py]({{Source}}tools/route/splitRouteFiles.py)

| Option | Description |
|--------|-------------|
| **-f** {{DT_FILE}}<br> **--detector-file** {{DT_FILE}} | read detectors from FILE |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-s** {{DT_INT}}<br> **--step** {{DT_INT}} | time split step in seconds; *default:* **900** |
| **-n** {{DT_INT}}<br> **--next-day** {{DT_INT}} | how far to go into the next day in seconds; *default:* **2700** |
| **-t** {{DT_STR}}<br> **--types-file** {{DT_STR}} | write vehicle types to FILE; *default:* **vehtypes.xml** |
| **--route-collection** {{DT_Routes}} | write route collection for the distributions to FILE |
| **-o** {{DT_STR}}<br> **--routes-prefix** {{DT_STR}} | let time splitted route files start with PREFIX; *default:* **validate/validate_mofr_** |
| **-e** {{DT_STR}}<br> **--edge-count** {{DT_STR}} | dump number of routes for each edge to FILE |
| **-p** {{DT_STR}}<br> **--pickle-edges** {{DT_STR}} | dump used edges as pickled set to FILE |
| **-a** {{DT_FLOAT}}<br> **--saturday-factor** {{DT_FLOAT}} | generate saturday files scaled down by FACTOR; *default:* **0.0** |
| **-u** {{DT_FLOAT}}<br> **--sunday-factor** {{DT_FLOAT}} | generate sunday files scaled down by FACTOR; *default:* **0.0** |
| **-m** {{DT_BOOL}}<br> **--modify-id** {{DT_BOOL}} | try to make vehicle and route ids unique; *default:* **False** |
| **routefiles** {{DT_FILE}} | list of route files |

### [tracegenerator.py]({{Source}}tools/route/tracegenerator.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | SUMO network to use (mandatory); *required* |
| **-2** {{DT_NET}}<br> **--net2** {{DT_NET}} | immediately match routes to a second network |
| **-r** {{DT_Routes}}<br> **--routes** {{DT_Routes}} | route file to use (mandatory); *required* |
| **-s** {{DT_FLOAT}}<br> **--step** {{DT_FLOAT}} | distance between successive trace points; *default:* **10** |
| **-d** {{DT_FLOAT}}<br> **--delta** {{DT_FLOAT}} | maximum distance between edge and trace points when matching to the second net; *default:* **1** |
| **-x** {{DT_FLOAT}}<br> **--x-offset** {{DT_FLOAT}} | offset to add to traces; *default:* **0.0** |
| **-y** {{DT_FLOAT}}<br> **--y-offset** {{DT_FLOAT}} | offset to add to traces; *default:* **0.0** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | trace or route output (mandatory); *required* |

### [tracemapper.py]({{Source}}tools/route/tracemapper.py)

| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | SUMO network to use; *required* |
| **-t** {{DT_FILE}}<br> **--trace** {{DT_FILE}} | trace files to use, separated by comma; *required* |
| **-d** {{DT_FLOAT}}<br> **--delta** {{DT_FLOAT}} | maximum distance between edge and trace points; *default:* **1.0** |
| **-a** {{DT_FLOAT}}<br> **--air-dist-factor** {{DT_FLOAT}} | maximum factor between airline and route distance between successive trace points; *default:* **2.0** |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | route output; *required* |
| **-p** {{DT_FILE}}<br> **--poi-output** {{DT_FILE}} | generate POI output for the trace |
| **-y** {{DT_FILE}}<br> **--polygon-output** {{DT_FILE}} | generate polygon output for the mapped edges |
| **--geo** {{DT_BOOL}} | read trace with geo-coordinates; *default:* **False** |
| **--direction** {{DT_BOOL}} | try to use direction of consecutive points when mapping; *default:* **False** |
| **--vehicle-class** {{DT_STR}} | filters the edges by the vehicle class the route is meant for |
| **--fill-gaps** {{DT_FLOAT}} | repair disconnected routes bridging gaps of up to x meters; *default:* **0.0** |
| **-g** {{DT_FLOAT}}<br> **--gap-penalty** {{DT_FLOAT}} | penalty to add for disconnected routes (default of -1 adds the distance between the two endpoints as penalty); *default:* **-1** |
| **--internal** {{DT_BOOL}} | include internal edges in generated shapes; *default:* **False** |
| **--spread** {{DT_FLOAT}} | spread polygons laterally to avoid overlap |
| **--blur** {{DT_FLOAT}} | maximum random disturbance to route geometry; *default:* **0** |
| **-l** {{DT_STR}}<br> **--layer** {{DT_STR}} | layer for generated polygons; *default:* **100** |
| **-b** {{DT_BOOL}}<br> **--debug** {{DT_BOOL}} | print out the debugging messages; *default:* **False** |

## `tools/shapes`

### [circlePolygon.py]({{Source}}tools/shapes/circlePolygon.py)

Make a circle polygon

| Option | Description |
|--------|-------------|
| **files** {{DT_FILE}} | List of XML files to plot |
| **-r** {{DT_FLOAT}}<br> **--radius** {{DT_FLOAT}} | default radius; *default:* **100** |
| **-p** {{DT_STR}}<br> **--prefix** {{DT_STR}} | id prefix; *default:* **poly** |
| **-t** {{DT_STR}}<br> **--type** {{DT_STR}} | type string; *default:* **unknown** |
| **--color** {{DT_STR}} | color string; *default:* **1,0,0** |
| **-f** {{DT_BOOL}}<br> **--fill** {{DT_BOOL}} | fill the polygons; *default:* **False** |
| **-l** {{DT_FLOAT}}<br> **--layer** {{DT_FLOAT}} | layer; *default:* **-1** |
| **-x** {{DT_INT}}<br> **--corners** {{DT_INT}} | default number of corners; *default:* **100** |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | output file (default: standard output) |

### [poly2edgedata.py]({{Source}}tools/shapes/poly2edgedata.py)

Transform polygons with params into edgedata with attributes

| Option | Description |
|--------|-------------|
| **polyfiles** {{DT_FILE}} | List of polygon files to convert |
| **-n** {{DT_NET}}<br> **--netfile** {{DT_NET}} | Network file on which to map the polygons; *required* |
| **-p** {{DT_FILE}}<br> **--patchfile** {{DT_FILE}} | Load a file with patches to apply during import |
| **-r** {{DT_FLOAT}}<br> **--radius** {{DT_FLOAT}} | radius for finding edges near polygons; *default:* **20** |
| **--shapecut** {{DT_FLOAT}} | Shorten polygon and edge shapes to FLOAT to increase robustness of angular comparison; *default:* **40** |
| **--angle-tolerance** {{DT_FLOAT}} | Match polygons and edges if their angle differs by no more than DEGREES; *default:* **20** |
| **--min-length** {{DT_FLOAT}} | minimum edge length that may be mapped to; *default:* **10** |
| **-s** {{DT_STR}}<br> **--split-attributes** {{DT_STR}} | If a reverse edge is found, split the values of the given attribute list among edge and reverse edge |
| **-S** {{DT_STR}}<br> **--nosplit-attributes** {{DT_STR}} | If a reverse edge is found, split the values of all attributes except the given attribute list among edge and reverse edge |
| **-f** {{DT_STR}}<br> **--filter** {{DT_STR}} | Read a list of triplets ATTR,MIN,MAX and only keep polygons where value ATTR is within [MIN,MAX] |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | edgedata interval begin time; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | edgedata interval end time); *default:* **1:0:0:0** |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | output file; *required* |

## `tools/tls`

### [buildTransitions.py]({{Source}}tools/tls/buildTransitions.py)

Create tlLogic definitions with branching plans based on simplified input.

| Option | Description |
|--------|-------------|
| **-d** {{DT_FILE}}<br> **--description-file** {{DT_FILE}} | Input logic description file (mandatory); *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | Output route file (mandatory); *required* |
| **-y** {{DT_INT}}<br> **--yellow-time** {{DT_INT}} | Duration of yellow phase; *default:* **3** |
| **-r** {{DT_INT}}<br> **--red-time** {{DT_INT}} | Duration of all-red phase during each transition; *default:* **0** |

### [createTlsCsv.py]({{Source}}tools/tls/createTlsCsv.py)

Create tls links from sumo net as needed by tls_csv2SUMO.py. You have to edit the link number field (preset with g). The comment gives the link number shown on demand in SUMO-GUI

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | Input file name; *required* |

### [tls_csv2SUMO.py]({{Source}}tools/tls/tls_csv2SUMO.py)

Create TLS XML file from CSV file.

| Option | Description |
|--------|-------------|
| **TLS_CSV** {{DT_FILE}} | TLS definition CSV file |
| **NET** {{DT_NET}} | Network file |
| **-d** {{DT_BOOL}}<br> **--debug** {{DT_BOOL}} | Print additional debug info; *default:* **False** |

### [tls_csvSignalGroups.py]({{Source}}tools/tls/tls_csvSignalGroups.py)

Converts a CSV file with green times per signal group into the SUMO format.

| Option | Description |
|--------|-------------|
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | File path to TLL output file (SUMO additional file) / prefix for generated CSV files.; *default:* **tls.add.xml** |
| **-i** {{DT_FILE}}<br> **--input** {{DT_FILE}} | File path to input CSV or TLL file(s). Multiple file paths have to be separated by ','. |
| **-r** {{DT_BOOL}}<br> **--reverse** {{DT_BOOL}} | Interpret input files in TLL format and convert them to CSV files.; *default:* **False** |
| **-g** {{DT_BOOL}}<br> **--group** {{DT_BOOL}} | Join signals with identical states into one signal group when converting to CSV format.; *default:* **False** |
| **--tls-from-net** {{DT_BOOL}} | Convert TL programs stored within the net file to CSV format.; *default:* **False** |
| **--tls-filter** {{DT_STR}} | Comma-separated list of traffic lights which the reverse conversion from TLL to CSV should be limited to. |
| **--delimiter** {{DT_STR}} | CSV delimiter used for input and template files.; *default:* **;** |
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | File path to SUMO network file. Optional for creating TL XML, obligatory for converting TL XML to CSV.; *required* |
| **-m** {{DT_STR}}<br> **--make-input-dir** {{DT_STR}} | Create input file template(s) from the SUMO network file in the given directory. |
| **-d** {{DT_BOOL}}<br> **--debug** {{DT_BOOL}} | Output debugging information; *default:* **False** |

## `tools/trigger`

### [calibrator2edgedata.py]({{Source}}tools/trigger/calibrator2edgedata.py)

convert calibrators (possibly overlapping) to edgedata

| Option | Description |
|--------|-------------|
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | name of output file; *required* |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **calibratorfiles** {{DT_Routes}} | Set one or more input route files |

### [createOvertakingReroutes.py]({{Source}}tools/trigger/createOvertakingReroutes.py)

Generate rerouters with overtakeReroute definitions

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | define the net file (mandatory); *required* |
| **-r** {{DT_Routes}}<br> **--route-files** {{DT_Routes}} | define additional files to be loaded by the router; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | define the output filename; *required* |
| **--vclass** {{DT_STR}} | only consider edges which permit the given vehicle class; *default:* **rail** |
| **--min-length** {{DT_FLOAT}} | minimum siding length; *default:* **100.0** |
| **--max-length** {{DT_FLOAT}} | maximum siding length |
| **--max-detour-factor** {{DT_FLOAT}} | Maximum factor by which the siding may be longer than the main path; *default:* **2** |
| **--min-priority** {{DT_INT}} | Minimum edge priority value to be eligible as siding |
| **-x** {{DT_INT}}<br> **--exclude-usage** {{DT_INT}} | Exclude all edges that are used at least INT times by loaded routes |
| **-X** {{DT_INT}}<br> **--exclude-reverse-usage** {{DT_INT}} | Exclude all edges that are used at least INT times in reverse by loaded routes; *default:* **1** |
| **--reversal-penalty** {{DT_FLOAT}} | Set penalty for reversals, by default sidings with reversals are forbidden; *default:* **-1** |
| **--no-defer** {{DT_BOOL}} | Write more overtakingReroute alternatives and do not defer decision; *default:* **False** |
| **--routing-threads** {{DT_INT}} | Perform routing on multiple threads; *default:* **11** |
| **--prefix** {{DT_STR}} | prefix for the rerouter ids; *default:* **rr** |
| **-b** {{DT_TIME}}<br> **--begin** {{DT_TIME}} | interval begin time; *default:* **0** |
| **-e** {{DT_TIME}}<br> **--end** {{DT_TIME}} | interval end time (default 1 day); *default:* **1:0:0:0** |

### [csv2vss.py]({{Source}}tools/trigger/csv2vss.py)

| Option | Description |
|--------|-------------|
| **-o** {{DT_STR}}<br> **--outfile** {{DT_STR}} | name of output file |
| **-d** {{DT_STR}}<br> **--detectorfile** {{DT_STR}} | name of detector file |
| **-s** {{DT_FLOAT}}<br> **--scale** {{DT_FLOAT}} | scaling factor for time; *default:* **60** |
| **csvfile** {{DT_STR}} | name of input file |

### [meandata2vss.py]({{Source}}tools/trigger/meandata2vss.py)

| Option | Description |
|--------|-------------|
| **-o** {{DT_STR}}<br> **--outfile** {{DT_STR}} | name of output file |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | parse net for edge lengths |
| **-s** {{DT_FLOAT}}<br> **--scale** {{DT_FLOAT}} | scaling factor for speed; *default:* **1.0** |
| **-d** {{DT_FLOAT}}<br> **--minimum-deviation** {{DT_FLOAT}} | minimum relative deviation before applying a vss (needs a network); *default:* **0.0** |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to parse for speed |
| **meandatafile** {{DT_STR}} | name of input file |

### [sidings2poly.py]({{Source}}tools/trigger/sidings2poly.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_STR}}<br> **--netfile** {{DT_STR}} | name of input network file; *required* |
| **-r** {{DT_STR}}<br> **--rerouter-file** {{DT_STR}} | name of input railsignal block file; *required* |
| **-o** {{DT_STR}}<br> **--output** {{DT_STR}} | name of output file; *required* |
| **-u** {{DT_STR}}<br> **--hue** {{DT_STR}} | hue for polygons (float from [0,1] or 'random'); *default:* **random** |
| **-s** {{DT_STR}}<br> **--saturation** {{DT_STR}} | saturation for polygons (float from [0,1] or 'random'); *default:* **1** |
| **-b** {{DT_STR}}<br> **--brightness** {{DT_STR}} | brightness for polygons (float from [0,1] or 'random'); *default:* **1** |
| **-l** {{DT_STR}}<br> **--layer** {{DT_STR}} | layer for generated polygons; *default:* **100** |
| **--geo** {{DT_BOOL}} | write polygons with geo-coordinates; *default:* **False** |
| **--internal** {{DT_BOOL}} | include internal edges in generated shapes; *default:* **False** |
| **--spread** {{DT_FLOAT}} | spread polygons laterally to avoid overlap |
| **--blur** {{DT_FLOAT}} | maximum random disturbance to route geometry; *default:* **0** |
| **--scale-width** {{DT_FLOAT}} | group similar routes and scale width by group size multiplied with the given factor (in m) |
| **--filter-rerouters** {{DT_STR}} | only write output for the given list of rerouters |
| **--seed** {{DT_INT}} | random seed |

## `tools/turn-defs`

### [generateTurnRatios.py]({{Source}}tools/turn-defs/generateTurnRatios.py)

| Option | Description |
|--------|-------------|
| **-r** {{DT_FILE}}<br> **--route-files** {{DT_FILE}} | define the route file separated by comma(mandatory); *required* |
| **-n** {{DT_NET}}<br> **--net-file** {{DT_NET}} | load network for annotation turns with directions |
| **-o** {{DT_STR}}<br> **--output-file** {{DT_STR}} | define the output filename; *default:* **turnRatios.add.xml** |
| **-p** {{DT_BOOL}}<br> **--probabilities** {{DT_BOOL}} | calculate the turning probabilities instead of traffic volumes; *default:* **False** |
| **--id** {{DT_STR}} | define the interval id; *default:* **generated** |
| **-b** {{DT_STR}}<br> **--begin** {{DT_STR}} | custom begin time (seconds or H:M:S); *default:* **0** |
| **-e** {{DT_STR}}<br> **--end** {{DT_STR}} | custom end time (seconds or H:M:S) |
| **-i** {{DT_STR}}<br> **--interval** {{DT_STR}} | custom aggregation interval (seconds or H:M:S) |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **--split-types** {{DT_BOOL}} | split output by vehicle types; *default:* **False** |

### [turnCount2EdgeCount.py]({{Source}}tools/turn-defs/turnCount2EdgeCount.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-t** {{DT_FILE}}<br> **--turn-file** {{DT_FILE}} | Input turn-count file; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | Output edgeData file; *required* |
| **--edgedata-attribute** {{DT_STR}} | Write edgeData counts with the given attribute; *default:* **entered** |
| **--turn-attribute** {{DT_STR}} | Read turning counts from the given attribute; *default:* **count** |

### [turnFile2EdgeRelations.py]({{Source}}tools/turn-defs/turnFile2EdgeRelations.py)

Sample routes to match counts

| Option | Description |
|--------|-------------|
| **-t** {{DT_FILE}}<br> **--turn-file** {{DT_FILE}} | Input turn-count file; *required* |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | Output edgeRelations file; *required* |
| **--turn-attribute** {{DT_STR}} | Write turning 'probability' to the given attribute; *default:* **probability** |

## `tools/visualization`

### [macrOutput.py]({{Source}}tools/visualization/macrOutput.py)

| Option | Description |
|--------|-------------|
| **file** {{DT_FILE}} | An XML input file |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |

### [plot_csv_bars.py]({{Source}}tools/visualization/plot_csv_bars.py)

| Option | Description |
|--------|-------------|
| **-i** {{DT_FILE}}<br> **--input** {{DT_FILE}} | Defines the csv file to use as input |
| **--column** {{DT_INT}} | Selects the column to read values from; *default:* **1** |
| **-r** {{DT_BOOL}}<br> **--revert** {{DT_BOOL}} | Reverts the order of read values; *default:* **False** |
| **-w** {{DT_FLOAT}}<br> **--width** {{DT_FLOAT}} | Defines the width of the bars; *default:* **0.8** |
| **--space** {{DT_FLOAT}} | Defines the space between the bars; *default:* **0.2** |
| **--norm** {{DT_FLOAT}} | Divides the read numbers by this value before plotting them; *default:* **1.0** |
| **--show-values** {{DT_BOOL}} | Shows the values; *default:* **False** |
| **--values-offset** {{DT_FLOAT}} | Position offset for values; *default:* **1.0** |
| **--vertical** {{DT_BOOL}} | vertical bars are used; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |

### [plot_csv_pie.py]({{Source}}tools/visualization/plot_csv_pie.py)

| Option | Description |
|--------|-------------|
| **-i** {{DT_FILE}}<br> **--input** {{DT_FILE}} | Defines the csv file to use as input; *required* |
| **-p** {{DT_BOOL}}<br> **--percentage** {{DT_BOOL}} | Interprets read measures as percentages; *default:* **False** |
| **-r** {{DT_BOOL}}<br> **--revert** {{DT_BOOL}} | Reverts the order of read values; *default:* **False** |
| **--no-labels** {{DT_BOOL}} | Does not plot the labels; *default:* **False** |
| **--shadow** {{DT_BOOL}} | Puts a shadow below the circle; *default:* **False** |
| **--startangle** {{DT_FLOAT}} | Sets the start angle; *default:* **0** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |

### [plot_csv_timeline.py]({{Source}}tools/visualization/plot_csv_timeline.py)

| Option | Description |
|--------|-------------|
| **-i** {{DT_FILE}}<br> **--input** {{DT_FILE}} | Defines the input file to use; *required* |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **--columns** {{DT_STR}} | Defines which columns shall be plotted |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |

### [plot_net_dump.py]({{Source}}tools/visualization/plot_net_dump.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | Defines the network to read; *required* |
| **-i** {{DT_FILE}}<br> **--dump-inputs** {{DT_FILE}} | Defines the dump-output files to use as input; *required* |
| **-m** {{DT_STR}}<br> **--measures** {{DT_STR}} | Define which measure to plot; *default:* **speed,entered** |
| **--min-width** {{DT_FLOAT}} | Defines the minimum edge width; *default:* **0.5** |
| **--max-width** {{DT_FLOAT}} | Defines the maximum edge width; *default:* **3** |
| **--log-colors** {{DT_BOOL}} | If set, colors are log-scaled; *default:* **False** |
| **--log-widths** {{DT_BOOL}} | If set, widths are log-scaled; *default:* **False** |
| **--min-color-value** {{DT_FLOAT}} | If set, defines the minimum edge color value |
| **--max-color-value** {{DT_FLOAT}} | If set, defines the maximum edge color value |
| **--min-width-value** {{DT_FLOAT}} | If set, defines the minimum edge width value |
| **--max-width-value** {{DT_FLOAT}} | If set, defines the maximum edge width value |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **--color-bar-label** {{DT_STR}} | The label to put on the color bar |
| **--internal** {{DT_BOOL}} | include internal edges in generated shapes; *default:* **False** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to; for multiple time intervals use '%s' in the filename as a placeholder for the beginning of the time interval |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |
| **-w** {{DT_FLOAT}}<br> **--default-width** {{DT_FLOAT}} | Defines the default edge width; *default:* **0.1** |
| **--default-color** {{DT_STR}} | Defines the default edge color; *default:* **k** |

### [plot_net_selection.py]({{Source}}tools/visualization/plot_net_selection.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | Defines the network to read; *required* |
| **-i** {{DT_FILE}}<br> **--selection** {{DT_FILE}} | Defines the selection to read; *required* |
| **--selected-width** {{DT_FLOAT}} | Defines the width of selected edges; *default:* **1** |
| **--color** {{DT_STR}} | Defines the color of selected edges; *default:* **r** |
| **--edge-width** {{DT_FLOAT}} | Defines the width of not selected edges; *default:* **0.2** |
| **--edge-color** {{DT_STR}} | Defines the color of not selected edges; *default:* **#606060** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |

### [plot_net_speeds.py]({{Source}}tools/visualization/plot_net_speeds.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | Defines the network to read; *required* |
| **--edge-width** {{DT_FLOAT}} | Defines the edge width; *default:* **1** |
| **--edge-color** {{DT_STR}} | Defines the edge color; *default:* **k** |
| **--minV** {{DT_FLOAT}} | Define the minimum value boundary |
| **--maxV** {{DT_FLOAT}} | Define the maximum value boundary |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |

### [plot_net_trafficLights.py]({{Source}}tools/visualization/plot_net_trafficLights.py)

| Option | Description |
|--------|-------------|
| **-n** {{DT_NET}}<br> **--net** {{DT_NET}} | Defines the network to read; *required* |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **-w** {{DT_FLOAT}}<br> **--width** {{DT_FLOAT}} | Defines the width of the dots; *default:* **20** |
| **--color** {{DT_STR}} | Defines the dot color; *default:* **r** |
| **--edge-width** {{DT_FLOAT}} | Defines the edge width; *default:* **1** |
| **--edge-color** {{DT_STR}} | Defines the edge color; *default:* **k** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |

### [plot_summary.py]({{Source}}tools/visualization/plot_summary.py)

| Option | Description |
|--------|-------------|
| **-i** {{DT_FILE}}<br> **--summary-inputs** {{DT_FILE}} | Defines the summary-output files to use as input; *required* |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | If set, the script says what it's doing; *default:* **False** |
| **-m** {{DT_STR}}<br> **--measure** {{DT_STR}} | Define which measure to plot; *default:* **running** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |

### [plotStops.py]({{Source}}tools/visualization/plotStops.py)

This script plots a train schedule from a route file or stop-output file

| Option | Description |
|--------|-------------|
| **-r** {{DT_STR}}<br> **--route-file** {{DT_STR}} | Route file for obtaining the ordering of stops; *required* |
| **-a** {{DT_FILE}}<br> **--stop-file** {{DT_FILE}} | Read ordering of stops from file |
| **-s** {{DT_FILE}}<br> **--stopinfo-file** {{DT_FILE}} | plot simulation stop-output file |
| **-t** {{DT_STR}}<br> **--time-attribute** {{DT_STR}} | attribute(s) for plotting time; *default:* **until** |
| **-S** {{DT_FILE}}<br> **--stop-order-output** {{DT_FILE}} | write ordering and stop names to file; *default:* **stoplist.txt** |
| **-i** {{DT_STR}}<br> **--veh-id** {{DT_STR}} | plot route for the given vehicle id; *required* |
| **--filter-ids** {{DT_STR}} | only plot data points from the given list of ids |
| **--label** {{DT_STR}} | plot label (default input file name |
| **--legend** {{DT_BOOL}} | Add legend; *default:* **False** |
| **--csv-output** {{DT_STR}} | write plot as csv |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |

### [plotWKT.py]({{Source}}tools/visualization/plotWKT.py)

Plot a polygon, or the difference between two polygons, given in WKT format.

| Option | Description |
|--------|-------------|
| **filename** {{DT_FILE}} | Name of the WKT file with the primary polygon |
| **-f** {{DT_FILE}}<br> **--other-filename** {{DT_FILE}} | Name of the WKT file with the secondary polygon |
| **-d** {{DT_FILE}}<br> **--data-filename** {{DT_FILE}} | Name of supplementary WKT file |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Name of image file to write |
| **-a** {{DT_FLOAT}}<br> **--area-threshold** {{DT_FLOAT}} | Area threshold used to filter small holes; *default:* **0.01** |
| **-m** {{DT_STR}}<br> **--color** {{DT_STR}} | Color used to draw the polygons; *default:* **blue** |
| **-l** {{DT_STR}}<br> **--extra-color** {{DT_STR}} | Color used to draw supplementary data; *default:* **red** |

### [plotXMLAttributes.py]({{Source}}tools/visualization/plotXMLAttributes.py)

This script plots arbitrary xml attributes from xml files.

| Option | Description |
|--------|-------------|
| **files** {{DT_FILE}} | List of XML files to plot |
| **-x** {{DT_STR}}<br> **--xattr** {{DT_STR}} | attribute for x-axis |
| **-y** {{DT_STR}}<br> **--yattr** {{DT_STR}} | attribute for y-axis |
| **-i** {{DT_STR}}<br> **--idattr** {{DT_STR}} | attribute for grouping data points into lines; *default:* **id** |
| **--xelem** {{DT_STR}} | element for x-axis |
| **--yelem** {{DT_STR}} | element for y-axis |
| **--idelem** {{DT_STR}} | element for grouping data points into lines |
| **--show** {{DT_BOOL}} | show plot directly; *default:* **False** |
| **--csv-output** {{DT_STR}} | write plot as csv |
| **--filter-ids** {{DT_STR}} | only plot data points from the given list of ids |
| **-p** {{DT_FLOAT}}<br> **--pick-distance** {{DT_FLOAT}} | pick lines within the given distance in interactive plot mode; *default:* **1** |
| **--label** {{DT_STR}} | plot label (default input file name |
| **-j** {{DT_BOOL}}<br> **--join-files** {{DT_BOOL}} | Do not distinguis data points by file; *default:* **False** |
| **--join-x** {{DT_BOOL}} | if --xattr is a list concatenate the values; *default:* **False** |
| **--join-y** {{DT_BOOL}} | if --yattr is a list concatenate the values; *default:* **False** |
| **--join** {{DT_STR}} | separator to use for joining (default: ,); *default:* **,** |
| **--split-x** {{DT_BOOL}} | interpret the x value as a list of values; *default:* **False** |
| **--split-y** {{DT_BOOL}} | interpret the y value as a list of values; *default:* **False** |
| **--xfactor** {{DT_FLOAT}} | multiplier for x-data; *default:* **1** |
| **--yfactor** {{DT_FLOAT}} | multiplier for y-data; *default:* **1** |
| **--xbin** {{DT_FLOAT}} | binning size for x-data |
| **--ybin** {{DT_FLOAT}} | binning size for y-data |
| **--xclamp** {{DT_STR}} | clamp x values to range A:B or half-range A: / :B |
| **--yclamp** {{DT_STR}} | clamp y values to range A:B or half-range A: / :B |
| **--invert-yaxis** {{DT_BOOL}} | Invert the Y-Axis; *default:* **False** |
| **--xstr** {{DT_BOOL}} | Interpret x-data as string; *default:* **False** |
| **--ystr** {{DT_BOOL}} | Interpret y-data as string; *default:* **False** |
| **--scatterplot** {{DT_BOOL}} | Draw a scatterplot instead of lines; *default:* **False** |
| **--barplot** {{DT_BOOL}} | Draw a bar plot parallel to the y-axis; *default:* **False** |
| **--hbarplot** {{DT_BOOL}} | Draw a bar plot parallel to the x-axis; *default:* **False** |
| **--legend** {{DT_BOOL}} | Add legend; *default:* **False** |
| **--robust-parser** {{DT_BOOL}} | Use a standard XML-parser instead of a faster regex-based parser; *default:* **False** |
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | tell me what you are doing; *default:* **False** |
| **--colors** {{DT_STR}} | Defines the colors to use |
| **--colormap** {{DT_STR}} | Defines the colormap to use; *default:* **nipy_spectral** |
| **--colormap.center** {{DT_FLOAT}} | centers colors at the given value |
| **-l** {{DT_STR}}<br> **--labels** {{DT_STR}} | Defines the labels to use |
| **--xlim** {{DT_STR}} | Defines x-limits of the figure XMIN,XMAX |
| **--ylim** {{DT_STR}} | Defines y-limits of the figure YMIN,YMAX |
| **--xticks** {{DT_STR}} | Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE |
| **--yticks** {{DT_STR}} | Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE |
| **--xticks-file** {{DT_FILE}} | Load x-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--yticks-file** {{DT_FILE}} | Load y-axis ticks from file (LABEL or FLOAT:LABEL per line) |
| **--xtime0** {{DT_BOOL}} | Use a time formatter for x-ticks (hh); *default:* **False** |
| **--ytime0** {{DT_BOOL}} | Use a time formatter for y-ticks (hh); *default:* **False** |
| **--xtime1** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm); *default:* **False** |
| **--ytime1** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm); *default:* **False** |
| **--xtime2** {{DT_BOOL}} | Use a time formatter for x-ticks (hh:mm:ss); *default:* **False** |
| **--ytime2** {{DT_BOOL}} | Use a time formatter for y-ticks (hh:mm:ss); *default:* **False** |
| **--xgrid** {{DT_BOOL}} | Enable grid on x-axis; *default:* **False** |
| **--ygrid** {{DT_BOOL}} | Enable grid on y-axis; *default:* **False** |
| **--xticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--yticksorientation** {{DT_FLOAT}} | Set the orientation of the x-axis ticks |
| **--xlabel** {{DT_STR}} | Set the x-axis label |
| **--ylabel** {{DT_STR}} | Set the y-axis label |
| **--xlabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--ylabelsize** {{DT_INT}} | Set the size of the x-axis label; *default:* **16** |
| **--marker** {{DT_STR}} | marker for single points (default o for scatter, None otherwise) |
| **--linestyle** {{DT_STR}} | plot line style (default -); *default:* **-** |
| **--title** {{DT_STR}} | Set the title |
| **--titlesize** {{DT_INT}} | Set the title size; *default:* **16** |
| **--adjust** {{DT_STR}} | Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP |
| **-s** {{DT_STR}}<br> **--size** {{DT_STR}} | Defines the figure size X,Y; *default:* **False** |
| **--no-legend** {{DT_BOOL}} | Disables the legend; *default:* **False** |
| **--legend-position** {{DT_STR}} | Sets the legend position |
| **--dpi** {{DT_FLOAT}} | Define dpi resolution for figures |
| **--alpha** {{DT_FLOAT}} | Define background transparency of the figure in the range 0..1; *default:* **1.0** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | Comma separated list of filename(s) the figure shall be written to |
| **-b** {{DT_BOOL}}<br> **--blind** {{DT_BOOL}} | If set, the figure will not be shown; *default:* **False** |

## `tools/xml`

### [changeAttribute.py]({{Source}}tools/xml/changeAttribute.py)

Set or remove an attribute for the specified XML element.

| Option | Description |
|--------|-------------|
| **-f** {{DT_FILE}}<br> **--file** {{DT_FILE}} | define the XML input file; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | define the XML output file; *required* |
| **-t** {{DT_STR}}<br> **--tag** {{DT_STR}} | tag to edit |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to edit; *required* |
| **-v** {{DT_STR}}<br> **--value** {{DT_STR}} | value to update (deletes attribute if not specified) |
| **-u** {{DT_STR}}<br> **--upper-limit** {{DT_STR}} | updates to this maximum value (reduces all greater values) |

### [csv2xml.py]({{Source}}tools/xml/csv2xml.py)

Convert a CSV file to a XML file.

| Option | Description |
|--------|-------------|
| **source** {{DT_FILE}} | the input CSV file |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | name for generic output file; *required* |
| **-q** {{DT_STR}}<br> **--quotechar** {{DT_STR}} | the quoting character for fields |
| **-d** {{DT_STR}}<br> **--delimiter** {{DT_STR}} | the field separator of the input file; *default:* **;** |
| **-p** {{DT_BOOL}}<br> **--skip-root** {{DT_BOOL}} | the root element is not contained; *default:* **False** |
| **-t** {{DT_STR}}<br> **--type** {{DT_STR}} | convert the given csv-file into the specified format |
| **-x** {{DT_STR}}<br> **--xsd** {{DT_STR}} | xsd schema to use |
| **--flat** {{DT_BOOL}} | use csv header as flat structure instead of a schema; *default:* **False** |

### [filterElements.py]({{Source}}tools/xml/filterElements.py)

Remove elements in a XML file that match a certain attribute

| Option | Description |
|--------|-------------|
| **-f** {{DT_FILE}}<br> **--file** {{DT_FILE}} | define the XML input file; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | define the XML output file |
| **-t** {{DT_STR}}<br> **--tag** {{DT_STR}} | tag to edit |
| **-a** {{DT_STR}}<br> **--attribute** {{DT_STR}} | attribute to edit |
| **-r** {{DT_STR}}<br> **--remove-values** {{DT_STR}} | comma-separated list of values to filter by (deletes all occurences of tag if not specified) |
| **-k** {{DT_STR}}<br> **--keep-values** {{DT_STR}} | comma-separated list of values to keep (deletes all non-matching elements |
| **-x** {{DT_STR}}<br> **--remove-interval** {{DT_STR}} | comma-separated begin and end values of interval to filter by (deletes all occurences of tag if not specified) |
| **-i** {{DT_STR}}<br> **--keep-interval** {{DT_STR}} | comma-separated begin an end values of interval to keep (deletes all non-matching elements |
| **--remove-parent** {{DT_BOOL}} | removes the parent element if a child tag matches; *default:* **False** |

### [protobuf2xml.py]({{Source}}tools/xml/protobuf2xml.py)

Convert a protocol buffer to an XML file.

| Option | Description |
|--------|-------------|
| **source** {{DT_FILE}} | the input protobuf file |
| **-p** {{DT_STR}}<br> **--protodir** {{DT_STR}} | where to put and read .proto files; *default:* **.** |
| **-x** {{DT_STR}}<br> **--xsd** {{DT_STR}} | xsd schema to use; *required* |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | name for generic output file |

### [xml2csv.py]({{Source}}tools/xml/xml2csv.py)

Convert a XML file to a CSV file

| Option | Description |
|--------|-------------|
| **source** {{DT_FILE}} | the input data (stream given by digits or file |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | base name for output |
| **-s** {{DT_STR}}<br> **--separator** {{DT_STR}} | separating character for fields; *default:* **;** |
| **-d** {{DT_STR}}<br> **--decimal-separator** {{DT_STR}} | decimal separator for float values; *default:* **.** |
| **-q** {{DT_STR}}<br> **--quotechar** {{DT_STR}} | quoting character for fields |
| **-x** {{DT_STR}}<br> **--xsd** {{DT_STR}} | xsd schema to use |
| **-a** {{DT_BOOL}}<br> **--validation** {{DT_BOOL}} | enable schema validation; *default:* **False** |
| **--keep-attributes** {{DT_STR}} | Only keep the given attributes |
| **-p** {{DT_BOOL}}<br> **--split** {{DT_BOOL}} | split in different files for the first hierarchy level; *default:* **False** |

### [xml2protobuf.py]({{Source}}tools/xml/xml2protobuf.py)

Convert a XML file to a protocol buffer

| Option | Description |
|--------|-------------|
| **source** {{DT_FILE}} | the input data (given by digits or a file |
| **-p** {{DT_STR}}<br> **--protodir** {{DT_STR}} | where to put and read .proto files; *default:* **.** |
| **-x** {{DT_STR}}<br> **--xsd** {{DT_STR}} | xsd schema to use (mandatory); *required* |
| **-a** {{DT_BOOL}}<br> **--validation** {{DT_BOOL}} | enable schema validation; *default:* **False** |
| **-o** {{DT_FILE}}<br> **--output** {{DT_FILE}} | output file name |
