---
title: VehiclePermissions
---

# Introduction

SUMO allows modelling access restrictions via a predefined set of
so-called vehicle classes. Each vehicle has a vehicle class and each
simulation lane allows a set of vehicle classes. Vehicles may only drive
on lanes that allows its vehicle class.

This is useful for describing multi-modal traffic scenarios by
distinguishing between passenger cars, bicycles, tram and pedestrians.

# Vehicle definition

The vehicle class of a vehicle is defined by first defining a [vehicle
type](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types)
with the appropriate [vehicle
class](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#abstract_vehicle_class)
and then [assigning that type to the
vehicle](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes).
An example is given below:

```xml
<routes>
  <vType id="myType" vClass="bus"/>

  <vehicle id="veh0" type="myType" depart="0">
    <route edges="a b c"/>
  </vehicle>
</routes>
```

If no such definition is given, the vehicle class defaults to
*passenger* (normal passenger cars). By setting the  attribute, a set of
[default type parameters](../Vehicle_Type_Parameter_Defaults.md) is
automatically assigned to better correspond to a typical vehicle of that
class.

# Network definition

In a *.net.xml* file, each lane defines a set of permitted vehicle
classes. This definition is either

- taken from [custom input
  files](../Networks/PlainXML.md#edge_descriptions).
  It possible to define permissions for edges or individual lanes. To
  simplify definition, either the permitted classes or the prohibited
  classes can be specified using the attributes `allow, disallow`.
- set using the option **--default.disallow** {{DT_STR}}**,...**
- imported from an input source such as
  [OpenStreetMap](../Networks/Import/OpenStreetMap.md) according
  to a [customizable
  heuristic](../Networks/Import/OpenStreetMap.md#recommended_typemaps).
- set via [netedit](../Netedit/editModesCommon.md#inspect). Convenience
  features exist [for adding bicycle lanes, bus lanes and
  sidewalks](../Netedit/neteditPopupFunctions.md#restricted_lanes).
- changed during the simulation using [TraCI](../TraCI/Change_Lane_State.md).

For visualizing access permissions, either
[sumo-gui](../sumo-gui.md#road_access_permissions) or
[netedit](../Netedit/neteditUsageExamples.md#correcting_road_access_permissions) may be
used.

# Special cases

The vehicle class *ignoring* may drive on any edge.

The vehicle class *pedestrian* should not be assigned to a vehicle.
Instead, pedestrians should be modeled as [walking
persons](../Simulation/Pedestrians.md). During network building, no
direct connections between pedestrian-only lanes are built. Instead
options for [building pedestrian
intersections](../Simulation/Pedestrians.md#building_a_network_for_pedestrian_simulation)
should be used.

# Custom access restrictions

The default vehicle classes (including the values `custom1` and `custom2`) can handle most requirements for modelling network access restrictions.
However, in some domains such as shipping (draught, beam) and railroads (axle weight, grade), it may be necessary to provide for fine grained numerical access models.

For this, [duarouter](../duarouter.md) provides the option **--restriction-params** to set list of edge and vehicle type [generic parameters](GenericParameters.md) names.

- Only those vehicles where the restriction param has a lower value than the corresponding edge param are allowed to move on an edge.
- If a `<vType>` does not define a restriction param its value is initialized to *0*.
- If an `<edge>` does not define a restriction param its value is initialized to *1e40*.

Restrictions on ship draught could be modelled in the following manner:

`duarouter --restriction-params draught -n net.net.xml -r routes.rou.xml`

where *net.net.xml* has an edge defined as

```xml
<edge id="E" from="beg" to="end" priority="-1">
        <lane id="E_0" index="0" allow="ship" speed="13.90" length="100"/>
        <param key="draught" value="10"/>
</edge>
```

and *routes.rou.xml* defined with the following types:

```xml
    <vType id="0" vClass="ship">
        <param key="draught" value="5"/>
    </vType>
    <vType id="1" vClass="ship">
        <param key="draught" value="15"/>
    </vType>
```

In this example, ships of type "0" would be able to use edge "E" whereas ships of type "1" would be prohibited.
