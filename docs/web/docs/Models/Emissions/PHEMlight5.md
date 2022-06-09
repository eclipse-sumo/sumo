---
title: PHEMlight
---

# PHEMlight V5

This page only describes what changed for the new PHEMlight V5 model.
For details about PHEM and the older PHEMlight see the [PHEMlight page](PHEMlight.md).

PHEMlight5 uses data files that include the parameters of the modelled
emission classes. Data sets for two emission classes
are included in SUMO: PC_EU4_D_MW and PC_EU4_G for the new model),
resembling passenger vehicles, Euro Norm
4, powered by Diesel and gasoline respectively. Please note the change in names
compared to the original PHEMlight.

!!! caution
    Only two emission classes are included in the public version. Any further vehicle classes must be licensed from the TU Graz.

To use PHEMlight5, you can set an additional search path to these input
files using the configuration option **--phemlight-path** {{DT_FILE}} where {{DT_FILE}} is the path. Alternatively,
you may set the environment variable **PHEMLIGHT_PATH**. By default
SUMO is looking into <SUMO_HOME\>/data/emissions/PHEMlight5 and in the PHEMlight subdir
of your working directory.

In addition, your vehicles must be assigned to one of PHEMlight5's
vehicle emission classes, summarised in the following table. To use the
emission classes below prepend the class name with the PHEMlight5 prefix,
e.g. `emissionClass="PHEMlight5/PC_EU4_G"`.

## Explanations for the 5.0 naming conventions

The name is composed of the vehicle category and size, the technology
and the emission standard, all connected with underscores.

### Vehicle Category and Size Classes

- **PC**  passenger cars

- **LCV**  light commercial vehicles; differentiation into size classes
  according to legislation (N1-I, N1-II, N1-III); size class I RM <
  1305kg; size class II 1305kg < RM <= 1760kg; size class III 1760kg
  < RM <= 3500kg; "RM": reference mass (approx. Vehicle curb
  weight)
  
- **RT**  heavy duty vehicles - rigid trucks; size class I 2 axle
  trucks; size class II 3+ axle trucks
  
- **TT**  heavy duty vehicles - truck + trailer (incl. articulated
  trucks = tractor + semitrailer)
  
- **CB**  heavy duty vehicles - city bus
  
- **CO**  heavy duty vehicles - coach
  
- **MC_2S**  motorcycles - 2 stroke
  
- **MC_4S**  motorcycles - 4 stroke
  
### Technology

- **G**  gasoline engine (no HEV suffix: conventional powertrain)
  
- **D**  diesel engine (no HEV suffix: conventional powertrain)
  
- **HEV**  hybrid powertrain (additional suffix)
  
- **BEV_GEN1**  battery electric vehicles

### Examples

- **PC_EU6_G**  passenger car, gasoline engine, EURO 6
  
- **PC_BEV_GEN1**  passenger car, battery electric
  
- **LCV_EU6ab_G_III_HEV**  light commercial vehicle, size class III, hybrid gasoline powertrain, Euro 6ab
  
- **RT_EUIV_D_I**  rigid truck, size class I, diesel engine, EURO IV

- **CB_EUV_D**  city bus, diesel engine, Euro V

The following tables show the available classes. Except for the two Euro 4
passenger cars the classes need to be licensed from TU Graz.

## PHEMlight5 Emission Classes for Light Vehicles running on Diesel or Gasoline

<table style="border-collapse:collapse;border-spacing:0" class="tg"><tr><th>Passenger Cars</th><th colspan="3">Light Duty Vehicles</th></tr>
  <tr><td>PC_EU0_D</td><td>LCV_EU0_D_I</td><td>LCV_EU0_D_II</td><td>LCV_EU0_D_III</td></tr>
  <tr><td>PC_EU1_D</td><td>LCV_EU1_D_I</td><td>LCV_EU1_D_II</td><td>LCV_EU1_D_III</td></tr>
  <tr><td>PC_EU2_D</td><td>LCV_EU2_D_I</td><td>LCV_EU2_D_II</td><td>LCV_EU2_D_III</td></tr>
  <tr><td>PC_EU3_D</td><td>LCV_EU3_D_I</td><td>LCV_EU3_D_II</td><td>LCV_EU3_D_III</td></tr>
  <tr><td>PC_EU4_D</td><td>LCV_EU4_D_I</td><td>LCV_EU4_D_II</td><td>LCV_EU4_D_III</td></tr>
  <tr><td>PC_EU5_D</td><td>LCV_EU5_D_I</td><td>LCV_EU5_D_II</td><td>LCV_EU5_D_III</td></tr>
  <tr><td>PC_EU6ab_D</td><td>LCV_EU6ab_D_I</td><td>LCV_EU6ab_D_II</td><td>LCV_EU6ab_D_III</td></tr>
  <tr><td>PC_EU6c_D</td><td>LCV_EU6c_D_I</td><td>LCV_EU6c_D_II</td><td>LCV_EU6c_D_III</td></tr>
  <tr><td>PC_EU6d_D</td><td>LCV_EU6d_D_I</td><td>LCV_EU6d_D_II</td><td>LCV_EU6d_D_III</td></tr>
  <tr><td>PC_EU6d-Temp_D</td><td>LCV_EU6d-Temp_D_I</td><td>LCV_EU6d-Temp_D_II</td><td>LCV_EU6d-Temp_D_III</td></tr>
</table>

## PHEMlight5 Emission Classes for Light Vehicles running on alternative fuels

<table style="border-collapse:collapse;border-spacing:0" class="tg"><tr><th class="tg-uzvj" colspan="2">Passenger Cars</th><th class="tg-uzvj" colspan="3">Light Duty Vehicles<br></th></tr>
  <tr><td></td><td>PC_BEV</td><td>LCV_I_BEV</td><td>LCV_II_BEV</td><td>LCV_III_BEV</td></tr>
  <tr><td></td><td>PC_CNG_EU5</td><td>LCV_I_CNG_EU5</td><td>LCV_II_CNG_EU5</td><td>LCV_III_CNG_EU5</td></tr>
  <tr><td></td><td>PC_CNG_EU6</td><td>LCV_I_CNG_EU6</td><td>LCV_II_CNG_EU6</td><td>LCV_III_CNG_EU6</td></tr>
  <tr><td></td><td>PC_CNG_EU6c</td><td>LCV_I_CNG_EU6c</td><td>LCV_II_CNG_EU6c</td><td>LCV_III_CNG_EU6c</td></tr>
  <tr><td>H_PKW_D_EU5</td><td></td><td></td><td></td><td></td></tr>
  <tr><td>H_PKW_D_EU6</td><td>PC_D_HEV_EU6</td><td>LCV_I_D_HEV_EU6</td><td>LCV_II_D_HEV_EU6</td><td>LCV_III_D_HEV_EU6</td></tr>
  <tr><td></td><td>PC_D_HEV_EU6c</td><td>LCV_I_D_HEV_EU6c</td><td>LCV_II_D_HEV_EU6c</td><td>LCV_III_D_HEV_EU6c</td></tr>
  <tr><td>H_PKW_G_EU5</td><td></td><td></td><td></td><td></td></tr>
  <tr><td>H_PKW_G_EU6</td><td>PC_G_HEV_EU6</td><td>LCV_I_G_HEV_EU6</td><td>LCV_II_G_HEV_EU6</td><td>LCV_III_G_HEV_EU6</td></tr>
  <tr><td></td><td>PC_G_HEV_EU6c</td><td>LCV_I_G_HEV_EU6c</td><td>LCV_II_G_HEV_EU6c</td><td>LCV_III_G_HEV_EU6c</td></tr>
</table>

## PHEMlight5 Emission Classes for Motor Cycles

<table>
  <tr><th colspan="3">Motor Cycles</th></tr>
  <tr><td>MC_2S_EU0_G</td><td>MC_4S_EU0_G_g250cc</td><td>MC_4S_EU0_G_s250cc</td></tr>
  <tr><td>MC_2S_EU1_G</td><td>MC_4S_EU1_G_g250cc</td><td>MC_4S_EU1_G_s250cc</td></tr>
  <tr><td>MC_2S_EU2_G</td><td>MC_4S_EU2_G_g250cc</td><td>MC_4S_EU2_G_s250cc</td></tr>
  <tr><td>MC_2S_EU3_G</td><td>MC_4S_EU3_G_g250cc</td><td>MC_4S_EU3_G_s250cc</td></tr>
  <tr><td>MC_2S_EU4_G</td><td>MC_4S_EU4_G_g250cc</td><td>MC_4S_EU4_G_s250cc</td></tr>
  <tr><td>MC_2S_EU5_G</td><td>MC_4S_EU5_G_g250cc</td><td>MC_4S_EU5_G_s250cc</td></tr>
  <tr><td>MC_2S_EU6_G</td><td>MC_4S_EU6_G_g250cc</td><td>MC_4S_EU6_G_s250cc</td></tr>
  <tr><td colspan="3">MC_BEV_GEN1</td></tr>
</table>

## PHEMlight5 Emission Classes for Heavy Vehicles

<table style="border-collapse:collapse;border-spacing:0" class="tg"><tr><th class="tg-uzvj" colspan="2">Tractor/Trailor</th><th class="tg-uzvj" colspan="2">Coach</th><th class="tg-uzvj" colspan="2">Urban and Inter Urban Buses</th><th class="tg-uzvj" colspan="4">Trucks</th></tr>
  <tr><td></td><td></td><td></td><td></td><td></td><td>HDV_CB_BEV</td><td></td><td></td><td></td><td></td></tr>
  <tr><td></td><td></td><td></td><td></td><td></td><td>HDV_CB_CNG_EU6</td><td></td><td></td><td></td><td></td></tr>
  <tr><td>LSZ_D_EU0</td><td>LSZ_D_EU0</td><td>RB_D_EU0</td><td>HDV_CO_D_EU0</td><td>LB_D_EU0</td><td>HDV_CB_D_EU0</td><td>SOLO_LKW_D_EU0_I</td><td>HDV_RT_I_D_EU0</td><td>SOLO_LKW_D_EU0_II</td><td>HDV_RT_II_D_EU0</td></tr>
  <tr><td>LSZ_D_EU1</td><td>HDV_TT_D_EU1</td><td>RB_D_EU1</td><td>HDV_CO_D_EU1</td><td>LB_D_EU1</td><td>HDV_CB_D_EU1</td><td>SOLO_LKW_D_EU1_I</td><td>HDV_RT_I_D_EU1</td><td>SOLO_LKW_D_EU1_II</td><td>HDV_RT_II_D_EU1</td></tr>
  <tr><td>LSZ_D_EU2</td><td>HDV_TT_D_EU2</td><td>RB_D_EU2</td><td>HDV_CO_D_EU2</td><td>LB_D_EU2</td><td>HDV_CB_D_EU2</td><td>SOLO_LKW_D_EU2_I</td><td>HDV_RT_I_D_EU2</td><td>SOLO_LKW_D_EU2_II</td><td>HDV_RT_II_D_EU2</td></tr>
  <tr><td>LSZ_D_EU3</td><td>HDV_TT_D_EU3</td><td>RB_D_EU3</td><td>HDV_CO_D_EU3</td><td>LB_D_EU3</td><td>HDV_CB_D_EU3</td><td>SOLO_LKW_D_EU3_I</td><td>HDV_RT_I_D_EU3</td><td>SOLO_LKW_D_EU3_II</td><td>HDV_RT_II_D_EU3</td></tr>
  <tr><td>LSZ_D_EU4</td><td>HDV_TT_D_EU4</td><td>RB_D_EU4</td><td>HDV_CO_D_EU4</td><td>LB_D_EU4</td><td>HDV_CB_D_EU4</td><td>SOLO_LKW_D_EU4_I</td><td>HDV_RT_I_D_EU4</td><td>SOLO_LKW_D_EU4_II</td><td>HDV_RT_II_D_EU4</td></tr>
  <tr><td>LSZ_D_EU5</td><td>HDV_TT_D_EU5</td><td>RB_D_EU5</td><td>HDV_CO_D_EU5</td><td>LB_D_EU5</td><td>HDV_CB_D_EU5</td><td>SOLO_LKW_D_EU5_I</td><td>HDV_RT_I_D_EU5</td><td>SOLO_LKW_D_EU5_II</td><td>HDV_RT_II_D_EU5</td></tr>
  <tr><td>LSZ_D_EU6</td><td>HDV_TT_D_EU6</td><td>RB_D_EU6</td><td>HDV_CO_D_EU6</td><td>LB_D_EU6</td><td>HDV_CB_D_EU6</td><td>SOLO_LKW_D_EU6_I</td><td>HDV_RT_I_D_EU6</td><td>SOLO_LKW_D_EU6_II</td><td>HDV_RT_II_D_EU6</td></tr>
  <tr><td></td><td></td><td></td><td></td><td></td><td>HDV_CB_D_HEV_EU6</td><td></td><td></td><td></td><td></td></tr>
</table>

## Special Classes

The special class `zero` may be used to specify a vehicle without emissions.
