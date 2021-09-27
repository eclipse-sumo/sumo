---
title: OSM
---

# osmWebWizard.py

This script opens a web browser and allows selecting a geographic region
on a map. It also provides some controls for specifying random traffic
demand for different traffic modes. When clicking the 'Generate'-button,
the simulation network for the selected area is built based on [OSM data](../../Networks/Import/OpenStreetMap.md), random demand is
generated and [sumo-gui](../../sumo-gui.md) is started.

!!! note
    A [usage tutorial is available](../../Tutorials/OSMWebWizard.md).

All files that make up the scenario are created in a subfolder of the
working directory with the current timestamp (i.e.
{{SUMO}}/tools/2021-02-22-10-00-00/). If you edit the network, you can use the
script *build.bat* to rebuild the random demand.

Call:

```
python tools/osmWebWizard.py
```

The script will keep running so you can build multiple scenarios in your
web-browser. Stop the script when you're done to free up the port again.

!!! caution
    The script requires the environment variable *SUMO_HOME* to be set [as explained here](../../Basics/Basic_Computer_Skills.md#additional_environment_variables).

# osmGet.py

This script allows downloading a large area from OpenStreetMap. The area to import 
can be defined with one of the following options: 

- **--bbox** defines the box to retrieve in geo coordinates (west,south,east,north),
- **--area** allows to define the id of the area to retrieve and
- **--polygon** calculates the bounding box from polygon data in the specified file. 

Example:

```
python tools/osmGet.py --bbox <bounding-box> --tiles <INT>
```

The additional option **--tiles** allows to split larger areas into multiple tiles and download requests.

# osmBuild.py

The script creates a sumo network from the given osm file (i.e. from the output of osmGet.py).
Example:

```
python tools/osmBuild.py --osm-file <osm-file>
```

More additional options can be defined to manage the import. Example:

```
python tools/osmBuild.py --osm-file <osm-file> [--vehicle-classes (all|road|publicTransport|passenger)] [--type-file <TYPEMAP_FILE>] [--netconvert-options <OPT1,OPT2,OPT3>] [--polyconvert-options <OPT1,OPT2,OPT3>]
```

The option **--vehicle-classes** allows to filter the roads to import by vehicles classes. 
If "road" is given as parameter, only roads usable by road vehicles are
extracted, if "passenger" is given, only those accessible by passenger
vehicles are imported. When selecting "publicTransport" as parameter, only roads usable by road vehicles and
public transport are extracted. As default, "all" roads are imported.

When using the option **--type-file** an additional output file with polygons of rivers
and buildings as well as Points of Interest (POIs) will be generated.
This can be loaded in [sumo-gui](../../sumo-gui.md) for additional
visualization. Useful type files can be found at {{SUMO}}/data/typemap/.

Additional options for [netconvert](../../netconvert.md) and
[polyconvert](../../polyconvert.md) can be supplied using the options **--netconvert-options**
and **-polyconvert-options**.

# osmTaxiStop.py

This script import taxi stands from OSM data. Using the option **--type** you can choose which type of element to add in the SUMO network. For example:

```
python tools/import/osm/osmTaxiStop.py --osm-file <osm-file> -n <net-file> --type parkingArea
```

Will add the taxi stands as parkingAreas.
