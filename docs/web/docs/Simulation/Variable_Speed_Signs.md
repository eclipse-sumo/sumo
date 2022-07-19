---
title: Variable Speed Signs
---

One of the trigger objects that may be specified within an {{AdditionalFile}} allows the
simulation of variable speed signs. The syntax for such an object is given in the following

# Definition in a single file

```xml
<additional>>
    <variableSpeedSign id="vss0" lanes="middle_0">
        <step time="0" speed="2.8"/>
        <step time="100" speed="47.22"/>
        <step time="200" speed="-1"/>
    </variableSpeedSign>
</additional>
```


The following attributes are used within the `VariableSpeedSign` element:

| Attribute Name | Value Type  | Description                                                  |
| -------------- | ----------- | ------------------------------------------------------------ |
| **id**         | id (string) | The id of the Variable Speed Signal element (user choice)    |
| **lanes**      | ids (string list) | The ids of the lanes of the simulation network that shall be affected                |
| file           | string      | optional file in which the time and speed values are defined |

Each `step`-element is a combination of the time the next new speed shall be set
and the speed to set itself and uses the following attributes:

| Attribute Name | Value Type     | Description                                                                                                 |
| -------------- | -------------- | ----------------------------------------------------------------------------------------------------------- |
| **time**       | time (int)     | Time in which the speed will be changed                                                                     |
| speed          | speed (double) | New speed (if no value or a negative value is given, the speed will be reset to the original network value) |

# Definition with an external file

Instead of defining the speed changes of a `<variableSpeedSign>` in the same file, it may also be read from a second file as follows:

```xml
<additional>
  <variableSpeedSign id="VSS_ID" lanes="LANE_IDS" file="DEF_FILE">
     <step time="<TIME>" speed="<SPEED>"/>

     ... further entries ...

  </variableSpeedSign>
</additional>
```

A file name can be supplied, called <DEF_FILE\> within the schema above. This file must contain the
information about when a certain speed shall be set onto the lane. This
file has the following format:

```xml
<vss>
  <step time="<TIME>" speed="<SPEED>"/>
  <step time="<TIME>" speed="<SPEED>"/>

  ... further entries ...

</vss>
```

Examples for `variableSpeedSign` definitions can be obtained [from our test suites](https://sumo.dlr.de/extractTest.php?path=sumo/extended/variable_speed_signs)
