---
title: Car-Following Model
---

This short document describes how a new car-following model can be added
to SUMO. We'll do this by implementing a test model named "smartSK".
When implementing your own model, you should also grep (look for) occurrences of the name we use, here.

## The car-following model's class

The best thing is to start with an already existing model. Models are
located in {{SUMO}}/src/microsim/cfmodels and new added models should reside
here, too. Copy both MSCFModel_KraussOrig1.h and
MSCFModel_KraussOrig1.cpp and rename them. The name should be
"MSCFModel_<YOUR_MODELS_NAME\>", in our case "MSCFModel_SmartSK"

!!! convention
    Car-following model implementations are located in {{SUMO}}/src/microsim/cfmodels.

!!! convention
    A car-following class' name should start with "MSCFModel_".

Now, open both files and rename all occurrences of MSCFModel_KraussOrig1
into your class' name.

Add the files to the [{{SUMO}}/src/microsim/cfmodels/CMakeLists.txt]({{Source}}src/microsim/cfmodels/CMakeLists.txt).

## Loading into simulation

We now add the XML-elements which allow us to define and parse the
model's parameter. Extend the list of known elements
"SUMOXMLDefinitions::tags" located in [{{SUMO}}/src/utils/xml/SUMOXMLDefinitions.cpp]({{Source}}src/utils/xml/SUMOXMLDefinitions.cpp).
In SUMOXMLDefinitions.h:

```
SUMO_TAG_CF_SMART_SK,
```

In SUMOXMLDefinitions.cpp:

```
{ "carFollowing-SmartSK",     SUMO_TAG_CF_SMART_SK },
```

Furthermore you need to add an entry to
SUMOXMLDefinitions::carFollowModelValues\[\] also in
SUMOXMLDefinitions.cpp:

```
{ "SmartSK",     SUMO_TAG_CF_SMART_SK },
```

Car-following models are instantiated in MSVehicleType::build(...)
located in [{{SUMO}}/src/microsim/MSVehicleType.cpp]({{Source}}src/microsim/MSVehicleType.cpp). You'll find a switch, here
where you have to put the call to your model's constructor into.

```
case SUMO_TAG_CF_SMART_SK:
  model = new MSCFModel_SmartSK(vtype,
                                from.getCFParam(SUMO_ATTR_ACCEL, SUMOVTypeParameter::getDefaultAccel(from.vehicleClass)),
                                from.getCFParam(SUMO_ATTR_DECEL, SUMOVTypeParameter::getDefaultDecel(from.vehicleClass)),
                                from.getCFParam(SUMO_ATTR_SIGMA, SUMOVTypeParameter::getDefaultImperfection(from.vehicleClass)),
                                from.getCFParam(SUMO_ATTR_TAU, 1.),
                                from.getCFParam(SUMO_ATTR_TMP1, 1.),
                                from.getCFParam(SUMO_ATTR_TMP1, 1.),
                                from.getCFParam(SUMO_ATTR_TMP1, 1.),
                                from.getCFParam(SUMO_ATTR_TMP1, 1.),
                                from.getCFParam(SUMO_ATTR_TMP1, 1.));
```

You may note that the constructor is read with values from "from". The
first parameter, starting with SUMO_ATTR_ denotes the XML-attribute
which names the parameter. These attributes, if new, have to be added to {{SUMO}}. In SUMOXMLDefinitions.h:

```
SUMO_ATTR_TMP1,
SUMO_ATTR_TMP2,
SUMO_ATTR_TMP3,
SUMO_ATTR_TMP4,
SUMO_ATTR_TMP5,
```

In SUMOXMLDefinitions.cpp:

```
{ "tmp1",           SUMO_ATTR_TMP1 },
{ "tmp2",           SUMO_ATTR_TMP2 },
{ "tmp3",           SUMO_ATTR_TMP3 },
{ "tmp4",           SUMO_ATTR_TMP4 },
{ "tmp5",           SUMO_ATTR_TMP5 },
```

The second parameter denotes the default value. There are two different
mechanisms to define default values. Either they are vehicle class
independent (they are the same for passenger cars and for trucks for
instance) then you can simply state the number here or you want a
different default for the different vehicle classes then insert the
relevant code into [{{SUMO}}/src/utils/vehicle/SUMOVTypeParameter.cpp]({{Source}}src/utils/vehicle/SUMOVTypeParameter.cpp). You can only
have floating point values as model parameters at the moment.

You also have to define which parameter must be read in [{{SUMO}}/src/utils/vehicle/SUMOVehicleParserHelper.cpp]({{Source}}src/utils/vehicle/SUMOVehicleParserHelper.cpp), method getAllowedCFModelAttrs():

```
std::set<SumoXMLAttr> smartSKParams;
smartSKParams.insert(SUMO_ATTR_ACCEL);
smartSKParams.insert(SUMO_ATTR_DECEL);
smartSKParams.insert(SUMO_ATTR_SIGMA);
smartSKParams.insert(SUMO_ATTR_TAU);
smartSKParams.insert(SUMO_ATTR_TMP1);
smartSKParams.insert(SUMO_ATTR_TMP2);
smartSKParams.insert(SUMO_ATTR_TMP3);
smartSKParams.insert(SUMO_ATTR_TMP4);
smartSKParams.insert(SUMO_ATTR_TMP5);
allowedCFModelAttrs[SUMO_TAG_CF_SMART_SK] = smartSKParams;
```

Note, that we have to adapt the constructor to retrieve the additional
parameter (tmp1-tmp5). We have also to adapt the copy constructor
located in our class in MSCFModel_SmartSK::duplicate(...).

For further interaction, you also have to adapt the "id" of the model in
the model's .h class:

```
virtual int getModelID() const {
 return SUMO_TAG_CF_SMART_SK;
}
```

## Using Custom Parameters via TraCI

A carFollowModel can override the functions getParameter and setParameter inherited from MSCFModel. 
Any calls to 'traci.vehicle.setParameter' and 'traci.vehicle.getParameter' where the key starts with "carFollowModel." will then be forwarded to these methods (without the prefix).
The call
`traci.vehicle.setParameter(vehID, "carFollowModel.XYZ", "42")`
will be mapped onto the call
`MSCFModel::setParameter(MSVehicle* veh, "XYZ" , "42")` which is called on the current carFollowModel of the vehicle.


## XML Validation

Sumo performs [xml validation](../../XMLValidation.md). If you add new
XML elements or attributes you either need to adapt the XML schema files
in {{SUMO}}/data/xsd or add the option

```
--xml-validation never
```
