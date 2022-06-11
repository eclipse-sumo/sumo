---
title: PHEMlight5
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

!!! caution
    Please note that the order of the name parts is different than in the old PHEMlight.

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
  
- **CB**  city bus; midi: <= 15t, heavy: mix of > 15t, no suffix: mix of midi and heavy
  
- **CO**  coach
  
- **MC_2S**  motorcycles - 2 stroke
  
- **MC_4S**  motorcycles - 4 stroke; s250cc: < 250ccm, g250cc: > 250ccm
  
### Technology

- **G**  gasoline engine (no HEV suffix: conventional powertrain)
  
- **D**  diesel engine (no HEV suffix: conventional powertrain)
  
- **D_DPF**  diesel engine with particle filter
  
- **D_MW**  average between diesel engine with and without particle filter
  
- **D_SU**  diesel engine with software update
  
- **HEV**  hybrid powertrain (additional suffix)
  
- **BEV_GEN1**  battery electric vehicles

### Examples

- **PC_EU6_G**  passenger car, gasoline engine, EURO 6
  
- **PC_BEV_GEN1**  passenger car, battery electric
  
- **LCV_EU6ab_G_III_HEV**  light commercial vehicle, size class III, hybrid gasoline powertrain, Euro 6ab
  
- **RT_EUIV_D_I**  rigid truck, size class I, diesel engine, EURO IV

- **CB_EUV_D**  city bus, diesel engine, Euro V

## PHEMlight5 Emission Classes 

The following tables show the available classes. Except for the two highlighted Euro 4
passenger cars the classes need to be licensed from TU Graz.

### Light Vehicles running on Diesel

<table><tr><th>Passenger Cars</th><th colspan="3">Light Duty Vehicles</th></tr>
  <tr><td>PC_EU0_D</td><td>LCV_EU0_D_I</td><td>LCV_EU0_D_II</td><td>LCV_EU0_D_III</td></tr>
  <tr><td>PC_EU1_D</td><td>LCV_EU1_D_I</td><td>LCV_EU1_D_II</td><td>LCV_EU1_D_III</td></tr>
  <tr><td>PC_EU2_D</td><td>LCV_EU2_D_I</td><td>LCV_EU2_D_II</td><td>LCV_EU2_D_III</td></tr>
  <tr><td>PC_EU3_D</td><td>LCV_EU3_D_I</td><td>LCV_EU3_D_II</td><td>LCV_EU3_D_III</td></tr>
  <tr><td>PC_EU4_D</td><td>LCV_EU4_D_I</td><td>LCV_EU4_D_II</td><td>LCV_EU4_D_III</td></tr>
  <tr><td>PC_EU4_D_DPF</td><td>LCV_EU4_D_DPF_I</td><td>LCV_EU4_D_DPF_II</td><td>LCV_EU4_D_DPF_III</td></tr>
  <tr><td style="background-color:#34ff34">PC_EU4_D_MW</td><td>LCV_EU4_D_MW_I</td><td>LCV_EU4_D_MW_II</td><td>LCV_EU4_D_MW_III</td></tr>
  <tr><td>PC_EU5_D</td><td>LCV_EU5_D_I</td><td>LCV_EU5_D_II</td><td>LCV_EU5_D_III</td></tr>
  <tr><td>PC_EU5_D_SU</td><td></td><td></td><td></td></tr>
  <tr><td>PC_EU6ab_D</td><td>LCV_EU6ab_D_I</td><td>LCV_EU6ab_D_II</td><td>LCV_EU6ab_D_III</td></tr>
  <tr><td>PC_EU6c_D</td><td>LCV_EU6c_D_I</td><td>LCV_EU6c_D_II</td><td>LCV_EU6c_D_III</td></tr>
  <tr><td>PC_EU6d_D</td><td>LCV_EU6d_D_I</td><td>LCV_EU6d_D_II</td><td>LCV_EU6d_D_III</td></tr>
  <tr><td>PC_EU6d-Temp_D</td><td>LCV_EU6d-Temp_D_I</td><td>LCV_EU6d-Temp_D_II</td><td>LCV_EU6d-Temp_D_III</td></tr>
  <tr><td>PC_EU7_D</td><td></td><td></td><td></td></tr>
</table>

### Light Vehicles running on Gasoline

<table><tr><th>Passenger Cars</th><th colspan="3">Light Duty Vehicles</th></tr>
  <tr><td>PC_EU0_G</td><td>LCV_EU0_G_I</td><td>LCV_EU0_G_II</td><td>LCV_EU0_G_III</td></tr>
  <tr><td>PC_EU1_G</td><td>LCV_EU1_G_I</td><td>LCV_EU1_G_II</td><td>LCV_EU1_G_III</td></tr>
  <tr><td>PC_EU2_G</td><td>LCV_EU2_G_I</td><td>LCV_EU2_G_II</td><td>LCV_EU2_G_III</td></tr>
  <tr><td>PC_EU3_G</td><td>LCV_EU3_G_I</td><td>LCV_EU3_G_II</td><td>LCV_EU3_G_III</td></tr>
  <tr><td style="background-color:#34ff34">PC_EU4_G</td><td>LCV_EU4_G_I</td><td>LCV_EU4_G_II</td><td>LCV_EU4_G_III</td></tr>
  <tr><td>PC_EU5_G</td><td>LCV_EU5_G_I</td><td>LCV_EU5_G_II</td><td>LCV_EU5_G_III</td></tr>
  <tr><td>PC_EU6ab_G</td><td>LCV_EU6ab_G_I</td><td>LCV_EU6ab_G_II</td><td>LCV_EU6ab_G_III</td></tr>
  <tr><td>PC_EU6c_G</td><td>LCV_EU6c_G_I</td><td>LCV_EU6c_G_II</td><td>LCV_EU6c_G_III</td></tr>
  <tr><td>PC_EU6d_G</td><td>LCV_EU6d_G_I</td><td>LCV_EU6d_G_II</td><td>LCV_EU6d_G_III</td></tr>
  <tr><td>PC_EU6d-Temp_G</td><td>LCV_EU6d-Temp_G_I</td><td>LCV_EU6d-Temp_G_II</td><td>LCV_EU6d-Temp_G_III</td></tr>
  <tr><td>PC_EU7_G</td><td></td><td></td><td></td></tr>
</table>

### Light Vehicles running on alternative fuels

<table><tr><th>Passenger Cars</th><th colspan="3">Light Duty Vehicles</th></tr>
  <tr><td>PC_BEV_GEN1</td><td>LCV_BEV_GEN1_I</td><td>LCV_BEV_GEN1_II</td><td>LCV_BEV_GEN1_III</td></tr>
  <tr><td>PC_EU6ab_D_HEV</td><td>LCV_EU6ab_D_I_HEV</td><td>LCV_EU6ab_D_II_HEV</td><td>LCV_EU6ab_D_III_HEV</td></tr>
  <tr><td>PC_EU6c_D_HEV</td><td>LCV_EU6c_D_I_HEV</td><td>LCV_EU6c_D_II_HEV</td><td>LCV_EU6c_D_III_HEV</td></tr>
  <tr><td>PC_EU6d_D_HEV</td><td>LCV_EU6d_D_I_HEV</td><td>LCV_EU6d_D_II_HEV</td><td>LCV_EU6d_D_III_HEV</td></tr>
  <tr><td>PC_EU6d-Temp_D_HEV</td><td>LCV_EU6d-Temp_D_I_HEV</td><td>LCV_EU6d-Temp_D_II_HEV</td><td>LCV_EU6d-Temp_D_III_HEV</td></tr>
  <tr><td>PC_EU6ab_G_HEV</td><td>LCV_EU6ab_G_I_HEV</td><td>LCV_EU6ab_G_II_HEV</td><td>LCV_EU6ab_G_III_HEV</td></tr>
  <tr><td>PC_EU6c_G_HEV</td><td>LCV_EU6c_G_I_HEV</td><td>LCV_EU6c_G_II_HEV</td><td>LCV_EU6c_G_III_HEV</td></tr>
  <tr><td>PC_EU6d_G_HEV</td><td>LCV_EU6d_G_I_HEV</td><td>LCV_EU6d_G_II_HEV</td><td>LCV_EU6d_G_III_HEV</td></tr>
  <tr><td>PC_EU6d-Temp_G_HEV</td><td>LCV_EU6d-Temp_G_I_HEV</td><td>LCV_EU6d-Temp_G_II_HEV</td><td>LCV_EU6d-Temp_G_III_HEV</td></tr>
</table>

### Motor Cycles

<table>
  <tr><th colspan="3">Motor Cycles</th></tr>
  <tr><td>MC_2S_EU0_G</td><td>MC_4S_EU0_G_s250cc</td><td>MC_4S_EU0_G_g250cc</td></tr>
  <tr><td>MC_2S_EU1_G</td><td>MC_4S_EU1_G_s250cc</td><td>MC_4S_EU1_G_g250cc</td></tr>
  <tr><td>MC_2S_EU2_G</td><td>MC_4S_EU2_G_s250cc</td><td>MC_4S_EU2_G_g250cc</td></tr>
  <tr><td>MC_2S_EU3_G</td><td>MC_4S_EU3_G_s250cc</td><td>MC_4S_EU3_G_g250cc</td></tr>
  <tr><td>MC_2S_EU4_G</td><td>MC_4S_EU4_G_s250cc</td><td>MC_4S_EU4_G_g250cc</td></tr>
  <tr><td>MC_2S_EU5_G</td><td>MC_4S_EU5_G_s250cc</td><td>MC_4S_EU5_G_g250cc</td></tr>
  <tr><td>MC_2S_EU6_G</td><td>MC_4S_EU6_G_s250cc</td><td>MC_4S_EU6_G_g250cc</td></tr>
  <tr><td colspan="3">MC_BEV_GEN1</td></tr>
</table>

### Heavy Vehicles

<table><tr><th colspan="2">Rigid Truck</th><th>Truck + Trailer</th><th colspan="3">City Bus</th><th>Coach</th></tr>
  <tr><td>RT_EU0_D_I</td><td>RT_EU0_D_II</td><td>TT_EU0_D</td><td>CB_EU0_D</td><td>CB_EU0_D_Midi</td><td>CB_EU0_D_Heavy</td><td>CO_EU0_D</td></tr>
  <tr><td>RT_EUI_D_I</td><td>RT_EUI_D_II</td><td>TT_EUI_D</td><td>CB_EUI_D</td><td>CB_EUI_D_Midi</td><td>CB_EUI_D_Heavy</td><td>CO_EUI_D</td></tr>
  <tr><td>RT_EUII_D_I</td><td>RT_EUII_D_II</td><td>TT_EUII_D</td><td>CB_EUII_D</td><td>CB_EUII_D_Midi</td><td>CB_EUII_D_Heavy</td><td>CO_EUII_D</td></tr>
  <tr><td>RT_EUIII_D_I</td><td>RT_EUIII_D_II</td><td>TT_EUIII_D</td><td>CB_EUIII_D</td><td>CB_EUIII_D_Midi</td><td>CB_EUIII_D_Heavy</td><td>CO_EUIII_D</td></tr>
  <tr><td>RT_EUIV_D_I</td><td>RT_EUIV_D_II</td><td>TT_EUIV_D</td><td>CB_EUIV_D</td><td>CB_EUIV_D_Midi</td><td>CB_EUIV_D_Heavy</td><td>CO_EUIV_D</td></tr>
  <tr><td>RT_EUV_D_I</td><td>RT_EUV_D_II</td><td>TT_EUV_D</td><td>CB_EUV_D</td><td>CB_EUV_D_Midi</td><td>CB_EUV_D_Heavy</td><td>CO_EUV_D</td></tr>
  <tr><td></td><td></td><td></td><td>CB_EUV_D_HEV</td><td></td><td></td><td></td></tr>
  <tr><td>RT_EUVI_D_I</td><td>RT_EUVI_D_II</td><td>TT_EUVI_D</td><td>CB_EUVI_D</td><td>CB_EUVI_D_Midi</td><td>CB_EUVI_D_Heavy</td><td>CO_EUVI_D</td></tr>
  <tr><td></td><td></td><td></td><td>CB_EUVI_D_HEV</td><td></td><td></td><td></td></tr>
  <tr><td></td><td></td><td></td><td>CB_BEV_GEN1</td><td></td><td></td><td></td></tr>
</table>

### Special Classes

The special class `zero` may be used to specify a vehicle without emissions.

## Deterioration modeling

PHEMlight5 allows to model the effects of aging and temperature on emissions. There are two new options to enable these models.

- **--phemlight-year** the year your scenario simulates. PHEMlight will try to compute an average mileage for every emission class and adapt the emission values accordingly.
- **--phemlight-temperature** the ambient temperature. This will only affect the NOx value of some Diesel cars.

The underlying correction factors are read from the files Deterioration.det, Mileage.vma and NOxCor.tno in your PHEMlight5 directory.
