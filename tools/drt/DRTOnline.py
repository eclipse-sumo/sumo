#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    DRTOnline.py
# @author  Giuliana Armellini
# @author  Pablo Alvarez
# @author  Philip Ritzer
# @date    2020-02-15

"""
Simulate Demand Responsive Transport via TraCi
Track progress https://github.com/eclipse/sumo/issues/8256
"""

import os
import sys
from argparse import ArgumentParser

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci

def initOptions():
    argParser = ArgumentParser()
    
    argParser.add_argument("-n", "--network", dest="network", help="SUMO network file", metavar="FILE", required=True)
    argParser.add_argument("-r", "--reservations", dest="reservations", help="File with reservations (persons)", metavar="FILE", required=True)
    argParser.add_argument("-v", "--taxi", dest="taxis", help="File with drt vehicles (vehicles with taxi device)", metavar="FILE", required=True)
    argParser.add_argument("-g", dest="gui_settings", help="Load visualisation settings from FILE", metavar="FILE", required=False)
    argParser.add_argument("-o", dest="output", help="Name of output file", default='tripinfo.xml', required=False)

    argParser.add_argument("--c_ko", dest="c_ko", help="cost of ignoring a request", type=float, default=1000000000000, required=False)
    argParser.add_argument("--cost_per_trip", dest="cost_per_trip", help="avoid using multiple vehicles if trip time is similar", type=float, default=600, required=False)

    argParser.add_argument("--debug", help="get all info steps", action='store_true')
    
    argParser.add_argument("--drf", dest="drf", help="direct route factor", type=int, default=2, required=False)
    argParser.add_argument("--drf_min", dest="drf_min", help="minimum time for cases with short trips", type=int, default=600, required=False)
    argParser.add_argument("--max_wait", dest="max_wait", help="maximum waiting time for pickup", type=int, default=900, required=False)
    argParser.add_argument("--max_diff", dest="max_diff", help="maximum difference with assigned time", type=int, default=300, required=False)
    argParser.add_argument("--veh_wait", dest="veh_wait", help="maximum waiting time for passenger in the vehicle", type=int, default=180, required=False)
    argParser.add_argument("--sim_step", dest="sim_step", help="time window for request collection", type=int, default=30, required=False)

    return argParser

def rr_pair(req1, req2, v_type, rv_dict):
    r1_pu0, r1_pu1 = req1.tw_pickup
    r1_do0, r1_do1 = req1.tw_dropoff
    r2_pu0, r2_pu1 = req2.tw_pickup
    r2_do0, r2_do1 = req2.tw_dropoff

    # combination 1p2p 2p2d 2d1d
    if r1_pu0 <= r2_pu1 and r2_do0 <= r1_do1:
    # if earliest pick up of req 1 before latest pick up of req 2 and
    # if earliest drop off of req 2 before latest drop off of req 1
        tt_1p2p = int(traci.simulation.findRoute(req1.fromEdge, req2.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        tt_2p2d = rv_dict.get('%sy_%sz' % (req2.id, req2.id), False)
        if not tt_2p2d:
            tt_2p2d = req2.direct + 60 # TODO default stop time
            pair = '%sy_%sz' % (req2.id, req2.id) # 2p2d
            rv_dict[pair] = [tt_2p2d, -1, [req2.id, req2.id]]
        else:
            tt_2p2d = tt_2p2d[0]
        tt_2d1d = int(traci.simulation.findRoute(req1.toEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time

        if (r1_pu0 + tt_1p2p) > r2_pu1:
            pass # not possible
        elif (r1_pu0 + tt_1p2p + tt_2p2d) > r2_do1:
            pass # not possible
        elif (r1_pu0 + tt_1p2p + tt_2p2d + tt_2d1d) > r1_do1:
            pass # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (req1.id, req2.id) # 1p2p
            rv_dict[pair] = [tt_1p2p, 1, [req1.id, req2.id]] 
            pair = '%sz_%sz' % (req2.id, req1.id) # 2d1d
            rv_dict[pair] = [tt_2d1d, -1, [req2.id, req1.id]] 

    # combination 1p2p 2p1d 1d2d
    if r1_pu0 <= r2_pu1 and r1_do0 <= r2_do1:
    # if earliest pick up of req 1 before latest pick up of req 2 and
    # if earliest drop off of req 1 before latest drop off of req 2
        tt_1p2p = rv_dict.get('%sy_%sy' % (req1.id, req2.id), False)
        if not tt_1p2p:
            tt_1p2p = int(traci.simulation.findRoute(req1.fromEdge, req2.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        else:
            tt_1p2p = tt_1p2p[0]
        tt_2p1d = int(traci.simulation.findRoute(req2.fromEdge, req1.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        tt_1d2d = int(traci.simulation.findRoute(req1.toEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time

        if (r1_pu0 + tt_1p2p) > r2_pu1:
            pass # not possible 
        elif (r1_pu0 + tt_1p2p + tt_2p1d) > r1_do1:
            pass # not possible
        elif (r1_pu0 + tt_1p2p + tt_2p1d + tt_1d2d) > r2_do1:
            pass # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (req1.id, req2.id) # 1p2p
            if not rv_dict.get(pair, False):
                rv_dict[pair] = [tt_1p2p, 1, [req1.id, req2.id]]
            pair = '%sy_%sz' % (req2.id, req1.id) # 2p1d
            rv_dict[pair] = [tt_2p1d, -1, [req2.id, req1.id]]                
            pair = '%sz_%sz' % (req1.id, req2.id) # 1d2d
            rv_dict[pair] = [tt_1d2d, -1, [req1.id, req2.id]]

    # combination 2p1p 1p2d 2d1d
    if r2_pu0 <= r1_pu1 and r2_do0 <= r1_do1:
    # if earliest pick up of req 2 before latest pick up of req 1 and
    # if earliest drop off of req 2 before latest drop off of req 1
        tt_2p1p = int(traci.simulation.findRoute(req2.fromEdge, req1.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        tt_1p2d = int(traci.simulation.findRoute(req1.fromEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        tt_2d1d = rv_dict.get('%sz_%sz' % (req2.id, req1.id), False)
        if not tt_2d1d:
            tt_2d1d = int(traci.simulation.findRoute(req2.toEdge, req1.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        else:
            tt_2d1d = tt_2d1d[0]

        if (r2_pu0 + tt_2p1p) > r1_pu1:
            pass # not possible
        elif (r2_pu0 + tt_2p1p + tt_1p2d) > r2_do1:
            pass # not possible
        elif (r2_pu0 + tt_2p1p + tt_1p2d + tt_2d1d) > r1_do1:
            pass # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (req2.id, req1.id) # 2p1p
            rv_dict[pair] = [tt_2p1p, 1, [req2.id, req1.id]]
            pair = '%sy_%sz' % (req1.id, req2.id) # 1p2d
            rv_dict[pair] = [tt_1p2d, -1, [req1.id, req2.id]]
            pair = '%sz_%sz' % (req1.id, req2.id) # 2d1d
            if not rv_dict.get(pair, False):
                rv_dict[pair] = [tt_2d1d, -1, [req2.id, req1.id]]

    # combination 2p1p 1p1d 1d2d
    if r2_pu0 <= r1_pu1 and r1_do0 <= r2_do1:
    # if earliest pick up of req 2 before latest pick up of req 1 and
    # if earliest drop off of req 1 before latest drop off of req 2
        tt_2p1p = rv_dict.get('%sy_%sy' % (req2.id, req1.id), False)
        tt_1d2d = rv_dict.get('%sz_%sz' % (req1.id, req2.id), False)
        tt_1p1d = rv_dict.get('%sy_%sz' % (req1.id, req1.id), False)
        if not tt_1p1d:
            tt_1p1d = req1.direct + 60 # TODO default stop time
            pair = '%sy_%sz' % (req1.id, req1.id) # 1p1d
            rv_dict[pair] = [tt_1p1d, -1, [req1.id, req1.id]]
        else:
            tt_1p1d = tt_1p1d[0]
        if not tt_2p1p or not tt_1d2d:
            if not tt_2p1p:
                tt_2p1p = int(traci.simulation.findRoute(req2.fromEdge, req1.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
            else:
                tt_2p1p = tt_2p1p[0]
            if not tt_1d2d:
                tt_1d2d = int(traci.simulation.findRoute(req1.toEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
            else:
                tt_1d2d = tt_1d2d[0]
            tt_1p1d = rv_dict.get('%sy_%sz' % (req1.id, req1.id))[0]

            if (r2_pu0 + tt_2p1p) > r1_pu1:
                pass # not possible
            elif (r2_pu0 + tt_2p1p + tt_1p1d) > r1_do1:
                pass # not possible
            elif (r2_pu0 + tt_2p1p + tt_1p1d + tt_1d2d) > r2_do1:
                pass # not possible
            else:
                # pairs are possible
                pair = '%sy_%sy' % (req2.id, req1.id) # 2p1p
                if not rv_dict.get(pair, False):
                    rv_dict[pair] = [tt_2p1p, 1, [req2.id, req1.id]]
                pair = '%sz_%sz' % (req1.id, req2.id) # 1d2d
                if not rv_dict.get(pair, False):
                    rv_dict[pair] = [tt_1d2d, -1, [req1.id, req2.id]]

    # pair 1d2p
    if r1_do0 <= r2_pu1:
    # if earliest drop off of req 1 before latest pick up of req 2
        tt_1d2p = int(traci.simulation.findRoute(req1.toEdge, req2.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        if (r1_do0 + tt_1d2p) < r2_pu1:
        # if droping off req 1 at earliest time, req 2 can be pick up at least at latest time, then pair possible
            pair = '%sz_%sy' % (req1.id, req2.id) # 1d2p
            rv_dict[pair] = [tt_1d2p, 1, [req1.id, req2.id]]

    # pair 2d1p
    if r2_do0 <= r1_pu1:
    # if earliest drop off of req 2 before latest pick up of req 1
        tt_2d1p = int(traci.simulation.findRoute(req2.toEdge, req1.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time
        if (r2_do0 + tt_2d1p) < r1_pu1:
        # if droping off req 1 at earliest time, req 2 can be pick up at least at latest time, then pair possible
            pair = '%sz_%sy' % (req2.id, req1.id) # 2d1p
            rv_dict[pair] = [tt_2d1p, 1, [req2.id, req1.id]]

def get_rv(options, r_id_new, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step):
    # remove requests, that have been served in the last step - 
    # Avoid calculating unfeasible pairs
    for x_id in r_id_served:
        x = r_all[x_id]
        x.served = True
        for key in list(rv_dict):
            if x_id in rv_dict[key][2]:
                rv_dict.pop(key)

    r_remove = []
    pu_remove = []
    for x_id, x in r_all.items():

        if x.rejected or x.served:
            # if reservation has been served or rejected
            continue
        
        elif x_id in r_id_new:
            #if reservation is new
            # add direct route pair
            route_id = '%sy_%sz' % (x_id, x_id) # y: pickup / z: drop off
            rv_dict[route_id] = [x.direct+60, -1, [x_id, x_id]] # TODO default stop time

            # add vehicle-request pairs
            for v_id in fleet:
                # calculate travel time to pickup
                pickup_time = int(traci.simulation.findRoute(traci.vehicle.getRoadID(v_id), x.fromEdge, v_type, routingMode=0).travelTime)
                if step+pickup_time <= x.tw_pickup[1]:
                    # if vehicle on time, add to rv graph
                    route_id = '%s_%sy' % (v_id, x_id)
                    rv_dict[route_id] = [pickup_time+60, 1, [v_id, x_id]] # TODO default stop time

            # add request-request pairs only with:
            second_requests = list(set(r_all.keys()) ^ set(r_id_picked) ^ set(r_id_served))
            for req2 in second_requests:
                if req2 == x_id:
                    continue
                rr_pair(x, r_all.get(req2), v_type, rv_dict) # search possible pairs and add to rv
        
        elif not x.vehicle:
            # if reservation not assigned
            # check if pick-up still possible
            if x.tw_pickup[1] < step:
                # pickup time window surpass simulation time -> reject request
                # TODO persons stay in simulation, how can they be deleted?
                # TODO no rejection option should be implement in future
                x.rejected = True
                r_remove.append(x_id)
                print("reservation %s cannot be served" % x_id)
                continue
            
            remove = True
            for v_id in fleet:
                route_id = '%s_%sy' % (v_id, x_id)
                if rv_dict.get(route_id, False):
                    # if key not in rv, pair already infeasible
                    # calculate travel time to pickup
                    pickup_time = int(traci.simulation.findRoute(traci.vehicle.getRoadID(v_id), x.fromEdge, v_type, routingMode=0).travelTime)
                    if step+pickup_time <= x.tw_pickup[1]:
                        # if vehicle on time, add to rv graph
                        rv_dict[route_id][0] = pickup_time+60
                        remove = False
                    else:
                        # remove pair if pick-up not possible
                        rv_dict.pop(route_id)
            if remove:
                # if no vehicle available for pick-up on time
                x.rejected = True
                r_remove.append(x_id)
                print("reservation %s cannot be served" % x_id)

        elif x_id not in r_id_picked:
            # if reservation assigned but not picked up
            # TODO is this relevant?
            route_id = '%s_%sy' % (x.vehicle, x_id)
            if rv_dict.get(route_id, False):
                # calculate travel time to pickup
                pickup_time = int(traci.simulation.findRoute(traci.vehicle.getRoadID(v_id), x.fromEdge, v_type, routingMode=0).travelTime)               
                rv_dict[route_id][0] = pickup_time+60
                if step+pickup_time <= x.tw_pickup[1]:
                    print("time window should not be surpass")
        
        elif x_id in r_id_picked:
            # if reservation picked up, delete pair with stop
            pu_remove.append('%sy' % x_id)

        else:
            print("Attribute state not considered")

    # remove rejected, served and picked up reservations from rv graph
    for key in list(rv_dict):
        if set(rv_dict[key][2]) & set(r_remove):
            rv_dict.pop(key)
        else:
            stops = key.split("_")
            if set(pu_remove) & set(stops):
                rv_dict.pop(key)

def get_rtv(options, r_id_new, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step):
    # search possible routes
    # simple algorithm
    rtv_dict = {}
    route_id = {}
    for v_id in fleet:
        v_bin = [0] * len(fleet) # list of assigned vehicles for ILP
        v_bin[fleet.index(v_id)] = 1
        v_capacity = traci.vehicle.getPersonCapacity(v_id)
        next_stops = [s.actType for s in traci.vehicle.getStops(v_id) if s.actType]
        if len(next_stops) <= 1 and not traci.vehicle.getPersonIDList(v_id):
            # if vehicle idle
            # if 'not next_stops' not enough, last drop off stops stay in list
            # search possible pairs to consider
            pairs = [x_id for x_id, x in rv_dict.items() if set(r_id_new) & set(x[2])]
            # get first pair with faster route
            first_pairs = [x for x in pairs if x.startswith(v_id)]
            for first_pair in first_pairs:
                # search all possible routes ids starting with this
                route = first_pair
                i = 1
                while i < 2*len(r_id_new) and len(route.split("_"))-1 < 2*len(r_id_new):
                    for pair in pairs:
                        if len(route.split("_"))-1 >= 2*len(r_id_new):
                            break # no more stops possible

                        if not pair.startswith(route.split("_")[-1]):
                            continue # if pairs are not compatible

                        route_new = ("_").join([route, pair.split("_")[-1]])

                        # check if pick up before drop off
                        stops = route_new.split("_")
                        trip_pick = [s[:-1] for s in route_new.split('_')[1:] if s.endswith("y")]
                        trip_drop = [s[:-1] for s in route_new.split('_')[1:] if s.endswith("z")]
                        if len(trip_pick) != len(set(trip_pick)) or len(trip_drop) != len(set(trip_drop)):
                            continue # stop already in route

                        abort = False
                        for stop in trip_drop:
                            try:
                                index_pu = stops.index(('%sy' % stop))
                                index_do = stops.index(('%sz' % stop))
                            except:
                                abort = True
                                break # if stop not even picked up

                            if index_pu > index_do:
                                abort = True
                                break # request drop off before pick up (should not be assured with time windows?)
                        if abort:
                            continue

                        if route == first_pair:
                            pax = rv_dict[first_pair][1] + rv_dict[pair][1] # num passenger
                            route_tw = rv_dict[first_pair][0] + rv_dict[pair][0] # travel time
                        else:
                            pax = route_id[route][1] + rv_dict[pair][1] # num passenger
                            route_tw = route_id[route][0] + rv_dict[pair][0] # travel time
                        
                        # check capacity
                        if pax > v_capacity:
                            continue # capacity surpass

                        # check time window for stop
                        stop_id = pair.split("_")[-1]
                        if stop_id.endswith('y'):
                            stop_tw = r_all[stop_id[:-1]].tw_pickup
                            if route_tw > stop_tw[1]:
                                continue # max stop time surpass
                            elif route_tw < stop_tw[0] and pax == 1:
                                # if veh to early at stop, only possible if vehicle is empty (equivalent to pax = 1)
                                route_tw = stop_tw[0] # consider the extra stop time
                        else:
                            stop_tw = r_all[stop_id[:-1]].tw_dropoff
                            if route_tw > stop_tw[1]:
                                continue # max stop time surpass
                            # route_tw < stop_tw[0] only relevant if drop off time definition is implemented
                        
                        # add route id
                        route_id[route_new] = [route_tw, pax] # route time window and current passenger number
                        
                        #if len(route_new.split("_")) % 2 == 1:
                        # check if all reservation served                                
                        if set(trip_pick) == set(trip_drop):      
                            # add possible route to dict
                            r_bin = [0] * len(r_id_new) # list of assigned reservations for ILP
                            for s in trip_pick:
                                r_bin[r_id_new.index(s)] = 1
                            rtv_dict[route_new] = [route_tw, v_bin, r_bin, route_tw-sum(r_bin)*options.c_ko] 
                            # route_id: route travel time, served reservations, vehicle, requests, value
                            
                        route = route_new
                    i += 1
            if len(fleet) == 1:
                # if one vehicle darp, assign the fastest route with max reservation served
                key_list = list(rtv_dict.keys())
                value_list = [value[3] for key, value in rtv_dict.items()]
                key_index = value_list.index(min(value_list))
                return key_list[key_index]

        else:
            next_stops = ["%sy" % s.split(" ")[1] if s.split(" ")[0] == "pickup" else "%sz" % s.split(" ")[1] for s in next_stops]
            last_stops = [s.actType for s in traci.vehicle.getStops(v_id, -(len(r_id_picked)*2)) if s.actType]
            last_stops = ["%sy" % s.split(" ")[1] if s.split(" ")[0] == "pickup" else "%sz" % s.split(" ")[1] for s in last_stops]
            #for x_id in r_id_new:
                # search possible routes for new reservations



def main():
    
    # read inputs
    argParser = initOptions()
    options = argParser.parse_args()

    r_all = {}
    rv_dict = {}

    # start traci
    traci.start(['sumo-gui', '--net-file', '%s' %options.network, '-r', 
    '%s,%s' % (options.reservations, options.taxis), 
    "--device.taxi.dispatch-algorithm", "traci", '-g', '%s' %options.gui_settings,
    "--tripinfo-output", '%s' %options.output, "--tripinfo-output.write-unfinished"])

    # execute the TraCI control loop
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep(step)

        # get fleet and vType for route calculation 
        # only fleets with one vType are considered
        fleet = traci.vehicle.getTaxiFleet(-1)
        v_type = traci.vehicle.getTypeID(fleet[0])

        # get new reservations
        r_id_new = []
        for x in traci.person.getTaxiReservations(1):           

            # search direct travel time 
            direct = int(traci.simulation.findRoute(x.fromEdge, x.toEdge, v_type, x.depart, routingMode=0).travelTime)

            # add new reservation attributes
            setattr(x, 'direct', direct) # direct travel time
            setattr(x, 'vehicle', False) # id of assigned vehicle
            setattr(x, 'tw_pickup', [x.depart, x.depart+options.max_wait]) # pickup time window
            # drop off time window
            if x.direct*options.drf < options.drf_min:
                setattr(x, 'tw_dropoff', [x.tw_pickup[0]+direct, x.tw_pickup[1]+direct+options.drf_min])
            else:
                setattr(x, 'tw_dropoff', [x.tw_pickup[0]+direct, x.tw_pickup[1]+direct*options.drf])
            setattr(x, 'served', False) # if reservation served
            setattr(x, 'rejected', False) # if reservation rejected
            
            # add id to new reservations
            r_id_new.append(x.id)
            # add reservation to list
            r_all[x.id] = x

        # reservations already picked up
        r_id_picked = [x.id for x in traci.person.getTaxiReservations(8)]
        # reservations already served
        r_id_current = [x.id for x in traci.person.getTaxiReservations(0)]
        r_id_served = [x_id for x_id, x in r_all.items() if not x.served and x_id not in r_id_current]

        # if reservations pending
        if traci.person.getTaxiReservations(2):
            # search request-vehicles pairs
            get_rv(options, r_id_new, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step)
            
            # search trips (rtv graph)
            rtv_dict = get_rtv(options, r_id_new, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step)

            if isinstance(rtv_dict, str):
                # if one vehicle darp, assign route
                rtv_dict = rtv_dict.replace('y', '')
                rtv_dict = rtv_dict.replace('z', '')
                stops = rtv_dict.split("_")
                # assign route to vehicle
                traci.vehicle.dispatchTaxi(stops[0], stops[1:])
            #else:
                # of multiple vehicle darp, solve ILP
                # TODO implement ILP
            
        step += options.sim_step

    traci.close()

if __name__ == "__main__":
    main()