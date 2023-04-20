# Netedit attribute help

## edge
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Edge ID|
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
|width|non-negative float|Lane width for all lanes of this edge in meters (used for visualization) *default:* **-1**|
|endOffset|non-negative float|Move the stop line back from the intersection by the given amount *default:* **0.00**|
|shapeStart|position|Custom position in which shape start (by default position of junction from)|
|shapeEnd|position|Custom position in which shape end (by default position of junction from)|
|isBidi|boolean|Show if edge is bidirectional *default:* **0**|
|distance|unique float|Distance *default:* **0.00**|
|stopOffset|non-negative float|The stop offset as positive value in meters *default:* **0.00**|
|stopOException|list of discrete vClasses|Specifies, for which vehicle classes the stopOffset does NOT apply.|

## lane
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Lane ID (Automatic, non editable)|
|index|unique integer|The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one)|
|speed|non-negative float|Speed in meters per second *default:* **13.89**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|width|non-negative float|Width in meters (used for visualization) *default:* **-1**|
|endOffset|non-negative float|Move the stop line back from the intersection by the given amount *default:* **0.00**|
|acceleration|boolean|Enable or disable lane as acceleration lane *default:* **0**|
|customShape|list of unique positions|If the shape is given it overrides the computation based on edge shape|
|opposite|unique string|If given, this defines the opposite direction lane|
|changeLeft|list of discrete vClasses|Permit changing left only for to the given vehicle classes *default:* **all**|
|changeRight|list of discrete vClasses|Permit changing right only for to the given vehicle classes *default:* **all**|
|type|string|Lane type description (optional)|
|stopOffset|non-negative float|The stop offset as positive value in meters *default:* **0.00**|
|stopOException|list of discrete vClasses|Specifies, for which vehicle classes the stopOffset does NOT apply.|

## poly
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of the polygon|
|shape|list of unique positions|The shape of the polygon|
|color|color|The RGBA color with which the polygon shall be displayed *default:* **red**|
|fill|boolean|An information whether the polygon shall be filled *default:* **0**|
|lineWidth|non-negative float|The default line width for drawing an unfilled polygon *default:* **1**|
|layer|float|The layer in which the polygon lies *default:* **0.00**|
|type|string|A typename for the polygon|
|name|string|Polygon's name|
|imgFile|filename|A bitmap to use for rendering this polygon|
|relativePath|boolean|Enable or disable use image file as a relative path *default:* **0**|
|angle|angle[0, 360]|Angle of rendered image in degree *default:* **0.00**|

## poi
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of the POI|
|pos|unique position|The position in view|
|color|color|The color with which the POI shall be displayed *default:* **red**|
|type|string|A typename for the POI|
|name|string|Name of POI|
|layer|float|The layer of the POI for drawing and selecting *default:* **202.00**|
|width|non-negative float|Width of rendered image in meters *default:* **1.00**|
|height|non-negative float|Height of rendered image in meters *default:* **1.00**|
|imgFile|filename|A bitmap to use for rendering this POI|
|relativePath|boolean|Enable or disable use image file as a relative path *default:* **0**|
|angle|angle[0, 360]|Angle of rendered image in degree *default:* **0.00**|
|lane|unique string|The name of the lane at which the POI is located at|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|posLat|float|The lateral offset on the named lane at which the POI is located at *default:* **0.00**|
|lon|unique float|The longitude position of the parking vehicle on the view|
|lat|unique float|The latitude position of the parking vehicle on the view|

## junction
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of the node|
|pos|unique position|The x-y-z position of the node on the plane in meters|
|type|discrete string|An optional type for the node|
|shape|list of unique positions|A custom shape for that node|
|radius|non-negative float|Optional turning radius (for all corners) for that node in meters *default:* **1.5**|
|keepClear|boolean|Whether the junction-blocking-heuristic should be activated at this node *default:* **1**|
|rightOfWay|discrete string|How to compute right of way rules at this node *default:* **default**|
|fringe|discrete string|Whether this junction is at the fringe of the network *default:* **default**|
|name|string|Optional name of junction|
|tlType|discrete string|An optional type for the traffic light algorithm|
|tlLayout|discrete string|An optional layout for the traffic light plan|
|tl|string|An optional id for the traffic light program|

## busStop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of bus stop|
|lane|unique string|The name of the lane the bus stop shall be located at|
|startPos|unique string|The begin position on the lane (the lower position on the lane) in meters|
|endPos|unique string|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|name|string|Name of busStop|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1 if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|lines|list of strings|Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes|
|personCapacity|non-negative integer|Larger numbers of persons trying to enter will create an upstream jam on the sidewalk *default:* **6**|
|parkingLength|float|Optional space definition for vehicles that park at this stop *default:* **0.00**|
|color|color|The RGBA color with which the busStop shall be displayed|

## trainStop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of train stop|
|lane|unique string|The name of the lane the train stop shall be located at|
|startPos|unique string|The begin position on the lane (the lower position on the lane) in meters|
|endPos|unique string|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|name|string|Name of trainStop|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|lines|list of strings|Meant to be the names of the train lines that stop at this train stop. This is only used for visualization purposes|
|personCapacity|non-negative integer|Larger numbers of persons trying to enter will create an upstream jam on the sidewalk *default:* **6**|
|parkingLength|float|Optional space definition for vehicles that park at this stop *default:* **0.00**|
|color|color|The RGBA color with which the trainStop shall be displayed|

### access
child element of [busStop](#busstop), [trainStop](#trainstop)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the stop access shall be located at|
|pos|unique float|The position on the lane (the lower position on the lane) in meters *default:* **0.00**|
|length|non-negative float|The walking length of the access in meters *default:* **-1.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

## containerStop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of container stop|
|lane|unique string|The name of the lane the container stop shall be located at|
|startPos|unique string|The begin position on the lane (the lower position on the lane) in meters|
|endPos|unique string|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|name|string|Name of containerStop|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|lines|list of strings|meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes|
|containerCapacity|non-negative integer|Larger numbers of container trying to enter will create an upstream jam on the sidewalk *default:* **6**|
|parkingLength|float|Optional space definition for vehicles that park at this stop *default:* **0.00**|
|color|color|The RGBA color with which the containerStop shall be displayed|

## chargingStation
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of charging station|
|lane|unique string|Lane of the charging station location|
|startPos|unique string|Begin position in the specified lane|
|endPos|unique string|End position in the specified lane|
|name|string|Name of chargingStation|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|power|non-negative float|Charging power in W *default:* **22000.00**|
|efficiency|float|Charging efficiency [0,1] *default:* **0.95**|
|chargeInTransit|boolean|Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging *default:* **0**|
|chargeDelay|SUMOTime|Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins *default:* **0.00**|

## parkingArea
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of ParkingArea|
|lane|unique string|The name of the lane the Parking Area shall be located at|
|startPos|unique string|The begin position on the lane (the lower position on the lane) in meters|
|endPos|unique string|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|departPos|string|Lane position in that vehicle must depart when leaves parkingArea|
|name|string|Name of parkingArea|
|roadsideCapacity|non-negative integer| The number of parking spaces for road-side parking *default:* **0**|
|onRoad|boolean|If set, vehicles will park on the road lane and thereby reducing capacity *default:* **0**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|width|non-negative float|The width of the road-side parking spaces *default:* **3.20**|
|length|non-negative float|The length of the road-side parking spaces. By default (endPos - startPos) / roadsideCapacity *default:* **0.00**|
|angle|angle[0, 360]|The angle of the road-side parking spaces relative to the lane angle, positive means clockwise *default:* **0.00**|

### space
child element of [parkingArea](#parkingarea)

| Attribute | Type | Description |
|-----------|------|-------------|
|pos|unique position|The x-y-z position of the node on the plane in meters|
|name|string|Name of parking space|
|width|string|The width of the road-side parking spaces|
|length|string|The length of the road-side parking spaces|
|angle|string|The angle of the road-side parking spaces relative to the lane angle, positive means clockwise|
|slope|angle[0, 360]|The slope of the road-side parking spaces *default:* **0.00**|

## inductionLoop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of E1|
|lane|unique string|The id of the lane the detector shall be laid on. The lane must be a part of the network used|
|pos|unique float|The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length|
|period|SUMOTime|The aggregation period the values the detector collects shall be summed up *default:* **300.00**|
|name|string|Name of induction loop|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

## laneAreaDetector
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of E2|
|lane|unique string|The id of the lane the detector shall be laid on. The lane must be a part of the network used|
|pos|unique float|The position on the lane the detector shall be laid on in meters|
|length|non-negative float|The length of the detector in meters *default:* **10.00**|
|period|SUMOTime|The aggregation period the values the detector collects shall be summed up *default:* **300.00**|
|tl|string|The traffic light that triggers aggregation when switching|
|name|string|Name of lane area detector|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|timeThreshold|SUMOTime|The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) *default:* **1.00**|
|speedThreshold|non-negative float|The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s *default:* **1.39**|
|jamThreshold|non-negative float|The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m *default:* **10.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|lanes|list of unique strings|The sequence of lane ids in which the detector shall be laid on|
|endPos|unique float|The end position on the lane the detector shall be laid on in meters|

## entryExitDetector
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of E3|
|pos|unique position|X-Y position of detector in editor (Only used in netedit) *default:* **0,0**|
|period|SUMOTime|The aggregation period the values the detector collects shall be summed up *default:* **300.00**|
|name|string|Name of Entry Exit detector|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|timeThreshold|SUMOTime|The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) in s *default:* **1.00**|
|speedThreshold|float|The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s *default:* **1.39**|

### detEntry
child element of [entryExitDetector](#entryexitdetector)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The id of the lane the detector shall be laid on. The lane must be a part of the network used|
|pos|unique float|The position on the lane the detector shall be laid on in meters|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

### detExit
child element of [entryExitDetector](#entryexitdetector)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The id of the lane the detector shall be laid on. The lane must be a part of the network used|
|pos|unique float|The position on the lane the detector shall be laid on in meters|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

## edgeData
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of this set of measurements|
|file|filename|The path to the output file. The path may be relative|
|period|string|The aggregation period the values the detector collects shall be summed up|
|begin|string|The time to start writing. If not given, the simulation's begin is used.|
|end|string|The time to end writing. If not given the simulation's end is used.|
|excludeEmpty|discrete string|If set to true, edges/lanes which were not use by a vehicle during this period will not be written *default:* **default**|
|withInternal|boolean|If set, junction internal edges/lanes will be written as well *default:* **0**|
|maxTraveltime|SUMOTime|The maximum travel time in seconds to write if only very small movements occur *default:* **100000**|
|minSamples|SUMOTime|Consider an edge/lane unused if it has at most this many sampled seconds *default:* **0**|
|speedThreshold|float|The maximum speed to consider a vehicle halting; *default:* **0.1**|
|vTypes|list of strings|space separated list of vehicle type ids to consider|
|trackVehicles|boolean|whether aggregation should be performed over all vehicles that entered the edge/lane in the aggregation interval *default:* **0**|
|detectPersons|list of strings|Whether pedestrians shall be recorded instead of vehicles. Allowed value is walk|
|writeAttributes|list of strings|List of attribute names that shall be written|
|edges|list of strings|Restrict output to the given list of edge ids|
|edgesFile|filename|Restrict output to the given the list of edges given in file|
|aggregate|boolean|Whether the traffic statistic of all edges shall be aggregated into a single value *default:* **0**|

## laneData
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of this set of measurements|
|file|filename|The path to the output file. The path may be relative|
|period|string|The aggregation period the values the detector collects shall be summed up|
|begin|string|The time to start writing. If not given, the simulation's begin is used.|
|end|string|The time to end writing. If not given the simulation's end is used.|
|excludeEmpty|discrete string|If set to true, edges/lanes which were not use by a vehicle during this period will not be written *default:* **default**|
|withInternal|boolean|If set, junction internal edges/lanes will be written as well *default:* **0**|
|maxTraveltime|SUMOTime|The maximum travel time in seconds to write if only very small movements occur *default:* **100000**|
|minSamples|SUMOTime|Consider an edge/lane unused if it has at most this many sampled seconds *default:* **0**|
|speedThreshold|float|The maximum speed to consider a vehicle halting; *default:* **0.1**|
|vTypes|list of strings|space separated list of vehicle type ids to consider|
|trackVehicles|boolean|whether aggregation should be performed over all vehicles that entered the edge/lane in the aggregation interval *default:* **0**|
|detectPersons|list of strings|Whether pedestrians shall be recorded instead of vehicles. Allowed value is walk|
|writeAttributes|list of strings|List of attribute names that shall be written|
|edges|list of strings|Restrict output to the given list of edge ids|
|edgesFile|filename|Restrict output to the given the list of edges given in file|
|aggregate|boolean|Whether the traffic statistic of all edges shall be aggregated into a single value *default:* **0**|

## instantInductionLoop
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of Instant Induction Loop (E1Instant)|
|lane|unique string|The id of the lane the detector shall be laid on. The lane must be a part of the network used|
|pos|unique float|The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length|
|name|string|Name of instant induction loop|
|file|filename|The path to the output file|
|vTypes|list of strings|Space separated list of vehicle type ids to consider|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|

## routeProbe
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of RouteProbe|
|edge|unique string|The id of an edge in the simulation network|
|period|SUMOTime|The frequency in which to report the distribution *default:* **3600.00**|
|name|string|Name of route probe|
|file|filename|The file for generated output|
|begin|SUMOTime|The time at which to start generating output *default:* **0**|

## calibrator
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of Calibrator|
|edge|unique string|The id of edge in the simulation network|
|pos|non-negative float|The position of the calibrator on the specified lane *default:* **0.00**|
|period|SUMOTime|The aggregation interval in which to calibrate the flows. Default is step-length *default:* **1.00**|
|name|string|Name of Calibrator|
|routeProbe|string|The id of the routeProbe element from which to determine the route distribution for generated vehicles|
|output|filename|The output file for writing calibrator information or NULL|
|jamThreshold|non-negative float|A threshold value to detect and clear unexpected jamming *default:* **0.50**|
|vTypes|list of strings|space separated list of vehicle type ids to consider (empty to affect all types)|
|lane|unique string|The id of lane in the simulation network|

## rerouter
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of Rerouter|
|edges|list of unique strings|An edge id or a list of edge ids where vehicles shall be rerouted|
|pos|unique position|X,Y position in editor (Only used in netedit) *default:* **0,0**|
|name|string|Name of Rerouter|
|probability|probability[0, 1]|The probability for vehicle rerouting (0-1) *default:* **1.00**|
|timeThreshold|SUMOTime|The waiting time threshold (in s) that must be reached to activate rerouting (default -1 which disables the threshold) *default:* **0.00**|
|vTypes|list of strings|The list of vehicle types that shall be affected by this rerouter (empty to affect all types)|
|off|boolean|Whether the router should be inactive initially (and switched on in the gui) *default:* **0**|

### interval
child element of [rerouter](#rerouter)

| Attribute | Type | Description |
|-----------|------|-------------|
|begin|unique SUMOTime|Begin *default:* **0**|
|end|unique SUMOTime|End *default:* **3600**|

### destProbReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|Edge ID|
|probability|float|SUMO Probability *default:* **1.00**|

### closingReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|Edge ID|
|allow|list of vClasses|allowed vehicles|
|disallow|list of vClasses|disallowed vehicles|

### closingLaneReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|Lane ID|
|allow|list of vClasses|allowed vehicles|
|disallow|list of vClasses|disallowed vehicles|

### routeProbReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|route|unique string|Route|
|probability|float|SUMO Probability *default:* **1.00**|

### parkingAreaReroute
child element of [interval](#interval)

| Attribute | Type | Description |
|-----------|------|-------------|
|parking|unique string|ParkingArea ID|
|probability|float|SUMO Probability *default:* **1.00**|
|visible|boolean|Enable or disable visibility for parking area reroutes *default:* **1**|

## variableSpeedSign
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of Variable Speed Signal|
|pos|unique position|X-Y position of detector in editor (Only used in netedit) *default:* **0,0**|
|lanes|list of unique strings|List of Variable Speed Sign lanes|
|name|string|Name of Variable Speed Signal|
|vTypes|list of strings|Space separated list of vehicle type ids to consider (empty to affect all types)|

### step
child element of [variableSpeedSign](#variablespeedsign)

| Attribute | Type | Description |
|-----------|------|-------------|
|time|unique SUMOTime|Time|
|speed|string|Speed *default:* **13.89**|

## vaporizer
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Edge in which vaporizer is placed|
|begin|SUMOTime|Start Time *default:* **0**|
|end|SUMOTime|End Time *default:* **3600**|
|name|string|Name of vaporizer|

## tractionSubstation
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Traction substation ID|
|pos|unique position|X-Y position of detector in editor (Only used in netedit) *default:* **0,0**|
|voltage|non-negative float|Voltage of at connection point for the overhead wire *default:* **600**|
|currentLimit|non-negative float|Current limit of the feeder line *default:* **400**|

## overheadWire
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Overhead wire segment ID|
|substationId|unique string|Substation to which the circuit is connected|
|lanes|list of unique strings|List of consecutive lanes of the circuit|
|startPos|non-negative unique float|Starting position in the specified lane *default:* **0.0**|
|endPos|non-negative unique float|Ending position in the specified lane *default:* **179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.00**|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|forbiddenInnerLanes|list of strings|Inner lanes, where placing of overhead wire is restricted|

## overheadWireClamp
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Overhead wire clamp ID|
|wireClampStart|unique string|ID of the overhead wire segment, to the start of which the overhead wire clamp is connected|
|wireClampLaneStart|unique string|ID of the overhead wire segment lane of overheadWireIDStartClamp|
|wireClampEnd|unique string|ID of the overhead wire segment, to the end of which the overhead wire clamp is connected|
|wireClampLaneEnd|unique string|ID of the overhead wire segment lane of overheadWireIDEndClamp|

## trip
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The ID of trip|
|type|string|The id of the vehicle type to use for this trip *default:* **DEFAULT_VEHTYPE**|
|from|unique string|The ID of the edge the trip starts at|
|to|unique string|The ID of the edge the trip ends at|
|via|list of unique strings|List of intermediate edge ids which shall be part of the trip|
|color|color|This vehicle's color *default:* **yellow**|
|departLane|string|The lane on which thevehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0.00**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|depart|string|The departure time of the (first) trip which is generated using this trip definition *default:* **0.00**|
|fromJunction|unique string|The name of the junction the trip starts at|
|toJunction|unique string|The name of the junction the trip ends at|

## vehicle
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The ID of the vehicle|
|type|string|The id of the vehicle type to use for this vehicle *default:* **DEFAULT_VEHTYPE**|
|route|unique string|The id of the route the vehicle shall drive along|
|departEdge|unique string|The index of the edge within route the vehicle starts at|
|arrivalEdge|unique string|The index of the edge within route the vehicle ends at|
|color|color|This vehicle's color *default:* **yellow**|
|departLane|string|The lane on which thevehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0.00**|
|arrivalLane|string|The lane at which the vehicle shall leave the network *default:* **current**|
|arrivalPos|string|The position at which the vehicle shall leave the network *default:* **max**|
|arrivalSpeed|string|The speed with which the vehicle shall leave the network *default:* **current**|
|line|string|A string specifying the id of a public transport line which can be used when specifying person rides|
|personNumber|non-negative integer|The number of occupied seats when the vehicle is inserted *default:* **0**|
|containerNumber|non-negative integer|The number of occupied container places when the vehicle is inserted *default:* **0**|
|departPosLat|string|The lateral position on the departure lane at which the vehicle shall enter the net *default:* **center**|
|arrivalPosLat|string|The lateral position on the arrival lane at which the vehicle shall arrive *default:* **center**|
|insertionChecks|string|Insertion checks *default:* **all**|
|depart|string|The time step at which the vehicle shall enter the network *default:* **0.00**|

## flow
also child element of [calibrator](#calibrator)

| Attribute | Type | Description |
|-----------|------|-------------|
|route|unique string|The id of the route the vehicle shall drive along|
|begin|SUMOTime|First calibrator flow departure time *default:* **0**|
|end|SUMOTime|End of departure interval *default:* **3600**|
|color|color|This vehicle's color *default:* **yellow**|
|departLane|string|The lane on which thevehicle shall be inserted *default:* **first**|
|departPos|string|The position at which the vehicle shall enter the net *default:* **base**|
|departSpeed|string|The speed with which the vehicle shall enter the network *default:* **0.00**|
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
|vehsPerHour|string|Number of vehicles per hour, equally spaced *default:* **1800**|
|speed|string|Vehicle's speed *default:* **15.0**|
|id|unique string|The ID of the flow|
|from|unique string|The ID of the edge the flow starts at|
|to|unique string|The ID of the edge the flow ends at|
|via|list of unique strings|List of intermediate edge ids which shall be part of the flow|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|period|string|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2**|
|probability|string|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.5**|
|poisson|string|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.5**|
|fromJunction|unique string|The name of the junction the flow starts at|
|toJunction|unique string|The name of the junction the flow ends at|
|departEdge|unique string|The index of the edge within route the flow starts at|
|arrivalEdge|unique string|The index of the edge within route the flow ends at|

## vType
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of VehicleType|
|VTypeDist.|string|Vehicle Type Distribution|
|vClass|discrete vClass|An abstract vehicle class *default:* **passenger**|
|color|color|This vehicle type's color|
|length|non-negative float|The vehicle's netto-length (length) [m]|
|minGap|non-negative float|Empty space after leader [m]|
|maxSpeed|non-negative float|The vehicle's maximum velocity [m/s]|
|speedFactor|string|The vehicle's expected multiplicator for lane speed limits (or a distribution specifier)|
|desiredMaxSpeed|non-negative float|The vehicle's desired maximum velocity (interacts with speedFactor). Applicable when no speed limit applies (bicycles, some motorways) [m/s]|
|emissionClass|discrete string|An abstract emission class|
|guiShape|discrete string|How this vehicle is rendered|
|width|non-negative float|The vehicle's width [m] (only used for drawing) *default:* **1.8**|
|height|non-negative float|The vehicle's height [m] (only used for drawing) *default:* **1.5**|
|imgFile|filename|Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)|
|laneChangeModel|discrete string|The model used for changing lanes *default:* **default**|
|carFollowModel|discrete string|The model used for car-following *default:* **Krauss**|
|personCapacity|non-negative integer|The number of persons (excluding an autonomous driver) the vehicle can transport|
|containerCapacity|non-negative integer|The number of containers the vehicle can transport|
|boardingDuration|float|The time required by a person to board the vehicle *default:* **0.50**|
|loadingDuration|float|The time required to load a container onto the vehicle *default:* **90.00**|
|latAlignment|discrete string|The preferred lateral alignment when using the sublane-model *default:* **center**|
|minGapLat|non-negative float|The minimum lateral gap at a speed difference of 50km/h when using the sublane-model *default:* **0.12**|
|maxSpeedLat|non-negative float|The maximum lateral speed when using the sublane-model *default:* **1.00**|
|actionStepLength|non-negative float|The interval length for which vehicle performs its decision logic (acceleration and lane-changing) *default:* **0.00**|
|probability|non-negative float|The probability when being added to a distribution without an explicit probability *default:* **1.00**|
|osgFile|string|3D model file for this class|
|carriageLength|non-negative float|Carriage lengths|
|locomotiveLength|non-negative float|Locomotive lengths|
|carriageGap|non-negative float|Gap between carriages *default:* **1**|
|accel|non-negative float|The acceleration ability of vehicles of this type [m/s^2] *default:* **2.60**|
|decel|non-negative float|The deceleration ability of vehicles of this type [m/s^2] *default:* **4.50**|
|apparentDecel|non-negative float|The apparent deceleration of the vehicle as used by the standard model [m/s^2] *default:* **4.50**|
|emergencyDecel|non-negative float|The maximal physically possible deceleration for the vehicle [m/s^2] *default:* **4.50**|
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
|tauLast|non-negative float|Peter Wagner 2009 parameter *default:* **0**|
|apProb|non-negative float|Peter Wagner 2009 parameter *default:* **0**|
|adaptFactor|non-negative float|IDMM parameter *default:* **0**|
|adaptTime|non-negative float|IDMM parameter *default:* **0**|
|cc1|non-negative float|W99 parameter *default:* **1.3**|
|cc2|non-negative float|W99 parameter *default:* **8.0**|
|cc3|non-negative float|W99 parameter *default:* **-12.0**|
|cc4|non-negative float|W99 parameter *default:* **-0.25**|
|cc5|non-negative float|W99 parameter *default:* **0.35**|
|cc6|non-negative float|W99 parameter *default:* **6.0**|
|cc7|non-negative float|W99 parameter *default:* **0.25**|
|cc8|non-negative float|W99 parameter *default:* **2.0**|
|cc9|non-negative float|W99 parameter *default:* **1.5**|
|security|string|Wiedemann parameter|
|estimation|string|Wiedemann parameter|
|collisionMinGapFactor|string|MinGap factor parameter|
|k|string|K parameter|
|phi|string|Kerner Phi parameter|
|delta|string|IDM Delta parameter|
|stepping|string|IDM Stepping parameter|
|trainType|discrete string|Train Types *default:* **NGT400**|
|jmCrossingGap|non-negative float|Minimum distance to pedestrians that are walking towards the conflict point with the ego vehicle. *default:* **10**|
|jmIgnoreKeepClearTime|float|The accumulated waiting time after which a vehicle will drive onto an intersection even though this might cause jamming. *default:* **-1**|
|jmDriveAfterYellowTime|non-negative float|This value causes vehicles to violate a yellow light if the duration of the yellow phase is lower than the given threshold. *default:* **-1**|
|jmDriveAfterRedTime|float|This value causes vehicles to violate a red light if the duration of the red phase is lower than the given threshold. *default:* **-1**|
|jmDriveRedSpeed|string|This value causes vehicles affected by jmDriveAfterRedTime to slow down when violating a red light. *default:* **0.0**|
|jmIgnoreFoeProb|non-negative float|This value causes vehicles to ignore foe vehicles that have right-of-way with the given probability. *default:* **0.0**|
|jmIgnoreFoeSpeed|non-negative float|This value is used in conjunction with jmIgnoreFoeProb. Only vehicles with a speed below or equal to the given value may be ignored. *default:* **0.0**|
|jmSigmaMinor|string|This value configures driving imperfection (dawdling) while passing a minor link. *default:* **0.0**|
|jmTimegapMinor|non-negative float|This value defines the minimum time gap when passing ahead of a prioritized vehicle.  *default:* **1**|
|impatience|non-negative float|Willingess of drivers to impede vehicles with higher priority *default:* **0.0**|
|lcStrategic|non-negative float|The eagerness for performing strategic lane changing. Higher values result in earlier lane-changing. *default:* **1.0**|
|lcCooperative|non-negative float|The willingness for performing cooperative lane changing. Lower values result in reduced cooperation. *default:* **1.0**|
|lcSpeedGain|non-negative float|The eagerness for performing lane changing to gain speed. Higher values result in more lane-changing. *default:* **1.0**|
|lcKeepRight|non-negative float|The eagerness for following the obligation to keep right. Higher values result in earlier lane-changing. *default:* **1.0**|
|lcSublane|non-negative float|The eagerness for using the configured lateral alignment within the lane. Higher values result in increased willingness to sacrifice speed for alignment. *default:* **1.0**|
|lcOpposite|non-negative float|The eagerness for overtaking through the opposite-direction lane. Higher values result in more lane-changing. *default:* **1.0**|
|lcPushy|non-negative float|Willingness to encroach laterally on other drivers. *default:* **0.00**|
|lcPushyGap|non-negative float|Minimum lateral gap when encroaching laterally on other drives (alternative way to define lcPushy) *default:* **0.00**|
|lcAssertive|non-negative float|Willingness to accept lower front and rear gaps on the target lane. *default:* **1.0**|
|lcImpatience|non-negative float|Dynamic factor for modifying lcAssertive and lcPushy. *default:* **0.00**|
|lcTimeToImpatience|string|Time to reach maximum impatience (of 1). Impatience grows whenever a lane-change manoeuvre is blocked. *default:* **infinity**|
|lcAccelLat|non-negative float|Maximum lateral acceleration per second. *default:* **1.0**|
|lcLookaheadLeft|non-negative float|Factor for configuring the strategic lookahead distance when a change to the left is necessary (relative to right lookahead). *default:* **2.0**|
|lcSpeedGainRight|non-negative float|Factor for configuring the threshold asymmetry when changing to the left or to the right for speed gain. *default:* **0.1**|
|lcMaxSpeedLatStanding|string|Upper bound on lateral speed when standing. *default:* **0.00**|
|lcMaxSpeedLatFactor|non-negative float|Upper bound on lateral speed while moving computed as lcMaxSpeedLatStanding + lcMaxSpeedLatFactor * getSpeed() *default:* **1.00**|
|lcTurnAlignmentDistance|non-negative float|Distance to an upcoming turn on the vehicles route, below which the alignment should be dynamically adapted to match the turn direction. *default:* **0.00**|
|lcOvertakeRight|non-negative float|The probability for violating rules gainst overtaking on the right. *default:* **0.00**|
|lcKeepRightAcceptanceTime|float|Time threshold for the willingness to change right. *default:* **-1**|
|lcOvertakeDeltaSpeedFactor|float|Speed difference factor for the eagerness of overtaking a neighbor vehicle before changing lanes (threshold = factor*speedlimit). *default:* **0.00**|

## route
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of Route|
|edges|list of unique strings|The edges the vehicle shall drive along, given as their ids, separated using spaces|
|color|color|This route's color|
|repeat|non-negative integer|The number of times that the edges of this route shall be repeated *default:* **0**|
|cycleTime|SUMOTime|When defining a repeating route with stops and those stops use the until attribute, the times will be shifted forward by 'cycleTime' on each repeat *default:* **0**|

## taz
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of the TAZ|
|shape|list of unique positions|The shape of the TAZ|
|center|position|TAZ center|
|fill|boolean|An information whether the TAZ shall be filled *default:* **0**|
|color|color|The RGBA color with which the TAZ shall be displayed *default:* **red**|
|name|string|Name of POI|

### tazSource
child element of [taz](#taz)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of edge in the simulation network|
|weight|non-negative float|Depart weight associated to this Edge *default:* **1**|

### tazSink
child element of [taz](#taz)

| Attribute | Type | Description |
|-----------|------|-------------|
|edge|unique string|The id of edge in the simulation network|
|weight|non-negative float|Arrival weight associated to this Edget *default:* **1**|

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

### stop
child element of [route](#route), [trip](#trip), [flow](#flow)

| Attribute | Type | Description |
|-----------|------|-------------|
|lane|unique string|The name of the lane the stop shall be located at|
|startPos|unique float|The begin position on the lane (the lower position on the lane) in meters|
|endPos|unique float|The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m|
|friendlyPos|boolean|If set, no error will be reported if element is placed behind the lane. Instead, it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1 *default:* **0**|
|posLat|string|The lateral offset on the named lane at which the vehicle must stop|
|duration|non-negative SUMOTime|Minimum duration for stopping *default:* **60**|
|until|non-negative SUMOTime|The time step at which the route continues *default:* **0.00**|
|extension|SUMOTime|If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds *default:* **0**|
|triggered|discrete string|Whether a person or container or bth may end the stop *default:* **0**|
|expected|list of strings|List of elements that must board the vehicle before it may continue|
|permitted|list of strings|List of elements that can board the vehicle before it may continue|
|parking|discrete string|Whether the vehicle stops on the road or beside *default:* **0**|
|actType|string|Activity displayed for stopped person in GUI and output files|
|tripId|string|Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route|
|line|string|New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)|
|onDemand|boolean|Whether the stop may be skipped if no passengers wants to embark or disembark *default:* **0**|
|jump|SUMOTime|transfer time if there shall be a jump from this stop to the next route edge *default:* **-1**|
|busStop|list of unique strings|BusStop associated with this stop|
|containerStop|list of unique strings|ContainerStop associated with this stop|
|chargingStation|list of unique strings|ChargingStation associated with this stop|
|parkingArea|list of unique strings|ParkingArea associated with this stop|
|speed|non-negative float|Speed to be kept while driving between startPos and endPos *default:* **0.00**|
|edge|unique string|The ID of the edge the stop shall be located at|

## connection
| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the vehicles leave|
|to|unique string|The ID of the edge the vehicles may reach when leaving 'from'|
|fromLane|unique integer|the lane index of the incoming lane (numbers starting with 0)|
|toLane|unique integer|the lane index of the outgoing lane (numbers starting with 0)|
|pass|boolean|if set, vehicles which pass this (lane-2-lane) connection) will not wait *default:* **0**|
|keepClear|boolean|if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection *default:* **0**|
|contPos|non-negative float|If set to a more than 0 value, an internal junction will be built at this position (in m)/n from the start of the internal lane for this connection *default:* **-1.00**|
|uncontrolled|boolean|If set to true, This connection will not be TLS-controlled despite its node being controlled *default:* **0**|
|visibility|non-negative float|Vision distance between vehicles *default:* **-1.00**|
|linkIndex|integer|sets index of this connection within the controlling traffic light *default:* **-1**|
|linkIndex2|integer|sets index for the internal junction of this connection within the controlling traffic light *default:* **-1**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|speed|non-negative float|sets custom speed limit for the connection *default:* **-1.00**|
|length|non-negative float|sets custom length for the connection *default:* **-1.00**|
|customShape|list of unique positions|sets custom shape for the connection|
|changeLeft|list of discrete vClasses|Permit changing left only for to the given vehicle classes *default:* **all**|
|changeRight|list of discrete vClasses|Permit changing right only for to the given vehicle classes *default:* **all**|
|indirect|boolean|if set to true, vehicles will make a turn in 2 steps *default:* **0**|
|type|string|set a custom edge type (for applying vClass-specific speed restrictions)|
|dir|string|turning direction for this connection (computed)|
|state|string|link state for this connection (computed)|

## type
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of the edge|
|numLanes|non-negative integer|The number of lanes of the edge *default:* **1**|
|speed|non-negative float|The maximum speed allowed on the edge in m/s *default:* **13.89**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|spreadType|discrete string|The spreadType defines how to compute the lane geometry from the edge geometry (used for visualization) *default:* **right**|
|priority|integer|The priority of the edge *default:* **-1**|
|width|non-negative float|Lane width for all lanes of this edge in meters (used for visualization) *default:* **-1**|
|sidewalkWidth|float|The width of the sidewalk that should be added as an additional lane|
|bikeLaneWidth|float|The width of the bike lane that should be added as an additional lane|

## laneType
| Attribute | Type | Description |
|-----------|------|-------------|
|speed|non-negative float|The maximum speed allowed on the lane in m/s *default:* **13.89**|
|allow|list of vClasses|Explicitly allows the given vehicle classes (not given will be not allowed) *default:* **all**|
|disallow|list of vClasses|Explicitly disallows the given vehicle classes (not given will be allowed)|
|width|non-negative float|Lane width for all lanes of this lane in meters (used for visualization) *default:* **-1**|

## vTypeDistribution
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The id of VehicleType distribution|

## crossing
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Crossing ID|
|edges|list of unique strings|The (road) edges which are crossed|
|priority|boolean|Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections) *default:* **0**|
|width|non-negative float|The width of the crossings *default:* **4.00**|
|linkIndex|integer|sets the tls-index for this crossing *default:* **-1**|
|linkIndex2|integer|sets the opposite-direction tls-index for this crossing *default:* **-1**|
|customShape|list of unique positions|Overrides default shape of pedestrian crossing|

## walkingArea
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Walking Area ID|
|width|non-negative float|The width of the WalkingArea *default:* **2.00**|
|length|non-negative float|The length of the WalkingArea in meter|
|shape|list of unique positions|Overrides default shape of pedestrian sidewalk|

## person
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The name of the person|
|type|string|The id of the person type to use for this person *default:* **DEFAULT_VEHTYPE**|
|color|color|This person's color *default:* **yellow**|
|departPos|string|The position at which the person shall enter the net *default:* **base**|
|depart|non-negative float|The time step at which the person shall enter the network *default:* **0.00**|

### personTrip
child element of [person](#person), [personFlow](#personflow)

| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the person trip starts at|
|to|unique string|The ID of the edge the person trip ends at|
|arrivalPos|float|arrival position on the destination edge *default:* **-1**|
|vTypes|list of strings|List of possible vehicle types to take|
|modes|list of strings|List of possible traffic modes. Walking is always possible regardless of this value|
|lines|list of strings|list of vehicle alternatives to take for the person trip *default:* **ANY**|
|toBusStop|unique string|Id of the destination busStop|
|fromJunction|unique string|The name of the junction the person trip starts at|
|toJunction|unique string|The name of the junction the person trip ends at|

### ride
child element of [person](#person), [personFlow](#personflow)

| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the ride starts at|
|to|unique string|The ID of the edge the ride ends at|
|arrivalPos|float|arrival position on the destination edge *default:* **-1**|
|lines|list of strings|list of vehicle alternatives to take for the ride *default:* **ANY**|
|toBusStop|unique string|ID of the destination bus stop|

### walk
child element of [person](#person), [personFlow](#personflow)

| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the walk starts at|
|to|unique string|The ID of the edge the walk ends at|
|arrivalPos|float|arrival position on the destination edge *default:* **-1**|
|toBusStop|unique string|Id of the destination bus stop|
|edges|list of unique strings|id of the edges to walk|
|route|list of unique strings|The id of the route to walk|
|fromJunction|unique string|The name of the junction the walk starts at|
|toJunction|unique string|The name of the junction the walk ends at|

## personFlow
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The name of the person|
|type|string|The id of the person type to use for this person *default:* **DEFAULT_VEHTYPE**|
|color|color|This person's color *default:* **yellow**|
|departPos|string|The position at which the person shall enter the net *default:* **base**|
|begin|SUMOTime|First flow departure time *default:* **0**|
|end|SUMOTime|End of departure interval *default:* **3600**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|personsPerHour|string|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800**|
|period|string|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2**|
|probability|string|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.5**|
|poisson|string|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.5**|

## dataSet
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Data set ID|

### dataInterval
child element of [dataSet](#dataset)

| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|Interval ID|
|begin|SUMOTime|Data interval begin time *default:* **0**|
|end|SUMOTime|Data interval end time *default:* **3600**|

## container
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The name of the container|
|type|string|The id of the container type to use for this container *default:* **DEFAULT_CONTAINERTYPE**|
|color|color|This container's color *default:* **yellow**|
|depart|non-negative float|The time step at which the container shall enter the network *default:* **0.00**|

### transport
child element of [container](#container), [containerFlow](#containerflow)

| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the transport starts at|
|to|unique string|The ID of the edge the transport ends at|
|arrivalPos|float|arrival position on the destination edge *default:* **-1**|
|lines|list of strings|list of vehicle alternatives to take for the transport *default:* **ANY**|
|toContainerStop|unique string|ID of the destination container stop|

### tranship
child element of [container](#container), [containerFlow](#containerflow)

| Attribute | Type | Description |
|-----------|------|-------------|
|from|unique string|The ID of the edge the tranship starts at|
|to|unique string|The ID of the edge the tranship ends at|
|departPos|non-negative float|The position at which the tranship shall enter the net *default:* **0**|
|arrivalPos|float|arrival position on the destination edge *default:* **-1**|
|speed|non-negative float|speed of the container for this tranship in m/s *default:* **1.39**|
|toContainerStop|unique string|Id of the destination container stop|
|edges|list of unique strings|id of the edges to walk|

## containerFlow
| Attribute | Type | Description |
|-----------|------|-------------|
|id|unique string|The name of the container|
|type|string|The id of the container type to use for this container *default:* **DEFAULT_CONTAINERTYPE**|
|color|color|This container's color *default:* **yellow**|
|begin|SUMOTime|First flow departure time *default:* **0**|
|end|SUMOTime|End of departure interval *default:* **3600**|
|number|non-negative integer|probability for emitting a flow each second (not together with vehsPerHour or period) *default:* **1800**|
|containersPerHour|string|Number of flows per hour, equally spaced (not together with period or probability or poisson) *default:* **1800**|
|period|string|Insert equally spaced flows at that period (not together with vehsPerHour or probability or poisson) *default:* **2**|
|probability|string|probability for emitting a flow each second (not together with vehsPerHour or period or poisson) *default:* **0.5**|
|poisson|string|Insert flow expected vehicles per second with poisson distributed insertion rate (not together with period or vehsPerHour or probability) *default:* **0.5**|
