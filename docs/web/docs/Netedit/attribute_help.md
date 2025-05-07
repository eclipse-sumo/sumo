# Netedit attribute help

## edge
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of edge|
|from|unique string|The name of a node within the nodes-file the edge shall start at|
|to|unique string|The name of a node within the nodes-file the edge shall end at|
|speed|non-negative float|The maximum speed allowed on the edge in m/s *default:* **13.89**|
|priority|integer|The priority of the edge *default:* **-1**|
|numLanes|non-negative integer|The number of lanes of the edge *default:* **1**|
|type|string|The name of a type within the SUMO edge type file|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|shape|list of unique positions|If the shape is given it should start and end with the positions of the from-node and to-node|
|length|non-negative float|The length of the edge in meter|
|spreadType|discrete string|The spreadType defines how to compute the lane geometry from the edge geometry (used for visualization) *default:* **right**|
|name|string|street name (does not need to be unique, used for visualization)|
|width|non-negative float|Lane width for all lanes of this edge in meters (used for visualization) *default:* **3.20**|
|endOffset|non-negative float|Move the stop line back from the intersection by the given amount *default:* **0.00**|
|shapeStart|position|Custom position in which shape start (by default position of junction from)|
|shapeEnd|position|Custom position in which shape end (by default position of junction from)|
|isBidi|unique boolean|Show if edge is bidirectional *default:* **0**|
|distance|unique float|Distance *default:* **0.00**|
|stopOffset|non-negative float|The stop offset as positive value in meters *default:* **0.00**|
|stopOException|list of vClasses|Specifies, for which vehicle classes the stopOffset does NOT apply.|
|isRoundabout|unique boolean|Whether this edge is part of a roundabout *default:* **0**|

## lane
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of lane|
|index|unique integer|The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one)|
|speed|non-negative float|Speed in meters per second *default:* **13.89**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|width|non-negative float|Width in meters (used for visualization)|
|endOffset|non-negative float|Move the stop line back from the intersection by the given amount *default:* **0.00**|
|acceleration|boolean|Enable or disable lane as acceleration lane *default:* **0**|
|customShape|list of unique positions|If the shape is given it overrides the computation based on edge shape|
|opposite|unique string|If given, this defines the opposite direction lane|
|changeLeft|list of vClasses|Permit changing left only for to the given vehicle classes *default:* **all**|
|changeRight|list of vClasses|Permit changing right only for to the given vehicle classes *default:* **all**|
|type|string|Lane type description (optional)|
|stopOffset|non-negative float|The stop offset as positive value in meters *default:* **0.00**|
|stopOException|list of vClasses|Specifies, for which vehicle classes the stopOffset does NOT apply.|

## poly
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of poly|
|shape|list of unique positions|The shape of the polygon|
|name|string|Optional name for poly|
|color|color|The RGBA color with which the poly shall be displayed *default:* **red**|
|fill|boolean|An information whether the polygon shall be filled *default:* **0**|
|lineWidth|non-negative float|The default line width for drawing an unfilled polygon *default:* **1.00**|
|layer|float|The layer in which the polygon lies *default:* **0.00**|
|type|string|A typename for the polygon|
|imgFile|filename(Existent)|A bitmap to use for rendering this poly|
|angle|angle[0, 360]|Angle of rendered image in degree *default:* **0.00**|
|geo|boolean|Enable or disable GEO attributes *default:* **0**|
|geoShape|list of unique positions|A custom geo shape for this polygon|

## poi
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of poi|
|pos|unique position|The position in view|
|name|string|Optional name for poi|
|color|color|The RGBA color with which the poi shall be displayed *default:* **red**|
|type|string|A typename for the POI|
|icon|discrete string|POI Icon|
|layer|float|The layer of the POI for drawing and selecting *default:* **202.00**|
|width|non-negative float|Width of rendered image in meters *default:* **2.60**|
|height|non-negative float|Height of rendered image in meters *default:* **1.00**|
|imgFile|filename(Existent)|A bitmap to use for rendering this poi|
|angle|angle[0, 360]|Angle of rendered image in degree *default:* **0.00**|

## junction
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of junction|
|pos|unique position|The x-y-z position of the node on the plane in meters|
|type|discrete string|An optional type for the node|
|shape|list of unique positions|A custom shape for that node|
|radius|non-negative float|Optional turning radius (for all corners) for that node in meters *default:* **4.00**|
|keepClear|boolean|Whether the junction-blocking-heuristic should be activated at this node *default:* **1**|
|rightOfWay|discrete string|How to compute right of way rules at this node *default:* **default**|
|fringe|discrete string|Whether this junction is at the fringe of the network *default:* **default**|
|name|string|Optional name for junction|
|tlType|discrete string|An optional type for the traffic light algorithm|
|tlLayout|discrete string|An optional layout for the traffic light plan *default:* **default**|
|tl|string|An optional id for the traffic light program|
|isRoundabout|boolean|Whether this junction is part of a roundabout *default:* **0**|

## busStop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of busStop|
|lane|unique string|The name of the lane the busStop shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters *default:* **lane start**|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m *default:* **lane end**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|name|string|Optional name for busStop|
|color|color|The RGBA color with which the busStop shall be displayed *default:* **invisible**|
|lines|list of strings|Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes|
|personCapacity|non-negative integer|Larger numbers of persons trying to enter will create an upstream jam on the sidewalk *default:* **6**|
|parkingLength|float|Optional space definition for vehicles that park at this stop *default:* **0.00**|

## trainStop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of trainStop|
|lane|unique string|The name of the lane the trainStop shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters *default:* **lane start**|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m *default:* **lane end**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|name|string|Optional name for trainStop|
|color|color|The RGBA color with which the trainStop shall be displayed *default:* **invisible**|
|lines|list of strings|Meant to be the names of the train lines that stop at this train stop. This is only used for visualization purposes|
|personCapacity|non-negative integer|Larger numbers of persons trying to enter will create an upstream jam on the sidewalk *default:* **6**|
|parkingLength|float|Optional space definition for vehicles that park at this stop *default:* **0.00**|

### access
child element of [busStop](#busstop), [trainStop](#trainstop)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the access shall be located at|
|pos|unique float|The position on the lane the access shall be laid on in meters *default:* **0.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|length|non-negative float|The walking length of the access in meters (default is geometric length)|

## containerStop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of containerStop|
|lane|unique string|The name of the lane the containerStop shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters *default:* **lane start**|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m *default:* **lane end**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|name|string|Optional name for containerStop|
|color|color|The RGBA color with which the containerStop shall be displayed *default:* **invisible**|
|lines|list of strings|meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes|
|containerCapacity|non-negative integer|Larger numbers of container trying to enter will create an upstream jam on the sidewalk *default:* **6**|
|parkingLength|float|Optional space definition for vehicles that park at this stop|

## chargingStation
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of chargingStation|
|lane|unique string|The name of the lane the chargingStation shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters *default:* **lane start**|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m *default:* **lane end**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|name|string|Optional name for chargingStation|
|power|non-negative float|Charging power in W *default:* **22000.00**|
|efficiency|float|Charging efficiency [0,1] *default:* **0.95**|
|chargeInTransit|boolean|Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging *default:* **0**|
|chargeDelay|SUMOTime|Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins *default:* **0.00**|
|chargeType|discrete string|Battery charging type *default:* **normal**|
|waitingTime|SUMOTime|Waiting time before start charging *default:* **900.00**|
|parkingArea|string|Parking area the charging station is located|

## parkingArea
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of parkingArea|
|lane|unique string|The name of the lane the parkingArea shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters *default:* **lane start**|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m *default:* **lane end**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|name|string|Optional name for parkingArea|
|departPos|string|Lane position in that vehicle must depart when leaves parkingArea|
|acceptedBadges|list of strings|Accepted badges to access this parkingArea|
|roadsideCapacity|non-negative integer| The number of parking spaces for road-side parking *default:* **0**|
|onRoad|boolean|If set, vehicles will park on the road lane and thereby reducing capacity *default:* **0**|
|width|non-negative float|The width of the road-side parking spaces *default:* **3.20**|
|length|non-negative float|The length of the road-side parking spaces. By default (endPos - startPos) / roadsideCapacity|
|angle|angle[0, 360]|The angle of the road-side parking spaces relative to the lane angle, positive means clockwise *default:* **0.00**|
|lefthand|boolean|Enable or disable lefthand position *default:* **0**|

### space
child element of [parkingArea](#parkingarea)

| Attribute | Type | Description |
|-----------|------|-------------|
|pos|unique position|The x-y-z position of the node on the plane in meters|
|name|string|Optional name for space|
|width|string|The width of the road-side parking spaces|
|length|string|The length of the road-side parking spaces|
|angle|string|The angle of the road-side parking spaces relative to the lane angle, positive means clockwise|
|slope|angle[0, 360]|The slope of the road-side parking spaces *default:* **0.00**|

## inductionLoop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of inductionLoop|
|lane|unique string|The name of the lane the inductionLoop shall be located at|
|pos|unique float|The position on the lane the inductionLoop shall be laid on in meters *default:* **0.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|name|string|Optional name for inductionLoop|
|period|SUMOTime|The aggregation period the values the inductionLoop detector collects shall be summed up *default:* **300.00**|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|nextEdges|list of strings|List of edge ids that must all be part of the future route of the vehicle to qualify for detection|
|detectPersons|discrete string|Detect persons instead of vehicles (pedestrians or passengers) *default:* **none**|

## laneAreaDetector
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of laneAreaDetector|
|lane|unique string|The name of the lane the laneAreaDetector shall be located at|
|pos|unique float|The position on the lane the laneAreaDetector shall be laid on in meters *default:* **0.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|length|non-negative float|The length of the detector in meters *default:* **10.00**|
|name|string|Optional name for laneAreaDetector|
|period|SUMOTime|The aggregation period the values the laneAreaDetector detector collects shall be summed up *default:* **300.00**|
|tl|string|The traffic light that triggers aggregation when switching|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|nextEdges|list of strings|List of edge ids that must all be part of the future route of the vehicle to qualify for detection|
|detectPersons|discrete string|Detect persons instead of vehicles (pedestrians or passengers) *default:* **none**|
|timeThreshold|SUMOTime|The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) *default:* **1.00**|
|speedThreshold|non-negative float|The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s *default:* **1.39**|
|jamThreshold|non-negative float|The maximum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam in m *default:* **10.00**|
|show|boolean|Show detector in sumo-gui *default:* **1**|

## laneAreaDetector (multiLaneAreaDetector)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of multiLaneAreaDetector|
|lanes|list of unique strings|The sequence of lane ids in which the detector shall be laid on|
|pos|unique float|The position on the lane the multiLaneAreaDetector shall be laid on in meters *default:* **0.00**|
|endPos|unique float|The end position on the lane the detector shall be laid on in meters|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|period|SUMOTime|The aggregation period the values the multiLaneAreaDetector detector collects shall be summed up *default:* **300.00**|
|tl|string|The traffic light that triggers aggregation when switching|
|name|string|Optional name for multiLaneAreaDetector|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|nextEdges|list of strings|List of edge ids that must all be part of the future route of the vehicle to qualify for detection|
|detectPersons|discrete string|Detect persons instead of vehicles (pedestrians or passengers) *default:* **none**|
|timeThreshold|SUMOTime|The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) *default:* **1.00**|
|speedThreshold|non-negative float|The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s *default:* **1.39**|
|jamThreshold|non-negative float|The maximum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam in m *default:* **10.00**|
|show|boolean|Show detector in sumo-gui *default:* **1**|

## entryExitDetector
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of entryExitDetector|
|pos|unique position|X-Y position of detector in editor (Only used in netedit) *default:* **0.00,0.00**|
|name|string|Optional name for entryExitDetector|
|period|SUMOTime|The aggregation period the values the entryExitDetector detector collects shall be summed up *default:* **300.00**|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|nextEdges|list of strings|List of edge ids that must all be part of the future route of the vehicle to qualify for detection|
|detectPersons|discrete string|Detect persons instead of vehicles (pedestrians or passengers) *default:* **none**|
|openEntry|boolean|If set to true, no error will be reported if vehicles leave the detector without first entering it *default:* **0**|
|timeThreshold|SUMOTime|The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) *default:* **1.00**|
|speedThreshold|non-negative float|The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s *default:* **1.39**|
|expectArrival|boolean|Whether no warning should be issued when a vehicle arrives within the detector area. *default:* **0**|

### detEntry
child element of [entryExitDetector](#entryexitdetector)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the detEntry shall be located at|
|pos|unique float|The position on the lane the detEntry shall be laid on in meters *default:* **0.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

### detExit
child element of [entryExitDetector](#entryexitdetector)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the detExit shall be located at|
|pos|unique float|The position on the lane the detExit shall be laid on in meters *default:* **0.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

## edgeData
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of edgeData|
|file|filename|The path to the output file|
|period|SUMOTime|The aggregation period the values the detector collects shall be summed up *default:* **-1.00**|
|begin|SUMOTime|The time to start writing. If not given, the simulation's begin is used. *default:* **-1.00**|
|end|SUMOTime|The time to end writing. If not given the simulation's end is used. *default:* **-1.00**|
|excludeEmpty|discrete string|If set to true, edges/lanes which were not used by a vehicle during this period will not be written *default:* **false**|
|withInternal|boolean|If set, junction internal edges/lanes will be written as well *default:* **0**|
|maxTraveltime|float|The maximum travel time in seconds to write if only very small movements occur *default:* **100000.00**|
|minSamples|float|Consider an edge/lane unused if it has at most this many sampled seconds *default:* **0.00**|
|speedThreshold|float|The maximum speed to consider a vehicle halting; *default:* **0.10**|
|vTypes|list of strings|space separated list of vehicle type ids to consider|
|trackVehicles|boolean|whether aggregation should be performed over all vehicles that entered the edge/lane in the aggregation interval *default:* **0**|
|detectPersons|discrete string|Detect persons instead of vehicles (pedestrians or passengers) *default:* **none**|
|writeAttributes|list of strings|List of attribute names that shall be written|
|edges|list of strings|Restrict output to the given list of edge ids|
|edgesFile|filename(Existent)|Restrict output to the given list of edges given in file|
|aggregate|boolean|Whether the traffic statistic of all edges shall be aggregated into a single value *default:* **0**|

## laneData
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of laneData|
|file|filename|The path to the output file|
|period|SUMOTime|The aggregation period the values the detector collects shall be summed up *default:* **-1.00**|
|begin|SUMOTime|The time to start writing. If not given, the simulation's begin is used. *default:* **-1.00**|
|end|SUMOTime|The time to end writing. If not given the simulation's end is used. *default:* **-1.00**|
|excludeEmpty|discrete string|If set to true, edges/lanes which were not used by a vehicle during this period will not be written *default:* **false**|
|withInternal|boolean|If set, junction internal edges/lanes will be written as well *default:* **0**|
|maxTraveltime|float|The maximum travel time in seconds to write if only very small movements occur *default:* **100000.00**|
|minSamples|float|Consider an edge/lane unused if it has at most this many sampled seconds *default:* **0.00**|
|speedThreshold|float|The maximum speed to consider a vehicle halting; *default:* **0.10**|
|vTypes|list of strings|space separated list of vehicle type ids to consider|
|trackVehicles|boolean|whether aggregation should be performed over all vehicles that entered the edge/lane in the aggregation interval *default:* **0**|
|detectPersons|discrete string|Detect persons instead of vehicles (pedestrians or passengers) *default:* **none**|
|writeAttributes|list of strings|List of attribute names that shall be written|
|edges|list of strings|Restrict output to the given list of edge ids|
|edgesFile|filename(Existent)|Restrict output to the given list of edges given in file|
|aggregate|boolean|Whether the traffic statistic of all edges shall be aggregated into a single value *default:* **0**|

## instantInductionLoop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of instantInductionLoop|
|lane|unique string|The name of the lane the instantInductionLoop shall be located at|
|pos|unique float|The position on the lane the instantInductionLoop shall be laid on in meters *default:* **0.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|name|string|Optional name for instantInductionLoop|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|nextEdges|list of strings|List of edge ids that must all be part of the future route of the vehicle to qualify for detection|
|detectPersons|discrete string|Detect persons instead of vehicles (pedestrians or passengers) *default:* **none**|

## routeProbe
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of routeProbe|
|edge|unique string|The id of an edge in the simulation network|
|name|string|Optional name for routeProbe|
|period|SUMOTime|The frequency in which to report the distribution *default:* **3600.00**|
|file|filename|The path to the output file|
|begin|SUMOTime|The time at which to start generating output *default:* **0.00**|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|

## calibrator
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of calibrator|
|edge|unique string|The id of an edge in the simulation network|
|pos|unique float|The position on the lane the calibrator shall be laid on in meters *default:* **0.00**|
|name|string|Optional name for calibrator|
|period|SUMOTime|The aggregation interval in which to calibrate the flows. Default is step-length *default:* **1.00**|
|routeProbe|string|The id of the routeProbe element from which to determine the route distribution for generated vehicles|
|output|filename|Path to the output file for writing information|
|jamThreshold|non-negative float|A threshold value to detect and clear unexpected jamming *default:* **0.50**|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|

## calibrator (calibratorLane)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of calibratorLane|
|lane|unique string|The name of the lane the calibratorLane shall be located at|
|pos|unique float|The position on the lane the calibratorLane shall be laid on in meters *default:* **0.00**|
|name|string|Optional name for calibratorLane|
|period|SUMOTime|The aggregation interval in which to calibrate the flows. Default is step-length *default:* **1.00**|
|routeProbe|string|The id of the routeProbe element from which to determine the route distribution for generated vehicles|
|output|filename|Path to the output file for writing information|
|jamThreshold|non-negative float|A threshold value to detect and clear unexpected jamming *default:* **0.50**|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|

## flow (calibratorFlow)
| Attribute | Type | Description |
|-----------|------|-------------|
|route|unique string|The id of the route the vehicle shall drive along|
|begin|SUMOTime|First calibrator flow departure time *default:* **0.00**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|color|color|The RGBA color with which the calibratorFlow shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|type|string|The id of the vehicle type to use for this calibrator flow *default:* **DEFAULT_VEHTYPE**|
|vehsPerHour|integer|Number of vehicles per hour, equally spaced *default:* **1800**|
|speed|float|Vehicle's speed *default:* **15.00**|

## rerouter
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of rerouter|
|pos|unique position|X,Y position in editor (Only used in netedit) *default:* **0.00,0.00**|
|edges|list of unique strings|An edge id or a list of edge ids where vehicles shall be rerouted|
|name|string|Optional name for rerouter|
|probability|probability[0, 1]|The probability for vehicle rerouting (0-1) *default:* **1.00**|
|timeThreshold|SUMOTime|The waiting time threshold (in s) that must be reached to activate rerouting (default -1 which disables the threshold) *default:* **0.00**|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|off|boolean|Whether the router should be inactive initially (and switched on in the gui) *default:* **0**|
|optional|boolean|If rerouter is optional *default:* **0**|

### rerouterSymbol
child element of [rerouterSymbol](#reroutersymbol)

| Attribute | Type | Description |
|-----------|------|-------------|

### interval
child element of [rerouter](#rerouter)

| Attribute | Type | Description |
|-----------|------|-------------|
|begin|unique SUMOTime|Begin *default:* **0.00**|
|end|unique SUMOTime|End *default:* **3600.00**|

### destProbReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of an edge in the simulation network|
|probability|non-negative float|SUMO Probability *default:* **1.00**|

### closingReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of an edge in the simulation network|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|

### closingLaneReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the closingLaneReroute shall be located at|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|

### routeProbReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|route|unique string|Route|
|probability|non-negative float|SUMO Probability *default:* **1.00**|

### parkingAreaReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|parking|unique string|ParkingArea ID|
|probability|non-negative float|SUMO Probability *default:* **1.00**|
|visible|boolean|Enable or disable visibility for parking area reroutes *default:* **1**|

## variableSpeedSign
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of variableSpeedSign|
|lanes|list of unique strings|List of Variable Speed Sign lanes|
|pos|unique position|X-Y position of detector in editor (Only used in netedit) *default:* **0.00,0.00**|
|name|string|Optional name for variableSpeedSign|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|

### variableSpeedSignSymbol
child element of [variableSpeedSign](#variablespeedsign)

| Attribute | Type | Description |
|-----------|------|-------------|

### step
child element of [variableSpeedSign](#variablespeedsign)

| Attribute | Type | Description |
|-----------|------|-------------|
|time|unique SUMOTime|Time|
|speed|float|Speed *default:* **13.89**|

## vaporizer
| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of an edge in the simulation network|
|name|string|Optional name for vaporizer|
|begin|SUMOTime|Start Time *default:* **0.00**|
|end|SUMOTime|End Time *default:* **3600.00**|

## tractionSubstation
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of tractionSubstation|
|pos|unique position|X-Y position of detector in editor (Only used in netedit) *default:* **0.00,0.00**|
|voltage|non-negative float|Voltage of at connection point for the overhead wire *default:* **600.00**|
|currentLimit|non-negative float|Current limit of the feeder line *default:* **400.00**|

## overheadWire
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of overheadWire|
|substationId|unique string|Substation to which the circuit is connected|
|lanes|list of unique strings|List of consecutive lanes of the circuit|
|startPos|non-negative unique float|Starting position in the specified lane|
|endPos|non-negative unique float|Ending position in the specified lane|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|forbiddenInnerLanes|list of strings|Inner lanes, where placing of overhead wire is restricted|

## overheadWireClamp
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of overheadWireClamp|
|wireClampStart|unique string|ID of the overhead wire segment, to the start of which the overhead wire clamp is connected|
|wireClampLaneStart|unique string|ID of the overhead wire segment lane of overheadWireIDStartClamp|
|wireClampEnd|unique string|ID of the overhead wire segment, to the end of which the overhead wire clamp is connected|
|wireClampLaneEnd|unique string|ID of the overhead wire segment lane of overheadWireIDEndClamp|

## trip
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of trip|
|type|string|The id of the vehicle type to use for this trip *default:* **DEFAULT_VEHTYPE**|
|from|unique string|The ID of the edge the trip starts at|
|to|unique string|The ID of the edge the trip ends at|
|via|list of unique strings|List of intermediate edge ids which shall be part of the trip|
|color|color|The RGBA color with which the trip shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|depart|string|The departure time of the (first) trip which is generated using this trip definition *default:* **0**|

## trip (tripJunctions)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of tripJunctions|
|type|string|The id of the vehicle type to use for this trip *default:* **DEFAULT_VEHTYPE**|
|fromJunction|unique string|The name of the junction the trip starts at|
|toJunction|unique string|The name of the junction the trip ends at|
|color|color|The RGBA color with which the tripJunctions shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|depart|string|The departure time of the (first) tripJunctions which is generated using this trip definition *default:* **0**|

## trip (tripTAZs)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of tripTAZs|
|type|string|The id of the vehicle type to use for this trip *default:* **DEFAULT_VEHTYPE**|
|fromTaz|unique string|The name of the TAZ the trip starts at|
|toTaz|unique string|The name of the TAZ the trip ends at|
|color|color|The RGBA color with which the tripTAZs shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|depart|string|The departure time of the (first) tripTAZs which is generated using this trip definition *default:* **0**|

## vehicle
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of vehicle|
|type|string|The id of the vehicle type to use for this vehicle *default:* **DEFAULT_VEHTYPE**|
|route|unique string|The id of the route the vehicle shall drive along|
|departEdge|unique string|The index of the edge within route the vehicle starts at|
|arrivalEdge|unique string|The index of the edge within route the vehicle ends at|
|color|color|The RGBA color with which the vehicle shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|depart|string|The departure time of the (first) vehicle which is generated using this trip definition *default:* **0**|

## vehicle (vehicleWithRoute)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of vehicleWithRoute|
|type|string|The id of the vehicle type to use for this vehicle *default:* **DEFAULT_VEHTYPE**|
|departEdge|unique string|The index of the edge within route the vehicle starts at|
|arrivalEdge|unique string|The index of the edge within route the vehicle ends at|
|color|color|The RGBA color with which the vehicleWithRoute shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|depart|string|The departure time of the (first) vehicleWithRoute which is generated using this trip definition *default:* **0**|

## flow
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of flow|
|type|string|The id of the flow type to use for this flow *default:* **DEFAULT_VEHTYPE**|
|from|unique string|The ID of the edge the flow starts at|
|to|unique string|The ID of the edge the flow ends at|
|via|list of unique strings|List of intermediate edge ids which shall be part of the flow|
|color|color|The RGBA color with which the flow shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|begin|SUMOTime|First flow departure time *default:* **0.00**|
|terminate|discrete string|Criterion for flow termination *default:* **end**|
|spacing|discrete string|Criterion for flow spacing *default:* **vehsPerHour**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|vehsPerHour|float|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800.00**|
|period|float|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2.00**|
|probability|float|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.50**|
|poisson|float|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.50**|

## flow (flowJunctions)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of flowJunctions|
|type|string|The id of the flow type to use for this flow *default:* **DEFAULT_VEHTYPE**|
|fromJunction|unique string|The name of the junction the flow starts at|
|toJunction|unique string|The name of the junction the flow ends at|
|color|color|The RGBA color with which the flowJunctions shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|begin|SUMOTime|First flow departure time *default:* **0.00**|
|terminate|discrete string|Criterion for flow termination *default:* **end**|
|spacing|discrete string|Criterion for flow spacing *default:* **vehsPerHour**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|vehsPerHour|float|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800.00**|
|period|float|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2.00**|
|probability|float|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.50**|
|poisson|float|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.50**|

## flow (flowTAZs)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of flowTAZs|
|type|string|The id of the flow type to use for this flow *default:* **DEFAULT_VEHTYPE**|
|fromTaz|unique string|The name of the TAZ the flow starts at|
|toTaz|unique string|The name of the TAZ the flow ends at|
|color|color|The RGBA color with which the flowTAZs shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|begin|SUMOTime|First flow departure time *default:* **0.00**|
|terminate|discrete string|Criterion for flow termination *default:* **end**|
|spacing|discrete string|Criterion for flow spacing *default:* **vehsPerHour**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|vehsPerHour|float|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800.00**|
|period|float|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2.00**|
|probability|float|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.50**|
|poisson|float|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.50**|

## vType
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of vType|
|vClass|discrete |An abstract vehicle class *default:* **passenger**|
|color|color|The RGBA color with which the vType shall be displayed *default:* **invisible**|
|length|non-negative float|The vehicle's netto-length (length) [m]|
|minGap|non-negative float|Empty space after leader [m]|
|maxSpeed|non-negative float|The vehicle's maximum velocity [m/s]|
|speedFactor|string|The vehicle's expected multiplicator for lane speed limits (or a distribution specifier)|
|desiredMaxSpeed|non-negative float|The vehicle's desired maximum velocity (interacts with speedFactor). Applicable when no speed limit applies (bicycles, some motorways) [m/s]|
|emissionClass|discrete string|An abstract emission class|
|guiShape|discrete string|How this vehicle is rendered|
|width|non-negative float|The vehicle's width [m] (only used for drawing) *default:* **1.80**|
|height|non-negative float|The vehicle's height [m] (only used for drawing) *default:* **1.50**|
|parkingBadges|list of strings|The parking badges assigned to the vehicle|
|imgFile|filename(Existent)|A bitmap to use for rendering this vType|
|laneChangeModel|discrete string|The model used for changing lanes *default:* **default**|
|carFollowModel|discrete string|The model used for car-following *default:* **Krauss**|
|personCapacity|non-negative integer|The number of persons (excluding an autonomous driver) the vehicle can transport|
|containerCapacity|non-negative integer|The number of containers the vehicle can transport|
|boardingDuration|SUMOTime|The time required by a person to board the vehicle *default:* **0.50**|
|loadingDuration|SUMOTime|The time required to load a container onto the vehicle *default:* **90.00**|
|latAlignment|discrete string|The preferred lateral alignment when using the sublane-model *default:* **center**|
|minGapLat|non-negative float|The minimum lateral gap at a speed difference of 50km/h when using the sublane-model *default:* **0.12**|
|maxSpeedLat|non-negative float|The maximum lateral speed when using the sublane-model *default:* **1.00**|
|actionStepLength|non-negative SUMOTime|The interval length for which vehicle performs its decision logic (acceleration and lane-changing) *default:* **0.00**|
|probability|non-negative float|The probability when being added to a distribution without an explicit probability *default:* **1.00**|
|osgFile|string|3D model file for this class|
|accel|non-negative float|The acceleration ability of vehicles of this type [m/s^2] *default:* **2.60**|
|decel|non-negative float|The deceleration ability of vehicles of this type [m/s^2] *default:* **4.50**|
|apparentDecel|non-negative float|The apparent deceleration of the vehicle as used by the standard model [m/s^2] *default:* **4.50**|
|emergencyDecel|non-negative float|The maximal physically possible deceleration for the vehicle [m/s^2] *default:* **9.00**|
|sigma|float|Car-following model parameter *default:* **0.50**|
|tau|non-negative float|Car-following model parameter *default:* **1.00**|
|tmp1|string|SKRAUSSX parameter 1|
|tmp2|string|SKRAUSSX parameter 2|
|tmp3|string|SKRAUSSX parameter 3|
|tmp4|string|SKRAUSSX parameter 4|
|tmp5|string|SKRAUSSX parameter 5|
|tpreview|non-negative float|EIDM Look ahead / preview parameter [s] *default:* **4.00**|
|treaction|non-negative float|EIDM AP Reaction Time parameter [s] *default:* **0.50**|
|tPersDrive|non-negative float|EIDM Wiener Process parameter for the Driving Error [s] *default:* **3.00**|
|tPersEstimate|non-negative float|EIDM Wiener Process parameter for the Estimation Error [s] *default:* **10.00**|
|ccoolness|float|EIDM Coolness parameter of the Enhanced IDM [-] *default:* **0.99**|
|sigmaleader|non-negative float|EIDM leader speed estimation error parameter [-] *default:* **0.02**|
|sigmagap|non-negative float|EIDM gap estimation error parameter [-] *default:* **0.10**|
|sigmaerror|non-negative float|EIDM driving error parameter [-] *default:* **0.04**|
|jerkmax|non-negative float|EIDM maximal jerk parameter [m/s^3] *default:* **3.00**|
|epsilonacc|non-negative float|EIDM maximal negative acceleration between two Action Points (threshold) [m/s^2] *default:* **1.00**|
|taccmax|non-negative float|EIDM Time parameter until vehicle reaches amax after startup/driveoff [s] *default:* **1.20**|
|Mflatness|non-negative float|EIDM Flatness parameter of startup/driveoff curve [-] *default:* **2.00**|
|Mbegin|non-negative float|EIDM Shift parameter of startup/driveoff curve [-] *default:* **0.70**|
|vehdynamics|boolean|EIDM parameter if model shall include vehicle dynamics into the acceleration calculation [0/1] *default:* **0**|
|maxvehpreview|non-negative integer|EIDM parameter how many vehicles are taken into the preview calculation of the driver (at least always 1!) [-] *default:* **0**|
|tauLast|non-negative float|Peter Wagner 2009 parameter *default:* **0.00**|
|apProb|non-negative float|Peter Wagner 2009 parameter *default:* **0.00**|
|adaptFactor|non-negative float|IDMM parameter *default:* **0.00**|
|adaptTime|non-negative float|IDMM parameter *default:* **0.00**|
|cc1|non-negative float|W99 parameter *default:* **1.30**|
|cc2|non-negative float|W99 parameter *default:* **8.00**|
|cc3|non-negative float|W99 parameter *default:* **-12.00**|
|cc4|non-negative float|W99 parameter *default:* **-0.25**|
|cc5|non-negative float|W99 parameter *default:* **0.35**|
|cc6|non-negative float|W99 parameter *default:* **6.00**|
|cc7|non-negative float|W99 parameter *default:* **0.25**|
|cc8|non-negative float|W99 parameter *default:* **2.00**|
|cc9|non-negative float|W99 parameter *default:* **1.50**|
|security|string|Wiedemann parameter|
|estimation|string|Wiedemann parameter|
|collisionMinGapFactor|string|MinGap factor parameter|
|k|string|K parameter|
|phi|string|Kerner Phi parameter|
|delta|string|IDM Delta parameter|
|stepping|string|IDM Stepping parameter|
|trainType|discrete string|Train Types *default:* **NGT400**|
|jmCrossingGap|non-negative float|Minimum distance to pedestrians that are walking towards the conflict point with the ego vehicle. *default:* **10.00**|
|jmIgnoreKeepClearTime|float|The accumulated waiting time after which a vehicle will drive onto an intersection even though this might cause jamming.|
|jmDriveAfterYellowTime|non-negative float|This value causes vehicles to violate a yellow light if the duration of the yellow phase is lower than the given threshold.|
|jmDriveAfterRedTime|float|This value causes vehicles to violate a red light if the duration of the red phase is lower than the given threshold.|
|jmDriveRedSpeed|float|This value causes vehicles affected by jmDriveAfterRedTime to slow down when violating a red light. *default:* **0.00**|
|jmIgnoreFoeProb|non-negative float|This value causes vehicles to ignore foe vehicles that have right-of-way with the given probability. *default:* **0.00**|
|jmIgnoreFoeSpeed|non-negative float|This value is used in conjunction with jmIgnoreFoeProb. Only vehicles with a speed below or equal to the given value may be ignored. *default:* **0.00**|
|jmSigmaMinor|float|This value configures driving imperfection (dawdling) while passing a minor link. *default:* **0.00**|
|jmTimegapMinor|non-negative float|This value defines the minimum time gap when passing ahead of a prioritized vehicle.  *default:* **1.00**|
|impatience|non-negative float|Willingess of drivers to impede vehicles with higher priority *default:* **0.00**|
|lcStrategic|non-negative float|The eagerness for performing strategic lane changing. Higher values result in earlier lane-changing. *default:* **1.00**|
|lcCooperative|non-negative float|The willingness for performing cooperative lane changing. Lower values result in reduced cooperation. *default:* **1.00**|
|lcSpeedGain|non-negative float|The eagerness for performing lane changing to gain speed. Higher values result in more lane-changing. *default:* **1.00**|
|lcKeepRight|non-negative float|The eagerness for following the obligation to keep right. Higher values result in earlier lane-changing. *default:* **1.00**|
|lcSublane|non-negative float|The eagerness for using the configured lateral alignment within the lane. Higher values result in increased willingness to sacrifice speed for alignment. *default:* **1.00**|
|lcOpposite|non-negative float|The eagerness for overtaking through the opposite-direction lane. Higher values result in more lane-changing. *default:* **1.00**|
|lcPushy|non-negative float|Willingness to encroach laterally on other drivers. *default:* **0.00**|
|lcPushyGap|non-negative float|Minimum lateral gap when encroaching laterally on other drives (alternative way to define lcPushy) *default:* **0.00**|
|lcAssertive|non-negative float|Willingness to accept lower front and rear gaps on the target lane. *default:* **1.00**|
|lcImpatience|non-negative float|Dynamic factor for modifying lcAssertive and lcPushy. *default:* **0.00**|
|lcTimeToImpatience|string|Time to reach maximum impatience (of 1). Impatience grows whenever a lane-change manoeuvre is blocked. *default:* **infinity**|
|lcAccelLat|non-negative float|Maximum lateral acceleration per second. *default:* **1.00**|
|lcLookaheadLeft|non-negative float|Factor for configuring the strategic lookahead distance when a change to the left is necessary (relative to right lookahead). *default:* **2.00**|
|lcSpeedGainRight|non-negative float|Factor for configuring the threshold asymmetry when changing to the left or to the right for speed gain. *default:* **0.10**|
|lcMaxSpeedLatStanding|float|Upper bound on lateral speed when standing. *default:* **0.00**|
|lcMaxSpeedLatFactor|non-negative float|Upper bound on lateral speed while moving computed as lcMaxSpeedLatStanding + lcMaxSpeedLatFactor * getSpeed() *default:* **1.00**|
|lcTurnAlignmentDistance|non-negative float|Distance to an upcoming turn on the vehicles route, below which the alignment should be dynamically adapted to match the turn direction. *default:* **0.00**|
|lcOvertakeRight|non-negative float|The probability for violating rules gainst overtaking on the right. *default:* **0.00**|
|lcKeepRightAcceptanceTime|float|Time threshold for the willingness to change right.|
|lcOvertakeDeltaSpeedFactor|float|Speed difference factor for the eagerness of overtaking a neighbor vehicle before changing lanes (threshold = factor*speedlimit). *default:* **0.00**|

### vTypeRef
child element of [vTypeDistribution](#vtypedistribution)

| Attribute | Type | Description |
|-----------|------|-------------|
|typeDist.|unique string|VType distribution in which this vTypeRef is defined|
|refId|unique string|Reference ID of vType|
|probability|non-negative float|The probability when being added to a distribution without an explicit probability *default:* **1.00**|

## route
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of route|
|edges|list of unique strings|The edges the vehicle shall drive along, given as their ids, separated using spaces|
|color|color|The RGBA color with which the route shall be displayed *default:* **invisible**|
|repeat|non-negative integer|The number of times that the edges of this route shall be repeated *default:* **0**|
|cycleTime|SUMOTime|When defining a repeating route with stops and those stops use the until attribute, the times will be shifted forward by 'cycleTime' on each repeat *default:* **0.00**|

### routeRef
child element of [routeDistribution](#routedistribution)

| Attribute | Type | Description |
|-----------|------|-------------|
|routeDist.|unique string|Route distribution in which this routeRef is defined|
|refId|unique string|Reference ID of route|
|probability|non-negative float|The probability when being added to a distribution without an explicit probability *default:* **1.00**|

## route (routeEmbedded)
| Attribute | Type | Description |
|-----------|------|-------------|
|edges|list of unique strings|The edges the vehicle shall drive along, given as their ids, separated using spaces|
|color|color|The RGBA color with which the routeEmbedded shall be displayed *default:* **invisible**|
|repeat|non-negative integer|The number of times that the edges of this route shall be repeated *default:* **0**|
|cycleTime|SUMOTime|When defining a repeating route with stops and those stops use the until attribute, the times will be shifted forward by 'cycleTime' on each repeat *default:* **0.00**|

## taz
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of taz|
|shape|list of unique positions|The shape of the TAZ|
|center|position|TAZ center|
|name|string|Optional name for taz|
|color|color|The RGBA color with which the taz shall be displayed *default:* **red**|
|fill|boolean|An information whether the TAZ shall be filled *default:* **0**|

### tazSource
child element of [taz](#taz)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of an edge in the simulation network|
|weight|non-negative float|Depart weight associated to this Edge *default:* **1.00**|

### tazSink
child element of [taz](#taz)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of an edge in the simulation network|
|weight|non-negative float|Arrival weight associated to this Edge *default:* **1.00**|

## edgeRelation
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the edgeRel starts at|
|to|unique string|The ID of the edge the edgeRel ends at|

### tazRelation
child element of [dataInterval](#datainterval)

| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The name of the TAZ the TAZRel starts at|
|to|unique string|The name of the TAZ the TAZRel ends at|

## stop (stopLane)
| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the stopLane shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|posLat|string|The lateral offset on the named lane at which the vehicle must stop|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|triggered|discrete string|Whether a person or container or both may end the stop *default:* **false**|
|expected|list of strings|List of elements that must board the vehicle before it may continue|
|join|string|Joins this train to another upon reaching the stop|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|onDemand|boolean|Whether the stop may be skipped if no passengers wants to embark or disembark *default:* **0**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (stopBusStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|busStop|list of unique strings|BusStop associated with this stop|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|triggered|discrete string|Whether a person or container or both may end the stop *default:* **false**|
|expected|list of strings|List of elements that must board the vehicle before it may continue|
|join|string|Joins this train to another upon reaching the stop|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|onDemand|boolean|Whether the stop may be skipped if no passengers wants to embark or disembark *default:* **0**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (stopTrainStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|trainStop|list of unique strings|TrainStop associated with this stop|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|triggered|discrete string|Whether a person or container or both may end the stop *default:* **false**|
|expected|list of strings|List of elements that must board the vehicle before it may continue|
|join|string|Joins this train to another upon reaching the stop|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|onDemand|boolean|Whether the stop may be skipped if no passengers wants to embark or disembark *default:* **0**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (stopContainerStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|containerStop|list of unique strings|ContainerStop associated with this stop|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|triggered|discrete string|Whether a person or container or both may end the stop *default:* **false**|
|expected|list of strings|List of elements that must board the vehicle before it may continue|
|join|string|Joins this train to another upon reaching the stop|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|onDemand|boolean|Whether the stop may be skipped if no passengers wants to embark or disembark *default:* **0**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (stopChargingStation)
| Attribute | Type | Description |
|-----------|------|-------------|
|chargingStation|list of unique strings|ChargingStation associated with this stop|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|triggered|discrete string|Whether a person or container or both may end the stop *default:* **false**|
|expected|list of strings|List of elements that must board the vehicle before it may continue|
|join|string|Joins this train to another upon reaching the stop|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|onDemand|boolean|Whether the stop may be skipped if no passengers wants to embark or disembark *default:* **0**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (stopParkingArea)
| Attribute | Type | Description |
|-----------|------|-------------|
|parkingArea|list of unique strings|ParkingArea associated with this stop|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|triggered|discrete string|Whether a person or container or both may end the stop *default:* **false**|
|expected|list of strings|List of elements that must board the vehicle before it may continue|
|join|string|Joins this train to another upon reaching the stop|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|onDemand|boolean|Whether the stop may be skipped if no passengers wants to embark or disembark *default:* **0**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## connection
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the vehicles leave|
|to|unique string|The ID of the edge the vehicles may reach when leaving 'from'|
|fromLane|unique integer|the lane index of the incoming lane (numbers starting with 0)|
|toLane|unique integer|the lane index of the outgoing lane (numbers starting with 0)|
|pass|boolean|if set, vehicles which pass this (lane-2-lane) connection) will not wait *default:* **0**|
|keepClear|boolean|if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection *default:* **0**|
|contPos|non-negative float|If set to a more than 0 value, an internal junction will be built at this position (in m)/n from the start of the internal lane for this connection|
|uncontrolled|boolean|If set to true, This connection will not be TLS-controlled despite its node being controlled *default:* **0**|
|visibility|non-negative float|Vision distance between vehicles|
|linkIndex|integer|sets index of this connection within the controlling traffic light (-1 means automatic assignment) *default:* **-1**|
|linkIndex2|integer|sets index for the internal junction of this connection within the controlling traffic light (-1 means internal junction not controlled) *default:* **-1**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|speed|non-negative float|sets custom speed limit for the connection|
|length|non-negative float|sets custom length for the connection|
|customShape|list of unique positions|sets custom shape for the connection|
|changeLeft|list of vClasses|Permit changing left only for to the given vehicle classes *default:* **all**|
|changeRight|list of vClasses|Permit changing right only for to the given vehicle classes *default:* **all**|
|indirect|boolean|if set to true, vehicles will make a turn in 2 steps *default:* **0**|
|type|string|set a custom edge type (for applying vClass-specific speed restrictions)|
|dir|string|turning direction for this connection (computed)|
|state|string|link state for this connection (computed)|

## type
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of type|
|numLanes|non-negative integer|The number of lanes of the edge *default:* **1**|
|speed|non-negative float|The maximum speed allowed on the edge in m/s *default:* **13.89**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|spreadType|discrete string|The spreadType defines how to compute the lane geometry from the edge geometry (used for visualization) *default:* **right**|
|priority|integer|The priority of the edge *default:* **-1**|
|width|float|Lane width for all lanes of this edge in meters (used for visualization)|
|sidewalkWidth|float|The width of the sidewalk that should be added as an additional lane|
|bikeLaneWidth|float|The width of the bike lane that should be added as an additional lane|

## laneType
| Attribute | Type | Description |
|-----------|------|-------------|
|speed|non-negative float|The maximum speed allowed on the lane in m/s *default:* **13.89**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|width|non-negative float|Lane width for all lanes of this type in meters (used for visualization) *default:* **3.20**|

## routeDistribution
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of routeDistribution|

## vTypeDistribution
| Attribute | Type | Description |
|-----------|------|-------------|
|deterministic|non-negative integer|Deterministic distribution *default:* **-1**|
|id|unique string|ID of vTypeDistribution|

## crossing
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of crossing|
|edges|list of unique strings|The (road) edges which are crossed|
|priority|boolean|Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections) *default:* **0**|
|width|non-negative float|The width of the crossings *default:* **4.00**|
|linkIndex|integer|sets the tls-index for this crossing (-1 means automatic assignment) *default:* **-1**|
|linkIndex2|integer|sets the opposite-direction tls-index for this crossing (-1 means not assigned) *default:* **-1**|
|customShape|list of unique positions|Overrides default shape of pedestrian crossing|

## walkingArea
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of walkingArea|
|width|non-negative float|The width of the WalkingArea *default:* **2.00**|
|length|non-negative float|The length of the WalkingArea in meter|
|shape|list of unique positions|Overrides default shape of pedestrian sidewalk|

## person
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of person|
|type|string|The id of the person type to use for this person *default:* **DEFAULT_VEHTYPE**|
|color|color|The RGBA color with which the person shall be displayed *default:* **yellow**|
|departPos|string|The position at which the person shall enter the net *default:* **base**|
|depart|string|The departure time of the (first) person which is generated using this trip definition *default:* **0**|

## personFlow
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of personFlow|
|type|string|The id of the person type to use for this person *default:* **DEFAULT_VEHTYPE**|
|color|color|The RGBA color with which the personFlow shall be displayed *default:* **yellow**|
|departPos|string|The position at which the person shall enter the net *default:* **base**|
|begin|SUMOTime|First flow departure time *default:* **0.00**|
|terminate|discrete string|Criterion for flow termination *default:* **end**|
|spacing|discrete string|Criterion for flow spacing *default:* **personsPerHour**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|personsPerHour|float|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800.00**|
|period|float|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2.00**|
|probability|float|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.50**|
|poisson|float|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.50**|

## dataSet
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of dataSet|

### dataInterval
child element of [dataSet](#dataset)

| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of dataInterval|
|begin|SUMOTime|Data interval begin time *default:* **0.00**|
|end|SUMOTime|Data interval end time *default:* **3600.00**|

## container
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of container|
|type|string|The id of the container type to use for this container *default:* **DEFAULT_CONTAINERTYPE**|
|color|color|The RGBA color with which the container shall be displayed *default:* **yellow**|
|departPos|string|The position at which the container shall enter the net *default:* **base**|
|depart|string|The departure time of the (first) container which is generated using this trip definition *default:* **0**|

## containerFlow
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of containerFlow|
|type|string|The id of the container type to use for this container *default:* **DEFAULT_CONTAINERTYPE**|
|color|color|The RGBA color with which the containerFlow shall be displayed *default:* **yellow**|
|departPos|string|The position at which the container shall enter the net *default:* **base**|
|begin|SUMOTime|First flow departure time *default:* **0.00**|
|terminate|discrete string|Criterion for flow termination *default:* **end**|
|spacing|discrete string|Criterion for flow spacing *default:* **containersPerHour**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|containersPerHour|float|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800.00**|
|period|float|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2.00**|
|probability|float|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.50**|
|poisson|float|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.50**|

## edge (edgeData-edge)
| Attribute | Type | Description |
|-----------|------|-------------|

## internalLane
| Attribute | Type | Description |
|-----------|------|-------------|

## poi (poiLane)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of poiLane|
|lane|unique string|The name of the lane the poiLane shall be located at|
|pos|unique float|The position on the lane the poiLane shall be laid on in meters *default:* **0.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|posLat|float|The lateral offset on the named lane at which the POI is located at *default:* **0.00**|
|name|string|Optional name for poiLane|
|color|color|The RGBA color with which the poiLane shall be displayed *default:* **red**|
|type|string|A typename for the POI|
|icon|discrete string|POI Icon|
|layer|float|The layer of the POI for drawing and selecting *default:* **202.00**|
|width|non-negative float|Width of rendered image in meters *default:* **2.60**|
|height|non-negative float|Height of rendered image in meters *default:* **1.00**|
|imgFile|filename(Existent)|A bitmap to use for rendering this poiLane|
|angle|angle[0, 360]|Angle of rendered image in degree *default:* **0.00**|

## poi (poiGeo)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of poiGeo|
|lon|unique float|The longitude position of the parking vehicle on the view|
|lat|unique float|The latitude position of the parking vehicle on the view|
|name|string|Optional name for poiGeo|
|color|color|The RGBA color with which the poiGeo shall be displayed *default:* **red**|
|type|string|A typename for the POI|
|icon|discrete string|POI Icon|
|layer|float|The layer of the POI for drawing and selecting *default:* **202.00**|
|width|non-negative float|Width of rendered image in meters *default:* **2.60**|
|height|non-negative float|Height of rendered image in meters *default:* **1.00**|
|imgFile|filename(Existent)|A bitmap to use for rendering this poiGeo|
|angle|angle[0, 360]|Angle of rendered image in degree *default:* **0.00**|

## poly (jupedsim.walkable_area)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of jupedsim.walkable_area|
|shape|list of unique positions|The shape of the walkable area|
|name|string|Optional name for jupedsim.walkable_area|
|geo|boolean|Enable or disable GEO attributes *default:* **0**|
|geoShape|list of unique positions|A custom geo shape for this walkable area|

## poly (jupedsim.obstacle)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of jupedsim.obstacle|
|shape|list of unique positions|The shape of the obstacle|
|name|string|Optional name for jupedsim.obstacle|
|geo|boolean|Enable or disable GEO attributes *default:* **0**|
|geoShape|list of unique positions|A custom geo shape for this obstacle|

## flow (flowRoute)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of flowRoute|
|type|string|The id of the flow type to use for this flow *default:* **DEFAULT_VEHTYPE**|
|route|unique string|The id of the route the flow shall drive along|
|departEdge|unique string|The index of the edge within route the flow starts at|
|arrivalEdge|unique string|The index of the edge within route the flow ends at|
|color|color|The RGBA color with which the flowRoute shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|begin|SUMOTime|First flow departure time *default:* **0.00**|
|terminate|discrete string|Criterion for flow termination *default:* **end**|
|spacing|discrete string|Criterion for flow spacing *default:* **vehsPerHour**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|vehsPerHour|float|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800.00**|
|period|float|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2.00**|
|probability|float|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.50**|
|poisson|float|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.50**|

## flow (flowWithRoute)
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|ID of flowWithRoute|
|type|string|The id of the flow type to use for this flow *default:* **DEFAULT_VEHTYPE**|
|departEdge|unique string|The index of the edge within route the flow starts at|
|arrivalEdge|unique string|The index of the edge within route the flow ends at|
|color|color|The RGBA color with which the flowWithRoute shall be displayed *default:* **yellow**|
|departLane|string|The lane on which the vehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|begin|SUMOTime|First flow departure time *default:* **0.00**|
|terminate|discrete string|Criterion for flow termination *default:* **end**|
|spacing|discrete string|Criterion for flow spacing *default:* **vehsPerHour**|
|end|SUMOTime|End of departure interval *default:* **3600.00**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|vehsPerHour|float|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800.00**|
|period|float|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2.00**|
|probability|float|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.50**|
|poisson|float|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.50**|

## stop (waypointLane)
| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the waypointLane shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|posLat|string|The lateral offset on the named lane at which the vehicle must waypoint|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|speed|non-negative float|Speed to be kept while driving between startPos and endPos *default:* **0.00**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (waypointBusStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|busStop|list of unique strings|BusWaypoint associated with this waypoint|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|speed|non-negative float|Speed to be kept while driving between startPos and endPos *default:* **0.00**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (waypointTrainStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|trainStop|list of unique strings|TrainWaypoint associated with this waypoint|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|speed|non-negative float|Speed to be kept while driving between startPos and endPos *default:* **0.00**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (waypointContainerStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|containerStop|list of unique strings|ContainerWaypoint associated with this waypoint|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|speed|non-negative float|Speed to be kept while driving between startPos and endPos *default:* **0.00**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (waypointChargingStation)
| Attribute | Type | Description |
|-----------|------|-------------|
|chargingStation|list of unique strings|ChargingStation associated with this waypoint|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|speed|non-negative float|Speed to be kept while driving between startPos and endPos *default:* **0.00**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## stop (waypointParkingArea)
| Attribute | Type | Description |
|-----------|------|-------------|
|parkingArea|list of unique strings|ParkingArea associated with this waypoint|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0.00**|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **false**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|speed|non-negative float|Speed to be kept while driving between startPos and endPos *default:* **0.00**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge|
|split|string|Splits the train upon reaching the stop|

## personTrip (persontrip: edge->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: edge->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: edge->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: edge->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: edge->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: edge->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: edge->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: edge->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: taz->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: junction->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: busstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: trainstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: containerstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->containestop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: chargingstation->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toTaz|unique string|TAZ end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toJunction|unique string|Junction end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|busStop|unique string|BusStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|trainStop|unique string|TrainStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|containerStop|unique string|ContainerStop end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|chargingStation|unique string|ChargingStation end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## personTrip (persontrip: parkingarea->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|parkingArea|unique string|ParkingArea end ID|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip|
|walkFactor|non-negative float|Walk factor *default:* **0.00**|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## walk (walk: edge->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: edge->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edge->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: edge->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edge->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: edge->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edge->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: edge->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edge->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: edge->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edge->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: edge->containerstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edge->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: edge->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edge->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: edge->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: taz->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: taz->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: taz->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: taz->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: taz->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: taz->containerstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: taz->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: taz->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: taz->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: junction->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: junction->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: junction->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: junction->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: junction->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: junction->containerstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: junction->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: junction->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: junction->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: busstop->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: busstop->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: busstop->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: busstop->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: busstop->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: busstop->containerstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: busstop->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: busstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: busstop->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: trainstop->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: trainstop->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: trainstop->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: trainstop->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: trainstop->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: trainstop->containerstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: trainstop->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: trainstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: trainstop->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: containerstop->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: containerstop->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: containerstop->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: containerstop->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: containerstop->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: containerstop->containerstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: containerstop->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: containerstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: containerstop->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: chargingstation->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: chargingstation->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: chargingstation->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: chargingstation->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: chargingstation->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->containestop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: chargingstation->containestop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: chargingstation->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: chargingstation->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: chargingstation->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: parkingarea->edge in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this walk: parkingarea->taz in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this walk: parkingarea->junction in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this walk: parkingarea->busstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this walk: parkingarea->trainstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this walk: parkingarea->containerstop in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this walk: parkingarea->chargingstation in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: parkingarea->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this walk: parkingarea->parkingarea in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: edges)
| Attribute | Type | Description |
|-----------|------|-------------|
|edges|list of unique strings|list of consecutive edges|
|arrivalPos|float|Arrival position on the last edge|
|speed|non-negative float|speed of the person for this walk: edges in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## walk (walk: route)
| Attribute | Type | Description |
|-----------|------|-------------|
|route|list of unique strings|Route ID|
|arrivalPos|float|Arrival position on the destination edge|
|speed|non-negative float|speed of the person for this walk: route in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## ride (ride: edge->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: edge->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: edge->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: edge->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: edge->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: edge->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: edge->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: edge->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: taz->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: junction->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: busstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: trainstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: containerstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->containestop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: chargingstation->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## ride (ride: parkingarea->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the ride|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## stop (stopPerson: edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of an edge in the simulation network|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

## stop (stopPerson: busStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|busStop|unique string|Bus stop ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopPerson: trainStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|trainStop|unique string|Train stop ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopPerson: containerStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|containerStop|unique string|Container stop ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopPerson: chargingStation)
| Attribute | Type | Description |
|-----------|------|-------------|
|chargingStation|unique string|Charging station ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopPerson: parkingArea)
| Attribute | Type | Description |
|-----------|------|-------------|
|parkingArea|unique string|Parking area ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## transport (transport: edge->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: edge->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: edge->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: edge->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: edge->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: edge->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: edge->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: edge->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: taz->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: junction->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: busstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: trainstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: containerstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->containestop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: chargingstation->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|to|unique string|Edge end ID|
|arrivalPos|float|arrival position on the destination edge|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toTaz|unique string|TAZ end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toJunction|unique string|Junction end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|busStop|unique string|BusStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|trainStop|unique string|TrainStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|containerStop|unique string|ContainerStop end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|chargingStation|unique string|ChargingStation end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## transport (transport: parkingarea->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|parkingArea|unique string|ParkingArea end ID|
|lines|list of strings|list of vehicle alternatives to take for the transport|
|group|string|id of the travel group. Persons with the same group may share a taxi ride|

## tranship (tranship: edge->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edge->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edge->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edge->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edge->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edge->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edge->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edge->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|Edge start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: taz->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTaz|unique string|TAZ start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: junction->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromJunction|unique string|Junction start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: busstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromBusStop|unique string|BusStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: trainstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromTrainStop|unique string|TrainStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: containerstop->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromContainerStop|unique string|ContainerStop start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->containestop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: chargingstation->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ChargingStation start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|to|unique string|Edge end ID|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0.00**|
|arrivalPos|float|arrival position on the destination edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->taz)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toTaz|unique string|TAZ end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->junction)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|toJunction|unique string|Junction end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->busstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|busStop|unique string|BusStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->trainstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|trainStop|unique string|TrainStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->containerstop)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|containerStop|unique string|ContainerStop end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->chargingstation)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|chargingStation|unique string|ChargingStation end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: parkingarea->parkingarea)
| Attribute | Type | Description |
|-----------|------|-------------|
|fromChargingStation|unique string|ParkingArea start ID|
|parkingArea|unique string|ParkingArea end ID|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## tranship (tranship: edges)
| Attribute | Type | Description |
|-----------|------|-------------|
|edges|list of unique strings|list of consecutive edges|
|arrivalPos|float|Arrival position on the last edge|
|speed|non-negative float|speed of the person for this tranship in m/s (not together with duration) *default:* **1.39**|
|duration|non-negative SUMOTime|duration of the plan in second (not together with speed) *default:* **0.00**|

## stop (stopContainer: edge)
| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of an edge in the simulation network|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

## stop (stopContainer: busStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|busStop|unique string|Bus stop ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopContainer: trainStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|trainStop|unique string|Train stop ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopContainer: containerStop)
| Attribute | Type | Description |
|-----------|------|-------------|
|containerStop|unique string|Container stop ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopContainer: chargingStation)
| Attribute | Type | Description |
|-----------|------|-------------|
|chargingStation|unique string|Charging station ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |

## stop (stopContainer: parkingArea)
| Attribute | Type | Description |
|-----------|------|-------------|
|parkingArea|unique string|Parking area ID|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60.00**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|actType|string|Activity displayed for stopped person in GUI and output files |
