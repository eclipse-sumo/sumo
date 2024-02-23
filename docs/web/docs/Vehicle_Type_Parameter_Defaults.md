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


## Pedestrians and Two-Wheelers

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|pedestrian|<img src="images/Wikicommons_pedestrian.jpg" width="180" alt="pedestrian"/>|pedestrian|0.215m<sup>(1)</sup><br>0.478m<sup>(1)</sup><br>1.719m<sup>(1)</sup>|70kg|0.25m|1.5m/s<sup>2(23)</sup>|2m/s<sup>2(23)</sup>|5m/s<sup>2</sup>|37.58km/h<sup>(41)</sup>|5km/h<sup>(23)</sup>|0|zero|0.1|
|bicycle|<img src="images/Wikicommons_bicycle.jpg" width="180" alt="bicycle" />|bicycle|1.6m<sup>(17)</sup><br>0.65m<sup>(17)</sup><br>1.7m<sup>(*)</sup>|10kg|0.5m|1.2m/s<sup>2(19)</sup>|3m/s<sup>2(19)</sup>|7m/s<sup>2</sup>|50km/h|20km/h<sup>(19)</sup>|1|zero|0.1|
|moped|<img src="images/Wikicommons_moped.jpg" width="180" title="Von I, Kadi-kadi, CC BY-SA 3.0, https://commons.wikimedia.org/w/index.php?curid=2427813" alt="Moped" />|moped|2.1m<sup>(17)</sup><br>0.8m<sup>(17)</sup><br>1.7m<sup>(*)</sup>|80kg|2.5m|1.1m/s<sup>2(25)</sup>|7m/s<sup>2(26)</sup>|10m/s<sup>2</sup>|45km/h<sup>(24)</sup>|-|2|LDV_G_EU6|0.1|
|motorcycle|<img src="images/Wikicommons_motorcycle.jpg" width="180" alt="Motorcycle" />|motorcycle|2.2m<sup>(28)</sup><br>0.9m<sup>(28)</sup><br>1.5m<sup>(28)</sup>|200kg|2.5m|6m/s<sup>2(19)</sup>|10m/s<sup>2(27)</sup>|10m/s<sup>2</sup>|200km/h<sup>(28)</sup>|-|2|LDV_G_EU6|0.1|

### Recommended subclass settings

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|bicycle|<img src="images/escooter.jpg" width="180" alt="eScooter" />|scooter|1.1m<sup>(\*)</sup><br>0.4m<sup>(\*)</sup><br>1.2m<sup>(\*)</sup>|14kg(\*)|1.5m<sup>(39)</sup>|2m/s<sup>2(39)</sup>|4m/s<sup>2(39)</sup>|7m/s<sup>(\*)</sup>|20 km/h<sup>(39)</sup>|20 km/h|1<sup>(\*)</sup>|zero|0.1|

## Passenger Cars and Light Delivery

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|passenger|<img src="images/Wikicommons_passenger.jpg" width="180" alt="Passenger Car" />|passenger|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|PC_G_EU4|0.1|
|taxi||taxi|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|PC_G_EU4|0.05|
|evehicle||evehicle|5<sup>(29)</sup><br>1.8m<sup>(29)</sup><br>1.5m<sup>(29)</sup>|1500kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|4|PC_G_EU4|0.1|
|emergency|<img src="images/Wikicommons_emergency.jpg" width="180" alt="Emergency" />|delivery|6.5m<sup>(37)</sup><br>2.16m<sup>(37)</sup><br>2.86m<sup>(37)</sup>|5000kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|3|LDV|0|
|delivery|<img src="images/Wikicommons_delivery.jpg" width="180" alt="Delivery" />|delivery|6.5m<sup>(37)</sup><br>2.16m<sup>(37)</sup><br>2.86m<sup>(37)</sup>|5000kg|2.5m|2.6m/s<sup>2(29)</sup>|4.5m/s<sup>2(27)</sup>|9m/s<sup>2</sup>|200km/h<sup>(29)</sup>|-|3|LDV|0.05|

The `evehicle` has been added in a time when electric vehicles still seemed futuristic and is kept for backward compatibility.
The intended use is for any kind of new vehicle concept which should be visually distinguishable and is of unknown emission status (hence zero emissions).

### Recommended subclass settings

All passenger cars share the defaults listed above, the values below are just ideas what alternatives might be appropriate for different models:

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `person`<br>`Capacity` |
|--|--|--|--|--|
|passenger|<img src="images/Wikicommons_passenger_sedan.jpg" width="180" alt="Sedan" />|passenger/sedan||5|
|passenger|<img src="images/Wikicommons_passenger_hatchback.jpg" width="180" alt="Hatchback" />|passenger/hatchback||5|
|passenger|<img src="images/Wikicommons_passenger_wagon.jpg" width="180" alt="Passenger Wagon" />|passenger/wagon||5
|passenger|<img src="images/Wikicommons_passenger_van.jpg" width="180" alt="Passenger Van" />|passenger/van|4.7m<sup>(16)</sup><br>1.9m<sup>(16)</sup><br>1.73m<sup>(16)</sup>|6|

## Trucks and Busses

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|truck|<img src="images/Wikicommons_transport.jpg" width="180" alt="Truck" />|truck|7.1m<sup>(30)</sup><br>2.4m<sup>(30)</sup><br>2.4m<sup>(30)</sup>|12000kg|2.5m|1.3m/s<sup>2(31)</sup>|4m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|130km/h<sup>(35)</sup>|-|3|HDV|0.05|
|trailer|<img src="images/Wikicommons_transport_trailer.jpg" width="180" alt="Trailer" />|truck/trailer|16.5m<sup>(13)</sup><br>2.55m<sup>(13)</sup><br>4m<sup>(13)</sup>|15000kg|2.5m|1m/s<sup>2(31)</sup>|4m/s<sup>2</sup>|7m/s<sup>2</sup>|130km/h<sup>(35)</sup>|-|3|HDV|0.05|
|bus|<img src="images/Wikicommons_bus_city.jpg" width="180" alt="Citybus" />|bus|12m<sup>(3)</sup><br>2.5m<sup>(3)</sup><br>3.4m<sup>(3)</sup>|7500kg|2.5m|1.2m/s<sup>2(18)</sup>|4m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|85km/h<sup>(3)</sup>|-|85|Bus|0.1|
|coach|<img src="images/Wikicommons_overland.jpg" width="180" alt="Coach" />|bus/coach|14m<sup>(10)</sup><br>2.6m<sup>(10)</sup><br>4.m<sup>(10)</sup>|12000kg|2.5m|2.0m/s<sup>2(*)</sup>|see above|see above|100km/h<sup>(32)</sup>|-|70<sup>(9)</sup>|Coach|0.05

### Recommended subclass settings

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `minGap` | `accel` | `person`<br>`Capacity` |
|--|--|--|--|--|--|--|
|trailer|<img src="images/Wikicommons_transport_semitrailer.jpg" width="180" alt="Semitrailer" />|truck/semitrailer|16.5m<sup>(14)</sup><br>max. 2.55m<sup>(13)</sup><br>max. 4m<sup>(13)</sup>|2.5m|1.1m/s<sup>2(31)</sup>|3|
|bus|<img src="images/Wikicommons_bus_flexible.jpg" width="180" alt="Flexible Bus" />|bus/flexible|17.9m<sup>(3)</sup><br>2.5m<sup>(3)</sup><br>3.0m<sup>(3)</sup>|2.5m|1.2m/s<sup>2(18)</sup>|\~150|

## Rail

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|tram|<img src="images/Wikicommons_lightrail.jpg" width="180" alt="Tram" />|rail/railcar|22m<sup>(7)</sup><br>2.4m<sup>(7)</sup><br>3.2m<sup>(7)</sup>|37900kg|2.5m|1.0m/s<sup>2(18)</sup>|3.0m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|80km/h<sup>(36)</sup>|-|120<sup>(7\*)</sup>|zero|0|
|rail_urban|<img src="images/Wikicommons_rail_city.jpg" width="180" alt="S-Bahn" />|rail/railcar|3\*36.5m<sup>(4)</sup><br>3.0m<sup>(4)</sup><br>3.6m<sup>(4)</sup>|59000kg|5m|1.0m/s<sup>2(4)</sup>|3.0m/s<sup>2(27)</sup>|7m/s<sup>2</sup>|100km/h<sup>(4)</sup>|-|3\*100|zero|0|
|rail|<img src="images/Wikicommons_rail_slow.jpg" width="180" alt="Rail" />|rail|2\*67.5m<sup>(11)</sup><br>2.84m<sup>(11)</sup><br>3.75m<sup>(11)</sup>|79500kg<sup>(42)</sup>|5m|0.25m/s<sup>2(34)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|160km/h<sup>(11)</sup>|-|434<sup>(11)</sup>|HDV_D_EU0|0|
|rail_electric||rail|8*25m<sup>(12)</sup><br>2.95m<sup>(12)</sup><br>3.89m<sup>(12)</sup>|83000kg<sup>(42)</sup>|5m|0.5m/s<sup>2(33)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|220km/h<sup>(12)</sup>|-|425<sup>(12)</sup>|zero|0|
|rail_fast|<img src="images/Wikicommons_rail_fast.jpg" width="180" alt="ICE" />|rail|8\*25m<sup>(12)</sup><br>2.95m<sup>(12)</sup><br>3.89m<sup>(12)</sup>|409000kg|5m|0.5m/s<sup>2(33)</sup>|1.3m/s<sup>2(33)</sup>|5m/s<sup>2</sup>|330km/h<sup>(12)</sup>|-|425<sup>(12)</sup>|zero|0|

### Recommended subclass settings

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `maxSpeed` | `person`<br>`Capacity` |
|--|--|--|--|--|--|
|rail|<img src="images/Wikicommons_rail_cargo.jpg" width="180" alt="Cargo Rail" />|rail/cargo|\~750m (19m + x\*16m)<sup>(5, 6, 8)</sup><br>\~4.4m<sup>(6)</sup><br>\~3.0m<sup>(6)</sup>|120km/h<sup>(5)</sup>|1|

## Ships

| `vClass` | example | `guiShape` | `length`<br>`width`<br>`height` | `mass` | `minGap` | `accel` | `decel` | `emer`<br>`gency`<br>`Decel` | `maxSpeed` | `desired`<br>`MaxSpeed` | `person`<br>`Capacity` | `emission`<br>`Class`<br>[HBEFA3](Models/Emissions/HBEFA3-based.md) | `speed`<br>`Dev` |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|ship||ship|17m<br>4m<br>4m|100000kg|2.5|0.1m/s<sup>2</sup>|0.15m/s<sup>2</sup>|1m/s<sup>2</sup>|4.12 (8 Knots)|-|4|HDV_D_EU0|0.1|

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
- (10): <https://www.neoplan-bus.com/cms/media/de/content/downloads/skyliner/NEOPLAN_SKYLINER.pdf>
- (11): <https://de.wikipedia.org/wiki/DBAG-Baureihe_425>
- (12): <https://de.wikipedia.org/wiki/ICE_3>
- (14): `https://www.truck.man.eu/man/media/de/content_medien/doc/business_website_truck_master_1/einsatzgebiete/de_2/man-lkw-tg-fernverkehr-volumen.pdf` (dead link)
- (15): <https://de.wikipedia.org/wiki/Opel_Vivaro>
- (16): <https://de.wikipedia.org/wiki/Renault_Espace>
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
- (29): <https://web.archive.org/web/20140227232934/https://www.volkswagen.de/de/models/golf_7/trimlevel_overview.s9_trimlevel_detail.suffix.html/der-golf-cup~2Fcup.html>
- (30): <https://web.archive.org/web/20160320031009/https://www.buchbinder.de/de/fahrzeuge.html>
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
- (\*): estimated
