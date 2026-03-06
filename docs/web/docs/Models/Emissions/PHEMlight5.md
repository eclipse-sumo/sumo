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

| Passenger Cars | LCV I | LCV II | LCV III |
|----------------|-------|--------|---------|
| PC_EU0_D | LCV_EU0_D_I | LCV_EU0_D_II | LCV_EU0_D_III |
| PC_EU1_D | LCV_EU1_D_I | LCV_EU1_D_II | LCV_EU1_D_III |
| PC_EU2_D | LCV_EU2_D_I | LCV_EU2_D_II | LCV_EU2_D_III |
| PC_EU3_D | LCV_EU3_D_I | LCV_EU3_D_II | LCV_EU3_D_III |
| PC_EU4_D | LCV_EU4_D_I | LCV_EU4_D_II | LCV_EU4_D_III |
| PC_EU4_D_DPF | LCV_EU4_D_DPF_I | LCV_EU4_D_DPF_II | LCV_EU4_D_DPF_III |
| **PC_EU4_D_MW** (included) | LCV_EU4_D_MW_I | LCV_EU4_D_MW_II | LCV_EU4_D_MW_III |
| PC_EU5_D | LCV_EU5_D_I | LCV_EU5_D_II | LCV_EU5_D_III |
| PC_EU5_D_SU | | | |
| PC_EU6ab_D | LCV_EU6ab_D_I | LCV_EU6ab_D_II | LCV_EU6ab_D_III |
| PC_EU6c_D | LCV_EU6c_D_I | LCV_EU6c_D_II | LCV_EU6c_D_III |
| PC_EU6d_D | LCV_EU6d_D_I | LCV_EU6d_D_II | LCV_EU6d_D_III |
| PC_EU6d-Temp_D | LCV_EU6d-Temp_D_I | LCV_EU6d-Temp_D_II | LCV_EU6d-Temp_D_III |
| PC_EU7_D | | | |

### Light Vehicles running on Gasoline

| Passenger Cars | LCV I | LCV II | LCV III |
|----------------|-------|--------|---------|
| PC_EU0_G | LCV_EU0_G_I | LCV_EU0_G_II | LCV_EU0_G_III |
| PC_EU1_G | LCV_EU1_G_I | LCV_EU1_G_II | LCV_EU1_G_III |
| PC_EU2_G | LCV_EU2_G_I | LCV_EU2_G_II | LCV_EU2_G_III |
| PC_EU3_G | LCV_EU3_G_I | LCV_EU3_G_II | LCV_EU3_G_III |
| **PC_EU4_G** (included) | LCV_EU4_G_I | LCV_EU4_G_II | LCV_EU4_G_III |
| PC_EU5_G | LCV_EU5_G_I | LCV_EU5_G_II | LCV_EU5_G_III |
| PC_EU6ab_G | LCV_EU6ab_G_I | LCV_EU6ab_G_II | LCV_EU6ab_G_III |
| PC_EU6c_G | LCV_EU6c_G_I | LCV_EU6c_G_II | LCV_EU6c_G_III |
| PC_EU6d_G | LCV_EU6d_G_I | LCV_EU6d_G_II | LCV_EU6d_G_III |
| PC_EU6d-Temp_G | LCV_EU6d-Temp_G_I | LCV_EU6d-Temp_G_II | LCV_EU6d-Temp_G_III |
| PC_EU7_G | | | |

### Light Vehicles running on alternative fuels

| Passenger Cars | LCV I | LCV II | LCV III |
|----------------|-------|--------|---------|
| PC_BEV_GEN1 | LCV_BEV_GEN1_I | LCV_BEV_GEN1_II | LCV_BEV_GEN1_III |
| PC_EU6ab_D_HEV | LCV_EU6ab_D_I_HEV | LCV_EU6ab_D_II_HEV | LCV_EU6ab_D_III_HEV |
| PC_EU6c_D_HEV | LCV_EU6c_D_I_HEV | LCV_EU6c_D_II_HEV | LCV_EU6c_D_III_HEV |
| PC_EU6d_D_HEV | LCV_EU6d_D_I_HEV | LCV_EU6d_D_II_HEV | LCV_EU6d_D_III_HEV |
| PC_EU6d-Temp_D_HEV | LCV_EU6d-Temp_D_I_HEV | LCV_EU6d-Temp_D_II_HEV | LCV_EU6d-Temp_D_III_HEV |
| PC_EU6ab_G_HEV | LCV_EU6ab_G_I_HEV | LCV_EU6ab_G_II_HEV | LCV_EU6ab_G_III_HEV |
| PC_EU6c_G_HEV | LCV_EU6c_G_I_HEV | LCV_EU6c_G_II_HEV | LCV_EU6c_G_III_HEV |
| PC_EU6d_G_HEV | LCV_EU6d_G_I_HEV | LCV_EU6d_G_II_HEV | LCV_EU6d_G_III_HEV |
| PC_EU6d-Temp_G_HEV | LCV_EU6d-Temp_G_I_HEV | LCV_EU6d-Temp_G_II_HEV | LCV_EU6d-Temp_G_III_HEV |

### Motor Cycles

| MC 2-stroke | MC 4-stroke (&lt;250cc) | MC 4-stroke (&gt;250cc) |
|-------------|:---------------------:|:---------------------:|
| MC_2S_EU0_G | MC_4S_EU0_G_s250cc | MC_4S_EU0_G_g250cc |
| MC_2S_EU1_G | MC_4S_EU1_G_s250cc | MC_4S_EU1_G_g250cc |
| MC_2S_EU2_G | MC_4S_EU2_G_s250cc | MC_4S_EU2_G_g250cc |
| MC_2S_EU3_G | MC_4S_EU3_G_s250cc | MC_4S_EU3_G_g250cc |
| MC_2S_EU4_G | MC_4S_EU4_G_s250cc | MC_4S_EU4_G_g250cc |
| MC_2S_EU5_G | MC_4S_EU5_G_s250cc | MC_4S_EU5_G_g250cc |
| MC_2S_EU6_G | MC_4S_EU6_G_s250cc | MC_4S_EU6_G_g250cc |
| MC_BEV_GEN1 | | |

### Heavy Vehicles

| Rigid Truck I | Rigid Truck II | Truck + Trailer | City Bus | City Bus Midi | City Bus Heavy | Coach |
|---------------|----------------|:---------------:|----------|---------------|----------------|-------|
| RT_EU0_D_I | RT_EU0_D_II | TT_EU0_D | CB_EU0_D | CB_EU0_D_Midi | CB_EU0_D_Heavy | CO_EU0_D |
| RT_EUI_D_I | RT_EUI_D_II | TT_EUI_D | CB_EUI_D | CB_EUI_D_Midi | CB_EUI_D_Heavy | CO_EUI_D |
| RT_EUII_D_I | RT_EUII_D_II | TT_EUII_D | CB_EUII_D | CB_EUII_D_Midi | CB_EUII_D_Heavy | CO_EUII_D |
| RT_EUIII_D_I | RT_EUIII_D_II | TT_EUIII_D | CB_EUIII_D | CB_EUIII_D_Midi | CB_EUIII_D_Heavy | CO_EUIII_D |
| RT_EUIV_D_I | RT_EUIV_D_II | TT_EUIV_D | CB_EUIV_D | CB_EUIV_D_Midi | CB_EUIV_D_Heavy | CO_EUIV_D |
| RT_EUV_D_I | RT_EUV_D_II | TT_EUV_D | CB_EUV_D | CB_EUV_D_Midi | CB_EUV_D_Heavy | CO_EUV_D |
| | | | CB_EUV_D_HEV | | | |
| RT_EUVI_D_I | RT_EUVI_D_II | TT_EUVI_D | CB_EUVI_D | CB_EUVI_D_Midi | CB_EUVI_D_Heavy | CO_EUVI_D |
| | | | CB_EUVI_D_HEV | | | |
| | | | CB_BEV_GEN1 | | | |

### Special Classes

The special class `zero` may be used to specify a vehicle without emissions.

## Deterioration modeling

PHEMlight5 allows to model the effects of aging and temperature on emissions. There are two new options to enable these models.

- **--phemlight-year** the year your scenario simulates. PHEMlight will try to compute an average mileage for every emission class and adapt the emission values accordingly.
- **--phemlight-temperature** the ambient temperature. This will only affect the NOx value of some Diesel cars.

The underlying correction factors are read from the files Deterioration.det, Mileage.vma and NOxCor.tno in your PHEMlight5 directory.

## Dynamic modification of parameters

SUMO 1.20.0 added the possibility to change parameters which were a static part of the emission class definition before.
Most of the parameters have the same name and meaning as with the [electric model](../Electric.md) and can be defined as
params in the vehicle type definition. Furthermore the type attribute `mass` is now being respected by PHEMlight as well.
If any of the new attributes or params is not defined the default will be taken from the emission class as before.
For reference you find a list of the evaluated params below.

| key                      | Value Type | Unit           | Description                                             |
| ------------------------ | ---------- | -------------- | ------------------------------------------------------- |
| maximumPower             | float      | W              | Maximum power which the vehicle can reach               |
| vehicleMass              | float      | kg             | Vehicle mass (deprecated)                               |
| loading                  | float      | kg             | Additional mass                                         |
| rotatingMass             | float      | kg             | (Equivalent) mass of internal rotating elements         |
| frontSurfaceArea         | float      | m<sup>2</sup>  | Front surface area                                      |
| airDragCoefficient       | float      | -              | Air drag coefficient                                    |
| radialDragCoefficient    | float      | -              | Radial drag coefficient                                 |
| rollDragCoefficient      | float      | -              | Rolling resistance coefficient                          |
| wheelRadius              | float      | m              | Radius of the wheels                                    |
| constantPowerIntake      | float      | W              | Avg. (constant) power of consumers                      |

Please be aware that `vehicleMass` should not be used for total mass anymore. Please use the `mass` attribute (for empty mass)
and the `loading` param instead.
