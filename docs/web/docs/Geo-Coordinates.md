---
title: Geo-Coordinates
---

# Geo-Referenced Networks

SUMO networks are always encoded in Cartesian coordinates (meters) and
may contain geo-referencing information to allow conversion to lon,lat.
By default the Cartesian coordinates use the UTM-projection with the
origin shifted to so that the lower left corner of the network is at
0,0.

!!! note
    the projection information is encoded in the `<location>`-element at the top of the *.net.xml* file.

- When importing a network from
  [OSM](Networks/Import/OpenStreetMap.md), geo-referencing is
  automatically included in the generated *.net.xml* file
- When importing a network from
  [plain-xml](Networks/PlainXML.md)
  files, coordinates may be given in lon,lat and importing using a
  projection option such as **--proj.utm**
- When importing a network from Shapefile, the availability of
  geo-referencing depends on the format of the source data.
  
## osmWebWizard with aerial/satellite imagery

The de-facto standard for projecting background image layers on the internet is the [Web Mercator Projection](https://en.wikipedia.org/wiki/Web_Mercator_projection). When activating the *Satellite background* setting in [osmWebWizard.py](Tutorials/OSMWebWizard.md), the default projection is changed from UTM to Web-Mercator (proj string `+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs`) to avoid a projection mismatch between the network and the downloaded image layer. Note, that the coordinate origin (0,0) will still be shifted to the lower left network corner.

# Checking Geo-Coordinates

In [sumo-gui](sumo-gui.md) and [netedit](Netedit/index.md) when right-clicking anywhere in a
geo-referenced network, the option *copy cursor geo-position to
clipboard* is available. The resulting *lat,lon* coordinates are
suitable for pasting into any map engine such as \[maps.google.com\] or
\[maps.bing.com\]. Also, the network coordinates as well as the
geo-coordinates at the cursor position are shown in the bottom
right-corner of the window.

# Performing coordinate-transformations

- using
  [TraCI](TraCI/Simulation_Value_Retrieval.md#command_0x82_position_conversion),
  coordinates can be transformed between network-coordinates (m,m) and
  geo-coordinates (lon,lat) and vice versa
- using [sumolib](Tools/Sumolib.md#coordinate_transformations)
  , coordinates can be transformed between network-coordinates (m,m)
  and geo-coordinates (lon,lat) and vice versa. Converting back and forth between lon,lat and raw UTM (m,m) is also supported.

# Using geo-coordinates in XML-inputs

The [duarouter](duarouter.md) application supports attributes [fromLonLat, toLonLat, viaLonLat](Demand/Shortest_or_Optimal_Path_Routing.md#trip_definitions) to map trips directly onto the closest network [edge or junction](Demand/Shortest_or_Optimal_Path_Routing.md#mapmatching) for the given coordinates.

# Obtaining output with geo-coordinates

- A network can be exported as *plain-xml* in geo-coordinates using
  the netconvert command

```
netconvert --sumo-net-file myNet.net.xml --plain-output-prefix plain --proj.plain-geo
```

- [FCD-output](Simulation/Output/FCDOutput.md) can be obtained
  in geo-coordinates by adding the option **--fcd-output.geo**
  
- [duarouter](duarouter.md) supports option **--write-trips.geo** to create trip definitions with geo-coordinates (see above)
  
# Mapping geo-coordinates

It is often desirable to convert between geo-coordinates (lon,lat) and road coordinates (laneID, offset). 
This is typically accomplished in a 2-step process of first converting lon,lat to x,y-network coordinates (in m) and the matching those coordinates to the closest lane. The following resources can be useful for this task:

- [how to match trajectories](FAQ.md#how_do_i_generate_sumo_routes_from_gps_traces)
- [traci.moveToXY](TraCI/Change_Vehicle_State.md#move_to_xy_0xb4) moves a vehicle to an appropriate network position
- [convert geo-coordinates to edge with sumolib](Tools/Sumolib.md#locate_nearby_edges_based_on_the_geo-coordinate)
- [TraCI](TraCI/Simulation_Value_Retrieval.md#command_0x82_position_conversion) converts between coordinates (x,y or lon,lat) and edges

