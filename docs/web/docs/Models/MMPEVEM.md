---
title: MMPEVEM
---

# Overview

Since version 1.11.0, SUMO comes with an energy consumption model for electric
vehicles (EVs) developed by the Teaching and Research Area Mechatronics in
Mobile Propulsion (MMP) of RWTH Aachen University. MMPEVEM (**MMP**'s
**E**lectric **V**ehicle **E**mission **M**odel), as it is called, is
implemented as a SUMO emission model (hence the name) and considers the
components of the powertrain individually in order to accurately compute the
power consumption in every simulation step. This includes the transmission
(i.e. a single reduction gear and a differential) with constant efficiency, the
electric motor (as well as its inverter) whose power losses are
read/interpolated from a characteristic map, a constant power consumer to
represent auxiliary devices such as air conditioning, and the battery.



# Using the Model

To utilise MMPEVEM, set `emissionClass="MMPEVEM"` in the `vType` definitions of
the vehicles you are interested in and add the following parameter:
```
<param key="has.battery.device" value="true"/>
```
Equipping the `vType` with a battery device is necessary because it triggers the
parsing and loading of all energy-related quantities. Other than that, there is
currently no interaction between MMPEVEM and said device. This especially
entails that the battery's state and output are *not* affected by or based on
the former.


Although default parameters are available, it is strongly advised to explicitly
specify *all* the model parameters listed below and to make sure that they are
compatible with one another. Failing to do so may lead to wrong or even invalid
operating points (cf. [Output](#output)) which skew the overall energy
consumption.


## Model Parameters

MMPEVEM shares a lot of SUMO's
[native electric vehicle parameters](Electric.md#defining-electric-vehicles) and
introduces only a couple of new ones. You can find exemplary vehicle type
definitions at `{SUMO_HOME}/data/emissions/MMPEVEM`[^1].

| Key                       | Value Type   | Default                               | Description                                                           |
|---------------------------|--------------|---------------------------------------|-----------------------------------------------------------------------|
| vehicleMass               |  float (> 0) | 1000.0 [kg]                           | Mass of the vehicle                                                   |
| wheelRadius               |  float (> 0) | 0.3588 [m]                            | Wheel radius                                                          |
| internalMomentOfInertia   | float (>= 0) | 0.01 [kg * m^2]                       | Internal components' moment of inertia                                |
| rollDragCoefficient       | float (>= 0) | 0.01 [1]                              | Rolling resistance coefficient                                        |
| airDragCoefficient        | float (>= 0) | 0.6 [1]                               | Air drag coefficient                                                  |
| frontSurfaceArea          | float (>= 0) | 5.0 [m^2]                             | Cross-sectional area of the front of the vehicle                      |
| gearRatio                 |  float (> 0) | 10.0 [1]                              | Combined ratio of the single reduction gear and the differential      |
| gearEfficiency            |  float (> 0) | 0.96 [1]                              | Combined efficiency of the single reduction gear and the differential |
| maximumTorque             | float (>= 0) | 310.0 [Nm]                            | Maximum generative torque of the electric motor                       |
| maximumPower              | float (>= 0) | 107000.0 [W]                          | Maximum generative power of the electric motor                        |
| maximumRecuperationTorque | float (>= 0) | 95.5 [Nm]                             | Maximum recuperation torque of the electric motor                     |
| maximumRecuperationPower  | float (>= 0) | 42800.0 [W]                           | Maximum recuperation power of the electric motor                      |
| internalBatteryResistance |  float (> 0) | 0.1142 [Ohm]                          | Internal battery resistance                                           |
| nominalBatteryVoltage     | float (>= 0) | 396.0 [V]                             | Nominal battery voltage                                               |
| constantPowerIntake       |        float | 100.0 [W]                             | Constant power consumption of auxiliary devices                       |
| powerLossMap              |       string | `2,1\|-1e9,1e9;-1e9,1e9\|0,0,0,0`[^2] | String representation of the electric motor's power loss map          |


[^1]: For the sake of compatibility, the XMLs in
  `{SUMO_HOME}/data/emissions/MMPEVEM` contain all
  [native electric vehicle parameters](Electric.md#defining-electric-vehicles).
[^2]: All operating points result in a power loss of 0 W in the default map.


## Power Loss Map

The power loss map describes the electric motor's power loss (in W) as a
function of motor speed (in rpm) and torque (in Nm). It is passed to the model
using a custom string format which shall be explicated using the mock power loss
map below.

|             |     **0 rpm** |  **4000 rpm** |  **8000 rpm** |
|-------------|---------------|---------------|---------------|
| **-100 Nm** |       826.7 W |      4295.6 W |               |
|    **0 Nm** |         0.0 W |      2162.9 W |      6137.4 W |
|  **100 Nm** |       826.7 W |      4295.6 W |      9317.5 W |
|  **200 Nm** |      3297.4 W |      8361.2 W |     14988.6 W |

Its string representation is:
```
2,1|0,4000,8000;-100,0,100,200|826.7,4295.6,nan,0.0,2162.9,6137.4,826.7,4295.6,9317.5,3297.4,8361.2,14988.6
```
The string consists of three parts delimited by the `|` character:
- The first part signifies that it maps from ℝ² to ℝ¹ and does not change
  for MMPEVEM's power loss maps.
- The middle section contains the axis values in ascending order. Axes are
  separated by semi-colons while commas are used to delimit the values of each
  axis.
- The final part contains the row-major flattened entries of the map (i.e. the
  first row of the table followed by the second row etc.) separated by commas.
  Undefined operating points (e.g. (8000 rpm, -100 Nm) in this example) are
  passed as `nan`.


### Tools

For permanent magnet synchronous machines (PMSMs), power loss maps can be
generated with the
[Electric Machine Design Tool](https://github.com/TUMFTM/Electric_Machine_Design)
developed by Kalt et al.[^3] and converted into the expected format using
`{SUMO_HOME}/tools/contributed/convertMapMat2XML.M`. The tool and the conversion
script require MATLAB® 2017 or newer.

[^3]: [S. Kalt, J. Erhard, B. Danquah and M. Lienkamp, "Electric Machine Design
  Tool for Permanent Magnet Synchronous Machines," 2019 Fourteenth International
  Conference on Ecological Vehicles and Renewable Energies (EVER), 2019,
  pp. 1-7, doi:
  10.1109/EVER.2019.8813601](https://doi.org/10.1109/EVER.2019.8813601)



## Example

This is an example `vType` definition of a generic electric SUV and was taken
from `{SUMO_HOME}/data/emissions/MMPEVEM/SUV.xml`.
```
<vType id="SUV" vClass="passenger" emissionClass="MMPEVEM" actionStepLength="1.0">
  <param key="has.battery.device" value="true"/>
  <param key="vehicleMass" value="2100"/>
  <param key="wheelRadius" value="0.3835"/>
  <param key="internalMomentOfInertia" value="16"/>
  <param key="rollDragCoefficient" value="0.01"/>
  <param key="airDragCoefficient" value="0.29"/>
  <param key="frontSurfaceArea" value="3.23"/>
  <param key="gearRatio" value="9.325"/>
  <param key="gearEfficiency" value="0.96"/>
  <param key="maximumTorque" value="380"/>
  <param key="maximumPower" value="180000"/>
  <param key="maximumRecuperationTorque" value="180"/>
  <param key="maximumRecuperationPower" value="105000"/>
  <param key="internalBatteryResistance" value="0.1575"/>
  <param key="nominalBatteryVoltage" value="405"/>
  <param key="constantPowerIntake" value="360"/>
  <param key="powerLossMap" value="2,1|0,413.7931,827.5862,1241.3793,1655.1724,2068.9655,2482.7586,2896.5517,3310.3448,3724.1379,4137.931,4551.7241,4965.5172,5379.3103,5793.1034,6206.8966,6620.6897,7034.4828,7448.2759,7862.069,8275.8621,8689.6552,9103.4483,9517.2414,9931.0345,10344.8276,10758.6207,11172.4138,11586.2069,12000;-192.8157,-176.7477,-160.6797,-144.6118,-128.5438,-112.4758,-96.4078,-80.3399,-64.2719,-48.2039,-32.1359,-16.068,0,16.068,32.1359,48.2039,64.2719,80.3399,96.4078,112.4758,128.5438,144.6118,160.6797,176.7477,192.8157,208.8836,224.9516,241.0196,257.0876,273.1555,289.2235,305.2915,321.3594,337.4274,353.4954,369.5634,385.6313|3059.92,3292.2,3684.4,4125.3,4607.96,5128.38,5683.86,6272.42,6892.5,7542.89,8222.55,8930.61,9666.35,10429.1,10310.8,11089.67,10785.82,10390.27,10810.04,10481.93,10979.63,11522.42,11203.92,11766.01,12353.56,12963.17,12593.66,13195.25,13812.23,14443.49,2571.18,2794.54,3160.93,3574.57,4028.8,4519.78,5044.9,5602.28,6190.42,6808.15,7454.48,8128.58,8829.74,9557.33,10310.8,11089.67,10785.82,10390.27,10810.04,10481.93,10979.63,11522.42,11203.92,11766.01,12353.56,12963.17,12593.66,13195.25,13812.23,14443.49,2124.95,2338.35,2679.36,3066.29,3492.74,3955.01,4450.61,4977.7,5534.87,6120.96,6735.04,7376.31,8044.08,8737.75,9456.79,10200.74,10785.82,10390.27,10810.04,10481.93,10979.63,11522.42,11203.92,11766.01,12353.56,12963.17,12593.66,13195.25,13812.23,14443.49,1721.21,1923.61,2239.66,2600.44,2999.77,3434.09,3900.98,4398.68,4925.83,5481.31,6064.23,6673.8,7309.37,7970.36,8656.27,9366.63,10064.34,10390.27,10810.04,10481.93,10979.63,11522.42,11203.92,11766.01,12353.56,12963.17,12593.66,13195.25,13812.23,14443.49,1359.97,1550.33,1841.86,2177.04,2549.9,2957,3396.01,3865.22,4363.31,4889.2,5442.04,6021.05,6625.62,7255.17,7909.23,8587.34,9289.11,9670.59,10040.46,10481.93,10979.63,11522.42,11203.92,11766.01,12353.56,12963.17,12593.66,13195.25,13812.23,14443.49,1041.22,1218.5,1485.94,1796.07,2143.13,2523.76,2935.71,3377.32,3847.31,4344.64,4868.47,5418.07,5992.82,6592.18,7215.67,7862.87,8533.39,9003.21,9328.15,9724.18,10175.82,10671.82,11203.92,11766.01,12353.56,12963.17,12593.66,13195.25,13812.23,14443.49,764.98,928.14,1171.9,1457.55,1779.45,2134.36,2520.07,2934.97,3377.82,3847.62,4343.53,4864.85,5410.97,5981.38,6575.6,7193.22,7833.87,8384.9,8669.14,9024.21,9434.59,9888.97,10379.02,10898.58,11443.04,12008.93,12593.66,13195.25,13812.23,14443.49,531.24,679.23,899.76,1161.47,1458.87,1788.79,2149.09,2538.18,2954.86,3398.14,3867.21,4361.39,4880.08,5422.77,5989,6578.39,7190.55,7813.13,8060.28,8378.21,8751.34,9168.33,9620.81,10102.57,10608.98,11136.54,11682.6,12245.16,12822.71,13414.09,339.99,471.78,669.49,907.82,1181.39,1487.07,1822.77,2186.96,2578.42,2996.21,3439.52,3907.69,4400.14,4916.35,5455.9,6018.38,6603.43,7210.74,7499.12,7783.23,8122.55,8505.71,8924.32,9372.16,9844.56,10337.99,10849.81,11377.99,11920.98,12477.63,191.25,305.79,481.12,696.62,947,1229.19,1541.13,1881.29,2248.5,2641.82,3060.46,3503.76,3971.15,4462.13,4976.27,5513.19,6072.52,6653.95,6983.77,7237,7545.53,7897.97,8285.9,8703.07,9144.81,9607.58,10088.72,10586.18,11098.42,11624.26,85,181.26,334.63,527.86,755.71,1015.15,1304.14,1621.18,1965.1,2334.97,2730.02,3149.58,3593.11,4060.1,4550.14,5062.82,5597.81,6154.78,6512.75,6737.81,7018.29,7342.77,7702.83,8092.19,8506.18,8941.24,9394.7,9864.51,10349.11,10847.33,21.25,98.19,230.02,401.53,607.51,844.95,1111.82,1406.62,1728.22,2075.66,2448.2,2845.17,3266.03,3710.27,4177.48,4667.27,5179.3,5713.25,6085.03,6284.42,6539.38,6838.46,7173.21,7537.36,7926.2,8336.18,8764.61,9209.44,9669.11,10142.42,0,58.32,172.48,327.47,517.95,740.81,993.98,1275.91,1585.42,1921.55,2283.51,2670.64,3082.37,3518.18,3977.64,4460.35,4965.97,5494.17,5876.18,6057.77,6296.78,6581.41,6902.92,7254.84,7632.29,8031.6,8450,8885.36,9336.06,9800.88,21.25,98.19,230.02,401.53,607.51,844.95,1111.82,1406.62,1728.22,2075.66,2448.2,2845.17,3266.03,3710.27,4177.48,4667.27,5179.3,5713.25,6085.03,6284.42,6539.38,6838.46,7173.21,7537.36,7926.2,8336.18,8764.61,9209.44,9669.11,10142.42,85,181.26,334.63,527.86,755.71,1015.15,1304.14,1621.18,1965.1,2334.97,2730.02,3149.58,3593.11,4060.1,4550.14,5062.82,5597.81,6154.78,6512.75,6737.81,7018.29,7342.77,7702.83,8092.19,8506.18,8941.24,9394.7,9864.51,10349.11,10847.33,191.25,305.79,481.12,696.62,947,1229.19,1541.13,1881.29,2248.5,2641.82,3060.46,3503.76,3971.15,4462.13,4976.27,5513.19,6072.52,6653.95,6983.77,7237,7545.53,7897.97,8285.9,8703.07,9144.81,9607.58,10088.72,10586.18,11098.42,11624.26,339.99,471.78,669.49,907.82,1181.39,1487.07,1822.77,2186.96,2578.42,2996.21,3439.52,3907.69,4400.14,4916.35,5455.9,6018.38,6603.43,7210.74,7499.12,7783.23,8122.55,8505.71,8924.32,9372.16,9844.56,10337.99,10849.81,11377.99,11920.98,12477.63,531.24,679.23,899.76,1161.47,1458.87,1788.79,2149.09,2538.18,2954.86,3398.14,3867.21,4361.39,4880.08,5422.77,5989,6578.39,7190.55,7813.13,8060.28,8378.21,8751.34,9168.33,9620.81,10102.57,10608.98,11136.54,11682.6,12245.16,12822.71,13414.09,764.98,928.14,1171.9,1457.55,1779.45,2134.36,2520.07,2934.97,3377.82,3847.62,4343.53,4864.85,5410.97,5981.38,6575.6,7193.22,7833.87,8384.9,8669.14,9024.21,9434.59,9888.97,10379.02,10898.58,11443.04,12008.93,12593.66,13195.25,13812.23,14443.49,1041.22,1218.5,1485.94,1796.07,2143.13,2523.76,2935.71,3377.32,3847.31,4344.64,4868.47,5418.07,5992.82,6592.18,7215.67,7862.87,8533.39,9003.21,9328.15,9724.18,10175.82,10671.82,11203.92,11766.01,12353.56,12963.17,13592.32,14239.12,14902.19,15580.51,1359.97,1550.33,1841.86,2177.04,2549.9,2957,3396.01,3865.22,4363.31,4889.2,5442.04,6021.05,6625.62,7255.17,7909.23,8587.34,9289.11,9670.59,10040.46,10481.93,10979.63,11522.42,12102.14,12712.82,13350.05,14010.59,14692.09,15392.85,16111.74,16848.03,1721.21,1923.61,2239.66,2600.44,2999.77,3434.09,3900.98,4398.68,4925.83,5481.31,6064.23,6673.8,7309.37,7970.36,8656.27,9366.63,10064.34,10390.27,10810.04,11302.37,11852.06,12448.17,13082.76,13750.09,14446.05,15167.75,15913.26,16681.4,17471.69,18284.23,2124.95,2338.35,2679.36,3066.29,3492.74,3955.01,4450.61,4977.7,5534.87,6120.96,6735.04,7376.31,8044.08,8737.75,9456.79,10200.74,10785.82,11166.28,11641.97,12191.89,12801.15,13459.15,14158.41,14893.73,15661.64,16460.13,17288.37,18146.71,19036.72,18284.23,2571.18,2794.54,3160.93,3574.57,4028.8,4519.78,5044.9,5602.28,6190.42,6808.15,7454.48,8128.58,8829.74,9557.33,10310.8,11089.67,11562.13,12003.73,12542.85,13158.98,13837.82,14569.51,15347.47,16167.76,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,3059.92,3292.2,3684.4,4125.3,4607.96,5128.38,5683.86,6272.42,6892.5,7542.89,8222.55,8930.61,9666.35,10429.1,11218.29,12017.36,12398.26,12909.18,13521.37,14215.18,14977.47,15799.94,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,3591.16,3831.31,4249.75,4718.46,5230.21,5780.83,6367.49,6988.11,7641.1,8325.17,9039.24,9782.41,10553.91,11353.06,12179.27,12844.59,13300.53,13891.26,14589.29,15376.72,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,4164.89,4411.88,4856.98,5354.06,5895.56,6477.12,7095.77,7749.36,8436.22,9154.99,9904.55,10683.97,11492.43,12329.22,13193.73,13735.1,14277.15,14961.52,15763.13,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,4781.13,5033.91,5506.1,6032.11,6604.01,7217.24,7868.72,8556.18,9277.86,10032.35,10818.49,11635.29,12481.9,13357.58,14244.44,14696.41,15339.05,16136.09,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,5439.86,5697.4,6197.11,6752.59,7355.56,8001.21,8686.33,9408.55,10166.02,10957.26,11781.06,12636.37,13522.32,14438.12,15180.17,15738.39,16501.23,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,6141.09,6402.34,6930,7515.52,8150.2,8829.02,9548.61,10306.48,11100.69,11929.71,12792.25,13687.22,14613.7,15570.87,16190.45,16874.33,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,6884.82,7148.74,7704.78,8320.88,8987.93,9700.67,10455.55,11249.96,12081.89,12949.7,13852.06,14787.83,15756.02,16747.9,17286.49,18122.91,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,7671.05,7936.6,8521.45,9168.69,9868.76,10616.16,11407.15,12239.01,13109.61,14017.24,14960.5,15938.2,16949.3,17791.47,18483.6,18122.91,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,8499.78,8765.92,9380,10058.93,10792.69,11575.49,12403.42,13273.61,14183.84,15132.32,16117.57,17138.33,18193.54,18923.23,18483.6,18122.91,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,9371.01,9636.7,10280.44,10991.61,11759.72,12578.66,13444.35,14353.78,15304.6,16294.94,17323.25,18388.23,19488.72,18923.23,18483.6,18122.91,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,10284.74,10548.93,11222.76,11966.74,12769.84,13625.68,14529.95,15479.5,16471.87,17505.11,18577.57,19687.88,20674.02,18923.23,18483.6,18122.91,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,11240.96,11502.63,12206.97,12984.3,13823.05,14716.53,15660.21,16650.78,17685.67,18762.81,19880.51,21037.3,20674.02,18923.23,18483.6,18122.91,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23,12239.69,12497.78,13233.06,14044.31,14919.37,15851.22,16835.13,17867.61,18945.98,20068.07,21232.07,22436.48,20674.02,18923.23,18483.6,18122.91,17785.24,17438.49,17067.26,16667.61,16242.8,17182.8,16678.16,17611.3,17028.57,17930.23,17288.37,18146.71,19036.72,18284.23"/>
</vType>
```



# Output

Start SUMO with the `--emission-output` option in order to generate an XML file
with the emissions of all vehicles. In the XML, the attribute `electricity`
shows the electric energy consumption (in Wh) of a vehicle in a given time step.
Please note that this value is only provided by MMPEVEM if and only if the
`vtype` definition of the vehicle in question was set accordingly.


MMPEVEM is a backward-facing model, meaning that it expects a velocity,
acceleration, slope, and time step size to calculate the energy demand. This
also means that it has no way to stop the vehicle from running into invalid
operating points. The latter can be caused by implausible accelerations (for
instance due to teleportations), wrong vehicle parameters, etc. If the model
finds itself in such a state, it returns `nan`. On a related note, MMPEVEM
assumes that the driver perfectly operates the mechanical brakes such that
torque and power are clipped to the motor's limits during recuperation.



# Publications
- [L. Koch et al., "Accurate physics-based modeling of electric vehicle energy
consumption in the SUMO traffic microsimulator," 2021 IEEE International
Intelligent Transportation Systems Conference (ITSC), 2021, pp. 1650-1657, doi:
10.1109/ITSC48978.2021.9564463](https://doi.org/10.1109/ITSC48978.2021.9564463)

