---
title: Purgatory/TrafficOnline
permalink: /Purgatory/TrafficOnline/
---

!!! missing
    This text is currently under work

# GSM-based Surveillance

TrafficOnline (1) is a traffic surveillance system based on tracking GSM
cellular phones located in vehicles. This system was developed by a
consortium within the "Verkehrsmanagement 2010" initiative. One of
TrafficOnline’s sub-topics dealt with a simulation-based evaluation of
the system. A simulation was used because it allows modeling and
reproduction of certain traffic situations at well-defined time and
space. It was decided to use SUMO for this purpose due to its
availability as open source what allows to extend it easily.

Below, it is described what steps have been done to perform the
simulation. The telephony model originally implemented in SUMO was
removed from the code after a while.

## How Vehicles can be tracked using GSM

GSM-telephony takes place within a network of stationary cells and each
"connected", meaning currently used, cellular phone is assigned to a
"serving cell", mostly the cell with the best connection to the phone.
Additionally, a list of the next best serving cells is hold within the
phone. Evaluations done during the TrafficOnline project have shown that
one can find distinct combinations of these list’s entries at certain
areal positions when the quality of each connection between a cellular
phone and a cell is taken into account. These additional points which
allow determining a cellular phone’s position will be called
*TOL-points* in the following.

Especially in the case of a moving call, so-called "handovers" of calls
between two cells get necessary when the call moves from one serving
cell to another one. Handovers do not take place for unused phones. Used
and unused phones are assigned to a "location area" (LA), a structure
made up of several GSM-cells. Due to the larger size of location areas
in comparison to cells, handovers of unused cellular phones are more
seldom than those of used ones.

Handovers at cell- and LA-boundaries are tracked by the GSM-system.
Additionally, it is possible for the system to obtain and store the list
of best serving cells of each cellular phone. All this information
allows tracking a moving, cellular phone through the network at certain
positions, made up of the cell and la-boundaries and of TOL-points. The
density of these positions is larger for used phones, where not only
crossing of LA-boundaries can be observed by the system. When projected
on the road network, one can compute traveling times for a cellular
phone between the positions knowing their distances. Additional
information about the GSM-network and how the TOL-systems works can be
found in (2, 3).

## Usage of SUMO in TrafficOnline

Five areas located in Berlin, Germany, as shown in figure 1, were chosen
for which partners supplied information about road traffic amounts by
means of induction loop and infrared sensor data, and about the
utilization of the corresponding GSM network in the form of log-files
containing anonymous data of calls. The areas were chosen in order to
cover a large-most number of different road types and possible
disturbances. For these areas, the locations of the corresponding
TOL-points were prepared, encoded into geo-coordinates, and assigned to
the road sections.

### Traffic Simulation

The digital road networks for the specified areas were extracted from a
digital road map bought from NavTeq (6). Only those roads near to those
concerned by TOL-points were extracted for the simulation, other roads
were discarded. The resulting maps were converted into the SUMO-format
using a tool from the SUMO-package, [NETCONVERT](../NETCONVERT.md).
This tool automatically computes right-of-way rules at junctions and the
necessary connections between lanes across the junctions. Both are
missing within most digital networks. After this prior conversion, the
obtained SUMO-networks were compared with satellite photos from
GoogleEarth (7) in order to find mismatches in the number of lanes
between digital and real roads and to locate junctions equipped with
traffic lights. The original digital road data was adjusted in order to
eliminate the mismatches found using GoogleEarth. Afterwards the
conversion was redone. Finally, further work was done in order to
eliminate falsely computed connections between lanes and to make the
simulated traffic lights’ programs fit more to reality.

The demand of an average day for each of the areas was modeled using
point count data collected between June and August 2006 at weekdays from
Tuesday to Thursday. Where possible, within highway areas where
induction loops are located at all on- and off-ramps, vehicle routes
could be built automatically using a tool from the SUMO-package named
[DFROUTER](../DFROUTER.md). This tool imports count data,
determines whether a detector may be used as a source, sink, or none of
both for the given area, and calculates the probabilities for using
routes between each sink/source pair. In addition,
[DFROUTER](../DFROUTER.md) can compute vehicle flows which shall be
inserted at source positions in a format the simulation can read. In
those scenarios in which sensors were less dense in reality, the routes
and their distributions were built by hand. The traffic amount for
average traffic was implemented into the simulation using so-called
"emitters". In [SUMO](../SUMO.md), these simulation structures can
be placed at a certain position of a lane and can insert vehicles into
the network. Each vehicle is inserted at a defined time. In our case
these vehicles’ routes were assigned from the previously computed route
distributions. The vehicles’ types were assigned to the vehicles using a
vehicle type distribution. The flows used by the emitters were computed
using [DFROUTER](../DFROUTER.md) in all cases.

Where possible, count data which was not used for emitting vehicles was
used to validate the simulation. Figure 2 shows a set of comparisons
between flows from real life and the simulation across a real and a
simulated induction loop. A comparison of simulated and real speeds was
not possible, because they were not in the provided sensor data set.

### Simulation of Cellular Telephony

In order to keep the simulation fast, no attempt was done to determine
the correct serving cell of a device by using the vehicle’s position.
Instead, the cell boundaries of real-world cells, given as a set of
GIS-polygons, were laid over the simulated road network to get the
positions both cross. At the resulting intersections, so called
"cell-actors" were placed within the simulation. When a simulated
cellular phone passes such a "cell-actor", it is informed about having
entered a new cell and both the prior and the new serving cell are
informed about the phone’s cell change. If a simulated phone in
connected mode enters a cell or if a phone starts a call within a cell,
this cell increments one of the internal "moving call" counters – either
the one for incoming or the one for outgoing calls. Besides this, the
information about the duration each phone was within the cell is
computed, too. This information is used for calibrating the telephony
model, as shown later on. The applied approach assumes that stationary
calls are first of all made by slowly moving pedestrians or not moving
occupants of the area. Due to this assumption and the fact that it is
not yet possible to simulate single persons with SUMO, the stationary
calls are not generated by the simulation using a telephony model.
Instead, the amounts of stationary calls, again split into incoming and
outgoing calls, are once extracted from real-life data and later passed
to the simulation. This is done for each cell in intervals of 5 minutes.
These statistics are joined with the moving calls generated by the
simulation when writing the output. In combination with the number of
simulated in-vehicle devices this information represents the
"COLLECTORCS" output. The TOL-points are modeled similar to the cell
crossings. "TOL-actors" are placed on the road network at the positions
of the TOL-points. They save the call id of each cellular phone that
crosses them in “connected mode” together with the id of the ROL-point
and the time the vehicle has crossed it. This information is immediately
saved into the data table "COLLECTORPOS", by what this output need by
the TrafficOnline algorithm is completely implemented.

When starting their trips, the simulated in-vehicle cellular phones do
not know the cell they are located in because they have not yet passed a
"cell-actor". Their state is set to idle initially. As soon as a
simulated cellular phone is assigned to a cell, it starts asking the
simulation whether to switch into the connected mode, what is repeated
every step of the simulation. In order to get the probability to start a
call, a simulation of a normal day was performed at first for each of
the simulated areas. Within these simulation runs, every vehicle was
equipped with exactly one cellular phone device. The previously
mentioned durations a device was located in the area were summed up over
an interval of 30 minutes. This sum’s unit is vehicle-seconds per 30
minutes. Based upon this value and the mean number of moving calls
within the same period retrieved from real-life data, the probability to
start a call within a cell i has been computed for a given time interval
using the following formula:

```
                                                          (1)
```

where is the probability to start a call within cell i within the
interval, is the number of measured cellular calls within this cell and
interval, and is the duration vehicle j was within cell i within the
interval.

The information about the GSM traffic (amount of calls, changeovers
etc.) was not provided for every cell of all the simulation areas.
Furthermore, it was assumed that there are only minor differences
between the parameters of the telephony behavior regarding cells within
a small area. Thus, a common, time-dependent starting probability of a
call was calculated for each area. A generic probability for all areas
was neglected because of the infrastructural differences, amongst others
concerning the road types and the means of travel, between them. As the
information about the direction of the connections (incoming – outgoing)
is not yet used by the TOL algorithm, the percentage of moving calls was
set up to 50% for all simulation runs. As the results of analyses of the
real-life GSM data show, the percentage of incoming calls for moving
calls is about 45%. In addition to the probability for a cellular phone
to switch to connected mode, also the duration of a call had to be
modeled. The available real-life data contains the information about the
duration of each call, so a distribution function could be generated.
For the right part of the distribution, starting at duration of 60
seconds, the curve was assumed to be a log-normal distribution. The
parameters were obtained by fitting the data to the log-normal
distribution function concerning the respective range of values.
Consequently, the computation of the call durations was modeled using
the following equation:

```
                                                    (2)
```

where is the duration of a call in ms, and are uniformly distributed
random number in range (0, 1\] and is a uniformly distributed random
number in range \[0, 1\]. A comparison between the measured and the
modeled call duration frequencies is shown in figure 3. Calibrated with
the probability curves, validation simulations using normal day traffic
were performed. Figure 4 shows a comparison between the amounts of
moving calls for the days real data existed, their mean value, and the
simulation results. It is evident that the model is accurate concerning
the moving calls, though it fails within cells in which vehicles begin
their route, shown in the diagram at the bottom right of figure 4.

## The current Approach

As said, the original code used within TrafficOnline was removed from
SUMO. There were several reasons for this

- The original code used hard-coded values for telephony probability
- No further usage was avised after the project's end
- The code was bloated by the telephony "simulation"

The current approach is to use [TraCI](../TraCI.md) for simulating
GSM-based surveillance. We set up the scenarios as done in
TrafficOnline, but determining if a vehicle is crossing a cell or a LA
boundary is done in an external script which is also responsible for
simulating the telephony behaviour.