---
title: netedit
---

# From 30.000 feet

**netedit** is the graphical editor for networks, additional infrastructure,
traffic objects and traffic data.

- **Purpose:** Create and modify simulation scenarios
- **System:** portable (Linux/Windows/Mac is tested); opens a window
- **Input (optional):** a network and any files belonging to a scenario
- **Output:** a network and any files belonging to a scenario
- **Programming Language:** C++

# Usage Description

For an overview of features see [netedit usage documentation](Netedit/index.md).

## Options

You may use a XML schema definition file for setting up a netedit
configuration: [sumoConfiguration.xsd](https://sumo.dlr.de/xsd/neteditConfiguration.xsd).

### Configuration
| Option | Description |
|--------|-------------|
| **-c** {{DT_FILE}}<br> **--configuration-file** {{DT_FILE}} | Loads the named config on startup |
| **-C** {{DT_FILE}}<br> **--save-configuration** {{DT_FILE}} | Saves current configuration into FILE |
| **--save-configuration.relative** {{DT_BOOL}} | Enforce relative paths when saving the configuration; *default:* **false** |
| **--save-template** {{DT_FILE}} | Saves a configuration template (empty) into FILE |
| **--save-schema** {{DT_FILE}} | Saves the configuration schema into FILE |
| **--save-commented** {{DT_BOOL}} | Adds comments to saved template, configuration, or schema; *default:* **false** |

### Input
| Option | Description |
|--------|-------------|
| **--sumocfg-file** {{DT_FILE}} | Load sumo config |
| **-a** {{DT_FILE}}<br> **--additional-files** {{DT_FILE}} | Load additional and shapes descriptions from FILE(s) |
| **-r** {{DT_FILE}}<br> **--route-files** {{DT_FILE}} | Load demand elements descriptions from FILE(s) |
| **-d** {{DT_FILE}}<br> **--data-files** {{DT_FILE}} | Load data elements descriptions from FILE(s) |
| **-m** {{DT_FILE}}<br> **--meandata-files** {{DT_FILE}} | Load meanData descriptions from FILE(s) |
| **--ignore-missing-inputs** {{DT_BOOL}} | Reset path values (additional, route, data...) after loading netedit config; *default:* **false** |
| **--selection-file** {{DT_FILE}} | Load element selection |
| **--test-file** {{DT_FILE}} | Test file |
| **-s** {{DT_FILE}}<br> **--sumo-net-file** {{DT_FILE}} | Read SUMO-net from FILE |
| **-n** {{DT_FILE}}<br> **--node-files** {{DT_FILE}} | Read XML-node defs from FILE |
| **-e** {{DT_FILE}}<br> **--edge-files** {{DT_FILE}} | Read XML-edge defs from FILE |
| **-x** {{DT_FILE}}<br> **--connection-files** {{DT_FILE}} | Read XML-connection defs from FILE |
| **-i** {{DT_FILE}}<br> **--tllogic-files** {{DT_FILE}} | Read XML-traffic light defs from FILE |
| **-t** {{DT_FILE}}<br> **--type-files** {{DT_FILE}} | Read XML-type defs from FILE |
| **--shapefile-prefix** {{DT_FILE}} | Read shapefiles (ArcView, Tiger, ...) from files starting with 'FILE' |
| **--dlr-navteq-prefix** {{DT_FILE}} | Read converted Navteq GDF data (unsplitted Elmar-network) from path 'FILE' |
| **--osm-files** {{DT_FILE}} | Read OSM-network from path 'FILE(s)' |
| **--opendrive-files** {{DT_FILE}} | Read OpenDRIVE-network from FILE |
| **--visum-file** {{DT_FILE}} | Read VISUM-net from FILE |
| **--vissim-file** {{DT_FILE}} | Read VISSIM-net from FILE |
| **--robocup-dir** {{DT_FILE}} | Read RoboCup-net from DIR |
| **--matsim-files** {{DT_FILE}} | Read MATSim-net from FILE |
| **--itsumo-files** {{DT_FILE}} | Read ITSUMO-net from FILE |
| **--heightmap.shapefiles** {{DT_FILE}} | Read heightmap from ArcGIS shapefile |
| **--heightmap.geotiff** {{DT_FILE}} | Read heightmap from GeoTIFF |

### Output
| Option | Description |
|--------|-------------|
| **--tls-file** {{DT_STR}} | File in which TLS Programs must be saved |
| **--edgetypes-file** {{DT_STR}} | File in which edgeTypes must be saved |
| **--write-license** {{DT_BOOL}} | Include license info into every output file; *default:* **false** |
| **--output-prefix** {{DT_STR}} | Prefix which is applied to all output files. The special string 'TIME' is replaced by the current time. |
| **--precision** {{DT_INT}} | Defines the number of digits after the comma for floating point output; *default:* **2** |
| **--precision.geo** {{DT_INT}} | Defines the number of digits after the comma for lon,lat output; *default:* **6** |
| **--output.compression** {{DT_STR}} | Defines the standard compression algorithm (currently only for parquet output) |
| **--output.format** {{DT_STR}} | Defines the standard output format if not derivable from the file name ('xml', 'csv', 'parquet'); *default:* **xml** |
| **--output.column-header** {{DT_STR}} | How to derive column headers from attribute names ('none', 'tag', 'auto', 'plain'); *default:* **tag** |
| **--output.column-separator** {{DT_STR}} | Separator in CSV output; *default:* **;** |
| **-H** {{DT_BOOL}}<br> **--human-readable-time** {{DT_BOOL}} | Write time values as hour:minute:second or day:hour:minute:second rather than seconds; *default:* **false** |
| **-o** {{DT_FILE}}<br> **--output-file** {{DT_FILE}} | The generated net will be written to FILE |
| **-p** {{DT_FILE}}<br> **--plain-output-prefix** {{DT_FILE}} | Prefix of files to write plain xml nodes, edges and connections to |
| **--plain-output.lanes** {{DT_BOOL}} | Write all lanes and their attributes even when they are not customized; *default:* **false** |
| **--junctions.join-output** {{DT_FILE}} | Writes information about joined junctions to FILE (can be loaded as additional node-file to reproduce joins |
| **--prefix** {{DT_STR}} | Defines a prefix for edge and junction IDs |
| **--prefix.junction** {{DT_STR}} | Defines a prefix for junction IDs |
| **--prefix.edge** {{DT_STR}} | Defines a prefix for edge IDs |
| **--amitran-output** {{DT_FILE}} | The generated net will be written to FILE using Amitran format |
| **--matsim-output** {{DT_FILE}} | The generated net will be written to FILE using MATSim format |
| **--opendrive-output** {{DT_FILE}} | The generated net will be written to FILE using OpenDRIVE format |
| **--dlr-navteq-output** {{DT_FILE}} | The generated net will be written to dlr-navteq files with the given PREFIX |
| **--dlr-navteq.version** {{DT_STR}} | The dlr-navteq output format version to write; *default:* **6.5** |
| **--dlr-navteq.precision** {{DT_INT}} | The network coordinates are written with the specified level of output precision; *default:* **2** |
| **--output.street-names** {{DT_BOOL}} | Street names will be included in the output (if available); *default:* **false** |
| **--output.original-names** {{DT_BOOL}} | Writes original names, if given, as parameter; *default:* **false** |
| **--output.removed-nodes** {{DT_BOOL}} | Writes IDs of nodes remove with --geometry.remove into edge param; *default:* **false** |
| **--street-sign-output** {{DT_FILE}} | Writes street signs as POIs to FILE |
| **--ptstop-output** {{DT_FILE}} | Writes public transport stops to FILE |
| **--ptline-output** {{DT_FILE}} | Writes public transport lines to FILE |
| **--ptline-clean-up** {{DT_BOOL}} | Clean-up pt stops that are not served by any line; *default:* **false** |
| **--parking-output** {{DT_FILE}} | Writes parking areas to FILE |
| **--railway.topology.output** {{DT_FILE}} | Analyze topology of the railway network |
| **--polygon-output** {{DT_FILE}} | Write shapes that are embedded in the network input and that are not supported by polyconvert (OpenDRIVE) |
| **--opendrive-output.straight-threshold** {{DT_FLOAT}} | Builds parameterized curves whenever the angular change between straight segments exceeds FLOAT degrees; *default:* **1e-08** |
| **--opendrive-output.lefthand-left** {{DT_BOOL}} | Write lanes in lefthand networks on the left side (positive indices); *default:* **false** |
| **--opendrive-output.shape-match-dist** {{DT_FLOAT}} | Match loaded shapes to the closest edge within FLOAT and export as road objects; *default:* **-1** |

### Projection
| Option | Description |
|--------|-------------|
| **--simple-projection** {{DT_BOOL}} | Uses a simple method for projection; *default:* **false** |
| **--proj.scale** {{DT_FLOAT}} | Scaling factor for input coordinates; *default:* **1** |
| **--proj.rotate** {{DT_FLOAT}} | Rotation (clockwise degrees) for input coordinates; *default:* **0** |
| **--proj.utm** {{DT_BOOL}} | Determine the UTM zone (for a universal transversal mercator projection based on the WGS84 ellipsoid); *default:* **false** |
| **--proj.dhdn** {{DT_BOOL}} | Determine the DHDN zone (for a transversal mercator projection based on the bessel ellipsoid, "Gauss-Krueger"); *default:* **false** |
| **--proj** {{DT_STR}} | Uses STR as proj.4 definition for projection; *default:* **!** |
| **--proj.inverse** {{DT_BOOL}} | Inverses projection; *default:* **false** |
| **--proj.dhdnutm** {{DT_BOOL}} | Convert from Gauss-Krueger to UTM; *default:* **false** |
| **--proj.plain-geo** {{DT_BOOL}} | Write geo coordinates in plain-xml; *default:* **false** |

### Processing
| Option | Description |
|--------|-------------|
| **--default.action-step-length** {{DT_FLOAT}} | Length of the default interval length between action points for the car-following and lane-change models (in seconds). If not specified, the simulation step-length is used per default. Vehicle- or VType-specific settings override the default. Must be a multiple of the simulation step-length.; *default:* **0** |
| **--default.speeddev** {{DT_FLOAT}} | Select default speed deviation. A negative value implies vClass specific defaults (0.1 for the default passenger class); *default:* **-1** |
| **--speed-in-kmh** {{DT_BOOL}} | vmax is parsed as given in km/h; *default:* **false** |
| **--construction-date** {{DT_STR}} | Use YYYY-MM-DD date to determine the readiness of features under construction |
| **--flatten** {{DT_BOOL}} | Remove all z-data; *default:* **false** |
| **--plain.extend-edge-shape** {{DT_BOOL}} | If edge shapes do not end at the node positions, extend them; *default:* **false** |
| **--numerical-ids** {{DT_BOOL}} | Remaps alphanumerical IDs of nodes and edges to ensure that all IDs are integers; *default:* **false** |
| **--numerical-ids.node-start** {{DT_INT}} | Remaps IDs of nodes to integers starting at INT; *default:* **2147483647** |
| **--numerical-ids.edge-start** {{DT_INT}} | Remaps IDs of edges to integers starting at INT; *default:* **2147483647** |
| **--reserved-ids** {{DT_FILE}} | Ensures that generated ids do not included any of the typed IDs from FILE (sumo-gui selection file format) |
| **--kept-ids** {{DT_FILE}} | Ensures that objects with typed IDs from FILE (sumo-gui selection file format) are not renamed |
| **--dismiss-vclasses** {{DT_BOOL}} | Removes vehicle class restrictions from imported edges; *default:* **false** |
| **--geometry.split** {{DT_BOOL}} | Splits edges across geometry nodes; *default:* **false** |
| **-R** {{DT_BOOL}}<br> **--geometry.remove** {{DT_BOOL}} | Replace nodes which only define edge geometry by geometry points (joins edges); *default:* **false** |
| **--geometry.remove.keep-edges.explicit** {{DT_STR_LIST}} | Ensure that the given list of edges is not modified |
| **--geometry.remove.keep-edges.input-file** {{DT_FILE}} | Ensure that the edges in FILE are not modified (Each id on a single line. Selection files from sumo-gui are also supported) |
| **--geometry.remove.keep-ptstops** {{DT_BOOL}} | Ensure that edges with public transport stops are not modified; *default:* **false** |
| **--geometry.remove.min-length** {{DT_FLOAT}} | Allow merging edges with differing attributes when their length is below min-length; *default:* **0** |
| **--geometry.remove.width-tolerance** {{DT_FLOAT}} | Allow merging edges with differing lane widths if the difference is below FLOAT; *default:* **0** |
| **--geometry.remove.max-junction-size** {{DT_FLOAT}} | Prevent removal of junctions with a size above FLOAT as defined by custom edge endpoints; *default:* **-1** |
| **--geometry.max-segment-length** {{DT_FLOAT}} | splits geometry to restrict segment length; *default:* **0** |
| **--geometry.min-dist** {{DT_FLOAT}} | reduces too similar geometry points; *default:* **-1** |
| **--geometry.max-angle** {{DT_FLOAT}} | Warn about edge geometries with an angle above DEGREES in successive segments; *default:* **99** |
| **--geometry.max-angle.fix** {{DT_BOOL}} | Straighten edge geometries with an angle above max-angle successive segments; *default:* **false** |
| **--geometry.min-radius** {{DT_FLOAT}} | Warn about edge geometries with a turning radius less than METERS at the start or end; *default:* **9** |
| **--geometry.min-radius.fix** {{DT_BOOL}} | Straighten edge geometries to avoid turning radii less than geometry.min-radius; *default:* **false** |
| **--geometry.min-radius.fix.railways** {{DT_BOOL}} | Straighten edge geometries to avoid turning radii less than geometry.min-radius (only railways); *default:* **true** |
| **--geometry.junction-mismatch-threshold** {{DT_FLOAT}} | Warn if the junction shape is to far away from the original node position; *default:* **20** |
| **--geometry.check-overlap** {{DT_FLOAT}} | Warn if edges overlap by more than the given threshold value; *default:* **0** |
| **--geometry.check-overlap.vertical-threshold** {{DT_FLOAT}} | Ignore overlapping edges if they are separated vertically by the given threshold.; *default:* **4** |
| **--geometry.avoid-overlap** {{DT_BOOL}} | Modify edge geometries to avoid overlap at junctions; *default:* **true** |
| **--join-lanes** {{DT_BOOL}} | join adjacent lanes that have the same permissions and which do not admit lane-changing (sidewalks and disallowed lanes); *default:* **false** |
| **--ptline.match-dist** {{DT_FLOAT}} | Matches stops outside the road network to the referencing pt line when below the given distance; *default:* **100** |
| **--ptstop-output.no-bidi** {{DT_BOOL}} | Skips automatic generation of stops on the bidi-edge of a loaded stop; *default:* **false** |
| **--geometry.max-grade** {{DT_FLOAT}} | Warn about edge geometries with a grade in % above FLOAT.; *default:* **10** |
| **--geometry.max-grade.fix** {{DT_BOOL}} | Smooth edge geometries with a grade above the warning threshold.; *default:* **true** |
| **--offset.disable-normalization** {{DT_BOOL}} | Turn off normalizing node positions |
| **--offset.x** {{DT_FLOAT}} | Adds FLOAT to net x-positions; *default:* **0** |
| **--offset.y** {{DT_FLOAT}} | Adds FLOAT to net y-positions; *default:* **0** |
| **--offset.z** {{DT_FLOAT}} | Adds FLOAT to net z-positions; *default:* **0** |
| **--flip-y-axis** {{DT_BOOL}} | Flips the y-coordinate along zero; *default:* **false** |
| **--roundabouts.guess** {{DT_BOOL}} | Enable roundabout-guessing; *default:* **true** |
| **--roundabouts.guess.max-length** {{DT_FLOAT}} | Structures with a circumference above FLOAT threshold are not classified as roundabout; *default:* **3500** |
| **--roundabouts.visibility-distance** {{DT_FLOAT}} | Default visibility when approaching a roundabout; *default:* **9** |
| **--opposites.guess** {{DT_BOOL}} | Enable guessing of opposite direction lanes usable for overtaking; *default:* **false** |
| **--opposites.guess.fix-lengths** {{DT_BOOL}} | Ensure that opposite edges have the same length; *default:* **true** |
| **--fringe.guess** {{DT_BOOL}} | Enable guessing of network fringe nodes; *default:* **false** |
| **--fringe.guess.speed-threshold** {{DT_FLOAT}} | Guess disconnected edges above the given speed as outer fringe; *default:* **13.8889** |
| **--lefthand** {{DT_BOOL}} | Assumes left-hand traffic on the network; *default:* **false** |
| **--edges.join** {{DT_BOOL}} | Merges edges which connect the same nodes and are close to each other (recommended for VISSIM import); *default:* **false** |
| **--speed.offset** {{DT_FLOAT}} | Modifies all edge speeds by adding FLOAT; *default:* **0** |
| **--speed.factor** {{DT_FLOAT}} | Modifies all edge speeds by multiplying by FLOAT; *default:* **1** |
| **--speed.minimum** {{DT_FLOAT}} | Modifies all edge speeds to at least FLOAT; *default:* **0** |
| **--edges.join-tram-dist** {{DT_FLOAT}} | Joins tram edges into road lanes with similar geometry (within FLOAT distance); *default:* **-1** |

### Building Defaults
| Option | Description |
|--------|-------------|
| **-L** {{DT_INT}}<br> **--default.lanenumber** {{DT_INT}} | The default number of lanes in an edge; *default:* **1** |
| **--default.lanewidth** {{DT_FLOAT}} | The default width of lanes; *default:* **-1** |
| **--default.spreadtype** {{DT_STR}} | The default method for computing lane shapes from edge shapes; *default:* **right** |
| **-S** {{DT_FLOAT}}<br> **--default.speed** {{DT_FLOAT}} | The default speed on an edge (in m/s); *default:* **13.89** |
| **--default.friction** {{DT_FLOAT}} | The default friction on an edge; *default:* **1** |
| **-P** {{DT_INT}}<br> **--default.priority** {{DT_INT}} | The default priority of an edge; *default:* **-1** |
| **--default.type** {{DT_STR}} | The default edge type |
| **--default.sidewalk-width** {{DT_FLOAT}} | The default width of added sidewalks; *default:* **2** |
| **--default.bikelane-width** {{DT_FLOAT}} | The default width of added bike lanes; *default:* **1** |
| **--default.crossing-width** {{DT_FLOAT}} | The default width of a pedestrian crossing; *default:* **4** |
| **--default.crossing-speed** {{DT_FLOAT}} | The default speed 'limit' on a pedestrian crossing (in m/s); *default:* **2.78** |
| **--default.walkingarea-speed** {{DT_FLOAT}} | The default speed 'limit' on a pedestrian walkingarea (in m/s); *default:* **2.78** |
| **--default.allow** {{DT_STR}} | The default for allowed vehicle classes |
| **--default.disallow** {{DT_STR}} | The default for disallowed vehicle classes |
| **--default.junctions.keep-clear** {{DT_BOOL}} | Whether junctions should be kept clear by default; *default:* **true** |
| **--default.junctions.radius** {{DT_FLOAT}} | The default turning radius of intersections; *default:* **4** |
| **--default.connection-length** {{DT_FLOAT}} | The default length when overriding connection lengths; *default:* **-1** |
| **--default.connection.cont-pos** {{DT_FLOAT}} | Whether/where connections should have an internal junction; *default:* **-1** |
| **--default.right-of-way** {{DT_STR}} | The default algorithm for computing right of way rules ('default', 'edgePriority'); *default:* **default** |

### Tls Building
| Option | Description |
|--------|-------------|
| **--tls.discard-loaded** {{DT_BOOL}} | Does not instantiate traffic lights loaded from other formats than plain-XML; *default:* **false** |
| **--tls.discard-simple** {{DT_BOOL}} | Does not instantiate traffic lights at geometry-like nodes loaded from other formats than plain-XML; *default:* **false** |
| **--tls.set** {{DT_STR_LIST}} | Interprets STR[] as list of junctions to be controlled by TLS |
| **--tls.unset** {{DT_STR_LIST}} | Interprets STR[] as list of junctions to be not controlled by TLS |
| **--tls.guess** {{DT_BOOL}} | Turns on TLS guessing; *default:* **false** |
| **--tls.guess.threshold** {{DT_FLOAT}} | Sets minimum value for the sum of all incoming lane speeds when guessing TLS; *default:* **69.4444** |
| **--tls.taz-nodes** {{DT_BOOL}} | Sets district nodes as tls-controlled; *default:* **false** |
| **--tls.guess.joining** {{DT_BOOL}} | Includes node clusters into guess; *default:* **false** |
| **--tls.join** {{DT_BOOL}} | Tries to cluster tls-controlled nodes; *default:* **false** |
| **--tls.join-dist** {{DT_FLOAT}} | Determines the maximal distance for joining traffic lights (defaults to 20); *default:* **20** |
| **--tls.join-exclude** {{DT_STR_LIST}} | Interprets STR[] as list of tls ids to exclude from joining |
| **--tls.uncontrolled-within** {{DT_BOOL}} | Do not control edges that lie fully within a joined traffic light. This may cause collisions but allows old traffic light plans to be used; *default:* **false** |
| **--tls.ignore-internal-junction-jam** {{DT_BOOL}} | Do not build mutually conflicting response matrix, potentially ignoring vehicles that are stuck at an internal junction when their phase has ended; *default:* **false** |
| **--tls.guess-signals** {{DT_BOOL}} | Interprets tls nodes surrounding an intersection as signal positions for a larger TLS. This is typical pattern for OSM-derived networks; *default:* **false** |
| **--tls.guess-signals.dist** {{DT_FLOAT}} | Distance for interpreting nodes as signal locations; *default:* **25** |
| **--tls.guess-signals.slack** {{DT_INT}} | Number of uncontrolled entry edges to accept and still consider the central node as a traffic light; *default:* **0** |
| **--tls.cycle.time** {{DT_INT}} | Use INT as cycle duration; *default:* **90** |
| **--tls.green.time** {{DT_INT}} | Use INT as green phase duration; *default:* **31** |
| **-D** {{DT_FLOAT}}<br> **--tls.yellow.min-decel** {{DT_FLOAT}} | Defines smallest vehicle deceleration; *default:* **3** |
| **--tls.yellow.patch-small** {{DT_BOOL}} | Given yellow times are patched even if being too short; *default:* **false** |
| **--tls.yellow.time** {{DT_INT}} | Set INT as fixed time for yellow phase durations; *default:* **-1** |
| **--tls.red.time** {{DT_INT}} | Set INT as fixed time for red phase duration at traffic lights that do not have a conflicting flow; *default:* **5** |
| **--tls.allred.time** {{DT_INT}} | Set INT as fixed time for intermediate red phase after every switch; *default:* **0** |
| **--tls.minor-left.max-speed** {{DT_FLOAT}} | Use FLOAT as threshold for allowing left-turning vehicles to move in the same phase as oncoming straight-going vehicles; *default:* **19.44** |
| **--tls.left-green.time** {{DT_INT}} | Use INT as green phase duration for left turns (s). Setting this value to 0 disables additional left-turning phases; *default:* **6** |
| **--tls.nema.vehExt** {{DT_INT}} | Set INT as fixed time for intermediate vehext phase after every switch; *default:* **2** |
| **--tls.nema.yellow** {{DT_INT}} | Set INT as fixed time for intermediate NEMA yellow phase after every switch; *default:* **3** |
| **--tls.nema.red** {{DT_INT}} | Set INT as fixed time for intermediate NEMA red phase after every switch; *default:* **2** |
| **--tls.crossing-min.time** {{DT_INT}} | Use INT as minimum green duration for pedestrian crossings (s).; *default:* **4** |
| **--tls.crossing-clearance.time** {{DT_INT}} | Use INT as clearance time for pedestrian crossings (s).; *default:* **5** |
| **--tls.scramble.time** {{DT_INT}} | Use INT as green phase duration for pedestrian scramble phase (s).; *default:* **5** |
| **--tls.half-offset** {{DT_STR_LIST}} | TLSs in STR[] will be shifted by half-phase |
| **--tls.quarter-offset** {{DT_STR_LIST}} | TLSs in STR[] will be shifted by quarter-phase |
| **--tls.default-type** {{DT_STR}} | TLSs with unspecified type will use STR as their algorithm; *default:* **static** |
| **--tls.layout** {{DT_STR}} | Set phase layout four grouping opposite directions or grouping all movements for one incoming edge ['opposites', 'incoming']; *default:* **opposites** |
| **--tls.no-mixed** {{DT_BOOL}} | Avoid phases with green and red signals for different connections from the same lane; *default:* **false** |
| **--tls.min-dur** {{DT_INT}} | Default minimum phase duration for traffic lights with variable phase length; *default:* **5** |
| **--tls.max-dur** {{DT_INT}} | Default maximum phase duration for traffic lights with variable phase length; *default:* **50** |
| **--tls.group-signals** {{DT_BOOL}} | Assign the same tls link index to connections that share the same states; *default:* **false** |
| **--tls.ungroup-signals** {{DT_BOOL}} | Assign a distinct tls link index to every connection; *default:* **false** |
| **--tls.rebuild** {{DT_BOOL}} | rebuild all traffic light plans in the network; *default:* **false** |

### Ramp Guessing
| Option | Description |
|--------|-------------|
| **--ramps.guess** {{DT_BOOL}} | Enable ramp-guessing; *default:* **false** |
| **--ramps.guess-acceleration-lanes** {{DT_BOOL}} | Guess on-ramps and mark acceleration lanes if they exist but do not add new lanes; *default:* **true** |
| **--ramps.max-ramp-speed** {{DT_FLOAT}} | Treat edges with speed > FLOAT as no ramps; *default:* **-1** |
| **--ramps.min-highway-speed** {{DT_FLOAT}} | Treat edges with speed < FLOAT as no highways; *default:* **21.9444** |
| **--ramps.ramp-length** {{DT_FLOAT}} | Use FLOAT as ramp-length; *default:* **100** |
| **--ramps.min-weave-length** {{DT_FLOAT}} | Use FLOAT as minimum ramp-length; *default:* **50** |
| **--ramps.set** {{DT_STR_LIST}} | Tries to handle the given edges as ramps |
| **--ramps.unset** {{DT_STR_LIST}} | Do not consider the given edges as ramps |
| **--ramps.no-split** {{DT_BOOL}} | Avoids edge splitting; *default:* **false** |

### Edge Removal
| Option | Description |
|--------|-------------|
| **--keep-edges.min-speed** {{DT_FLOAT}} | Only keep edges with speed in meters/second > FLOAT; *default:* **-1** |
| **--remove-edges.explicit** {{DT_STR_LIST}} | Remove edges in STR[] |
| **--keep-edges.explicit** {{DT_STR_LIST}} | Only keep edges in STR[] or those which are kept due to other keep-edges or remove-edges options |
| **--keep-edges.input-file** {{DT_FILE}} | Only keep edges in FILE (Each id on a single line. Selection files from sumo-gui are also supported) or those which are kept due to other keep-edges or remove-edges options |
| **--remove-edges.input-file** {{DT_FILE}} | Remove edges in FILE. (Each id on a single line. Selection files from sumo-gui are also supported) |
| **--keep-edges.postload** {{DT_BOOL}} | Remove edges after loading, patching and joining; *default:* **false** |
| **--keep-edges.in-boundary** {{DT_STR_LIST}} | Only keep edges which are located within the given boundary (given either as CARTESIAN corner coordinates <xmin,ymin,xmax,ymax> or as polygon <x0,y0,x1,y1,...>) |
| **--keep-edges.in-geo-boundary** {{DT_STR_LIST}} | Only keep edges which are located within the given boundary (given either as GEODETIC corner coordinates <lon-min,lat-min,lon-max,lat-max> or as polygon <lon0,lat0,lon1,lat1,...>) |
| **--keep-edges.by-vclass** {{DT_STR_LIST}} | Only keep edges which allow one of the vclasses in STR[] |
| **--remove-edges.by-vclass** {{DT_STR_LIST}} | Remove edges which allow only vclasses from STR[] |
| **--keep-edges.by-type** {{DT_STR_LIST}} | Only keep edges where type is in STR[] |
| **--keep-edges.components** {{DT_INT}} | Only keep the INT largest weakly connected components; *default:* **0** |
| **--remove-edges.by-type** {{DT_STR_LIST}} | Remove edges where type is in STR[] |
| **--remove-edges.isolated** {{DT_BOOL}} | Removes isolated edges; *default:* **false** |
| **--keep-lanes.min-width** {{DT_FLOAT}} | Only keep lanes with width in meters > FLOAT; *default:* **0.01** |

### Unregulated Nodes
| Option | Description |
|--------|-------------|
| **--keep-nodes-unregulated** {{DT_BOOL}} | All nodes will be unregulated; *default:* **false** |
| **--keep-nodes-unregulated.explicit** {{DT_STR_LIST}} | Do not regulate nodes in STR[] |
| **--keep-nodes-unregulated.district-nodes** {{DT_BOOL}} | Do not regulate district nodes; *default:* **false** |

### Junctions
| Option | Description |
|--------|-------------|
| **--junctions.right-before-left.speed-threshold** {{DT_FLOAT}} | Allow building right-before-left junctions when the incoming edge speeds are below FLOAT (m/s); *default:* **13.6111** |
| **--junctions.left-before-right** {{DT_BOOL}} | Build left-before-right junctions instead of right-before-left junctions; *default:* **false** |
| **--no-internal-links** {{DT_BOOL}} | Omits internal links; *default:* **false** |
| **--no-turnarounds** {{DT_BOOL}} | Disables building turnarounds |
| **--no-turnarounds.tls** {{DT_BOOL}} | Disables building turnarounds at tls-controlled junctions; *default:* **false** |
| **--no-turnarounds.geometry** {{DT_BOOL}} | Disables building turnarounds at geometry-like junctions; *default:* **true** |
| **--no-turnarounds.except-deadend** {{DT_BOOL}} | Disables building turnarounds except at dead end junctions; *default:* **false** |
| **--no-turnarounds.except-turnlane** {{DT_BOOL}} | Disables building turnarounds except at at junctions with a dedicated turning lane; *default:* **false** |
| **--no-turnarounds.fringe** {{DT_BOOL}} | Disables building turnarounds at fringe junctions; *default:* **false** |
| **--no-left-connections** {{DT_BOOL}} | Disables building connections to left; *default:* **false** |
| **--junctions.join** {{DT_BOOL}} | Joins junctions that are close to each other (recommended for OSM import); *default:* **false** |
| **--junctions.join-dist** {{DT_FLOAT}} | Determines the maximal distance for joining junctions (defaults to 10); *default:* **10** |
| **--junctions.join.parallel-threshold** {{DT_FLOAT}} | The angular threshold in degress for rejection of parallel edges when joining junctions; *default:* **30** |
| **--junctions.join-exclude** {{DT_STR_LIST}} | Interprets STR[] as list of junctions to exclude from joining |
| **--junctions.join-same** {{DT_FLOAT}} | Joins junctions that have similar coordinates even if not connected; *default:* **-1** |
| **--junctions.attach-removed** {{DT_FLOAT}} | Attach junction to the closest edge within FLOAT distance that has it's id in param removedNodeIDs (for joining networks); *default:* **-1** |
| **--max-join-ids** {{DT_INT}} | Abbreviate junction or TLS id if it joins more than INT junctions; *default:* **4** |
| **--junctions.corner-detail** {{DT_INT}} | Generate INT intermediate points to smooth out intersection corners; *default:* **5** |
| **--junctions.internal-link-detail** {{DT_INT}} | Generate INT intermediate points to smooth out lanes within the intersection; *default:* **5** |
| **--junctions.scurve-stretch** {{DT_FLOAT}} | Generate longer intersections to allow for smooth s-curves when the number of lanes changes; *default:* **0** |
| **--junctions.join-turns** {{DT_BOOL}} | Builds common edges for turning connections with common from- and to-edge. This causes discrepancies between geometrical length and assigned length due to averaging but enables lane-changing while turning; *default:* **false** |
| **--junctions.limit-turn-speed** {{DT_FLOAT}} | Limits speed on junctions to an average lateral acceleration of at most FLOAT (m/s^2); *default:* **5.5** |
| **--junctions.limit-turn-speed.min-angle** {{DT_FLOAT}} | Do not limit turn speed for angular changes below FLOAT (degrees). The value is subtracted from the geometric angle before computing the turning radius.; *default:* **15** |
| **--junctions.limit-turn-speed.min-angle.railway** {{DT_FLOAT}} | Do not limit turn speed for angular changes below FLOAT (degrees) on railway edges. The value is subtracted from the geometric angle before computing the turning radius.; *default:* **35** |
| **--junctions.limit-turn-speed.warn.straight** {{DT_FLOAT}} | Warn about turn speed limits that reduce the speed of straight connections by more than FLOAT; *default:* **5** |
| **--junctions.limit-turn-speed.warn.turn** {{DT_FLOAT}} | Warn about turn speed limits that reduce the speed of turning connections (no u-turns) by more than FLOAT; *default:* **22** |
| **--junctions.small-radius** {{DT_FLOAT}} | Default radius for junctions that do not require wide vehicle turns; *default:* **1.5** |
| **--junctions.higher-speed** {{DT_BOOL}} | Use maximum value of incoming and outgoing edge speed on junction instead of average; *default:* **false** |
| **--junctions.minimal-shape** {{DT_BOOL}} | Build junctions with minimal shapes (ignoring edge overlap); *default:* **false** |
| **--junctions.endpoint-shape** {{DT_BOOL}} | Build junction shapes based on edge endpoints (ignoring edge overlap); *default:* **false** |
| **--internal-junctions.vehicle-width** {{DT_FLOAT}} | Assumed vehicle width for computing internal junction positions; *default:* **1.8** |
| **--rectangular-lane-cut** {{DT_BOOL}} | Forces rectangular cuts between lanes and intersections; *default:* **false** |
| **--check-lane-foes.roundabout** {{DT_BOOL}} | Allow driving onto a multi-lane road if there are foes on other lanes (at roundabouts); *default:* **true** |
| **--check-lane-foes.all** {{DT_BOOL}} | Allow driving onto a multi-lane road if there are foes on other lanes (everywhere); *default:* **false** |

### Pedestrian
| Option | Description |
|--------|-------------|
| **--sidewalks.guess** {{DT_BOOL}} | Guess pedestrian sidewalks based on edge speed; *default:* **false** |
| **--sidewalks.guess.max-speed** {{DT_FLOAT}} | Add sidewalks for edges with a speed equal or below the given limit; *default:* **13.89** |
| **--sidewalks.guess.min-speed** {{DT_FLOAT}} | Add sidewalks for edges with a speed above the given limit; *default:* **5.8** |
| **--sidewalks.guess.from-permissions** {{DT_BOOL}} | Add sidewalks for edges that allow pedestrians on any of their lanes regardless of speed; *default:* **false** |
| **--sidewalks.guess.exclude** {{DT_STR_LIST}} | Do not guess sidewalks for the given list of edges |
| **--crossings.guess** {{DT_BOOL}} | Guess pedestrian crossings based on the presence of sidewalks; *default:* **false** |
| **--crossings.guess.speed-threshold** {{DT_FLOAT}} | At uncontrolled nodes, do not build crossings across edges with a speed above the threshold; *default:* **13.89** |
| **--crossings.guess.roundabout-priority** {{DT_BOOL}} | Give priority to guessed crossings at roundabouts; *default:* **true** |
| **--walkingareas** {{DT_BOOL}} | Always build walking areas even if there are no crossings; *default:* **false** |
| **--walkingareas.join-dist** {{DT_FLOAT}} | Do not create a walkingarea between sidewalks that are connected by a pedestrian junction within FLOAT; *default:* **15** |

### Bicycle
| Option | Description |
|--------|-------------|
| **--bikelanes.guess** {{DT_BOOL}} | Guess bike lanes based on edge speed; *default:* **false** |
| **--bikelanes.guess.max-speed** {{DT_FLOAT}} | Add bike lanes for edges with a speed equal or below the given limit; *default:* **22.22** |
| **--bikelanes.guess.min-speed** {{DT_FLOAT}} | Add bike lanes for edges with a speed above the given limit; *default:* **5.8** |
| **--bikelanes.guess.from-permissions** {{DT_BOOL}} | Add bike lanes for edges that allow bicycles on any of their lanes regardless of speed; *default:* **false** |
| **--bikelanes.guess.exclude** {{DT_STR_LIST}} | Do not guess bikelanes for the given list of edges |

### Railway
| Option | Description |
|--------|-------------|
| **--railway.signals.discard** {{DT_BOOL}} | Discard all railway signal information loaded from other formats than plain-xml; *default:* **false** |
| **--railway.topology.repair** {{DT_BOOL}} | Repair topology of the railway network; *default:* **false** |
| **--railway.topology.repair.minimal** {{DT_BOOL}} | Repair topology of the railway network just enough to let loaded public transport lines to work; *default:* **false** |
| **--railway.topology.repair.connect-straight** {{DT_BOOL}} | Allow bidirectional rail use wherever rails with opposite directions meet at a straight angle; *default:* **false** |
| **--railway.topology.repair.stop-turn** {{DT_BOOL}} | Add turn-around connections at all loaded stops.; *default:* **false** |
| **--railway.topology.repair.bidi-penalty** {{DT_FLOAT}} | Penalty factor for adding new bidi edges to connect public transport stops; *default:* **1.2** |
| **--railway.topology.all-bidi** {{DT_BOOL}} | Make all rails usable in both direction; *default:* **false** |
| **--railway.topology.all-bidi.input-file** {{DT_FILE}} | Make all rails edge ids from FILE usable in both direction |
| **--railway.topology.direction-priority** {{DT_BOOL}} | Set edge priority values based on estimated main direction; *default:* **false** |
| **--railway.topology.extend-priority** {{DT_BOOL}} | Extend loaded edge priority values based on estimated main direction; *default:* **false** |
| **--railway.geometry.straighten** {{DT_BOOL}} | Move junctions to straighten a sequence of rail edges; *default:* **false** |
| **--railway.signal.guess.by-stops** {{DT_BOOL}} | Guess signals that guard public transport stops; *default:* **false** |
| **--railway.signal.permit-unsignalized** {{DT_STR_LIST}} | List rail classes that may run without rail signals; *default:* **tram,cable_car** |
| **--railway.access-distance** {{DT_FLOAT}} | The search radius for finding suitable road accesses for rail stops; *default:* **150** |
| **--railway.max-accesses** {{DT_INT}} | The maximum road accesses registered per rail stops; *default:* **5** |
| **--railway.access-factor** {{DT_FLOAT}} | The walking length of the access is computed as air-line distance multiplied by FLOAT; *default:* **1.5** |

### Formats
| Option | Description |
|--------|-------------|
| **--discard-params** {{DT_STR_LIST}} | Remove the list of keys from all params |
| **--ignore-change-restrictions** {{DT_STR_LIST}} | List vehicle classes that may ignore lane changing restrictions ('all' discards all restrictions); *default:* **authority** |
| **--ignore-widths** {{DT_BOOL}} | Whether lane widths shall be ignored.; *default:* **false** |
| **--osm.skip-duplicates-check** {{DT_BOOL}} | Skips the check for duplicate nodes and edges; *default:* **false** |
| **--osm.elevation** {{DT_BOOL}} | Imports elevation data; *default:* **false** |
| **--osm.layer-elevation** {{DT_FLOAT}} | Reconstruct (relative) elevation based on layer data. Each layer is raised by FLOAT m; *default:* **0** |
| **--osm.layer-elevation.max-grade** {{DT_FLOAT}} | Maximum grade threshold in % at 50km/h when reconstructing elevation based on layer data. The value is scaled according to road speed.; *default:* **10** |
| **--osm.oneway-spread-right** {{DT_BOOL}} | Whether one-way roads should be spread to the side instead of centered; *default:* **false** |
| **--osm.lane-access** {{DT_BOOL}} | Import lane-specific access restrictions; *default:* **false** |
| **--osm.bike-access** {{DT_BOOL}} | Import bike lanes and fix directions and permissions on bike paths; *default:* **false** |
| **--osm.sidewalks** {{DT_BOOL}} | Import sidewalks; *default:* **false** |
| **--osm.oneway-reverse-sidewalk** {{DT_BOOL}} | Default to building two sidewalks on oneway streets (may affect divided roads); *default:* **false** |
| **--osm.crossings** {{DT_BOOL}} | Import crossings; *default:* **false** |
| **--osm.turn-lanes** {{DT_BOOL}} | Import turning arrows from OSM to help with connection building; *default:* **false** |
| **--osm.stop-output.length** {{DT_FLOAT}} | The default length of a public transport stop in FLOAT m; *default:* **25** |
| **--osm.stop-output.length.bus** {{DT_FLOAT}} | The default length of a bus stop in FLOAT m; *default:* **15** |
| **--osm.stop-output.length.tram** {{DT_FLOAT}} | The default length of a tram stop in FLOAT m; *default:* **25** |
| **--osm.stop-output.length.train** {{DT_FLOAT}} | The default length of a train stop in FLOAT m; *default:* **200** |
| **--osm.railsignals** {{DT_STR_LIST}} | Specify custom rules for importing railway signals; *default:* **DEFAULT** |
| **--osm.all-attributes** {{DT_BOOL}} | Whether additional attributes shall be imported; *default:* **false** |
| **--osm.extra-attributes** {{DT_STR_LIST}} | List of additional attributes that shall be imported from OSM via osm.all-attributes (set 'all' to import all); *default:* **all** |
| **--osm.speedlimit-none** {{DT_FLOAT}} | The speed limit to be set when there is no actual speed limit in reality; *default:* **39.4444** |
| **--osm.annotate-defaults** {{DT_BOOL}} | Whether edges shoulds carry information on the usage of typemap defaults; *default:* **false** |
| **--matsim.keep-length** {{DT_BOOL}} | The edge lengths given in the MATSim-file will be kept; *default:* **false** |
| **--matsim.lanes-from-capacity** {{DT_BOOL}} | The lane number will be computed from the capacity; *default:* **false** |
| **--shapefile.street-id** {{DT_STR}} | Read edge ids from column STR |
| **--shapefile.from-id** {{DT_STR}} | Read from-node ids from column STR |
| **--shapefile.to-id** {{DT_STR}} | Read to-node ids from column STR |
| **--shapefile.type-id** {{DT_STR}} | Read type ids from column STR |
| **--shapefile.laneNumber** {{DT_STR}} | Read lane number from column STR |
| **--shapefile.speed** {{DT_STR}} | Read speed from column STR |
| **--shapefile.length** {{DT_STR}} | Read custom edge length from column STR |
| **--shapefile.width** {{DT_STR}} | Read total edge width from column STR |
| **--shapefile.name** {{DT_STR}} | Read (non-unique) name from column STR |
| **--shapefile.node-join-dist** {{DT_FLOAT}} | Distance threshold for determining whether distinct shapes are connected (used when from-id and to-id are not available); *default:* **0** |
| **--shapefile.add-params** {{DT_STR_LIST}} | Add the list of field names as edge params |
| **--shapefile.use-defaults-on-failure** {{DT_BOOL}} | Uses edge type defaults on problems; *default:* **false** |
| **--shapefile.all-bidirectional** {{DT_BOOL}} | Insert edges in both directions; *default:* **false** |
| **--shapefile.guess-projection** {{DT_BOOL}} | Guess the proper projection; *default:* **false** |
| **--shapefile.traditional-axis-mapping** {{DT_BOOL}} | Use traditional axis order (lon, lat); *default:* **false** |
| **--dlr-navteq.tolerant-permissions** {{DT_BOOL}} | Allow more vehicle classes by default; *default:* **false** |
| **--dlr-navteq.keep-length** {{DT_BOOL}} | The edge lengths given in the DLR Navteq-file will be kept; *default:* **false** |
| **--vissim.join-distance** {{DT_FLOAT}} | Structure join offset; *default:* **5** |
| **--vissim.default-speed** {{DT_FLOAT}} | Use FLOAT as default speed; *default:* **50** |
| **--vissim.speed-norm** {{DT_FLOAT}} | Factor for edge velocity; *default:* **1** |
| **--vissim.report-unset-speeds** {{DT_BOOL}} | Writes lanes without an explicit speed set; *default:* **false** |
| **--visum.language-file** {{DT_FILE}} | Load language mappings from FILE |
| **--visum.use-type-priority** {{DT_BOOL}} | Uses priorities from types; *default:* **false** |
| **--visum.use-type-laneno** {{DT_BOOL}} | Uses lane numbers from types; *default:* **false** |
| **--visum.use-type-speed** {{DT_BOOL}} | Uses speeds from types; *default:* **false** |
| **--visum.connector-speeds** {{DT_FLOAT}} | Sets connector speed; *default:* **100** |
| **--visum.connectors-lane-number** {{DT_INT}} | Sets connector lane number; *default:* **3** |
| **--visum.no-connectors** {{DT_BOOL}} | Excludes connectors; *default:* **true** |
| **--visum.recompute-lane-number** {{DT_BOOL}} | Computes the number of lanes from the edges' capacities; *default:* **false** |
| **--visum.verbose-warnings** {{DT_BOOL}} | Prints all warnings, some of which are due to VISUM misbehaviour; *default:* **false** |
| **--visum.lanes-from-capacity.norm** {{DT_FLOAT}} | The factor for flow to no. lanes conversion; *default:* **1800** |
| **--opendrive.import-all-lanes** {{DT_BOOL}} | Imports all lane types; *default:* **false** |
| **--opendrive.curve-resolution** {{DT_FLOAT}} | The geometry resolution in m when importing curved geometries as line segments.; *default:* **2** |
| **--opendrive.advance-stopline** {{DT_FLOAT}} | Allow stop lines to be built beyond the start of the junction if the geometries allow so; *default:* **0** |
| **--opendrive.min-width** {{DT_FLOAT}} | The minimum lane width for determining start or end of variable-width lanes; *default:* **1.8** |
| **--opendrive.internal-shapes** {{DT_BOOL}} | Import internal lane shapes; *default:* **false** |
| **--opendrive.position-ids** {{DT_BOOL}} | Sets edge-id based on road-id and offset in m (legacy); *default:* **false** |
| **--opendrive.lane-shapes** {{DT_BOOL}} | Use custom lane shapes to compensate discarded lane types; *default:* **false** |
| **--opendrive.signal-groups** {{DT_BOOL}} | Use the OpenDRIVE controller information for the generated signal program; *default:* **false** |
| **--opendrive.ignore-misplaced-signals** {{DT_BOOL}} | Ignore traffic signals which do not control any driving lane; *default:* **false** |

### Netedit
| Option | Description |
|--------|-------------|
| **--new-network** {{DT_BOOL}} | Start netedit with a new network; *default:* **false** |
| **--attribute-help-output** {{DT_FILE}} | Write attribute help to file |
| **--ignore-supermode-question** {{DT_BOOL}} | Ignore question dialog during changing between supermodes in undo-redo; *default:* **false** |
| **--ignore.additionalelements** {{DT_BOOL}} | Ignore additional elements during loading of sumo-configs; *default:* **false** |
| **--ignore.routeelements** {{DT_BOOL}} | Ignore route elements during loading of sumo-configs; *default:* **false** |
| **--e2.friendlyPos.automatic** {{DT_BOOL}} | If the lane is shorter than the additional, automatically enable friendlyPos; *default:* **true** |
| **--force-saving** {{DT_BOOL}} | If enabled, elements will be saved regardless of whether they have been edited or not; *default:* **false** |
| **--node-prefix** {{DT_STR}} | Prefix for node naming; *default:* **J** |
| **--edge-prefix** {{DT_STR}} | Prefix for edge naming; *default:* **E** |
| **--edge-infix** {{DT_STR}} | Enable edge-infix (<fromNodeID><infix><toNodeID>) |
| **--busStop-prefix** {{DT_STR}} | Prefix for busStop naming; *default:* **bs** |
| **--trainStop-prefix** {{DT_STR}} | Prefix for trainStop naming; *default:* **ts** |
| **--containerStop-prefix** {{DT_STR}} | Prefix for containerStop naming; *default:* **ct** |
| **--chargingStation-prefix** {{DT_STR}} | Prefix for chargingStation naming; *default:* **cs** |
| **--parkingArea-prefix** {{DT_STR}} | Prefix for parkingArea naming; *default:* **pa** |
| **--e1Detector-prefix** {{DT_STR}} | Prefix for e1Detector naming; *default:* **e1** |
| **--e2Detector-prefix** {{DT_STR}} | Prefix for e2Detector naming; *default:* **e2** |
| **--e3Detector-prefix** {{DT_STR}} | Prefix for e3Detector naming; *default:* **e3** |
| **--e1InstantDetector-prefix** {{DT_STR}} | Prefix for e1InstantDetector naming; *default:* **e1i** |
| **--rerouter-prefix** {{DT_STR}} | Prefix for rerouter naming; *default:* **rr** |
| **--calibrator-prefix** {{DT_STR}} | Prefix for calibrator naming; *default:* **ca** |
| **--routeProbe-prefix** {{DT_STR}} | Prefix for routeProbe naming; *default:* **rp** |
| **--vss-prefix** {{DT_STR}} | Prefix for variable speed sign naming; *default:* **vs** |
| **--tractionSubstation-prefix** {{DT_STR}} | Prefix for traction substation naming; *default:* **tr** |
| **--overheadWire-prefix** {{DT_STR}} | Prefix for overhead wire naming; *default:* **ow** |
| **--polygon-prefix** {{DT_STR}} | Prefix for polygon naming; *default:* **po** |
| **--poi-prefix** {{DT_STR}} | Prefix for poi naming; *default:* **poi** |
| **--jps.walkableArea-prefix** {{DT_STR}} | Prefix for jps walkable area naming; *default:* **jps.walkable_area** |
| **--jps.obstacle-prefix** {{DT_STR}} | Prefix for jps obstacle naming; *default:* **jps.obstacle** |
| **--route-prefix** {{DT_STR}} | Prefix for route naming; *default:* **r** |
| **--routeDistribution-prefix** {{DT_STR}} | Prefix for route distribution naming; *default:* **rd** |
| **--vType-prefix** {{DT_STR}} | Prefix for type naming; *default:* **t** |
| **--vTypeDistribution-prefix** {{DT_STR}} | Prefix for type distribution naming; *default:* **td** |
| **--vehicle-prefix** {{DT_STR}} | Prefix for vehicle naming; *default:* **v** |
| **--trip-prefix** {{DT_STR}} | Prefix for trip naming; *default:* **t** |
| **--flow-prefix** {{DT_STR}} | Prefix for flow naming; *default:* **f** |
| **--person-prefix** {{DT_STR}} | Prefix for person naming; *default:* **p** |
| **--personflow-prefix** {{DT_STR}} | Prefix for personFlow naming; *default:* **pf** |
| **--container-prefix** {{DT_STR}} | Prefix for container naming; *default:* **c** |
| **--containerflow-prefix** {{DT_STR}} | Prefix for containerFlow naming; *default:* **cf** |
| **--dataSet-prefix** {{DT_STR}} | Prefix for dataSet naming; *default:* **ds** |
| **--meanDataEdge-prefix** {{DT_STR}} | Prefix for meanDataEdge naming; *default:* **ed** |
| **--meanDataLane-prefix** {{DT_STR}} | Prefix for meanDataLane naming; *default:* **ld** |

### Visualisation
| Option | Description |
|--------|-------------|
| **--disable-laneIcons** {{DT_BOOL}} | Disable icons of special lanes; *default:* **false** |
| **-T** {{DT_BOOL}}<br> **--disable-textures** {{DT_BOOL}} | Disable textures; *default:* **false** |
| **-g** {{DT_FILE}}<br> **--gui-settings-file** {{DT_FILE}} | Load visualisation settings from FILE |
| **--registry-viewport** {{DT_BOOL}} | Load current viewport from registry; *default:* **false** |
| **--window-size** {{DT_STR_LIST}} | Create initial window with the given x,y size |
| **--window-pos** {{DT_STR_LIST}} | Create initial window at the given x,y position |
| **--gui-testing** {{DT_BOOL}} | Enable overlay for screen recognition; *default:* **false** |
| **--gui-testing-debug** {{DT_BOOL}} | Enable output messages during GUI-Testing; *default:* **false** |
| **--gui-testing-debug-gl** {{DT_BOOL}} | Enable output messages during GUI-Testing specific of gl functions; *default:* **false** |
| **--gui-testing.setting-output** {{DT_FILE}} | Save gui settings in the given settings-output file |

### Time
| Option | Description |
|--------|-------------|
| **--begin** {{DT_TIME}} | Defines the begin time in seconds; The simulation starts at this time; *default:* **0** |
| **--end** {{DT_TIME}} | Defines the end time in seconds; The simulation ends at this time; *default:* **-1** |

### Report
| Option | Description |
|--------|-------------|
| **-v** {{DT_BOOL}}<br> **--verbose** {{DT_BOOL}} | Switches to verbose output; *default:* **false** |
| **--print-options** {{DT_BOOL}} | Prints option values before processing; *default:* **false** |
| **-?** {{DT_BOOL}}<br> **--help** {{DT_BOOL}} | Prints this screen or selected topics; *default:* **false** |
| **-V** {{DT_BOOL}}<br> **--version** {{DT_BOOL}} | Prints the current version; *default:* **false** |
| **-X** {{DT_STR}}<br> **--xml-validation** {{DT_STR}} | Set schema validation scheme of XML inputs ("never", "local", "auto" or "always"); *default:* **local** |
| **--xml-validation.net** {{DT_STR}} | Set schema validation scheme of SUMO network inputs ("never", "local", "auto" or "always"); *default:* **never** |
| **--xml-validation.routes** {{DT_STR}} | Set schema validation scheme of SUMO route inputs ("never", "local", "auto" or "always"); *default:* **local** |
| **-W** {{DT_BOOL}}<br> **--no-warnings** {{DT_BOOL}} | Disables output of warnings; *default:* **false** |
| **--aggregate-warnings** {{DT_INT}} | Aggregate warnings of the same type whenever more than INT occur; *default:* **-1** |
| **-l** {{DT_FILE}}<br> **--log** {{DT_FILE}} | Writes all messages to FILE (implies verbose) |
| **--message-log** {{DT_FILE}} | Writes all non-error messages to FILE (implies verbose) |
| **--error-log** {{DT_FILE}} | Writes all warnings and errors to FILE |
| **--log.timestamps** {{DT_BOOL}} | Writes timestamps in front of all messages; *default:* **false** |
| **--log.processid** {{DT_BOOL}} | Writes process ID in front of all messages; *default:* **false** |
| **--language** {{DT_STR}} | Language to use in messages; *default:* **C** |
| **--ignore-errors** {{DT_BOOL}} | Continue on broken input; *default:* **false** |
| **--ignore-errors.connections** {{DT_BOOL}} | Continue on invalid connections; *default:* **false** |
| **--show-errors.connections-first-try** {{DT_BOOL}} | Show errors in connections at parsing; *default:* **false** |
| **--ignore-errors.edge-type** {{DT_BOOL}} | Continue on unknown edge types; *default:* **false** |

### Random Number
| Option | Description |
|--------|-------------|
| **--random** {{DT_BOOL}} | Initialises the random number generator with the current system time; *default:* **false** |
| **--seed** {{DT_INT}} | Initialises the random number generator with the given value; *default:* **23423** |
