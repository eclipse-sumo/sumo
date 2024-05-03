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

## Pedestrians and Two-Wheelers

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|pedestrian|<img src="images/Wikicommons_Pedestrian.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Pedestrian_crossing,_Omagh_-_geograph.org.uk_-_4485453.jpg" alt="pedestrian"/>|pedestrian<br><img src="images/pedestrian.PNG" width="80" alt="pedestrian"/>|0.215m<sup>(1)</sup><br>0.478m<sup>(1)</sup><br>1.719m<sup>(1)</sup>|70kg|0.25m|1.5m/s<sup>2(23)</sup>|2m/s<sup>2(23)</sup>|5m/s<sup>2</sup>|37.58km/h<sup>(41)</sup>|5km/h<sup>(23)</sup>|0|zero|0.1|
|bicycle|<img src="images/Wikicommons_bicycle.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:15-07-12-Ciclistas-en-Mexico-RalfR-N3S_8973.jpg" alt="bicycle" />|bicycle<br><img src="images/bicycle.PNG" width="80" alt="bicycle" />|1.6m<sup>(17)</sup><br>0.65m<sup>(17)</sup><br>1.7m<sup>(*)</sup>|10kg|0.5m|1.2m/s<sup>2(19)</sup>|3m/s<sup>2(19)</sup>|7m/s<sup>2</sup>|50km/h|20km/h<sup>(19)</sup>|1|zero|0.1|
|moped|<img src="images/Wikicommons_moped.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Simson_S51_B2-4_from_1980_original_condition_I.jpg" alt="Moped" />|moped<br><img src="images/moped.PNG" width="80" alt="Moped" />|2.1m<sup>(43)</sup><br>0.8m<sup>(43)</sup><br>1.7m<sup>(*)</sup>|80kg|2.5m|1.1m/s<sup>2(25)</sup>|7m/s<sup>2(26)</sup>|10m/s<sup>2</sup>|45km/h<sup>(24)</sup>|-|2|LDV_G_EU6|0.1|
|motorcycle|<img src="images/Wikicommons_motorcycle.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Yamaha_R1_2006.jpg" alt="Motorcycle" />|motorcycle<br><img src="images/motorcycle.PNG" width="80" alt="Motorcycle" />|2.2m<sup>(28)</sup><br>0.9m<sup>(28)</sup><br>1.5m<sup>(28)</sup>|200kg|2.5m|6m/s<sup>2(19)</sup>|10m/s<sup>2(27)</sup>|10m/s<sup>2</sup>|200km/h<sup>(28)</sup>|-|2|LDV_G_EU6|0.1|
|scooter|<img src="images/Wikicommons_escooter.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Scooter_el%C3%A9ctrico_-_A740010.jpg" alt="eScooter" />|scooter<br><img src="images/escooter.PNG" width="80" alt="eScooter" />|1.2m<sup>(\*)</sup><br>0.5m<sup>(\*)</sup><br>1.7m<sup>(\*)</sup>|10kg<sup>(\*)</sup>|0.5m<sup>(39)</sup>|1.2m/s<sup>2(39)</sup>|3m/s<sup>2(39)</sup>|7m/s<sup>2(\*)</sup>|25km/h<sup>(39)</sup>|20km/h|1|zero|0.1|

## Passenger Cars and Light Delivery

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|passenger|<img src="images/Wikicommons_passenger.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:2019_Toyota_Corolla_Icon_Tech_VVT-i_Hybrid_1.8.jpg" alt="Passenger Car" />|passenger<br><img src="images/passenger.PNG" width="80" alt="Passenger Car" />|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|PC_G_EU4|0.1|
|taxi|<img src="images/Wikicommons_taxi.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Taxis_at_EDDT-(jha).jpg" alt="Taxi" />|taxi<br><img src="images/taxi.PNG" width="80" alt="Taxi" />|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|PC_G_EU4|0.05|
|evehicle|<img src="images/Wikicommons_evehicle.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:2019_Tesla_Model_3,_Front_Left,_05-30-2021.jpg" alt="Evehicle" />|evehicle<br><img src="images/evehicle.PNG" width="80" alt="Evehicle" />|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|zero|0.1|
|emergency|<img src="images/Wikicommons_emergency.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:NSW_Ambulance_Australia.png" alt="Emergency" />|emergency<br><img src="images/emergency.PNG" width="80" alt="Emergency" />|6.5m<sup>(37)</sup><br>2.16m<sup>(37)</sup><br>2.86m<sup>(37)</sup>|5000kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|3|LDV|0|
|delivery|<img src="images/Wikicommons_delivery.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:2008_Dodge_Sprinter_DC.jpg" alt="Delivery" />|delivery<br><img src="images/delivery.PNG" width="80" alt="Delivery" />|6.5m<sup>(37)</sup><br>2.16m<sup>(37)</sup><br>2.86m<sup>(37)</sup>|5000kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|3|LDV|0.05|

The `evehicle` has been added in a time when electric vehicles still seemed futuristic and is kept for backward compatibility.
The intended use is for any kind of new vehicle concept which should be visually distinguishable and is of unknown emission status (hence zero emissions).

### Recommended subclass settings

All passenger cars share the defaults listed above, the values below are just ideas what alternatives might be appropriate for different models:

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `person`<br>`Capacity` |
|--|--|--|--|--|
|passenger|<img src="images/Wikicommons_passenger_sedan.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Peugeot_301_(2012)_IMG_8110.jpg" alt="Sedan" />|passenger/sedan<br><img src="images/passenger-sedan.PNG" width="80" alt="Sedan" />||5|
|passenger|<img src="images/Wikicommons_passenger_hatchback.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:%C5%A0koda_Felicia_%26_%C5%A0koda_Felicia_Combi.JPG" alt="Hatchback" />|passenger/hatchback<br><img src="images/passenger-hatch.PNG" width="80" alt="Hatchback" />||5|
|passenger|<img src="images/Wikicommons_passenger_wagon.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:2014_Mitsubishi_Outlander_(ZJ_MY14.5)_ES_4WD_wagon_(2015-05-29)_02.jpg" alt="Passenger Wagon" />|passenger/wagon<br><img src="images/passenger-wagon.PNG" width="80" alt="Passenger Wagon" />||5|
|passenger|<img src="images/Wikicommons_passenger_van.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Nissan_Largo_1993.jpg" alt="Passenger Van" />|passenger/van<br><img src="images/passenger-van.PNG" width="80" alt="Passenger Van" />|4.7m<sup>(16)</sup><br>1.9m<sup>(16)</sup><br>1.73m<sup>(16)</sup>|6|

## Trucks and Busses

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|truck|<img src="images/Wikicommons_transport.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:MAN_M2000_Pritschenwagen.jpg" alt="Truck" />|truck<br><img src="images/truck.PNG" width="80" alt="Truck" />|7.1m<sup>(30)</sup><br>2.4m<sup>(30)</sup><br>2.4m<sup>(30)</sup>|12000kg|2.5m|1.3m/s<sup>2(31)</sup>|4m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|130km/h<sup>(35)</sup>|-|3|HDV|0.05|
|trailer|<img src="images/Wikicommons_transport_trailer.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:R%2BL_Carriers_truck_with_tandem_trailer,_Ypsilanti,_Michigan.jpg" alt="Trailer" />|truck/trailer<br><img src="images/truck-trailer.PNG" width="80" alt="Trailer" />|16.5m<sup>(13)</sup><br>2.55m<sup>(13)</sup><br>4m<sup>(13)</sup>|15000kg|2.5m|1m/s<sup>2(31)</sup>|4m/s<sup>2</sup>|7m/s<sup>2</sup>|130km/h<sup>(35)</sup>|-|3|HDV|0.05|
|bus|<img src="images/Wikicommons_bus_city.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:EVAG_O530_3413_Holthuser_Tal.jpg" alt="Citybus" />|bus<br><img src="images/bus.PNG" width="80" alt="Citybus" />|12m<sup>(3)</sup><br>2.5m<sup>(3)</sup><br>3.4m<sup>(3)</sup>|7500kg|2.5m|1.2m/s<sup>2(18)</sup>|4m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|85km/h<sup>(3)</sup>|-|85|Bus|0.1|
|coach|<img src="images/Wikicommons_overland.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Baltic_Shuttle_Bus_in_Estonia.jpg" alt="Coach" />|bus/coach<br><img src="images/bus-coach.PNG" width="80" alt="Coach" />|14m<sup>(9)</sup><br>2.6m<sup>(9)</sup><br>4.m<sup>(9)</sup>|12000kg|2.5m|2.0m/s<sup>2(*)</sup>|see above|see above|100km/h<sup>(32)</sup>|-|70<sup>(9)</sup>|Coach|0.05

### Recommended subclass settings

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `minGap` | `accel` | `person`<br>`Capacity` |
|--|--|--|--|--|--|--|
|trailer|<img src="images/Wikicommons_transport_semitrailer.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Target_Store_Tractor_Trailer_Delivery_Truck._Target_Logo,_Target_Stores,_Target_Sign,_pics_by_Mike_Mozart_of_TheToyChannel_and_JeepersMedia_on_YouTube._-Target_-TargetStore_-TargetLogo_-TargetSign_-TargetTractorTrailer_-TargetStoreTruck.jpg" alt="Semitrailer" />|truck/semitrailer<br><img src="images/truck-semitrailer.PNG" width="80" alt="Semitrailer" />|16.5m<sup>(14)</sup><br>max. 2.55m<sup>(13)</sup><br>max. 4m<sup>(13)</sup>|2.5m|1.1m/s<sup>2(31)</sup>|3|
|bus|<img src="images/Wikicommons_bus_flexible.jpg" width="180" titel="https://commons.wikimedia.org/wiki/File:200E_busz_(RVY-715).jpg" alt="Flexible Bus" />|bus/flexible<br><img src="images/bus-flexible.PNG" width="80" alt="Flexible Bus" />|17.9m<sup>(3)</sup><br>2.5m<sup>(3)</sup><br>3.0m<sup>(3)</sup>|2.5m|1.2m/s<sup>2(18)</sup>|\~150|

## Rail

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|tram|<img src="images/Wikicommons_lightrail.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Berlin-bvg-sl-m10-gt6-12zrk-961796.jpg" alt="Tram" />|rail/railcar<br><img src="images/rail-railcar.PNG" width="80" alt="Tram" />|22m<sup>(7)</sup><br>2.4m<sup>(7)</sup><br>3.2m<sup>(7)</sup>|37900kg|2.5m|1.0m/s<sup>2(18)</sup>|3.0m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|80km/h<sup>(36)</sup>|-|120<sup>(7\*)</sup>|zero|0|
|rail_urban|<img src="images/Wikicommons_rail_city.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Bahnhof_Berlin-Wannsee,_S-Bahn-Bereitstellung.jpg" alt="S-Bahn" />|rail/railcar<br><img src="images/rail.PNG" width="80" alt="S-Bahn" />|3\*36.5m<sup>(4)</sup><br>3.0m<sup>(4)</sup><br>3.6m<sup>(4)</sup>|59000kg|5m|1.0m/s<sup>2(4)</sup>|3.0m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|100km/h<sup>(4)</sup>|-|3\*100|zero|0|
|rail|<img src="images/Wikicommons_rail_slow.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Arriva_Trains_Wales_train_in_Hereford_Station_-_geograph.org.uk_-_5709854.jpg" alt="Rail" />|rail<br><img src="images/rail.PNG" width="80" alt="Rail" />|2\*67.5m<sup>(11)</sup><br>2.84m<sup>(11)</sup><br>3.75m<sup>(11)</sup>|79500kg<sup>(42)</sup>|5m|0.25m/s<sup>2(34)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|160km/h<sup>(11)</sup>|-|434<sup>(11)</sup>|HDV_D_EU0|0|
|rail_electric|<img src="images/Wikicommons_rail_electric.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:426_027-9_Regio_DB_Wesel_-_Emmerich_(8661340238).jpg" alt="Rail" />|rail<br><img src="images/rail.PNG" width="80" alt="Rail" />|8*25m<sup>(12)</sup><br>2.95m<sup>(12)</sup><br>3.89m<sup>(12)</sup>|83000kg<sup>(42)</sup>|5m|0.5m/s<sup>2(33)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|220km/h<sup>(12)</sup>|-|425<sup>(12)</sup>|zero|0|
|rail_fast|<img src="images/Wikicommons_rail_fast.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Praest_DB_ICE_4610_ICE_123_Frankfurt_(51335112115).jpg" alt="ICE" />|rail<br><img src="images/rail.PNG" width="80" alt="ICE" />|8\*25m<sup>(12)</sup><br>2.95m<sup>(12)</sup><br>3.89m<sup>(12)</sup>|409000kg|5m|0.5m/s<sup>2(33)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|330km/h<sup>(12)</sup>|-|425<sup>(12)</sup>|zero|0|

### Recommended subclass settings

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `maxSpeed` | `person`<br>`Capacity` |
|--|--|--|--|--|--|
|rail|<img src="images/Wikicommons_rail_cargo.jpg" width="180" title="https://commons.wikimedia.org/wiki/File:Rail_Cargo_Hungaria_Taurus_loc.jpg" alt="Cargo Rail" />|rail/cargo<br><img src="images/rail-cargo.PNG" width="80" alt="Cargo Rail" />|\~750m (19m + x\*16m)<sup>(5, 6, 8)</sup><br>\~4.4m<sup>(6)</sup><br>\~3.0m<sup>(6)</sup>|120km/h<sup>(5)</sup>|1|

## Ships

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|ship|<img src="images/Wikicommons_ship.jpeg" width="180" title="https://commons.wikimedia.org/wiki/File:DANA_2004_ubt.jpeg" alt="Ship" />|ship<br><img src="images/ship.PNG" width="80" alt="Ship" />|17m<br>4m<br>4m|100000kg|2.5|0.1m/s<sup>2</sup>|0.15m/s<sup>2</sup>|1m/s<sup>2</sup>|4.12 (8 Knots)|-|4|HDV_D_EU0|0.1|

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
- (26): <https://www.colliseum.net/wiki/Bremsversuche_mit_dem_Kleinkraftrad_S_50_/_S_51>
- (27): <https://web.archive.org/web/20120622045505/https://www.unfallaufnahme.info/uebersichten-listen-und-tabellen/geschwindigkeiten-und-bremswege/index.html>
- (28): <https://de.wikipedia.org/wiki/BMW_R_1200_GS_K25>
- (29): <https://www.autoscout24.de/auto/technische-daten/mercedes-benz/vito/vito-111-cdi-kompakt-2003-2014-transporter-diesel/>
- (30): <https://www.car-plus.de/news/17-7-5-tonner-mieten#:~:text=Unser%207%2C5%2DTonner,-Sie%20mieten%20bei&text=Der%20Laderaum%20ist%206%2C10,mit%20einer%20Maul%2D%20oder%20Kugelkopfkupplung.>
- (31): <https://www.colliseum.net/wiki/Lkw-Anfahrbeschleunigungswerte_f%C3%BCr_die_Praxis>
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
- (\*): estimated
