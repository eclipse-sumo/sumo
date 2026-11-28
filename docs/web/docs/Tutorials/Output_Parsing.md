---
title: Output Parsing
---

In this tutorial you will learn how to

- set up an abstract network using edge types,
- create repeatedly identical vehicles using flows,
- reroute vehicles dynamically such that they drive endlessly,
- analyze disaggregated output files using the [sumolib python library](../Tools/Sumolib.md) and built-in statistic tools,
- visualize spatial and diffed scenario data in [sumo-gui](../sumo-gui.md) and [netedit](../Netedit/index.md),

and as a bonus how to

- use socket output for online evaluation and saving disk space.

Despite the keywords online, socket, and python API, this tutorial does
*not* cover any TraCI related content. All complete example files can be found in `tests/complex/tutorial/output_parsing/data`.

## Network setup

The goal is to build a simple network where the vehicles drive in
circles so we set up four nodes at the corners as follows
(`circular.nod.xml`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<nodes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/nodes_file.xsd">
    <node id="bottom-left" x="0" y="0"/>
    <node id="bottom-right" x="1250" y="0"/>
    <node id="top-right" x="1250" y="1250"/>
    <node id="top-left" x="0" y="1250"/>
</nodes>
```

All edges connecting the nodes should have the same number of lanes and
the same maximum speed. In order to save on typing we define the edge
type in a separate file (`circular.typ.xml`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<types xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/types_file.xsd">
    <type id="edgeType" numLanes="2" speed="36.1"/>
</types>
```

Finally we define the edges connecting the nodes (`circular.edg.xml`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<edges xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/edges_file.xsd">
    <edge from="bottom-left" id="bottom" to="bottom-right" type="edgeType"/>
    <edge from="bottom-right" id="right" to="top-right" type="edgeType"/>
    <edge from="top-right" id="top" to="top-left" type="edgeType"/>
    <edge from="top-left" id="left" to="bottom-left" type="edgeType"/>
</edges>
```

The [netconvert](../netconvert.md) call is very straightforward:

```
netconvert -n circular.nod.xml -t circular.typ.xml -e circular.edg.xml -o circular.net.xml
```

In order to simplify the resulting net (and get the highest speed out of
the simulation), we omit turnarounds and simplify the movement over
junctions by removing the junction internal lanes. The complete
netconvert configuration file is as follows (`circular.netccfg`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<netconvertConfiguration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/netconvertConfiguration.xsd">

    <input>
        <node-files value="circular.nod.xml"/>
        <edge-files value="circular.edg.xml"/>
        <type-files value="circular.typ.xml"/>
    </input>

    <output>
        <output-file value="circular.net.xml"/>
    </output>

    <junctions>
        <no-internal-links value="true"/>
        <no-turnarounds value="true"/>
    </junctions>

</netconvertConfiguration>
```

Call netconvert again using this configuration file:

```
netconvert -c circular.netccfg
```

Try

```
sumo-gui -n circular.net.xml
```

for a look at the final network.

## Route, flow, and rerouter setup

Because [additional files are loaded before route files](../sumo.md#loading_order_of_input_files), any `<route>` referenced by a `<rerouter>` in an additional file must already exist when that additional file is parsed. A clean layout (used in `tests/complex/tutorial/output_parsing/data`) is to place the shared circular `<route>` definitions together with the `<rerouter>` definitions in `circular.add.xml`, and keep vehicle types and `<flow>` definitions in `circular.rou.xml` (without duplicating `<route>` IDs across both files).

Create `circular.add.xml` with the counter-clockwise [routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#routes) and [rerouters](../Simulation/Rerouter.md#assigning_a_new_route) on edges `bottom` and `top` so vehicles continue driving endlessly:

```xml
<?xml version="1.0" encoding="UTF-8"?>

<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">
    <route id="routeRight" edges="bottom right top left"/>
    <route id="routeLeft" edges="top left bottom right"/>
    <route id="routeTop" edges="right top left bottom"/>
    <route id="routeBottom" edges="left bottom right top"/>
    <rerouter id="rerouterBottom" edges="bottom">
        <interval begin="0" end="100000">
            <routeProbReroute id="routeRight"/>
        </interval>
    </rerouter>
    <rerouter id="rerouterTop" edges="top">
        <interval begin="0" end="100000">
            <routeProbReroute id="routeLeft"/>
        </interval>
    </rerouter>
</additional>
```

Next, create `circular.rou.xml` containing the [vehicle types](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types) (`car` and `truck`) and the [traffic flows](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#repeated_vehicles_flows) referencing those routes:

```xml
<?xml version="1.0" encoding="UTF-8"?>

<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/routes_file.xsd">
    <vType accel="1.5" decel="4.5" id="car" length="5" maxSpeed="36.1"/>
    <vType accel="0.4" decel="4.5" id="truck" length="12" maxSpeed="22.2"/>
    <flow begin="0" departPos="free" id="carRight" period="1" number="70" route="routeRight" type="car"/>
    <flow begin="0" departPos="free" id="carTop" period="1" number="70" route="routeTop" type="car"/>
    <flow begin="0" departPos="free" id="carLeft" period="1" number="70" route="routeLeft" type="car"/>
    <flow begin="0" departPos="free" id="carBottom" period="1" number="70" route="routeBottom" type="car"/>
    <flow begin="0" departPos="free" id="truckRight" period="1" number="30" route="routeRight" type="truck"/>
    <flow begin="0" departPos="free" id="truckTop" period="1" number="30" route="routeTop" type="truck"/>
    <flow begin="0" departPos="free" id="truckLeft" period="1" number="30" route="routeLeft" type="truck"/>
    <flow begin="0" departPos="free" id="truckBottom" period="1" number="30" route="routeBottom" type="truck"/>
</routes>
```

## Putting it all together

With all input files completed, create the SUMO configuration file `output_file.sumocfg`:

```xml
<?xml version="1.0" encoding="UTF-8"?>

<sumoConfiguration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/sumoConfiguration.xsd">
    <input>
        <net-file value="circular.net.xml"/>
        <route-files value="circular.rou.xml"/>
        <additional-files value="circular.add.xml"/>
    </input>

    <output>
        <fcd-output value="dump.xml"/>
    </output>

    <time>
        <begin value="0"/>
        <end value="1000"/>
    </time>
</sumoConfiguration>
```

Run the simulation to write the [FCD output](../Simulation/Output/FCDOutput.md) file `dump.xml`:

```
sumo -c output_file.sumocfg
```

## Analyzing the output

### Streaming XML parsing with `sumolib`

`sumolib.xml.parse` streams arbitrary SUMO XML output files element by element without loading the entire document into memory. Save the following script as `parse_output.py` to compute the mean vehicle speed per simulation step from `dump.xml`:

```python
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib

total_speed = 0.0
total_samples = 0
for step in sumolib.xml.parse("dump.xml", "timestep"):
    vehicles = step.vehicle or []
    if vehicles:
        mean_speed = sum(float(v.speed) for v in vehicles) / len(vehicles)
        total_speed += sum(float(v.speed) for v in vehicles)
        total_samples += len(vehicles)
        if float(step.time) % 250 == 0:
            print(f"t={step.time}: {len(vehicles)} vehicles, mean speed={mean_speed:.2f} m/s")

if total_samples:
    print(f"Overall mean speed: {total_speed / total_samples:.2f} m/s ({total_samples} samples)")
```

For very large flat outputs where nested child elements are not needed, `sumolib.xml.parse_fast("dump.xml", "vehicle", ["id", "speed", "x", "y"])` extracts selected attributes with minimal overhead.

### Computing statistics from disaggregated outputs

SUMO provides ready-made command-line tools to aggregate and plot disaggregated outputs such as [FCD output](../Simulation/Output/FCDOutput.md) (`dump.xml`), [tripinfo output](../Simulation/Output/TripInfo.md) (`--tripinfo-output`), or stop outputs without writing custom code:

- **[attributeStats.py](../Tools/Output.md#attributestatspy)** computes count, minimum, maximum, mean, standard deviation, quartiles, and optional histograms for any XML attribute:

```
python "%SUMO_HOME%/tools/output/attributeStats.py" -e vehicle -a speed --binwidth 2 dump.xml
```

- **[plotXMLAttributes.py](../Tools/Visualization.md#plotxmlattributespy)** plots scatter/line charts or histograms directly from one or more XML output files:

```
python "%SUMO_HOME%/tools/visualization/plotXMLAttributes.py" dump.xml -x time -y speed -i id
```

## Visualizing spatial and diffed scenario data in sumo-gui and netedit

To inspect spatial metrics per edge or lane, add an `<edgeData>` definition to `circular.add.xml` (see [Edge- or Lane-based Traffic Measures](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md)):

```xml
<edgeData id="ed1" freq="250" file="edgedata_base.xml"/>
```

After running a second scenario variant (for example, with modified truck share or speed limit writing `edgedata_alt.xml`), compute per-edge attribute differences between the two runs using **[edgeDataDiff.py](../Tools/Output.md#edgedatadiffpy)**:

```
python "%SUMO_HOME%/tools/output/edgeDataDiff.py" edgedata_base.xml edgedata_alt.xml edgedata_diff.xml
```

Both raw spatial data (`edgedata_base.xml`) and diffed data (`edgedata_diff.xml`) can be visualized directly on the network:

- **In [sumo-gui](../sumo-gui.md#visualizing_edge-related_data)**: Start `sumo-gui -n circular.net.xml --edgedata-files edgedata_diff.xml` (or load the file via *File* -> *Load EdgeData*), open *View Settings* (<kbd>F9</kbd>) -> *Streets*, and set **Color**: *by edgeData* (choosing attributes such as `speed`, `density`, or `entered`) and optionally **Scale width**: *by edgeData*. Advancing the simulation time steps through the `<interval>` periods of the data file.
- **In [netedit](../Netedit/editModesData.md)**: Load the network and open the edgeData file via *File* -> *Data Elements* -> *Load Data Elements...* (<kbd>Ctrl</kbd> + <kbd>B</kbd>), switch to **Data** supermode (<kbd>F4</kbd>) and **Inspect** mode (<kbd>I</kbd>), and select the desired interval and attribute in the *EdgeData* toolbar/sidebar to color and inspect per-edge values or differences.

## Bonus: Socket communication

Any SUMO output option accepts `<HOST>:<PORT>` instead of a file path, streaming the XML output directly over a TCP socket without writing large files to disk. Create `output_socket.sumocfg`:

```xml
<?xml version="1.0" encoding="UTF-8"?>

<sumoConfiguration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/sumoConfiguration.xsd">
    <input>
        <net-file value="circular.net.xml"/>
        <route-files value="circular.rou.xml"/>
        <additional-files value="circular.add.xml"/>
    </input>

    <output>
        <fcd-output value="localhost:54321"/>
    </output>

    <time>
        <begin value="0"/>
        <end value="1000"/>
    </time>
</sumoConfiguration>
```

Save the following script as `socket_receiver.py` — it starts listening on port `54321`, launches `sumo -c output_socket.sumocfg`, and streams the incoming socket directly into `sumolib.xml.parse`:

```python
import os
import socket
import subprocess
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib

PORT = 54321
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("localhost", PORT))
    server.listen(1)

    sumo_proc = subprocess.Popen([sumolib.checkBinary("sumo"), "-c", "output_socket.sumocfg", "--no-step-log"])
    conn, _ = server.accept()
    with conn, conn.makefile("rb") as stream:
        for step in sumolib.xml.parse(stream, "timestep"):
            vehicles = step.vehicle or []
            if vehicles and float(step.time) % 250 == 0:
                mean_speed = sum(float(v.speed) for v in vehicles) / len(vehicles)
                print(f"t={step.time}: {len(vehicles)} vehicles, mean speed={mean_speed:.2f} m/s")
    sumo_proc.wait()
```
