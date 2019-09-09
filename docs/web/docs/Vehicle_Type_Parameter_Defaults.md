---
title: Vehicle Type Parameter Defaults
permalink: /Vehicle_Type_Parameter_Defaults/
---

This page describes the vehicle parameter defaults depending on the
vehicle class. If no values are given the values for passenger are used.
It uses as default always the values in the first line for each vehicle
class. The other values are just as an information (to be used with own
vehicle types).

<table>
<thead>
<tr class="header">
<th><p>vClass (SVC)</p></th>
<th><p>example</p></th>
<th><p>shape (SVS)</p></th>
<th><p>length<br />
width<br />
height</p></th>
<th><p>minGap</p></th>
<th><p>a<sub>max</sub><sup>(**)</sup><br />
accel</p></th>
<th><p>b<sup>(**)</sup><br />
decel</p></th>
<th><p>b<sub>e</sub><br />
emergency decel</p></th>
<th><p>v<sub>max</sub><br />
maxSpeed</p></th>
<th><p>seats</p></th>
<th><p><a href="Models/Emissions/HBEFA3-based" title="wikilink">emissionClass (HBEFA3)</a></p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p>pedestrian</p></td>
<td><figure>
<img src="images/images/Wikicommons_pedestrian.jpg" title="wikicommons_pedestrian.jpg" width="100" alt="" /><figcaption>wikicommons_pedestrian.jpg</figcaption>
</figure></td>
<td><p>pedestrian</p></td>
<td><p>0.215m<sup>(1)</sup><br />
0.478m<sup>(1)</sup><br />
1.719m<sup>(1)</sup></p></td>
<td><p>0.25m</p></td>
<td><p>1.5m/s<sup>2(23)</sup></p></td>
<td><p>2m/s<sup>2(23)</sup></p></td>
<td><p>5m/s<sup>2</sup></p></td>
<td><p>5.4km/h<sup>(23)</sup></p></td>
<td><p>-</p></td>
<td><p>zero</p></td>
</tr>
<tr class="even">
<td><p><strong>two-wheeled vehicle</strong></p></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>bicycle</p></td>
<td><figure>
<img src="images/Wikicommons_bicycle.jpg" title="wikicommons_bicycle.jpg" width="100" alt="" /><figcaption>wikicommons_bicycle.jpg</figcaption>
</figure></td>
<td><p>bicycle</p></td>
<td><p>1.6m<sup>(17)</sup><br />
0.65m<sup>(17)</sup><br />
1.7m<sup>(*)</sup></p></td>
<td><p>0.5m</p></td>
<td><p>1.2m/s<sup>2(19)</sup></p></td>
<td><p>3m/s<sup>2(19)</sup></p></td>
<td><p>7m/s<sup>2</sup></p></td>
<td><p>20km/h<sup>(19)</sup></p></td>
<td><p>1</p></td>
<td><p>zero</p></td>
</tr>
<tr class="even">
<td><p>moped</p></td>
<td><figure>
<img src="images/Wikicommons_moped.jpg" title="wikicommons_moped.jpg" width="100" alt="" /><figcaption>wikicommons_moped.jpg</figcaption>
</figure></td>
<td><p>moped</p></td>
<td><p>2.1m<sup>(17)</sup><br />
0.8m<sup>(17)</sup><br />
1.7m<sup>(*)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1.1m/s<sup>2(25)</sup></p></td>
<td><p>7m/s<sup>2(26)</sup></p></td>
<td><p>10m/s<sup>2</sup></p></td>
<td><p>45km/h<sup>(24)</sup></p></td>
<td><p>2</p></td>
<td><p>zero</p></td>
</tr>
<tr class="odd">
<td><p>motorcycle</p></td>
<td><figure>
<img src="images/Wikicommons_motorcycle.jpg" title="wikicommons_motorcycle.jpg" width="100" alt="" /><figcaption>wikicommons_motorcycle.jpg</figcaption>
</figure></td>
<td><p>motorcycle</p></td>
<td><p>2.2m<sup>(28)</sup><br />
0.9m<sup>(28)</sup><br />
1.5m<sup>(28)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>6m/s<sup>2(19)</sup></p></td>
<td><p>10m/s<sup>2(27)</sup></p></td>
<td><p>10m/s<sup>2</sup></p></td>
<td><p>200km/h<sup>(28)</sup></p></td>
<td><p>2</p></td>
<td><p>LDV_G_EU6</p></td>
</tr>
<tr class="even">
<td><p><strong>passenger car</strong></p></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>passenger</p></td>
<td><figure>
<img src="images/Wikicommons_passenger.jpg" title="wikicommons_passenger.jpg" width="100" alt="" /><figcaption>wikicommons_passenger.jpg</figcaption>
</figure></td>
<td><p>passenger</p></td>
<td><p>4.3m<sup>(29)</sup><br />
1.8m<sup>(29)</sup><br />
1.5m<sup>(29)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>2.9m/s<sup>2(29)</sup></p></td>
<td><p>7.5m/s<sup>2(27)</sup></p></td>
<td><p>9m/s<sup>2</sup></p></td>
<td><p>180km/h<sup>(29)</sup></p></td>
<td><p>5</p></td>
<td><p>LDV_G_EU4</p></td>
</tr>
<tr class="even">
<td><figure>
<img src="images/Wikicommons_passenger_sedan.jpg" title="wikicommons_passenger_sedan.jpg" width="100" alt="" /><figcaption>wikicommons_passenger_sedan.jpg</figcaption>
</figure></td>
<td><p>passenger/sedan</p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>5</p></td>
<td><p>LDV_G_EU4</p></td>
<td></td>
</tr>
<tr class="odd">
<td><figure>
<img src="images/Wikicommons_passenger_hatchback.jpg" title="wikicommons_passenger_hatchback.jpg" width="100" alt="" /><figcaption>wikicommons_passenger_hatchback.jpg</figcaption>
</figure></td>
<td><p>passenger/hatchback</p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>5</p></td>
<td><p>LDV_G_EU4</p></td>
<td></td>
</tr>
<tr class="even">
<td><figure>
<img src="images/Wikicommons_passenger_wagon.jpg" title="wikicommons_passenger_wagon.jpg" width="100" alt="" /><figcaption>wikicommons_passenger_wagon.jpg</figcaption>
</figure></td>
<td><p>passenger/wagon</p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>5</p></td>
<td><p>LDV_G_EU4</p></td>
<td></td>
</tr>
<tr class="odd">
<td><figure>
<img src="images/Wikicommons_passenger_van.jpg" title="wikicommons_passenger_van.jpg" width="100" alt="" /><figcaption>wikicommons_passenger_van.jpg</figcaption>
</figure></td>
<td><p>passenger/van</p></td>
<td><p>4.7m<sup>(16)</sup><br />
1.9m<sup>(16)</sup><br />
1.73m<sup>(16)</sup></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>6</p></td>
<td><p>LDV_G_EU4</p></td>
<td></td>
</tr>
<tr class="even">
<td><p>emergency</p></td>
<td><figure>
<img src="images/Wikicommons_emergency.jpg" title="wikicommons_emergency.jpg" width="100" alt="" /><figcaption>wikicommons_emergency.jpg</figcaption>
</figure></td>
<td><p>delivery</p></td>
<td><p>6.5m<sup>(37)</sup><br />
2.16m<sup>(37)</sup><br />
2.86m<sup>(37)</sup></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>3</p></td>
<td><p>LDV</p></td>
</tr>
<tr class="odd">
<td><p>delivery</p></td>
<td><figure>
<img src="images/Wikicommons_delivery.jpg" title="wikicommons_delivery.jpg" width="100" alt="" /><figcaption>wikicommons_delivery.jpg</figcaption>
</figure></td>
<td><p>delivery</p></td>
<td><p>6.5m<sup>(37)</sup><br />
2.16m<sup>(37)</sup><br />
2.86m<sup>(37)</sup></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>3</p></td>
<td><p>LDV</p></td>
</tr>
<tr class="even">
<td><p><strong>truck</strong></p></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>truck</p></td>
<td><figure>
<img src="images/Wikicommons_transport.jpg" title="wikicommons_transport.jpg" width="100" alt="" /><figcaption>wikicommons_transport.jpg</figcaption>
</figure></td>
<td><p>truck</p></td>
<td><p>7.1m<sup>(30)</sup><br />
2.4m<sup>(30)</sup><br />
2.4m<sup>(30)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1.3m/s<sup>2(31)</sup></p></td>
<td><p>4m/s<sup>2(27)</sup></p></td>
<td><p>7m/s<sup>2</sup></p></td>
<td><p>130km/h<sup>(35)</sup></p></td>
<td><p>3</p></td>
<td><p>HDV</p></td>
</tr>
<tr class="even">
<td><p>trailer</p></td>
<td><figure>
<img src="images/Wikicommons_transport_semitrailer.jpg" title="wikicommons_transport_semitrailer.jpg" width="100" alt="" /><figcaption>wikicommons_transport_semitrailer.jpg</figcaption>
</figure></td>
<td><p>truck/semitrailer</p></td>
<td><p>16.5m<sup>(14)</sup><br />
max. 2.55m<sup>(13)</sup><br />
max. 4m<sup>(13)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1.1m/s<sup>2(31)</sup></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>3</p></td>
<td><p>HDV</p></td>
</tr>
<tr class="odd">
<td><figure>
<img src="images/Wikicommons_transport_trailer.jpg" title="wikicommons_transport_trailer.jpg" width="100" alt="" /><figcaption>wikicommons_transport_trailer.jpg</figcaption>
</figure></td>
<td><p>truck/trailer</p></td>
<td><p>max. 18.75m<sup>(13)</sup><br />
max. 2.55m<sup>(13)</sup><br />
max. 4m<sup>(13)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1m/s<sup>2(31)</sup></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>3</p></td>
<td><p>HDV</p></td>
<td></td>
</tr>
<tr class="even">
<td><p><strong>bus</strong></p></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>bus</p></td>
<td><figure>
<img src="images/Wikicommons_bus_city.jpg" title="wikicommons_bus_city.jpg" width="100" alt="" /><figcaption>wikicommons_bus_city.jpg</figcaption>
</figure></td>
<td><p>bus</p></td>
<td><p>12m<sup>(3)</sup><br />
2.5m<sup>(3)</sup><br />
3.4m<sup>(3)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1.2m/s<sup>2(18)</sup></p></td>
<td><p>4m/s<sup>2(27)</sup></p></td>
<td><p>7m/s<sup>2</sup></p></td>
<td><p>85km/h<sup>(3)</sup></p></td>
<td><p>~85</p></td>
<td><p>Bus</p></td>
</tr>
<tr class="even">
<td><figure>
<img src="images/Wikicommons_bus_flexible.jpg" title="wikicommons_bus_flexible.jpg" width="100" alt="" /><figcaption>wikicommons_bus_flexible.jpg</figcaption>
</figure></td>
<td><p>bus/flexible</p></td>
<td><p>17.9m<sup>(3)</sup><br />
2.5m<sup>(3)</sup><br />
3.0m<sup>(3)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1.2m/s<sup>2(18)</sup></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>~150</p></td>
<td><p>Bus</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>coach</p></td>
<td><figure>
<img src="images/Wikicommons_overland.jpg" title="wikicommons_overland.jpg" width="100" alt="" /><figcaption>wikicommons_overland.jpg</figcaption>
</figure></td>
<td><p>bus/coach</p></td>
<td><p>14m<sup>(10)</sup><br />
2.6m<sup>(10)</sup><br />
4.m<sup>(10)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>2.0m/s<sup>2(*)</sup></p></td>
<td><p><em>see above</em></p></td>
<td><p><em>see above</em></p></td>
<td><p>100km/h<sup>(32)</sup></p></td>
<td><p>70<sup>(9)</sup></p></td>
<td><p>Coach</p></td>
</tr>
<tr class="even">
<td><p><strong>rail</strong></p></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>tram</p></td>
<td><figure>
<img src="images/Wikicommons_lightrail.jpg" title="wikicommons_lightrail.jpg" width="100" alt="" /><figcaption>wikicommons_lightrail.jpg</figcaption>
</figure></td>
<td><p>rail/railcar</p></td>
<td><p>~22m<sup>(7)</sup><br />
~2.4m<sup>(7)</sup><br />
~3.2m<sup>(7)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1.0m/s<sup>2(18)</sup></p></td>
<td><p>3.0m/s<sup>2(27)</sup></p></td>
<td><p>7m/s<sup>2</sup></p></td>
<td><p>80km/h<sup>(36)</sup></p></td>
<td><p>~120<sup>(7, *)</sup></p></td>
<td><p>zero</p></td>
</tr>
<tr class="even">
<td><p>rail_urban</p></td>
<td><figure>
<img src="images/Wikicommons_rail_city.jpg" title="wikicommons_rail_city.jpg" width="100" alt="" /><figcaption>wikicommons_rail_city.jpg</figcaption>
</figure></td>
<td><p>rail/railcar</p></td>
<td><p>36.5m(*2/*4)<sup>(4)</sup><br />
3.0m<sup>(4)</sup><br />
3.6m<sup>(4)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>1.0m/s<sup>2(4)</sup></p></td>
<td><p>3.0m/s<sup>2(27)</sup></p></td>
<td><p>7m/s<sup>2</sup></p></td>
<td><p>100km/h<sup>(4)</sup></p></td>
<td><p>~300(*2/*4)</p></td>
<td><p>zero</p></td>
</tr>
<tr class="odd">
<td><p>rail</p></td>
<td><figure>
<img src="images/Wikicommons_rail_slow.jpg" title="wikicommons_rail_slow.jpg" width="100" alt="" /><figcaption>wikicommons_rail_slow.jpg</figcaption>
</figure></td>
<td><p>rail</p></td>
<td><p>67.5m(*2)<sup>(11)</sup><br />
2.84m<sup>(11)</sup><br />
3.75m<sup>(11)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>0.25m/s<sup>2(34)</sup></p></td>
<td><p>1.3m/s<sup>2(33)</sup></p></td>
<td><p>5m/s<sup>2</sup></p></td>
<td><p>160km/h<sup>(11)</sup></p></td>
<td><p>~434(*2)<sup>(11)</sup></p></td>
<td><p>HDV_D_EU0</p></td>
</tr>
<tr class="even">
<td><figure>
<img src="images/Wikicommons_rail_cargo.jpg" title="wikicommons_rail_cargo.jpg" width="100" alt="" /><figcaption>wikicommons_rail_cargo.jpg</figcaption>
</figure></td>
<td><p>rail/cargo</p></td>
<td><p>750m (~18.9m + x*~16m)<sup>(5, 6, 8)</sup><br />
~4.4m<sup>(6)</sup><br />
~3.0m<sup>(6)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>0.25m/s<sup>2(34)</sup></p></td>
<td><p>1.3m/s<sup>2(33)</sup></p></td>
<td><p>5m/s<sup>2</sup></p></td>
<td><p>120km/h<sup>(5)</sup></p></td>
<td><p>1</p></td>
<td><p>HDV_D_EU0</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>rail_electric</p></td>
<td><figure>
<img src="images/Wikicommons_rail_fast.jpg" title="wikicommons_rail_fast.jpg" width="100" alt="" /><figcaption>wikicommons_rail_fast.jpg</figcaption>
</figure></td>
<td><p>rail</p></td>
<td><p>~25m(*8)<sup>(12)</sup><br />
2.95m<sup>(12)</sup><br />
3.89m<sup>(12)</sup></p></td>
<td><p>2.5m</p></td>
<td><p>0.5m/s<sup>2(33)</sup></p></td>
<td><p>1.3m/s<sup>2(33)</sup></p></td>
<td><p>5m/s<sup>2</sup></p></td>
<td><p>330km/h<sup>(12)</sup></p></td>
<td><p>425<sup>(12)</sup></p></td>
<td><p>zero</p></td>
</tr>
<tr class="even">
<td><p><strong>electric vehicle</strong></p></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>evehicle</p></td>
<td></td>
<td><p>evehicle</p></td>
<td><p>see passenger</p></td>
<td><p>see passenger</p></td>
<td><p>see passenger</p></td>
<td><p>see passenger</p></td>
<td><p>see passenger</p></td>
<td><p>see passenger</p></td>
<td><p>see passenger</p></td>
<td><p>zero</p></td>
</tr>
<tr class="even">
<td><p><strong>ships</strong></p></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>ship</p></td>
<td></td>
<td><p>ship</p></td>
<td><p>17m<br />
4m<br />
4m</p></td>
<td><p>2.5</p></td>
<td><p>0.1m/s<sup>2</sup></p></td>
<td><p>0.1m/s<sup>2</sup></p></td>
<td><p>1m/s<sup>2</sup></p></td>
<td><p>4.12 (8 Knots)</p></td>
<td></td>
<td><p>HDV_D_EU0</p></td>
</tr>
</tbody>
</table>

- (1):
  <http://www.baua.de/de/Publikationen/AWE/Band3/AWE108.pdf?__blob=publicationFile&v=4>;
- (2): <http://www.motorrad-leuschner-d.de/motorraeder.html>
- (3): www.bvg.de/index.php/de/binaries/asset/download/470184/file/1-1
- (4): <http://de.wikipedia.org/wiki/S-Bahn_Berlin> und folgende
- (5): <http://de.wikipedia.org/wiki/Schieneng%C3%BCterverkehr>
- (6): <http://de.wikipedia.org/wiki/Bombardier_Traxx>
- (7): <http://de.wikipedia.org/wiki/GTxN/M/S>
- (8): <http://de.wikipedia.org/wiki/Flachwagen>
- (9): <http://de.wikipedia.org/wiki/Reisebus>
- (10):
  <http://www.neoplan-bus.com/cms/media/de/content/downloads/skyliner/NEOPLAN_SKYLINER.pdf>
- (11): <http://de.wikipedia.org/wiki/DBAG-Baureihe_425>
- (12): <http://de.wikipedia.org/wiki/ICE_3>
- (14):
  <http://www.mantruckandbus.com/man/media/migrated/doc/master_1/Transportloesungen__de_.pdf>
- (15): <http://de.wikipedia.org/wiki/Opel_Vivaro>
- (16): <http://de.wikipedia.org/wiki/Renault_Espace>
- (17): Fußgeher- und Fahrradverkehr: Planungsprinzipien; Hermann
  Knoflacher; Böhlau Verlag Wien, 1995
- (18): RiLSA 2010
- (19): <http://www.gutax.de/Diplomarbeit_Manfred_Bulla.pdf>
- (20):
  <http://tuprints.ulb.tu-darmstadt.de/905/1/Schroeder_Frank_FZD_Diss.pdf>
- (21): <http://www.unfallrekonstruktion.de/pdf/nickel.pdf>
- (22): <http://www.trb.org/publications/nchrp/nchrp_rpt_505.pdf>
  (p48)
- (23): <http://jml2012.indexcopernicus.com/fulltxt.php?ICID=1022800>
  (pdf p.229-230)
- (24): <http://de.wikipedia.org/wiki/Kleinkraftrad>
- (25): <http://de.wikipedia.org/wiki/Vespa_LX>
- (26):
  <http://www.colliseum.net/wiki/Bremsversuche_mit_dem_Kleinkraftrad_S_50_/_S_51>
- (27):
  <http://www.unfallaufnahme.info/uebersichten-listen-und-tabellen/geschwindigkeiten-und-bremswege/index.html>
- (28): <http://www.bmw-motorrad.de/>
- (29):
  <http://www.volkswagen.de/de/models/golf_7/trimlevel_overview.s9_trimlevel_detail.suffix.html/der-golf-cup~2Fcup.html#/tab=2cf06ac5c643bf892618c835c90c32ac>
- (30): <https://www.buchbinder.de/de/fahrzeuge.html>
- (31):
  <http://www.colliseum.net/wiki/Lkw-Anfahrbeschleunigungswerte_f%C3%BCr_die_Praxis>
- (32):
  <http://de.wikipedia.org/wiki/Zul%C3%A4ssige_H%C3%B6chstgeschwindigkeit_im_Stra%C3%9Fenverkehr_(Deutschland)>
- (33):
  <http://www.ice-fansite.com/index.php?option=com_content&view=article&id=195:ice1-techdat&catid=50:icet&Itemid=69>
- (34):
  <http://en.wikipedia.org/wiki/Orders_of_magnitude_(acceleration)>
- (35):
  <https://de.answers.yahoo.com/question/index?qid=20061122162708AAtzoPP>
- (36): <http://ka.stadtwiki.net/H%C3%B6chstgeschwindigkeit_(Bahn)>
- (37):
  <http://www.rettungsdienst.brk.de/technik/rtw/rtw_by_2010/daten>

- (\*): estimated
- (\*\*): need to be revisited to fit to model behaviour