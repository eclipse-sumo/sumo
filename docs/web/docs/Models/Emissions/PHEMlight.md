---
title: PHEMlight
---

# PHEM (Passenger Car and Heavy Duty Emission Model)

[PHEM (Passenger Car and Heavy Duty Emission
Model)](https://www.itna.tugraz.at/assets/files/areas/em/Phem_en.pdf)
is an instantaneous vehicle emission model developed by the TU Graz
since 1999. PHEM is based on an extensive European set of vehicle
measurements and covers passenger cars, light duty vehicles and heavy
duty vehicles from city buses up to 40 ton semi-trailers.

Within [COLOMBO](http://web.archive.org/web/20240722054138/https://verkehrsforschung.dlr.de/en/projects/colombo), a simplified version of PHEM,
embedded into SUMO, was developed. The [PHEMlight](https://www.itna.tugraz.at/assets/files/areas/em/PHEMlight_en.pdf) model is the result of
this work. An in-depth description can be found in
[COLOMBO](http://web.archive.org/web/20240722054138/https://verkehrsforschung.dlr.de/en/projects/colombo)'s deliverable
[D4.2](https://web.archive.org/web/20170808122505/https://www.colombo-fp7.eu/results_deliverables.php). The model
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

!!! caution
    Only two emission classes are included in the public version. Any further vehicle classes must be [licensed from the TU Graz](https://www.itna.tugraz.at/en/research/areas/em/simulation/phemlight.html).

To use PHEMlight, you can set an additional search path to these input
files using the configuration option **--phemlight-path** {{DT_FILE}} where {{DT_FILE}} is the path. Alternatively,
you may set the environment variable **PHEMLIGHT_PATH**. By default
SUMO is looking into <SUMO_HOME\>/data/emissions/PHEMlight and in the PHEMlight subdir
of your working directory.

In addition, your vehicles must be assigned to one of PHEMlight's
vehicle emission classes, summarised in the following table. To use the
emission classes below prepend the class name with the PHEMlight prefix,
e.g. `emissionClass="PHEMlight/PKW_G_EU4"`.

## Explanations for the 2.0 naming conventions

The name is composed of the vehicle category and size, the technology
and the emission standard, all connected with underscores.

### Vehicle Category and Size Classes

- **PC**  passenger cars

- **LCV**  light commercial vehicles; differentiation into size classes
  according to legislation (N1-I, N1-II, N1-III); size class I RM <
  1305kg; size class II 1305kg < RM <= 1760kg; size class III 1760kg
  < RM <= 3500kg; "RM": reference mass (approx. Vehicle curb
  weight)

- **HDV_RT**  heavy duty vehicles - rigid trucks; size class I 2 axle
  trucks; size class II 3+ axle trucks

- **HDV_TT**  heavy duty vehicles - truck + trailer (incl. articulated
  trucks = tractor + semitrailer)

- **HDV_CB**  heavy duty vehicles - city bus

- **HDV_CO**  heavy duty vehicles - coach

- **MC_2S**  motorcycles - 2 stroke (not included)

- **MC_4S**  motorcycles - 4 stroke (not included)

- **MOP**  moped (not included)

### Technology

- **G**  gasoline engine (conventional powertrain)

- **D**  diesel engine (conventional powertrain)

- **G_HEV**  gasoline engine (parallel hybrid powertrain)

- **D_HEV**  diesel engine (parallel hybrid powertrain)

- **CNG**  Compressed Natural Gas engine

- **BEV**  battery electric vehicles

### Examples

- **PC_G_EU6**  passenger car, gasoline engine, EURO 6

- **PC_BEV**  passenger car, battery electric

- **LCV_III_CNG_EU6**  light commercial vehicle, size class III, CNG engine, Euro 6

- **HDV_RT_I_D_EU4**  rigid truck, size class I, diesel engine, EURO IV

- **HDV_CB_D_EU5**  city bus, diesel engine, Euro V

### Remarks on PM emission

Currently, only exhaust emissions are being reported. Therefore, PMx from PHEMLight is equivalent to PM2.5.

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
    (recuperation, boosting, charging etc.)

Also note that plugin hybrids (where the main powertrain is electric)
should be modelled as battery vehicles and need to switch their emission
class (and thus their vehicle type in SUMO) when running conventional.

The following tables show the available classes. Some classes
(especially for motor cycles) were removed in the 2.0 version and some
(especially alternative fuels) are only available there.

## PHEMlight Emission Classes for Light Vehicles running on Diesel or Gasoline

*PC = Passenger Cars; LDV = Light Duty Vehicles (I/II/III = weight categories)*

| PC (old) | PC (2.0.0.0) | LDV I (old) | LDV I (2.0.0.0) | LDV II (old) | LDV II (2.0.0.0) | LDV III (old) | LDV III (2.0.0.0) |
|----------|:------------:|-------------|:---------------:|--------------|:----------------:|---------------|:-----------------:|
| PKW_D_EU0 | PC_D_EU0 | LNF_D_EU0_I | LCV_I_D_EU0 | LNF_D_EU0_II | LCV_II_D_EU0 | LNF_D_EU0_III | LCV_III_D_EU0 |
| PKW_D_EU1 | PC_D_EU1 | LNF_D_EU1_I | LCV_I_D_EU1 | LNF_D_EU1_II | LCV_II_D_EU1 | LNF_D_EU1_III | LCV_III_D_EU1 |
| PKW_D_EU2 | PC_D_EU2 | LNF_D_EU2_I | LCV_I_D_EU2 | LNF_D_EU2_II | LCV_II_D_EU2 | LNF_D_EU2_III | LCV_III_D_EU2 |
| PKW_D_EU3 | PC_D_EU3 | LNF_D_EU3_I | LCV_I_D_EU3 | LNF_D_EU3_II | LCV_II_D_EU3 | LNF_D_EU3_III | LCV_III_D_EU3 |
| PKW_D_EU4 | PC_D_EU4 | LNF_D_EU4_I | LCV_I_D_EU4 | LNF_D_EU4_II | LCV_II_D_EU4 | LNF_D_EU4_III | LCV_III_D_EU4 |
| PKW_D_EU5 | PC_D_EU5 | LNF_D_EU5_I | LCV_I_D_EU5 | LNF_D_EU5_II | LCV_II_D_EU5 | LNF_D_EU5_III | LCV_III_D_EU5 |
| PKW_D_EU6 | PC_D_EU6 | LNF_D_EU6_I | LCV_I_D_EU6 | LNF_D_EU6_II | LCV_II_D_EU6 | LNF_D_EU6_III | LCV_III_D_EU6 |
| | PC_D_EU6c | | LCV_I_D_EU6c | | LCV_II_D_EU6c | | LCV_III_D_EU6c |
| PKW_G_EU0 | PC_G_EU0 | LNF_G_EU0_I | LCV_I_G_EU0 | LNF_G_EU0_II | LCV_II_G_EU0 | LNF_G_EU0_III | LCV_III_G_EU0 |
| PKW_G_EU1 | PC_G_EU1 | LNF_G_EU1_I | LCV_I_G_EU1 | LNF_G_EU1_II | LCV_II_G_EU1 | LNF_G_EU1_III | LCV_III_G_EU1 |
| PKW_G_EU2 | PC_G_EU2 | LNF_G_EU2_I | LCV_I_G_EU2 | LNF_G_EU2_II | LCV_II_G_EU2 | LNF_G_EU2_III | LCV_III_G_EU2 |
| PKW_G_EU3 | PC_G_EU3 | LNF_G_EU3_I | LCV_I_G_EU3 | LNF_G_EU3_II | LCV_II_G_EU3 | LNF_G_EU3_III | LCV_III_G_EU3 |
| PKW_G_EU4 | PC_G_EU4 | LNF_G_EU4_I | LCV_I_G_EU4 | LNF_G_EU4_II | LCV_II_G_EU4 | LNF_G_EU4_III | LCV_III_G_EU4 |
| PKW_G_EU5 | PC_G_EU5 | LNF_G_EU5_I | LCV_I_G_EU5 | LNF_G_EU5_II | LCV_II_G_EU5 | LNF_G_EU5_III | LCV_III_G_EU5 |
| PKW_G_EU6 | PC_G_EU6 | LNF_G_EU6_I | LCV_I_G_EU6 | LNF_G_EU6_II | LCV_II_G_EU6 | LNF_G_EU6_III | LCV_III_G_EU6 |
| | PC_G_EU6c | | LCV_I_G_EU6c | | LCV_II_G_EU6c | | LCV_III_G_EU6c |

## PHEMlight Emission Classes for Light Vehicles running on alternative fuels

*PC = Passenger Cars; LDV = Light Duty Vehicles (I/II/III = weight categories)*

| PC (old) | PC (2.0.0.0) | LDV I (2.0.0.0) | LDV II (2.0.0.0) | LDV III (2.0.0.0) |
|----------|:------------:|:---------------:|:----------------:|:-----------------:|
| | PC_BEV | LCV_I_BEV | LCV_II_BEV | LCV_III_BEV |
| | PC_CNG_EU5 | LCV_I_CNG_EU5 | LCV_II_CNG_EU5 | LCV_III_CNG_EU5 |
| | PC_CNG_EU6 | LCV_I_CNG_EU6 | LCV_II_CNG_EU6 | LCV_III_CNG_EU6 |
| | PC_CNG_EU6c | LCV_I_CNG_EU6c | LCV_II_CNG_EU6c | LCV_III_CNG_EU6c |
| H_PKW_D_EU5 | | | | |
| H_PKW_D_EU6 | PC_D_HEV_EU6 | LCV_I_D_HEV_EU6 | LCV_II_D_HEV_EU6 | LCV_III_D_HEV_EU6 |
| | PC_D_HEV_EU6c | LCV_I_D_HEV_EU6c | LCV_II_D_HEV_EU6c | LCV_III_D_HEV_EU6c |
| H_PKW_G_EU5 | | | | |
| H_PKW_G_EU6 | PC_G_HEV_EU6 | LCV_I_G_HEV_EU6 | LCV_II_G_HEV_EU6 | LCV_III_G_HEV_EU6 |
| | PC_G_HEV_EU6c | LCV_I_G_HEV_EU6c | LCV_II_G_HEV_EU6c | LCV_III_G_HEV_EU6c |

## PHEMlight Emission Classes for Motor Cycles

Motor cycles are only available for the old data set (pre 2016)

| Two-Wheelers | Motor Cycles (2T) | Motor Cycles (4T) |
|--------------|:-----------------:|:-----------------:|
| KKR_G_EU0 | MR_G_EU0_2T | MR_G_EU0_4T |
| KKR_G_EU1 | MR_G_EU1_2T | MR_G_EU1_4T |
| KKR_G_EU2 | MR_G_EU2_2T | MR_G_EU2_4T |
| KKR_G_EU3 | MR_G_EU3_2T | MR_G_EU3_4T |
| KKR_G_EU4 | MR_G_EU4_2T | MR_G_EU4_4T |
| | MR_G_EU5_2T | MR_G_EU5_4T |

## PHEMlight Emission Classes for Heavy Vehicles

*TT = Tractor/Trailer; Bus = Urban and Inter Urban Buses; Truck I/II = weight categories*

| TT (old) | TT (2.0.0.0) | Coach (old) | Coach (2.0.0.0) | Bus (old) | Bus (2.0.0.0) | Truck I (old) | Truck I (2.0.0.0) | Truck II (old) | Truck II (2.0.0.0) |
|----------|:------------:|-------------|:---------------:|-----------|:-------------:|---------------|:-----------------:|----------------|:-----------------:|
| | | | | | HDV_CB_BEV | | | | |
| | | | | | HDV_CB_CNG_EU6 | | | | |
| LSZ_D_EU0 | LSZ_D_EU0 | RB_D_EU0 | HDV_CO_D_EU0 | LB_D_EU0 | HDV_CB_D_EU0 | SOLO_LKW_D_EU0_I | HDV_RT_I_D_EU0 | SOLO_LKW_D_EU0_II | HDV_RT_II_D_EU0 |
| LSZ_D_EU1 | HDV_TT_D_EU1 | RB_D_EU1 | HDV_CO_D_EU1 | LB_D_EU1 | HDV_CB_D_EU1 | SOLO_LKW_D_EU1_I | HDV_RT_I_D_EU1 | SOLO_LKW_D_EU1_II | HDV_RT_II_D_EU1 |
| LSZ_D_EU2 | HDV_TT_D_EU2 | RB_D_EU2 | HDV_CO_D_EU2 | LB_D_EU2 | HDV_CB_D_EU2 | SOLO_LKW_D_EU2_I | HDV_RT_I_D_EU2 | SOLO_LKW_D_EU2_II | HDV_RT_II_D_EU2 |
| LSZ_D_EU3 | HDV_TT_D_EU3 | RB_D_EU3 | HDV_CO_D_EU3 | LB_D_EU3 | HDV_CB_D_EU3 | SOLO_LKW_D_EU3_I | HDV_RT_I_D_EU3 | SOLO_LKW_D_EU3_II | HDV_RT_II_D_EU3 |
| LSZ_D_EU4 | HDV_TT_D_EU4 | RB_D_EU4 | HDV_CO_D_EU4 | LB_D_EU4 | HDV_CB_D_EU4 | SOLO_LKW_D_EU4_I | HDV_RT_I_D_EU4 | SOLO_LKW_D_EU4_II | HDV_RT_II_D_EU4 |
| LSZ_D_EU5 | HDV_TT_D_EU5 | RB_D_EU5 | HDV_CO_D_EU5 | LB_D_EU5 | HDV_CB_D_EU5 | SOLO_LKW_D_EU5_I | HDV_RT_I_D_EU5 | SOLO_LKW_D_EU5_II | HDV_RT_II_D_EU5 |
| LSZ_D_EU6 | HDV_TT_D_EU6 | RB_D_EU6 | HDV_CO_D_EU6 | LB_D_EU6 | HDV_CB_D_EU6 | SOLO_LKW_D_EU6_I | HDV_RT_I_D_EU6 | SOLO_LKW_D_EU6_II | HDV_RT_II_D_EU6 |
| | | | | | HDV_CB_D_HEV_EU6 | | | | |

## Special Classes

The special class `zero` may be used to specify a vehicle without emissions.

## Further Resources

- [COLOMBO: Deliverable 4.2; Extended Simulation Tool PHEM coupled to
  SUMO with User
  Guide](https://web.archive.org/web/20170808122505/https://www.colombo-fp7.eu/results_deliverables.php); February,
  2014

 <div style="border:1px solid #909090; min-height: 35px;" align="right">
<span style="float: right; margin-top: -5px;"><a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm"><img src="../../images/FP7-small.gif" alt="Seventh Framework Programme"></a>
<a href="http://web.archive.org/web/20240722054138/https://verkehrsforschung.dlr.de/en/projects/colombo"><img src="../../images/COLOMBO-small.png" alt="COLOMBO project"></a></span>
<span style="">This part of SUMO was developed, reworked, or extended within the project
<a href="http://web.archive.org/web/20240722054138/https://verkehrsforschung.dlr.de/en/projects/colombo">"COLOMBO"</a>, co-funded by the European Commission within the <a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm">Seventh Framework Programme</a>.</span></div>
