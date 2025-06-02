---
title: Vehicle Type Parameter Defaults
---

This page describes the vehicle parameter defaults depending on the
vehicle class. If no values are given the values for passenger are used.
It uses as default always the values in the first line for each vehicle
class. The other values (in the subsections) are just an information (to be used with own
vehicle types).

!!! note
    The correct setting may be dependent on the car-following model in use. This is especially true for `accel` and `decel` which
    usually do *not* denote the maximum values achievable by the vehicle but rather the convenient values. If you use a different model than Krauss you should probably revisit the values.

Please note that `personCapacity` and `mass` do usually not include a driver in the tables below.
(So a bicycle has a capacity of 1 and this does not mean the children's seat but the driver; and a mass of 10kg
obviously does not include the driver.) The only exception here is the default passenger car which has
a capacity of 4 while the average car out there has probably rather 5 places. But using all 5 places is
probably rather exceptional so the historic default has been kept and it is maybe the better value for taxi scenarios as well.
Please also note that the mass in the handbook
of a private car usually *includes* the average driver's weight.

Whether the exclusion is correct or not also depends on the way you set up the simulation. If persons are
modelled explicitly it is right, if not, you probably do not care about personCapacity and variable mass anyway,
but if you do please revisit your vType settings.

### On default emission classes
Note that with the transition to [HBEFA4](Models/Emissions/HBEFA4-based.md) there are no more aggregated emission classes like `HDV` available.
That means the default emission class for each vehicle class is usually the most frequent vehicle in the 2022 fleet composition as estimated in the HBEFA. This can vastly understimate real emissions which may be dominated by older vehicles, so if precision is important you are encouraged to use a distribution of vehicle types fitting your modelled vehicle fleet. It is also possible to go back to the [HBEFA3](Models/Emissions/HBEFA3-based.md) classes.

Please also note that the default emission class for all electric trains is `zero` and all other trains (and ships) still use the dirtiest diesel emissions of the HBEFA3 model. You are encouraged to model your train (and ship) explicitly if consumption is of interest. This includes adapting parameters like the mass (which means the empty vehicle mass) and loading.

## Pedestrians and Two-Wheelers

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class` | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|pedestrian|<img src="images/Wikicommons_Pedestrian.jpg" width="180" alt="pedestrian"/><br><small>[(C) Pedestrian crossing, Omagh by Kenneth Allen](https://commons.wikimedia.org/wiki/File:Pedestrian_crossing,_Omagh_-_geograph.org.uk_-_4485453.jpg)<br>[CC-BY-SA-2.0](https://creativecommons.org/licenses/by-sa/2.0/deed.en)</small>|pedestrian<br><img src="images/pedestrian.PNG" width="80" alt="pedestrian"/>|0.215m<sup>(1)</sup><br>0.478m<sup>(1)</sup><br>1.719m<sup>(1)</sup>|70kg|0.25m|1.5m/s<sup>2(23)</sup>|2m/s<sup>2(23)</sup>|5m/s<sup>2</sup>|37.58km/h<sup>(41)</sup>|5km/h<sup>(23)</sup>|0|zero|0.1|
|bicycle|<img src="images/Wikicommons_bicycle.jpg" width="180" alt="bicycle"/><br><small>[(C) Daniele Leonzio](https://commons.wikimedia.org/wiki/File:Bicycle,_Milan.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|bicycle<br><img src="images/bicycle.PNG" width="80" alt="bicycle" />|1.6m<sup>(17)</sup><br>0.65m<sup>(17)</sup><br>1.7m<sup>(*)</sup>|10kg|0.5m|1.2m/s<sup>2(19)</sup>|3m/s<sup>2(19)</sup>|7m/s<sup>2</sup>|50km/h|20km/h<sup>(19)</sup>|1|zero|0.1|
|moped|<img src="images/Wikicommons_moped.jpg" width="180" alt="Moped" /><br><small>[(C) Max schwalbe](https://commons.wikimedia.org/wiki/File:Simson_S51_B2-4_from_1980_original_condition_I.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|moped<br><img src="images/moped.PNG" width="80" alt="Moped" />|2.1m<sup>(43)</sup><br>0.8m<sup>(43)</sup><br>1.7m<sup>(*)</sup>|80kg|2.5m|1.1m/s<sup>2(25)</sup>|7m/s<sup>2(26)</sup>|10m/s<sup>2</sup>|45km/h<sup>(24)</sup>|-|2|[HBEFA4/<br>Moped_le50cc_Euro-2](Models/Emissions/HBEFA4-based.md#motor_cycles)|0.1|
|motorcycle|<img src="images/Wikicommons_motorcycle.jpg" width="180" alt="Motorcycle" /><br><small>[(C) Thomas Wiersma](https://commons.wikimedia.org/wiki/File:Yamaha_R1_2006.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|motorcycle<br><img src="images/motorcycle.PNG" width="80" alt="Motorcycle" />|2.2m<sup>(28)</sup><br>0.9m<sup>(28)</sup><br>1.5m<sup>(28)</sup>|200kg|2.5m|6m/s<sup>2(19)</sup>|10m/s<sup>2(27)</sup>|10m/s<sup>2</sup>|200km/h<sup>(28)</sup>|-|2|[HBEFA4/<br>MC_4S_gt250cc_preEuro](Models/Emissions/HBEFA4-based.md#motor_cycles)|0.1|
|scooter|<img src="images/Wikicommons_escooter.jpg" width="180" alt="eScooter" /><br><small>[(C) Rjcastillo](https://commons.wikimedia.org/wiki/File:Scooter_el%C3%A9ctrico_-_A740010.jpg)<br>[CC-BY-4.0](https://creativecommons.org/licenses/by/4.0/deed.en)</small>|scooter<br><img src="images/escooter.PNG" width="80" alt="eScooter" />|1.2m<sup>(\*)</sup><br>0.5m<sup>(\*)</sup><br>1.7m<sup>(\*)</sup>|10kg<sup>(\*)</sup>|0.5m<sup>(39)</sup>|1.2m/s<sup>2(39)</sup>|3m/s<sup>2(39)</sup>|7m/s<sup>2(\*)</sup>|25km/h<sup>(39)</sup>|20km/h|1|zero|0.1|

## Passenger Cars and Light Delivery

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class` | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|passenger|<img src="images/Wikicommons_passenger.jpg" width="180" alt="Passenger Car" /><br><small>[(C) Vauxford](https://commons.wikimedia.org/wiki/File:2019_Toyota_Corolla_Icon_Tech_VVT-i_Hybrid_1.8.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|passenger<br><img src="images/passenger.PNG" width="80" alt="Passenger Car" />|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|[HBEFA4/<br>PC_petrol_Euro-4](Models/Emissions/HBEFA4-based.md#passenger_cars)|0.1|
|taxi|<img src="images/Wikicommons_taxi.jpg" width="180" alt="Taxi" /><br><small>[(C) Matti Blume](https://commons.wikimedia.org/wiki/File:Taxis_at_EDDT-(jha).jpg)<br>[CC-BY-SA-2.0](https://creativecommons.org/licenses/by-sa/2.0/deed.en)</small>|taxi<br><img src="images/taxi.PNG" width="80" alt="Taxi" />|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|[HBEFA4/<br>PC_petrol_Euro-4](Models/Emissions/HBEFA4-based.md#passenger_cars)|0.05|
|evehicle|<img src="images/Wikicommons_evehicle.jpg" width="180" alt="Evehicle" /><br><small>[(C) Spsmiler](https://commons.wikimedia.org/wiki/File:GATEway-Podcar-NthGreenwich-London-S2620005.jpg)<br>[CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/deed.en)</small>|evehicle<br><img src="images/evehicle.PNG" width="80" alt="Evehicle" />|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|zero|0.1|
|emergency|<img src="images/Wikicommons_emergency.jpg" width="180" alt="Emergency" /><br><small>[(C) Helitak430](https://commons.wikimedia.org/wiki/File:NSW_Ambulance_Australia.png)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|emergency<br><img src="images/emergency.PNG" width="80" alt="Emergency" />|6.5m<sup>(37)</sup><br>2.16m<sup>(37)</sup><br>2.86m<sup>(37)</sup>|5000kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|3|[HBEFA4/<br>LCV_diesel_N1-III_Euro-6ab](Models/Emissions/HBEFA4-based.md#light_vehicles)|0|
|delivery|<img src="images/Wikicommons_delivery.jpg" width="180" alt="Delivery" /><br><small>[(C) IFCAR](https://commons.wikimedia.org/wiki/File:2008_Dodge_Sprinter_DC.jpg)<br>[Public Domain](https://en.wikipedia.org/wiki/en:public_domain)</small>|delivery<br><img src="images/delivery.PNG" width="80" alt="Delivery" />|6.5m<sup>(37)</sup><br>2.16m<sup>(37)</sup><br>2.86m<sup>(37)</sup>|5000kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|3|[HBEFA4/<br>LCV_diesel_N1-III_Euro-6ab](Models/Emissions/HBEFA4-based.md#light_vehicles)|0.05|

The `evehicle` has been added in a time when electric vehicles still seemed futuristic and is kept for backward compatibility.
The intended use is for any kind of new vehicle concept which should be visually distinguishable and is of unknown emission status (hence zero emissions).

### Recommended subclass settings

All passenger cars share the defaults listed above, the values below are just ideas what alternatives might be appropriate for different models:

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `person`<br>`Capacity` |
|--|--|--|--|--|
|passenger|<img src="images/Wikicommons_passenger_sedan.jpg" width="180" title="" alt="Sedan" /><br><small>[(C) Alexander-93](https://commons.wikimedia.org/wiki/File:Peugeot_301_(2012)_IMG_8110.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|passenger/sedan<br><img src="images/passenger-sedan.PNG" width="80" alt="Sedan" />||5|
|passenger|<img src="images/Wikicommons_passenger_hatchback.jpg" width="180" title="" alt="Hatchback" /><br><small>[(C) Ladislav "Fredy.00" Šafránek](https://commons.wikimedia.org/wiki/File:%C5%A0koda_Felicia_%26_%C5%A0koda_Felicia_Combi.JPG)<br>[Public Domain](https://en.wikipedia.org/wiki/en:public_domain)</small>|passenger/hatchback<br><img src="images/passenger-hatch.PNG" width="80" alt="Hatchback" />||5|
|passenger|<img src="images/Wikicommons_passenger_wagon.jpg" width="180" title="" alt="Passenger Wagon" /><br><small>[(C) OSX](https://commons.wikimedia.org/wiki/File:2014_Mitsubishi_Outlander_(ZJ_MY14.5)_ES_4WD_wagon_(2015-05-29)_02.jpg)<br>[Public Domain](https://en.wikipedia.org/wiki/en:public_domain)</small>|passenger/wagon<br><img src="images/passenger-wagon.PNG" width="80" alt="Passenger Wagon" />||5|
|passenger|<img src="images/Wikicommons_passenger_van.jpg" width="180" title="" alt="Passenger Van" /><br><small>[(C) Kuha455405](https://commons.wikimedia.org/wiki/File:Nissan_Largo_1993.jpg)<br>[CC-BY-SA-3.0](https://creativecommons.org/licenses/by-sa/3.0/deed.en)</small>|passenger/van<br><img src="images/passenger-van.PNG" width="80" alt="Passenger Van" />|4.7m<sup>(16)</sup><br>1.9m<sup>(16)</sup><br>1.73m<sup>(16)</sup>|6|

## Trucks and Busses

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class` | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|truck|<img src="images/Wikicommons_transport.jpg" width="180" alt="Truck" /><br><small>[(C) C-C-Baxter](https://commons.wikimedia.org/wiki/File:MAN_M2000_Pritschenwagen.jpg)<br>[CC-BY-SA-3.0](https://creativecommons.org/licenses/by-sa/3.0/deed.en)</small>|truck<br><img src="images/truck.PNG" width="80" alt="Truck" />|7.1m<sup>(30)</sup><br>2.4m<sup>(30)</sup><br>2.4m<sup>(30)</sup>|4500kg|2.5m|1.3m/s<sup>2(31)</sup>|4m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|130km/h<sup>(35)</sup>|-|3|[HBEFA4/<br>RT_le7.5t_Euro-VI_A-C](Models/Emissions/HBEFA4-based.md#heavy_goods_vehicles)|0.05|
|trailer|<img src="images/Wikicommons_transport_trailer.jpg" width="180" alt="Trailer" /><br><small>[(C) Dwight Burdette](https://commons.wikimedia.org/wiki/File:R%2BL_Carriers_truck_with_tandem_trailer,_Ypsilanti,_Michigan.jpg)<br>[CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/deed.en)</small>|truck/trailer<br><img src="images/truck-trailer.PNG" width="80" alt="Trailer" />|16.5m<sup>(13)</sup><br>2.55m<sup>(13)</sup><br>4m<sup>(13)</sup>|13000kg|2.5m|1m/s<sup>2(31)</sup>|4m/s<sup>2</sup>|7m/s<sup>2</sup>|130km/h<sup>(35)</sup>|-|3|[HBEFA4/<br>TT_AT_gt34-40t_Euro-VI_A-C](Models/Emissions/HBEFA4-based.md#heavy_goods_vehicles)|0.05|
|bus|<img src="images/Wikicommons_bus_city.jpg" width="180" alt="Citybus" /><br><small>[(C) Iwouldstay](https://commons.wikimedia.org/wiki/File:EVAG_O530_3413_Holthuser_Tal.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|bus<br><img src="images/bus.PNG" width="80" alt="Citybus" />|12m<sup>(3)</sup><br>2.5m<sup>(3)</sup><br>3.4m<sup>(3)</sup>|12000kg|2.5m|1.2m/s<sup>2(18)</sup>|4m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|85km/h<sup>(3)</sup>|-|85|[HBEFA4/<br>UBus_Std_gt15-18t_Euro-VI_A-C](Models/Emissions/HBEFA4-based.md#city_bus)|0.1|
|coach|<img src="images/Wikicommons_overland.jpg" width="180" alt="Coach" /><br><small>[(C) Leshe](https://commons.wikimedia.org/wiki/File:Baltic_Shuttle_Bus_in_Estonia.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|bus/coach<br><img src="images/bus-coach.PNG" width="80" alt="Coach" />|14m<sup>(9)</sup><br>2.6m<sup>(9)</sup><br>4.m<sup>(9)</sup>|25000kg|2.5m|2.0m/s<sup>2(*)</sup>|see above|see above|100km/h<sup>(32)</sup>|-|70<sup>(9)</sup>|[HBEFA4/<br>Coach_3-Axes_gt18t_Euro-VI_A-C](Models/Emissions/HBEFA4-based.md#coaches)|0.05

Please note that according to the HBEFA city busses with three axes are more frequent than the shorter ones but in order to keep the other values like length and mass consistent and have a stronger separation from the coaches it has been decided to use the emission class of a smaller bus.

### Recommended subclass settings

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `minGap` | `accel` | `person`<br>`Capacity` |
|--|--|--|--|--|--|--|
|trailer|<img src="images/Wikicommons_transport_semitrailer.jpg" width="180" alt="Semitrailer" /><br><small>[(C) Mike Mozart](https://commons.wikimedia.org/wiki/File:Target_Store_Tractor_Trailer_Delivery_Truck._Target_Logo,_Target_Stores,_Target_Sign,_pics_by_Mike_Mozart_of_TheToyChannel_and_JeepersMedia_on_YouTube._-Target_-TargetStore_-TargetLogo_-TargetSign_-TargetTractorTrailer_-TargetStoreTruck.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|truck/semitrailer<br><img src="images/truck-semitrailer.PNG" width="80" alt="Semitrailer" />|16.5m<sup>(14)</sup><br>max. 2.55m<sup>(13)</sup><br>max. 4m<sup>(13)</sup>|2.5m|1.1m/s<sup>2(31)</sup>|3|
|bus|<img src="images/Wikicommons_bus_flexible.jpg" width="180" alt="Flexible Bus" /><br><small>[(C) Kemenymate](https://commons.wikimedia.org/wiki/File:200E_busz_(RVY-715).jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|bus/flexible<br><img src="images/bus-flexible.PNG" width="80" alt="Flexible Bus" />|17.9m<sup>(3)</sup><br>2.5m<sup>(3)</sup><br>3.0m<sup>(3)</sup>|2.5m|1.2m/s<sup>2(18)</sup>|\~150|

## Rail

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class` | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|tram|<img src="images/Wikicommons_lightrail.jpg" width="180" title="" alt="Tram" /><br><small>[(C) Kurt Rasmussen, custom license](https://commons.wikimedia.org/wiki/File:Berlin-bvg-sl-m10-gt6-12zrk-961796.jpg)</small>|rail/railcar<br><img src="images/rail-railcar.PNG" width="80" alt="Tram" />|22m<sup>(7)</sup><br>2.4m<sup>(7)</sup><br>3.2m<sup>(7)</sup>|37900kg|2.5m|1.0m/s<sup>2(18)</sup>|3.0m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|80km/h<sup>(36)</sup>|-|120<sup>(7\*)</sup>|zero|0|
|rail_urban|<img src="images/Wikicommons_rail_city.jpg" width="180" title="" alt="S-Bahn" /><br><small>[(C) Andreas Lippold](https://commons.wikimedia.org/wiki/File:Bahnhof_Berlin-Wannsee,_S-Bahn-Bereitstellung.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|rail/railcar<br><img src="images/rail.PNG" width="80" alt="S-Bahn" />|3\*36.5m<sup>(4)</sup><br>3.0m<sup>(4)</sup><br>3.6m<sup>(4)</sup>|59000kg|5m|1.0m/s<sup>2(4)</sup>|3.0m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|100km/h<sup>(4)</sup>|-|3\*100|zero|0|
|rail|<img src="images/Wikicommons_rail_slow.jpg" width="180" title="" alt="Rail" /><br><small>[(C) Philip Halling](https://commons.wikimedia.org/wiki/File:Arriva_Trains_Wales_train_in_Hereford_Station_-_geograph.org.uk_-_5709854.jpg)<br>[CC-BY-SA-2.0](https://creativecommons.org/licenses/by-sa/2.0/deed.en)</small>|rail<br><img src="images/rail.PNG" width="80" alt="Rail" />|2\*67.5m<sup>(11)</sup><br>2.84m<sup>(11)</sup><br>3.75m<sup>(11)</sup>|79500kg<sup>(42)</sup>|5m|0.25m/s<sup>2(34)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|160km/h<sup>(11)</sup>|-|434<sup>(11)</sup>|[HBEFA3/<br>HDV_D_EU0](Models/Emissions/HBEFA3-based.md#hbefa3_heavy_duty_emission_classes)|0|
|rail_electric|<img src="images/Wikicommons_rail_electric.jpg" width="180" title="" alt="Rail" /><br><small>[(C) Rob Dammers](https://commons.wikimedia.org/wiki/File:426_027-9_Regio_DB_Wesel_-_Emmerich_(8661340238).jpg)<br>[CC-BY-2.0](https://creativecommons.org/licenses/by/2.0/deed.en)</small>|rail<br><img src="images/rail.PNG" width="80" alt="Rail" />|8*25m<sup>(12)</sup><br>2.95m<sup>(12)</sup><br>3.89m<sup>(12)</sup>|83000kg<sup>(42)</sup>|5m|0.5m/s<sup>2(33)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|220km/h<sup>(12)</sup>|-|425<sup>(12)</sup>|zero|0|
|rail_fast|<img src="images/Wikicommons_rail_fast.jpg" width="180" title="" alt="ICE" /><br><small>[(C) Rob Dammers](https://commons.wikimedia.org/wiki/File:Praest_DB_ICE_4610_ICE_123_Frankfurt_(51335112115).jpg)<br>[CC-BY-2.0](https://creativecommons.org/licenses/by/2.0/deed.en)</small>|rail<br><img src="images/rail.PNG" width="80" alt="ICE" />|8\*25m<sup>(12)</sup><br>2.95m<sup>(12)</sup><br>3.89m<sup>(12)</sup>|409000kg|5m|0.5m/s<sup>2(33)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|330km/h<sup>(12)</sup>|-|425<sup>(12)</sup>|zero|0|

### Recommended subclass settings

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `maxSpeed` | `person`<br>`Capacity` |
|--|--|--|--|--|--|
|rail|<img src="images/Wikicommons_rail_cargo.jpg" width="180" title="" alt="Cargo Rail" /><br><small>[(C) Railcargo](https://commons.wikimedia.org/wiki/File:Rail_Cargo_Hungaria_Taurus_loc.jpg)<br>[CC-BY-SA-3.0](https://creativecommons.org/licenses/by-sa/3.0/deed.en)</small>|rail/cargo<br><img src="images/rail-cargo.PNG" width="80" alt="Cargo Rail" />|\~750m (19m + x\*16m)<sup>(5, 6, 8)</sup><br>\~4.4m<sup>(6)</sup><br>\~3.0m<sup>(6)</sup>|120km/h<sup>(5)</sup>|1|

## Other

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class` | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|ship|<img src="images/Wikicommons_ship.jpeg" width="180" title="" alt="Ship" /><br><small>[(C) Tomasz Sienicki](https://commons.wikimedia.org/wiki/File:DANA_2004_ubt.jpeg)<br>[CC-BY-SA-3.0](https://creativecommons.org/licenses/by-sa/3.0/deed.en)</small>|ship<br><img src="images/ship.PNG" width="80" alt="Ship" />|17m<br>4m<br>4m|100000kg|2.5|0.1m/s<sup>2</sup>|0.15m/s<sup>2</sup>|1m/s<sup>2</sup>|4.12 (8 Knots)|-|4|[HBEFA3/<br>HDV_D_EU0](Models/Emissions/HBEFA3-based.md#hbefa3_heavy_duty_emission_classes)|0.1|
|subway|<img src="images/Wikicommons_subway.jpg" width="180" title="" alt="Subway" /><br><small>[(C) Pudelek (Marcin Szala)](https://commons.wikimedia.org/wiki/File:Berlin_Hauptbahnhof_U-bahn_-_train_type_F.jpg)<br>[CC-BY-SA-3.0](https://creativecommons.org/licenses/by-sa/3.0/deed.en)</small>|subway<br><img src="images/subway.PNG" width="80" alt="Subway" />|98.7m<sup>(44)</sup><br>2.65m<sup>(44)</sup><br>3.4m<sup>(44)</sup>|141400kg<sup>(44)</sup>|-|-|-|-|80km/h<sup>(45)</sup>|-|-|-|-|
|aircraft|<img src="images/Wikicommons_aircraft.jpg" width="180" title="" alt="Aircraft" /><br><small>[(C) Julian Herzog](https://commons.wikimedia.org/wiki/File:Airbus_A350-941_F-WWCF_MSN002_ILA_Berlin_2016_17.jpg)<br>[CC-BY-4.0](https://creativecommons.org/licenses/by/4.0/deed.en)</small>|aircraft<br><img src="images/aircraft.PNG" width="80" alt="Aircraft" />|70.51m<sup>(46)</sup><br>60m<sup>(46)</sup><br>19.33m<sup>(46)</sup>|162500kg<sup>(46)</sup>|-|-|-|-|969km/h<sup>(46)</sup>|-|425<sup>(46)</sup>|-|-|
|container|<img src="images/Wikicommons_container.jpg" width="180" title="" alt="Container" /><br><small>[(C) IPLManagement](https://commons.wikimedia.org/wiki/File:40_Foot_High_Cube_Shipping_Container_Depot.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|container|6.058m<sup>(47)</sup><br>2.43m<sup>(47)</sup><br>2.59m<sup>(47)</sup>|2.33t<sup>(47)</sup>|-|-|-|-|-|-|-|-|-|
|drone|<img src="images/Wikicommons_drone.jpg" width="180" title="" alt="Drone" /><br><small>[(C) Zarate123](https://commons.wikimedia.org/wiki/File:Unmanned_aerial_vehicle_aerial_photography_30.jpg)<br>[CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)</small>|drone<br>|294mm<sup>(48)</sup><br>373mm<sup>(48)</sup><br>101mm<sup>(48)</sup>|249g<sup>(48)</sup>|-|-|-|-|12m/s<sup>(48)</sup>|-|-|-|-|
|wheelchair|<img src="images/Wikicommons_wheelchair.png" width="180" title="" alt="Wheelchair" /><br><small>[(C) Laboratoires Servier](https://commons.wikimedia.org/wiki/File:Orthopedics_3_--_Smart-Servier.png)<br>[CC-BY-SA-3.0](https://creativecommons.org/licenses/by-sa/3.0/deed.en)</small>|wheelchair|103cm<sup>(49)</sup><br>62cm<sup>(49)</sup><br>90.5cm<sup>(49)</sup>|18.4kg<sup>(49)</sup>|-|-|-|-|-|-|-|-|-|
|cable_car|<img src="images/Wikicommons_cable_car.jpg" width="180" title="" alt="Cable_car" /><br><small>[(C) Wilfredo](https://commons.wikimedia.org/wiki/File:Telef%C3%A9rico_de_Caracas_1.jpg)<br>[CC-BY-SA-3.0](https://creativecommons.org/licenses/by-sa/3.0/deed.en)</small>|cable_car|-|-|-|-|-|-|12,5m/s<sup>(50)</sup>|-|-|-|-|

## Footnotes

- (1): <https://www.baua.de/DE/Angebote/Publikationen/AWE/AWE108.pdf?__blob=publicationFile>
- (2): <https://web.archive.org/web/20100531025842/https://www.motorrad-leuschner-d.de/motorraeder.html>
- (3): <https://web.archive.org/web/20110313224137/www.bvg.de/index.php/de/binaries/asset/download/470184/file/1-1>
- (4): <https://de.wikipedia.org/wiki/S-Bahn_Berlin>
- (5): <https://de.wikipedia.org/wiki/Schieneng%C3%BCterverkehr>
- (6): <https://de.wikipedia.org/wiki/Bombardier_Traxx>
- (7): <https://de.wikipedia.org/wiki/GTxN/M/S>
- (8): <https://de.wikipedia.org/wiki/Flachwagen>
- (9): <https://de.wikipedia.org/wiki/Reisebus>
- (11): <https://de.wikipedia.org/wiki/DBAG-Baureihe_425>
- (12): <https://de.wikipedia.org/wiki/ICE_3>
- (14): `https://www.truck.man.eu/man/media/de/content_medien/doc/business_website_truck_master_1/einsatzgebiete/de_2/man-lkw-tg-fernverkehr-volumen.pdf` (dead link)
- (15): <https://de.wikipedia.org/wiki/Opel_Vivaro>
- (16): <https://de.wikipedia.org/wiki/Renault_Espace_II>
- (17): Fußgeher- und Fahrradverkehr: Planungsprinzipien; Hermann Knoflacher; Böhlau Verlag Wien, 1995
- (18): RiLSA 2010
- (19): <https://www.colliseum.eu/wiki/images/b/bf/Geschwindigkeiten%2C_Verz%C3%B6gerungen_und_Beschleunigungen_nicht_motorisierter%2C_ungesch%C3%BCtzter_Verkehrsteilnehmer_am_Beispiel_Fahrrad_und_Inlineskates.pdf>
- (20): <https://tuprints.ulb.tu-darmstadt.de/905/1/Schroeder_Frank_FZD_Diss.pdf>
- (21): <https://www.unfallrekonstruktion.de/pdf/nickel.pdf>
- (22): <https://www.trb.org/publications/nchrp/nchrp_rpt_505.pdf> (p48)
- (23): `https://jml2012.indexcopernicus.com/fulltxt.php?ICID=1022800` (dead link) (pdf p.229-230)
- (24): <https://de.wikipedia.org/wiki/Kleinkraftrad>
- (25): <https://de.wikipedia.org/wiki/Vespa_LX>
- (26): <https://www.colliseum.eu/wiki/Bremsversuche_mit_dem_Kleinkraftrad_S_50_/_S_51>
- (27): <https://web.archive.org/web/20120622045505/https://www.unfallaufnahme.info/uebersichten-listen-und-tabellen/geschwindigkeiten-und-bremswege/index.html>
- (28): <https://de.wikipedia.org/wiki/BMW_R_1200_GS_K25>
- (29): <https://www.autoscout24.de/auto/technische-daten/mercedes-benz/vito/vito-111-cdi-kompakt-2003-2006-transporter-diesel/>
- (30): <https://www.car-plus.de/news/17-7-5-tonner-mieten#:~:text=Unser%207%2C5%2DTonner,-Sie%20mieten%20bei&text=Der%20Laderaum%20ist%206%2C10,mit%20einer%20Maul%2D%20oder%20Kugelkopfkupplung.>
- (31): <https://www.colliseum.eu/wiki/Lkw-Anfahrbeschleunigungswerte_f%C3%BCr_die_Praxis>
- (32): <https://de.wikipedia.org/wiki/Zul%C3%A4ssige_H%C3%B6chstgeschwindigkeit_im_Stra%C3%9Fenverkehr_(Deutschland)>
- (33): `https://www.ice-fansite.com/index.php?option=com_content&view=article&id=195:ice1-techdat&catid=50:icet&Itemid=69` (dead link)
- (34): <https://en.wikipedia.org/wiki/Orders_of_magnitude_(acceleration)>
- (35): https://www.researchgate.net/publication/311633504_Improving_Road_Safety_of_Tank_Truck_in_Indonesia_by_Speed_Limiter_Installation
- (36): <https://ka.stadtwiki.net/H%C3%B6chstgeschwindigkeit_(Bahn)>
- (37): <https://www.rettungsdienst.brk.de/leichte-sprache/abteilung/beschaffung/rettungswagen/rtw-by-2010.html>
- (39): <https://electric-scooter.guide/comparisons/electric-scooter-performance-tests/>
- (40): <https://www.chip.de/artikel/E-Scooter-Test-Die-besten-Elektro-Scooter-mit-Strassenzulassung_165587317.html>
- (41): Average speed on the 100m man's world record
- (42): Locomotive only
- (43): <https://de.wikipedia.org/wiki/Puch_MS_50>
- (44): <http://berliner-u-bahn.info/br_u.php>
- (45): <https://de.wikipedia.org/wiki/BVG-Baureihe_H>
- (46): <http://www.flugzeuginfo.net/acdata_php/acdata_7471_dt.php>
- (47): <https://de.wikipedia.org/wiki/ISO-Container>
- (48): <https://www.dji.com/de/mini-4-pro/specs>
- (49): <https://www.sanitaets-online.de/mobilitaet/rollstuehle/standard-rollstuhl/1153/bischoff-bischoff-rollstuhl-s-eco-300-sb-43>
- (50): <https://de.wikipedia.org/wiki/Pendelbahn#Geschwindigkeit_und_F%C3%B6rderleistung>
- (\*): estimated
