---
title: Sumolib
---

**sumolib** is a set of python modules for working with sumo networks,
simulation output and other simulation artifacts. For a detailed list of
available functions see the [pydoc generated
documentation](https://sumo.dlr.de/pydoc/sumolib.html). You can
[browse the code here]({{Source}}tools/sumolib).

# Module Overview

| Module | Purpose |
| --- | --- |
| `sumolib` | Binary discovery (`checkBinary`), running SUMO tools (`call`, `saveConfiguration`), file stream helpers (`openz`, `TeeFile`) |
| `sumolib.net` | Parsing `.net.xml` files, querying nodes, edges, lanes, connections, roundabouts, traffic lights, spatial lookup, coordinate projection, and shortest/fastest path routing |
| `sumolib.xml` | Streaming XML parsers (`parse`, `parse_fast`, `parse_fast_nested`, `parse_fast_structured`), XML document creation and serialization, SUMO headers, and XSD inspection (`sumolib.xml.xsd`) |
| `sumolib.route` | Computing route lengths with internal edges (`getLength`, `addInternal`) and map matching 2D coordinate traces onto networks (`mapTrace`) |
| `sumolib.geomhelper` | 2D geometry calculations: distances, polyline offsets, intersections, polygon containment, lateral offsetting, and angle conversions |
| `sumolib.options` | `ArgumentParser` extending `argparse` with SUMO configuration files (`-c`, `-C`, `--save-template`), time parsing, and prefixed subprogram options |
| `sumolib.miscutils` | Transparent file/gzip/URL/socket opening (`openz`), SUMO time parsing (`parseTime`, `humanReadableTime`), flow count calculation (`getFlowNumber`), benchmarking, and socket ports |
| `sumolib.statistics` | Descriptive statistics (`Statistics`), histograms, XML/text summaries, and calibration error metrics (`geh`, `sqv`) |
| `sumolib.shapes` | Reading and writing SUMO PoI (`sumolib.shapes.poi`) and polygon (`sumolib.shapes.polygon`) files |
| `sumolib.files` | Reading and writing selection files (`sumolib.files.selection`) and additional files (`sumolib.files.additional`) |
| `sumolib.sensors` | Reading and writing inductive loop (`e1Detector`) definitions (`sumolib.sensors.inductive_loop`) |
| `sumolib.output` | SAX helpers for `edgeData`/`laneData` (`dump.readDump`), inductive loops (`inductionloop.readInductLoop`), generic SAX queries, and trace converters (`sumolib.output.convert`) |
| `sumolib.vehicletype` | Sampling and writing `<vTypeDistribution>` definitions (`CreateVehTypeDistribution`, `VehAttribute`) |
| `sumolib.color` | Parsing and serializing SUMO RGBA color specifications (`RGBAColor`, `decodeXML`) |
| `sumolib.fpdiff` | Floating-point-tolerant line diffing and filtering (`diff`, `fpfilter`) |
| `sumolib.visualization.helpers` | Shared `matplotlib` plotting options, network plotting (`plotNet`), and figure lifecycle helpers |
| `sumolib.datastructures` | Ordered multiset (`OrderedMultiSet`) preserving insertion order |

# importing **sumolib** in a script

You can install `sumolib` as a standalone Python package via `pip install sumolib` (see [Downloads](../Downloads.md)). When running against a local SUMO checkout or installation, add the {{SUMO}}/tools directory to the python load path:

```python
import os
import sys
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
```

## Discovering and running SUMO binaries

`sumolib.checkBinary(name, bindir=None)` locates an executable such as `sumo`, `sumo-gui`, `netconvert`, or `duarouter` by checking `<NAME>_BINARY` (`GUISIM_BINARY` for `sumo-gui`), `bindir`, `$SUMO_HOME/bin`, the installed `sumo` package, and `../../bin` relative to `sumolib`:

```python
import sumolib

sumoBinary = sumolib.checkBinary('sumo')
netconvertBinary = sumolib.checkBinary('netconvert')
```

`sumolib.call(executable, args)` builds command-line flags from a parsed `sumolib.options.ArgumentParser` namespace and runs the executable, while `sumolib.saveConfiguration(executable, configoptions, filename)` writes the active options to a SUMO configuration file.

# loading a network file

```python
# import the library
import sumolib
# parse the net
net = sumolib.net.readNet('myNet.net.xml')
```

`readNet` transparently supports plain `.net.xml` and gzipped `.net.xml.gz` files. The following named arguments may be given to `readNet` (for example `readNet('myNet.net.xml', withInternal=True)`):

- `withPrograms` (bool): import all traffic light programs (default `False`)
- `withLatestPrograms` (bool): import only the last program for each traffic light, which is active in sumo by default (default `False`)
- `withConnections` (bool): import all connections (default `True`)
- `withFoes` (bool): import right-of-way information (default `True`)
- `withInternal` (bool): import internal edges and lanes (default `False`)
- `withPedestrianConnections` (bool): import connections between sidewalks and crossings (default `False`; implies `withInternal=True`)
- `withMacroConnectors` (bool): import mesoscopic/macro connector edges (default `False`)
- `lxml` (bool): use `lxml.etree.iterparse` when installed for faster parsing, or set to `False` to force `xml.sax` (default `True`)
- `maxcache` (int): LRU cache size for optimal path routing queries, or `0` to disable caching (default `1000`)

# Network queries, routing, and geometry (`sumolib.net`)

A parsed [`Net`](https://sumo.dlr.de/pydoc/sumolib/net.html#Net) provides access to [`Node`](https://sumo.dlr.de/pydoc/sumolib/net/node.html#Node), [`Edge`](https://sumo.dlr.de/pydoc/sumolib/net/edge.html#Edge), [`Lane`](https://sumo.dlr.de/pydoc/sumolib/net/lane.html#Lane), [`Connection`](https://sumo.dlr.de/pydoc/sumolib/net/connection.html#Connection), [`Roundabout`](https://sumo.dlr.de/pydoc/sumolib/net/roundabout.html#Roundabout), and [`TLS`](https://sumo.dlr.de/pydoc/sumolib/net.html#TLS) objects:

- **Nodes (`net.getNodes()`, `net.getNode(id)`, `net.hasNode(id)`)**: query `getCoord()`, `getCoord3D()`, `getShape()`, `getType()`, `getIncoming()`, `getOutgoing()`, `getConnections()`, `getNeighboringNodes()`, `getTLSID()`, `forbids()`, and `getParams()`.
- **Edges (`net.getEdges(withInternal=True)`, `net.getEdge(id)`, `net.hasEdge(id)`)**: query `getFromNode()`, `getToNode()`, `getLanes()`, `getLane(idx)`, `getLaneNumber()`, `getLength()`, `getSpeed()`, `getPriority()`, `getType()`, `getRoutingType()`, `getShape(includeJunctions=False)`, `getBoundingBox()`, `getIncoming()`, `getOutgoing()`, `getAllowedIncoming(vClass)`, `getAllowedOutgoing(vClass)`, `getConnections(toEdge)`, `allows(vClass)`, `getPermissions()`, `is_fringe()`, `isSpecial()`, `getBidi()`, and `getParams()`.
- **Lanes (`net.getLane(laneID)`)**: query `getEdge()`, `getIndex()`, `getLength()`, `getSpeed()`, `getWidth()`, `getShape(includeJunctions=False)`, `getShape3D()`, `getClosestLanePosAndDist(point)`, `getOutgoing()`, `getOutgoingLanes()`, `getIncoming()`, `allows(vClass)`, `getPermissions()`, `getNeigh()`, and `isAccelerationLane()`.
- **Network bounds and lookups**: `net.getBBoxXY()`, `net.getBBoxDiameter()`, `net.getBoundary()`, `net.getRoundabouts()`, `net.getEdgesByOrigID(origID)`, and `net.getUpstreamEdges(edge, distance, stopOnTLS, stopOnTurnaround)`.

## import a network and retrieve nodes and edges

```python
# import the library
import sumolib
# parse the net
net = sumolib.net.readNet('myNet.net.xml')

# retrieve the coordinate of a node based on its ID
print(net.getNode('myNodeID').getCoord())

# retrieve the successor node ID of an edge
nextNodeID = net.getEdge('myEdgeID').getToNode().getID()
```

## compute the length of the selected edges

```python
net.loadSelection('selection.txt')
cumulLength = 0.
for edge in net.getEdges():
    if edge.isSelected():
        cumulLength += edge.getLength()
```

## locate nearby edges based on the geo-coordinate

This requires the module [pyproj](https://github.com/pyproj4/pyproj) to be installed.
For larger networks [rtree](https://pypi.org/project/Rtree/) is also strongly recommended (without `rtree`, `getNeighboringEdges` and `getNeighboringLanes` emit a warning and fall back to a linear scan).

```python
net = sumolib.net.readNet('myNet.net.xml')
radius = 0.1
x, y = net.convertLonLat2XY(lon, lat)
edges = net.getNeighboringEdges(x, y, radius)
# pick the closest edge
if len(edges) > 0:
    distancesAndEdges = sorted([(dist, edge) for edge, dist in edges], key=lambda x:x[0])
    dist, closestEdge = distancesAndEdges[0]
```

## locate the nearest lane

`getNearestLane` returns the nearest lane, the position along it, and the
distance to it in meters. Only lanes at a distance less than the search
radius are considered; it returns `None` if no lane matches.

```python
result = net.getNearestLane(x, y, r=100, vClass="passenger")
if result is not None:
    lane, lanePos, dist = result
```

Omit `vClass` to consider all vehicle classes. To pass longitude and latitude
instead of network coordinates, use `isGeo=True` (requires `pyproj` and a
network with a geo-projection). The radius remains in meters:

```python
result = net.getNearestLane(lon, lat, r=100, vClass="passenger", isGeo=True)
```

Matching uses the actual lane shapes without extending them to junction
centers. To include internal lanes, load the network with `withInternal=True`.

## coordinate transformations

```python
net = sumolib.net.readNet('myNet.net.xml')

# network coordinates (lower left network corner is at x=0, y=0)
x, y = net.convertLonLat2XY(lon, lat)
lon, lat = net.convertXY2LonLat(x, y)

# raw UTM coordinates
x, y = net.convertLonLat2XY(lon, lat, True)
lon, lat = net.convertXY2LonLat(x, y, True)

# lane/offset coordinates
# from lane position to network coordinates
x, y = sumolib.geomhelper.positionAtShapeOffset(net.getLane(laneID).getShape(), lanePos)
# from network coordinates to lane position
result = net.getNearestLane(x, y, radius)
if result is not None:
    lane, lanePos, dist = result
```

You can check whether a network defines a geo-projection with `net.hasGeoProj()` and retrieve its offset with `net.getLocationOffset()`.
See also [TraCI/Interfacing_TraCI_from_Python\#coordinate_transformations](../TraCI/Interfacing_TraCI_from_Python.md#coordinate_transformations) and [Geo-Coordinates](../Geo-Coordinates.md).

## shortest paths, fastest paths, and reachability

`Net` includes Dijkstra-based routing over network edges respecting connections, vehicle class permissions (`vClass`), internal edge lengths/costs, turnarounds (`reversalPenalty`), and optional `routingType` cost factors (`preferences`):

```python
fromEdge = net.getEdge('edgeA')
toEdge = net.getEdge('edgeB')

# shortest path by distance (meters)
path, length = net.getShortestPath(fromEdge, toEdge, vClass='passenger')

# fastest path by free-flow travel time (seconds)
path, travelTime = net.getFastestPath(fromEdge, toEdge, vClass='passenger')

# all edges reachable from fromEdge for a vehicle class
reachable = net.getReachable(fromEdge, vclass='passenger')
```

Both `getShortestPath` and `getFastestPath` delegate to `net.getOptimalPath(...)` and return `(None, cost)` when no valid path exists within `maxCost`. Set `withInternal=True` to include internal junction edges in the returned tuple.

## inspecting and modifying traffic lights

Load a network with `withPrograms=True` or `withLatestPrograms=True` to access [`TLS`](https://sumo.dlr.de/pydoc/sumolib/net.html#TLS), [`TLSProgram`](https://sumo.dlr.de/pydoc/sumolib/net.html#TLSProgram), and [`Phase`](https://sumo.dlr.de/pydoc/sumolib/net.html#Phase) objects:

```python
net = sumolib.net.readNet('myNet.net.xml', withPrograms=True)
for tls in net.getTrafficLights():
    print(tls.getID(), tls.getLinks())
    for progID, prog in tls.getPrograms().items():
        for phase in prog.getPhases():
            print(progID, phase.duration, phase.state, phase.name)
```

Each `TLS` object also exposes `getConnections()`, `getEdges()`, `addProgram()`, `removePrograms()`, and `toXML()` to serialize updated `<tlLogic>` definitions (see [Simulation/Traffic_Lights](../Simulation/Traffic_Lights.md#accessing_indices_programmatically)).

# Parsing, manipulating, and writing XML (`sumolib.xml`)

`sumolib.xml` provides two families of streaming XML readers, both supporting plain `.xml` and gzipped `.xml.gz` files via `openz`:

- `sumolib.xml.parse(xmlfile, element_names=None, ...)`: uses `ElementTree.iterparse` to yield mutable `CompoundObject` trees for every matching element. Use this when elements span multiple lines, have deep child hierarchies, need XML modification and `toXML()` output, or have arbitrary attribute order.
- `sumolib.xml.parse_fast`, `sumolib.xml.parse_fast_nested`, and `sumolib.xml.parse_fast_structured`: use regular expressions line by line for high throughput on large SUMO outputs such as FCD files. Each target element must appear on its own line and requested attributes must appear in the exact order listed in the call. Pass `optional=True` to fill missing attributes with `None`, or pass `None` for attribute names to infer them from the XSD referenced in `noNamespaceSchemaLocation`.

## compute the average edge speed in a *plain xml* edge file

```python
speedSum = 0.0
edgeCount = 0
for edge in sumolib.xml.parse('myNet.edg.xml', ['edge']):
    speedSum += float(edge.speed)
    edgeCount += 1
avgSpeed = speedSum / edgeCount
```

!!! note
    This is just a processing example. To compute average travel speeds in a network, process [edgeData](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md), [tripinfos](../Simulation/Output/TripInfo.md) or [summary-output](../Simulation/Output/Summary.md) instead.

## parse all edges in a route file

```python
for route in sumolib.xml.parse_fast("myRoutes.rou.xml", 'route', ['edges']):
    edge_ids = route.edges.split()
    # do something with the vector of edge ids
```

## parse nested elements in large XML files

For large two-level files (such as FCD output), `parse_fast_nested` yields `(parentRecord, childRecord)` pairs and `parse_fast_structured` yields parent objects with child lists:

```python
for timestep, vehicle in sumolib.xml.parse_fast_nested(
        "fcd.xml", "timestep", ["time"], "vehicle", ["id", "speed", "lane"]):
    print(timestep.time, vehicle.id, vehicle.speed, vehicle.lane)

for timestep in sumolib.xml.parse_fast_structured(
        "fcd.xml", "timestep", ["time"],
        {"vehicle": ["id", "speed"], "person": ["id", "edge"]}):
    for veh in timestep.vehicle:
        print(timestep.time, veh.id, veh.speed)
```

## parse vehicles and their route edges in a route file

```python
for vehicle in sumolib.xml.parse("myRoutes.rou.xml", "vehicle"):
    route = vehicle.route[0] # access the first (and only) child element with name 'route'
    edges = route.edges.split()
```

with automatic data conversions (including depart time as "HH:MM:SS"):

```python
from sumolib.miscutils import parseTime
for vehicle in sumolib.xml.parse("myRoutes.rou.xml", "vehicle", attr_conversions={
            'depart' : parseTime,
            'edges' : (lambda x : x.split())}):
    edges = vehicle.route[0].edges
    if vehicle.depart > 42:
       ...
```

## parse all edges in a edge data (meanData) file

```python
for interval in sumolib.xml.parse("edgedata.xml", "interval"):
    for edge in interval.edge:
        # do something with the edge attributes i.e. edge.entered
```

## Manipulating and writing xml

```python
with open("patched.nod.xml", 'w') as outf:
    # setting attrs is optional, it results in a cleaner patch file
    attrs = {'node': ['id', 'x', 'y']}  # other attrs are not needed for patching
    # parse always returns a generator but there is only one root element
    nodes = list(sumolib.xml.parse('plain.nod.xml', 'nodes', attrs))[0]
    for node in nodes.node:
        node.addChild("param", { "key": "origPos", "value" : "%s %s" % (node.x, node.y) } )
        node.x = float(node.x) + random.randint(-20, 20)
        node.y = float(node.y) + random.randint(-20, 20)
    outf.write(nodes.toXML())
```

Additional helpers in `sumolib.xml`:

- `sumolib.xml.create_document(root_element_name, attrs=None, schema=None)`: creates a new root `CompoundObject` with SUMO XSD schema attributes.
- `sumolib.xml.writeHeader(outf, script=None, root=None, schemaPath=None, rootAttrs="", options=None, includeXMLDeclaration=True)` and `sumolib.xml.buildHeader(...)`: writes standard SUMO XML declarations, generation metadata comments, and optional root opening tags.
- `sumolib.xml.sum(elements, attrname)` and `sumolib.xml.average(elements, attrname)`: aggregate numeric attributes over parsed elements.
- `sumolib.xml.contextualRename(xmlTree, prefixes, attribute='id')`: consistently renames IDs and all attribute references to those IDs within a parsed XML tree.
- `sumolib.xml.xsd.XsdStructure(xsdFile)`: parses SUMO XSD schemas to inspect element attributes, child elements, and enumeration values.

# Routes and map matching (`sumolib.route`)

`sumolib.route` computes route lengths and maps coordinate traces onto a network:

- `sumolib.route.getLength(net, edges)`: computes total route length from edge objects or edge IDs, including internal edges when the network was loaded with `withInternal=True`.
- `sumolib.route.addInternal(net, edges)`: expands a sequence of normal edges into edge objects including the shortest internal edges between consecutive edges.
- `sumolib.route.mapTrace(trace, net, delta, ...)`: matches an ordered sequence of 2D `(x, y)` coordinates onto connected network edges within search radius `delta`, supporting `vClass`, `fastest`, `fillGaps`, `reversalPenalty`, and `vias`.

```python
net = sumolib.net.readNet('myNet.net.xml', withInternal=True)
routeEdges = sumolib.route.mapTrace([(100.0, 50.0), (150.0, 50.0), (220.0, 50.0)], net, delta=20.0, vClass='passenger')
fullEdges = sumolib.route.addInternal(net, routeEdges)
totalLength = sumolib.route.getLength(net, routeEdges)
```

# Geometry helpers (`sumolib.geomhelper`)

`sumolib.geomhelper` provides 2D vector, polyline, polygon, and angle operations:

- **Distances and lengths**: `distance(p1, p2)`, `polyLength(polygon)`, `addToBoundingBox(coordList, bbox=None)`, `isWithin(pos, shape)`.
- **Shape offsets and projections**: `polygonOffsetAndDistanceToPoint(point, polygon, perpendicular=False)`, `polygonOffsetWithMinimumDistanceToPoint(point, polygon, perpendicular=False)`, `distancePointToPolygon(point, polygon, perpendicular=False)`, `positionAtShapeOffset(shape, offset)`, `rotationAtShapeOffset(shape, offset)`.
- **Polygons and intersections**: `move2side(shape, amount)`, `line2boundary(shape, width)`, `splitPolygonAtLengths2D(polygon, lengths)`, `intersectsPolygon(polygon1, polygon2)`, `intersectsAtLengths2D(polygon1, polygon2)`, `intersectsLineSegment(...)`, `isClosedPolygon(polygon)`.
- **Angles**: `angle2D(p1, p2)`, `angleTo2D(p1, p2)`, `naviDegree(rad)`, `fromNaviDegree(degrees)`, `minAngleDegreeDiff(d1, d2)`.

# Command-line options and configuration files (`sumolib.options`)

`sumolib.options.ArgumentParser` is a drop-in replacement for Python's `argparse.ArgumentParser` that integrates SUMO-style XML configuration files and option metadata:

- Automatically adds `-c` / `--configuration-file`, `-C` / `--save-configuration`, and `--save-template`.
- Supports `fix_path=True` (resolving relative paths against the configuration file location), `category="..."` for grouping options in templates and `netedit` dialogs, and type validators such as `ArgumentParser.time`, `ArgumentParser.net_file`, `ArgumentParser.route_file`, and `ArgumentParser.additional_file`.
- `pullOptions(executable, argParser, groups=None, configoptions=None)` imports CLI options dynamically from a SUMO binary's `--save-template -` output.
- `assign_prefixed_options(args, allowed_programs)` extracts options meant for subprograms (such as `--duarouter-weights.random-factor 2`).

```python
from sumolib.options import ArgumentParser

ap = ArgumentParser(description="Analyze SUMO route files")
ap.add_argument("-n", "--net-file", type=ap.net_file, required=True, category="input", help="Input network file")
ap.add_argument("-b", "--begin", type=ap.time, default=0, category="time", help="Start time")
options = ap.parse_args()
```

# File I/O, time parsing, and miscellaneous utilities (`sumolib.miscutils`, `sumolib.files`, `sumolib.color`)

## `sumolib.miscutils`

- `sumolib.miscutils.openz(fileOrURL, mode="r", ...)`: transparently opens plain text or binary files, `.gz` compressed files, `http://`/`https://` URLs, `"stdout"`, `"stderr"`, and socket ports (`trySocket=True`). Also re-exported as `sumolib.openz`.
- `parseTime(t, factor=1)` and `humanReadableTime(seconds)`: convert between seconds and SUMO `D:HH:MM:SS` strings (handling special tokens such as `"triggered"`).
- `parseBool(val)`: parses SUMO XML boolean strings (`"true"`, `"True"`, `"1"`, `"yes"`, `"on"`, `"x"`).
- `getFlowNumber(flow)`: computes the number of vehicles produced by a `<flow>` element parsed by `sumolib.xml.parse`.
- `getFreeSocketPort()` and `getSocketStream(port, mode='rb')`: allocate an unused TCP port or stream data from a socket (commonly used with TraCI or socket output).
- `euclidean(a, b)`, `intIfPossible(val, precision=None)`, `getBaseName(filename)`, `flattenPath(filename, sep='_')`.
- `working_dir(dir)`, `benchmark`, `benchmark_cumulative`, `Benchmarker`, `Colorgen`, `priorityDictionary`.

## `sumolib.files` and `sumolib.color`

- `sumolib.files.selection.read(file, lanes2edges=True)` and `write(fdo, entries)`: read and write `netedit` / `sumo-gui` selection files.
- `sumolib.files.additional.write(name, elements)`: write an `<additional>` XML file from objects implementing `toXML()`.
- `sumolib.color.RGBAColor(r, g, b, a=None)` and `sumolib.color.decodeXML(c)`: parse named SUMO colors (such as `"red"`, `"invisible"`) or `"r,g,b[,a]"` strings and format them back with `.toXML()`.

# Statistics and comparison (`sumolib.statistics`, `sumolib.fpdiff`)

## compute the median speed using the [Statistics](https://sumo.dlr.de/pydoc/sumolib/statistics.html#Statistics) module

```python
edgeStats = sumolib.statistics.Statistics("edge speeds")
for edge in sumolib.xml.parse('myNet.edg.xml', ['edge']):
    edgeStats.add(float(edge.speed))
avgSpeed = edgeStats.median()
```

!!! note
    Attribute *speed* is optional in user-generated *.edg.xml* files but will always be included if that file was written by [netconvert](../netconvert.md) or [netedit](../Netedit/index.md).

`Statistics` tracks minimum/maximum values with optional labels, arithmetic mean (`avg()`), median (`median()`), quartiles (`quartiles()`), rank (`rank()`), standard deviation (`meanAndStdDev()`, `relStdDev()`), histograms (`histogram=True`), and formatted output via `toString()` or `toXML()`. `sumolib.statistics` also provides `geh(m, c)` for the GEH hourly flow comparison metric.

`sumolib.fpdiff` provides `diff(fromlines, tolines, tolerance, relTolerance=None)` and `fpfilter(...)` for comparing text or XML output files while ignoring floating-point deviations within an absolute or relative tolerance.

# Shapes, sensors, vehicle types, outputs, and plotting

- **PoIs and Polygons (`sumolib.shapes`)**: `sumolib.shapes.poi.read(filename)` and `sumolib.shapes.polygon.read(filenames, includeTaz=False)` load `<poi>`, `<poly>`, and optionally `<taz>` elements into `PoI` and `Polygon` objects with `toXML()` serialization and bounding boxes.
- **Sensors (`sumolib.sensors`)**: `sumolib.sensors.inductive_loop.read(filename)` reads `<e1Detector>` definitions into `InductiveLoop` objects with `toXML()`.
- **Output readers and converters (`sumolib.output`)**: `sumolib.output.dump.readDump(file, attrsToCollect, edgesToCollect=None)` aggregates `edgeData`/`laneData` intervals; `sumolib.output.inductionloop.readInductLoop(file, toCollect)` reads induction loop interval outputs; `parse_sax__byID` and `parse_sax__asList` extract flat numeric tables via SAX. Submodule `sumolib.output.convert` converts parsed FCD trajectories into external formats (`phem`, `omnet`, `ns2`, `shawn`, `gpsdat`, `kml`, `gpx`, `poi`, `trj`, `fcdfilter`).
- **Vehicle type distributions (`sumolib.vehicletype`)**: `CreateVehTypeDistribution` and `CreateMultiVehTypeDistributions` sample vehicle attributes (`VehAttribute`) from fixed values or `normal`, `lognormal`, `normalCapped`, `uniform`, and `gamma` distributions and write `<vTypeDistribution>` XML files.
- **Matplotlib helpers (`sumolib.visualization.helpers`)**: imported explicitly (`import sumolib.visualization.helpers`) so `sumolib` itself stays usable without `matplotlib`. Provides `addPlotOptions`, `addInteractionOptions`, `addNetOptions`, `applyPlotOptions`, `plotNet`, `openFigure`, `closeFigure`, and colormap helpers used across SUMO visualization scripts.
- **Ordered multiset (`sumolib.datastructures`)**: `OrderedMultiSet` provides a multiset that preserves insertion order.

# Further Examples

The *runner.py* files in the test subfolders of [{{SUMO}}/tests/tools/sumolib]({{Source}}tests/tools/sumolib) provide additional
examples for sumolib use. See also the [Output Parsing Tutorial](../Tutorials/Output_Parsing.md) for a complete walkthrough using `sumolib.xml` and `sumolib.statistics`.
