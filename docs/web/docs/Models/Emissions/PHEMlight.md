---
title: Models Emissions PHEMlight
permalink: /Models/Emissions/PHEMlight/
---

# PHEM (Passenger Car and Heavy Duty Emission Model)

[PHEM (Passenger Car and Heavy Duty Emission
Model)](http://www.ivt.tugraz.at/index.php?option=com_content&view=article&id=69:emissions&catid=27&lang=en&Itemid=301)
is an instantaneous vehicle emission model developed by the TU Graz
since 1999. PHEM is based on an extensive European set of vehicle
measurements and covers passenger cars, light duty vehicles and heavy
duty vehicles from city buses up to 40 ton semi-trailers.

Within [COLOMBO](http://colombo-fp7.eu/), a simplified version of PHEM,
embedded into SUMO, was developed. The PHEMlight model is the result of
this work. An in-depth description can be found in
[COLOMBO](http://colombo-fp7.eu/)'s deliverable
[D4.2](http://colombo-fp7.eu/results_deliverables.php). The model
received a major update to version 2.0.0.0 in the beginning of 2016
which also used new names for all vehicle classes, most notably all
classes now have an english prefix distinguishing passenger cars (PC_),
light (LCV_) and heavy duty vehicles (HDV_). Since the class ames are
disjoint both models can be used alternatively or even in parallel
although the old classes are considered deprecated.

PHEMlight uses data files that include the parameters of the modelled
emission classes. At the time being, data sets for two emission classes
are included in SUMO: PKW_D_EU4 and PKW_G_EU4 (PC_D_EU4 and
PC_G_EU4 for the new model), resembling passenger vehicles, Euro Norm
4, powered by Diesel (PKW_D_EU4) and gasoline (PKW_G_EU4).

To use PHEMlight, you can set an additional search path to these input
files using the configuration option  where  is the path. Alternatively,
you may set the environment variable **PHEMLIGHT_PATH**. By default
SUMO is only looking into <SUMO_HOME>/data/emissions/PHEMlight

In addition, your vehicles must be assigned to one of PHEMlight's
vehicle emission classes, summarised in the following table. To use the
emission classes below prepend the class name with the PHEMlight prefix,
e.g. "PHEMlight/PKW_G_EU4"}}.

## Explanations for the 2.0 naming conventions

The name is composed of the vehicle category and size, the technology
and the emission standard, all connected with underscores.

### Vehicle Category and Size Classes

  - PC :passenger cars
    LCV :light commercial vehicles; differentiation into size classes
    according to legislation (N1-I, N1-II, N1-III); size class I RM \<
    1305kg; size class II 1305kg \< RM \<= 1760kg; size class III 1760kg
    \< RM \<= 3500kg; "RM": reference mass (approx. Vehicle curb
    weight)
    HDV_RT :heavy duty vehicles - rigid trucks; size class I 2 axle
    trucks; size class II 3+ axle trucks
    HDV_TT :heavy duty vehicles - truck + trailer (incl. articulated
    trucks = tractor + semitrailer)
    HDV_CB :heavy duty vehicles - city bus
    HDV_CO :heavy duty vehicles - coach
    MC_2S :motorcycles - 2 stroke (not included)
    MC_4S :motorcycles - 4 stroke (not included)
    MOP :moped (not included)

### Technology

  - G :gasoline engine (conventional powertrain)
    D :diesel engine (conventional powertrain)
    G_HEV :gasoline engine (parallel hybrid powertrain)
    D_HEV :diesel engine (parallel hybrid powertrain)
    CNG :Compressed Natural Gas engine
    BEV :battery electric vehicles

### Examples

  - PC_G_EU6 :passenger car, gasoline engine, EURO 6
    PC_BEV :passenger car, battery electric
    LCV_III_CNG_EU6 :light commercial vehicle, size class III, CNG
    engine, Euro 6
    HDV_RT_I_D_EU4 :rigid truck, size class I, diesel engine, EURO
    IV
    HDV_CB_D_EU5 :city bus, diesel engine, Euro V

### Remarks on Hybrid vehicles

The fleet average "HEV" vehicles in PHEM and PHEMlight do NOT match with
single particular vehicles in the market (e.g. Toyota Prius) but have
been defined as follows:

1.  Baseline vehicle setup (e.g. vehicle size) similar than related
    conventional vehicle
2.  Total rated vehicle power of baseline conventional vehicle split to
    combustion engine and to electric motor (EM)
3.  Additional vehicle weight due to EM and battery considered
4.  Application of generic parallel hybrid operation strategy
    (recuperation, boosting, charching etc.)

Also note that plugin hybrids (where the main powertrain is electric)
should be modelled as battery vehicles and need to switch their emission
class (and thus their vehicle type in SUMO) when running conventional.

The following tables show the available classes. Some classes
(especially for motor cycles) were removed in the 2.0 version and some
(especially alternative fuels) are only available there.

## PHEMlight Emission Classes for Light Vehicles running on Diesel or Gasoline

| Passenger Cars | Light Duty Vehicles |
| -------------- | ------------------- |
|                | 2.0.0.0             |
| PKW_D_EU0    | PC_D_EU0          |
| PKW_D_EU1    | PC_D_EU1          |
| PKW_D_EU2    | PC_D_EU2          |
| PKW_D_EU3    | PC_D_EU3          |
| PKW_D_EU4    | PC_D_EU4          |
| PKW_D_EU5    | PC_D_EU5          |
| PKW_D_EU6    | PC_D_EU6          |
|                | PC_D_EU6c         |
| PKW_G_EU0    | PC_G_EU0          |
| PKW_G_EU1    | PC_G_EU1          |
| PKW_G_EU2    | PC_G_EU2          |
| PKW_G_EU3    | PC_G_EU3          |
| PKW_G_EU4    | PC_G_EU4          |
| PKW_G_EU5    | PC_G_EU5          |
| PKW_G_EU6    | PC_G_EU6          |
|                | PC_G_EU6c         |
|                |                     |

## PHEMlight Emission Classes for Light Vehicles running on alternative fuels

| Passenger Cars | Light Duty Vehicles |
| -------------- | ------------------- |
|                | 2.0.0.0             |
|                | PC_BEV             |
|                | PC_CNG_EU5        |
|                | PC_CNG_EU6        |
|                | PC_CNG_EU6c       |
| H_PKW_D_EU5 |                     |
| H_PKW_D_EU6 | PC_D_HEV_EU6     |
|                | PC_D_HEV_EU6c    |
| H_PKW_G_EU5 |                     |
| H_PKW_G_EU6 | PC_G_HEV_EU6     |
|                | PC_G_HEV_EU6c    |
|                |                     |

## PHEMlight Emission Classes for Motor Cycles

Motor cycles are only available for the old data set (pre 2016)

| Two-Wheelers | Motor Cycles   |
| ------------ | -------------- |
| KKR_G_EU0  | MR_G_EU0_2T |
| KKR_G_EU1  | MR_G_EU1_2T |
| KKR_G_EU2  | MR_G_EU2_2T |
| KKR_G_EU3  | MR_G_EU3_2T |
| KKR_G_EU4  | MR_G_EU4_2T |
|              | MR_G_EU5_2T |
|              |                |

## PHEMlight Emission Classes for Heavy Vehicles

| Tractor/Trailor | Coach           | Urban and Inter Urban Buses | Trucks          |
| --------------- | --------------- | --------------------------- | --------------- |
|                 | 2.0.0.0         |                             | 2.0.0.0         |
|                 |                 |                             |                 |
|                 |                 |                             |                 |
| LSZ_D_EU0     | LSZ_D_EU0     | RB_D_EU0                  | HDV_CO_D_EU0 |
| LSZ_D_EU1     | HDV_TT_D_EU1 | RB_D_EU1                  | HDV_CO_D_EU1 |
| LSZ_D_EU2     | HDV_TT_D_EU2 | RB_D_EU2                  | HDV_CO_D_EU2 |
| LSZ_D_EU3     | HDV_TT_D_EU3 | RB_D_EU3                  | HDV_CO_D_EU3 |
| LSZ_D_EU4     | HDV_TT_D_EU4 | RB_D_EU4                  | HDV_CO_D_EU4 |
| LSZ_D_EU5     | HDV_TT_D_EU5 | RB_D_EU5                  | HDV_CO_D_EU5 |
| LSZ_D_EU6     | HDV_TT_D_EU6 | RB_D_EU6                  | HDV_CO_D_EU6 |
|                 |                 |                             |                 |
|                 |                 |                             |                 |

## Special Classes

The special class  may be used to specify a vehicle without emissions.

## Further Ressources

  - [COLOMBO: Deliverable 4.2; Extended Simulation Tool PHEM coupled to
    SUMO with User
    Guide](http://colombo-fp7.eu/results_deliverables.php); February,
    2014