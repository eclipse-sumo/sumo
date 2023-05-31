---
title: Lane-Changing Model
---

This short document describes how a new lane-changing model can be added
to SUMO.

!!! caution
    A Lane-Changing Model is at least an order of magnitude more complicated
    than a car-following model (judging by the amount of code alone).

It is recommended to learn about the architecture of the model from the
Publication [SUMO's Lane-Changing
Model](https://elib.dlr.de/102254/1/Springer-SUMOs_Lane_changing_model.pdf).

## The lane-changing model's class

The best thing is to start with an already existing model. Models are
located in {{SUMO}}/src/microsim/lcmodels and new added models should reside
here, too. Copy both MSLCM_LC2013.h and
MSLCM_LC2013.cpp and rename them. The name should be
"MSLCModel_<YOUR_MODELS_NAME\>".

!!! convention
    lane-changing model implementations are located in {{SUMO}}/src/microsim/lcmodels.

!!! convention
    A lane-changing class' name should start with "MSLCModel_".

Now, open both files and rename all occurrences of MSLCM_LC2013
into your class' name.

Add the files to the [{{SUMO}}/src/microsim/lcmodels/CMakeLists.txt]({{Source}}src/microsim/lcmodels/CMakeLists.txt).

## Loading into simulation

### Updating the XSD schema files so they permit the new model name

All permited models must be defined in [{{SUMO}}/data/xsd/routeTypes.xsd]({{Source}}data/xsd/routeTypes.xsd) in the list of permitted values for the `laneChangeModel` attribute.

### Registering the model name as possible value

We now add the XML-elements which allow us to define and parse the
model's parameter. Extend the list of known elements
"SUMOXMLDefinitions::tags" located in [{{SUMO}}/src/utils/xml/SUMOXMLDefinitions.cpp]({{Source}}src/utils/xml/SUMOXMLDefinitions.cpp).
In `SUMOXMLDefinitions::laneChangeModelValues` add new values i.e. `{ "LCXYZ", LaneChangeModel::LCXYZ }` (but not as the last element in the array)
and add the corresponding element to `enum class LaneChangeModel`.

### Extending the list of loadable models
Lane-changing models are instantiated in `MSAbstractLaneChangeModel::build`
located in
[{{SUMO}}/src/microsim/lcmodels/MSAbstractLaneChangeModel.cpp]({{Source}}src/microsim/lcmodels/MSAbstractLaneChangeModel.cpp). You'll find a switch, here
where you have to put the call to your model's constructor into.

### Adding custom model parameters

You may note that all the parameters for the model are loaded within the
constructor rather than being passed as arguments.

However, you need to register any novel parameters in
`SUMOVehicleParserHelper::parseLCParams`
located in
[{{SUMO}}/src/utils/vehicle/SUMOVehicleParserHelper.cpp]({{Source}}src/utils/vehicle/SUMOVehicleParserHelper.cpp).


For further interaction, you also have to adapt the "id" of the model in
the new model's .h class:

```
virtual int getModelID() const {
 return LaneChangeModel::LCXYZ;
}
```

## Using Custom Parameters via TraCI

A LaneChangeModel can override the functions getParameter and setParameter
inherited from MSAbstractLaneChangeModel. 
Any calls to 'traci.vehicle.setParameter' and 'traci.vehicle.getParameter' where
the key starts with "laneChangeModel." will then be forwarded to these methods (without the prefix).
The call
`traci.vehicle.setParameter(vehID, "laneChangeModel.XYZ", "42")`
will be mapped onto the call
`MSAbstractLaneChangeModel::setParameter("XYZ" , "42")` which is called on the current laneChangeModel of the vehicle.


## XML Validation

Sumo performs [xml validation](../../XMLValidation.md). If you add new
XML elements or attributes you either need to adapt the XML schema files
in {{SUMO}}/data/xsd or add the option

```
--xml-validation never
```
