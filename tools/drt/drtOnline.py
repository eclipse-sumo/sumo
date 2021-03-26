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

# @file    drtOnline.py
# @author  Giuliana Armellini
# @date    2020-02-15

"""
Simulate Demand Responsive Transport via TraCi
Track progress https://github.com/eclipse/sumo/issues/8256
"""

import os
import sys
from argparse import ArgumentParser
from pulp import *
import rtvAlgorithm

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci

def initOptions():
    argParser = ArgumentParser()
    
    argParser.add_argument("-n", "--network", dest="network", help="SUMO network file", metavar="FILE", required=False)
    argParser.add_argument("-r", "--reservations", help="File with reservations (persons)", metavar="FILE", required=False)
    argParser.add_argument("-v", "--taxi", dest="taxis", help="File with drt vehicles (vehicles with taxi device)", metavar="FILE", required=False)
    argParser.add_argument("-c", dest="sumocfg", help="Sumo configuration file", metavar="FILE", required=False)
    argParser.add_argument("-s", dest="sumo", help="Run with sumo or sumo-gui (default: sumo)", default="sumo", required=False)
    argParser.add_argument("-g", dest="gui_settings", help="Load visualization settings from FILE", metavar="FILE", required=False)
    argParser.add_argument("-o", dest="output", help="Name of output file", default='tripinfo.xml', required=False)
    argParser.add_argument("--rtv", dest="rtv_algorithm", help="Method to search for possible routes (rtv). Available: 0: exhaustive search, 1: simple, 2: simple_rerouting", default='0', required=False)
    argParser.add_argument("--rtv-time", help="Timeout for exhaustive search (default 5 seconds)", type=float, default=5, required=False)
    argParser.add_argument("--ilp-time", help="Timeout for ILP solver (default 5 seconds)", type=float, default=5, required=False)
    
    argParser.add_argument("--c-ko", help="Cost of ignoring a request", type=int, default=1000000000000, required=False)
    argParser.add_argument("--cost-per-trip", help="Cost to avoid using multiple vehicles if trip time is similar (default 600 seconds)", type=int, default=600, required=False)

    argParser.add_argument("--debug", action='store_true')
    
    argParser.add_argument("--drf", dest="drf", help="Direct Route Factor", type=int, default=2, required=False)
    argParser.add_argument("--drf-min", help="Minimum time for cases with short trips", type=int, default=600, required=False)
    argParser.add_argument("--max-wait", help="Maximum waiting time for pickup", type=int, default=900, required=False)
    argParser.add_argument("--max-diff", help="Maximum difference with assigned time", type=int, default=300, required=False)
    argParser.add_argument("--veh-wait", help="Maximum waiting time for passenger in the vehicle", type=int, default=180, required=False)
    argParser.add_argument("--sim-step", help="Time window for request collection", type=int, default=30, required=False)
    argParser.add_argument("--end-time", help="Simulation time to close Traci (default 90000 sec - 25h)", type=int, default=90000, required=False)

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
        tt_1p2p = int(traci.simulation.findRoute(req1.fromEdge, req2.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
        tt_2p2d = rv_dict.get('%sy_%sz' % (req2.id, req2.id), False)
        if not tt_2p2d:
            tt_2p2d = req2.direct + 60 # TODO default stop time ticket #6714
            pair = '%sy_%sz' % (req2.id, req2.id) # 2p2d
            rv_dict[pair] = [tt_2p2d, -1, [req2.id, req2.id]]
        else:
            tt_2p2d = tt_2p2d[0]
        tt_2d1d = int(traci.simulation.findRoute(req1.toEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714

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
            tt_1p2p = int(traci.simulation.findRoute(req1.fromEdge, req2.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
        else:
            tt_1p2p = tt_1p2p[0]
        tt_2p1d = int(traci.simulation.findRoute(req2.fromEdge, req1.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
        tt_1d2d = int(traci.simulation.findRoute(req1.toEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714

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
        tt_2p1p = int(traci.simulation.findRoute(req2.fromEdge, req1.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
        tt_1p2d = int(traci.simulation.findRoute(req1.fromEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
        tt_2d1d = rv_dict.get('%sz_%sz' % (req2.id, req1.id), False)
        if not tt_2d1d:
            tt_2d1d = int(traci.simulation.findRoute(req2.toEdge, req1.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
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
            tt_1p1d = req1.direct + 60 # TODO default stop time ticket #6714
            pair = '%sy_%sz' % (req1.id, req1.id) # 1p1d
            rv_dict[pair] = [tt_1p1d, -1, [req1.id, req1.id]]
        else:
            tt_1p1d = tt_1p1d[0]
        if not tt_2p1p or not tt_1d2d:
            if not tt_2p1p:
                tt_2p1p = int(traci.simulation.findRoute(req2.fromEdge, req1.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
            else:
                tt_2p1p = tt_2p1p[0]
            if not tt_1d2d:
                tt_1d2d = int(traci.simulation.findRoute(req1.toEdge, req2.toEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
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
        tt_1d2p = int(traci.simulation.findRoute(req1.toEdge, req2.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
        if (r1_do0 + tt_1d2p) < r2_pu1:
        # if droping off req 1 at earliest time, req 2 can be pick up at least at latest time, then pair possible
            pair = '%sz_%sy' % (req1.id, req2.id) # 1d2p
            rv_dict[pair] = [tt_1d2p, 1, [req1.id, req2.id]]

    # pair 2d1p
    if r2_do0 <= r1_pu1:
    # if earliest drop off of req 2 before latest pick up of req 1
        tt_2d1p = int(traci.simulation.findRoute(req2.toEdge, req1.fromEdge, v_type, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
        if (r2_do0 + tt_2d1p) < r1_pu1:
        # if droping off req 1 at earliest time, req 2 can be pick up at least at latest time, then pair possible
            pair = '%sz_%sy' % (req2.id, req1.id) # 2d1p
            rv_dict[pair] = [tt_2d1p, 1, [req2.id, req1.id]]

def get_rv(options, r_id_new, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step):
    r_rv_remove = []
    r_all_remove = []
    for x_id, x in r_all.items():
        if x_id in r_id_new:
            # if reservation is new

            r_possible = False # check if request can be serve for a vehicle on time
            # add vehicle-request pairs
            for v_id in fleet:
                # calculate travel time to pickup
                pickup_time = int(traci.simulation.findRoute(traci.vehicle.getRoadID(v_id), x.fromEdge, v_type, routingMode=0).travelTime)
                if step+pickup_time <= x.tw_pickup[1]:
                    # if vehicle on time, add to rv graph
                    route_id = '%s_%sy' % (v_id, x_id)
                    rv_dict[route_id] = [pickup_time+60, 1, [v_id, x_id]] # TODO default stop time ticket #6714
                    r_possible = True
            
            if not r_possible:
                # reject request and remove person from simulation
                # TODO no rejection option should be implemented in future
                print("Reservation %s (person %s) cannot be served" % (x_id, x.persons))
                r_all_remove.append(x_id)
                r_rv_remove.extend(['%sy' %x_id, '%sz' %x_id])
                for person in x.persons:
                   traci.person.removeStages(person)
                continue

            # add direct route pair
            route_id = '%sy_%sz' % (x_id, x_id) # y: pickup / z: drop off
            rv_dict[route_id] = [x.direct+60, -1, [x_id, x_id]] # TODO default stop time ticket #6714

            # add request-request pairs only with:
            requests2 = set(r_all.keys()) ^ set(r_all_remove)
            for req2 in requests2:
                if req2 == x_id:
                    continue
                rr_pair(x, r_all.get(req2), v_type, rv_dict) # search possible pairs and add to rv
        
        elif not x.vehicle:
            # if reservation not assigned
            # check if pick-up still possible
            if x.tw_pickup[1] < step:
                # pickup time window surpass simulation time -> reject request
                # TODO no rejection option should be implemented in future
                print("Reservation %s (person %s) cannot be served" % (x_id, x.persons))
                r_all_remove.append(x_id)
                r_rv_remove.extend(['%sy' %x_id, '%sz' %x_id])
                for person in x.persons:
                    traci.person.removeStages(person)
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
                # if no vehicle available for pick-up on time -> reject request
                print("Reservation %s (person %s) cannot be served" % (x_id, x.persons))
                r_all_remove.append(x_id)
                r_rv_remove.extend(['%sy' %x_id, '%sz' %x_id])
                for person in x.persons:
                    traci.person.removeStages(person)

        elif x_id not in r_id_picked:
            # if reservation assigned but not picked up
            route_id = '%s_%sy' % (x.vehicle, x_id)
            if rv_dict.get(route_id, False):
                # update travel time to pickup
                pickup_time = int(traci.simulation.findRoute(traci.vehicle.getRoadID(x.vehicle), x.fromEdge, v_type, routingMode=0).travelTime)               
                rv_dict[route_id][0] = pickup_time+60 # TODO default stop time ticket #6714
                if options.debug and step+pickup_time > x.tw_pickup[1]: # Debug only
                    print("Time window surpassed by", step+pickup_time - x.tw_pickup[1])
        
        elif x_id in r_id_picked:
            # if reservation picked up, delete pick-up pair
            r_rv_remove.append('%sy' % x_id)
            # update travel time to drop off
            dropoff_time = int(traci.simulation.findRoute(traci.vehicle.getRoadID(x.vehicle), x.toEdge, v_type, routingMode=0).travelTime)
            route_id = '%s_%sz' % (x.vehicle, x_id)
            rv_dict[route_id] = [dropoff_time+60, -1, [x.vehicle, x_id]] # TODO default stop time ticket #6714
        else:
            print("Error: Reservation state not considered")

    # remove rejected, served and picked up reservations from rv graph
    if r_rv_remove:
        [rv_dict.pop(key) for key in list(rv_dict) if set(r_rv_remove) & set(key.split("_"))]

    # remove rejected reservations from r_all
    if r_all_remove:
        [r_all.pop(key) for key in r_all_remove]

def ilp_solve(options, v_num, r_num, costs, vehicle_constraints, request_constraints):
    # founds the combination of trips that minimize the costs function

    #req_costs = [request.cost for request in requests] # TODO to implement req with diff costs/priorities
    order_trips = costs.keys()

    ILP_result = []

    # Create the 'prob' variable to contain the problem data
    prob = LpProblem("DARP", LpMinimize)

    # 'Trips_vars' dict with the referenced Variables (all possible trips)
    Trips_vars = LpVariable.dicts("Trip", order_trips, cat='Binary')

    # add objective function
    prob += lpSum([costs[i] * Trips_vars[i] for i in order_trips]) - \
            options.c_ko * lpSum([sum(request_constraints[i]) * Trips_vars[i] for i in order_trips]), \
            "Total_Trips_Travel_Time"
    
    # cost of each route = trip cost - (cost of serve a request * Nr of request served)

    # add constraints
    for index in range(v_num):
        prob += lpSum([vehicle_constraints[i][index] * Trips_vars[i] for i in order_trips]) <= 1, \
                "Max_1_Trip_for_Vehicle_%s" % index

    for index in range(r_num):
        prob += lpSum([request_constraints[i][index] * Trips_vars[i] for i in order_trips]) <= 1, \
                "Max_1_Trip_for_Request_%s" % index


    prob += lpSum([sum(vehicle_constraints[i]) * Trips_vars[i] for i in order_trips]) >= 1, \
            "Avoid_null_result_by_assigning_at_least_one_vehicle"

    # The problem data is written into following file
    prob.writeLP("DRT_ilp.txt") # TODO write as temporary

    # The problem is solved using PuLP's Solver choice
    prob.solve(PULP_CBC_CMD(msg=0, timeLimit= options.ilp_time))

    if LpStatus[prob.status] != 'Optimal':
        sys.exit("No optimal solution could be found. Return value: %s" % LpStatus[prob.status])
    else:  # if optimal solution was found
        for v in prob.variables():
            if v.varValue == 1:
                result = v.name.split("Trip_")[1]
                ILP_result.append(result)

    return ILP_result

def main():
    
    # read inputs
    argParser = initOptions()
    options = argParser.parse_args()

    r_all = {}
    rv_dict = {}
    memory_problems = [0]

    # start traci
    if options.sumocfg:
        run_traci = [options.sumo, "-c", options.sumocfg, '--tripinfo-output.write-unfinished']
    else:
        run_traci = [options.sumo, '--net-file', '%s' %options.network, '-r', 
        '%s,%s' % (options.reservations, options.taxis), '-l', 'log.txt',
        '--device.taxi.dispatch-algorithm', 'traci',
        '--tripinfo-output', '%s' %options.output, '--tripinfo-output.write-unfinished']
        if options.gui_settings:
            run_traci.extend(['-g', '%s' %options.gui_settings])            

    traci.start(run_traci)

    # execute the TraCI control loop
    step = traci.simulation.getTime() + 10
    v_type = None
    rerouting = True
    while rerouting:
        traci.simulationStep(step)

        # get vType for route calculation
        if not v_type:
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
            setattr(x, 'delay', 0) # difference between assigned pick up time and real one
            setattr(x, 'tw_pickup', [x.depart, x.depart+options.max_wait]) # pickup time window
            # drop off time window
            if x.direct*options.drf < options.drf_min:
                setattr(x, 'tw_dropoff', [x.tw_pickup[0]+direct, x.tw_pickup[1]+direct+options.drf_min])
            else:
                setattr(x, 'tw_dropoff', [x.tw_pickup[0]+direct, x.tw_pickup[1]+direct*options.drf])
            
            # add id to new reservations
            r_id_new.append(x.id)
            # add reservation to list
            r_all[x.id] = x

        # unassigned reservations
        r_id_unassigned = [x.id for x_id, x in r_all.items() if not x.vehicle]
        
        # if reservations pending
        if r_id_unassigned:
            if options.debug:
                print('\nRun dispatcher')
                if r_id_new:
                    print('New reservations: ', r_id_new)
                print('Unassigned reservations: ', list(set(r_id_unassigned)-set(r_id_new)))

            # get fleet 
            fleet = traci.vehicle.getTaxiFleet(-1)
            if set(fleet) != set(fleet) & set(traci.vehicle.getIDList()): #TODO manage teleports
                print("\nVehicle %s is being teleported, skip to next step" % (set(fleet) - set(traci.vehicle.getIDList())))
                step += options.sim_step
                continue # if a vehicle is being teleported skip to next step
            
            # remove reservations already served
            r_id_current = [x.id for x in traci.person.getTaxiReservations(0)]
            r_id_served = list(set(r_all.keys()) - set(r_id_current))
            [r_all.pop(key) for key in r_id_served]
            [rv_dict.pop(key) for key in list(rv_dict) if set(r_id_served) & set(rv_dict[key][2])]

            # reservations already picked up
            r_id_picked = [x.id for x in traci.person.getTaxiReservations(8)]

            # search request-vehicles pairs
            if options.debug:
                print('Calculate RV-Graph')
            get_rv(options, r_id_new, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step)
            
            # search trips (rtv graph)
            # TODO define/import algorithm before to avoid checking each time
            # Maybe a list with the function as element and get the element (0, 1, 2)
            
            if options.debug:
                print('Calculate RTV-Graph')
            if options.rtv_algorithm == '0':
                rtv_dict, r_id_rtv, memory_problems = rtvAlgorithm.exhaustive_search(options, r_id_unassigned, r_id_picked, r_all, fleet, v_type, rv_dict, step, memory_problems)
            elif options.rtv_algorithm == '1':
                rtv_dict, r_id_rtv = rtvAlgorithm.simple(options, r_id_unassigned, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step)
            elif options.rtv_algorithm == '2':
                rtv_dict, r_id_rtv = rtvAlgorithm.simple_rerouting(options, r_id_unassigned, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step)
            #rtv_dict, r_id_rtv = get_rtv(options, r_id_unassigned, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step)

            if len(rtv_dict) == 0:
                step += options.sim_step
                continue # if no routes found

            elif len(rtv_dict.keys()) == 1:
                # if one vehicle darp, assign route
                best_routes = list(rtv_dict.keys())
            
            else:
                # if multiple vehicle darp, solve ILP with pulp
                vehicle_constraints = {}
                request_constraints = {}
                costs = {}
                trips = list(rtv_dict.keys()) # list of all trips for ILP solution parse

                # add bonus_cost to trip cost (makes trips with more served requests cheaper than splitting the requests to more
                # vehicles with smaller trips if both strategies would yield a similar cost)
                for idx, trip_id in enumerate(trips):
                    # rtv_dict[route] = [travel_time, v_bin, r_bin, value] 
                    bonus_cost = (sum(rtv_dict[trip_id][2]) + 1) * options.cost_per_trip # TODO specific cost for vehicle can be consider here
                    costs.update({idx: rtv_dict[trip_id][0] + bonus_cost})  # generate dict with cost
                    vehicle_constraints.update({idx: rtv_dict[trip_id][1]})  # generate dict with vehicle used in the trip
                    request_constraints.update({idx: rtv_dict[trip_id][2]})  # generate dict with served requests in the trip
                
                
                if options.debug:
                    print('Solve ILP')
                ilp_result = ilp_solve(options, len(fleet), len(r_id_rtv), costs, vehicle_constraints, request_constraints)
                
                # parse ILP result
                best_routes = [trips[int(route_index)] for route_index in ilp_result]
            
            # assign routes to vehicles
            for route_id in best_routes:
                stops = route_id.replace('y', '')
                stops = stops.replace('z', '')
                stops = stops.split("_")
                # first check if route different or better (when no optimal solution) than already assigned
                current_route = []
                try: # get current route
                    for taxi_stop in traci.vehicle.getStops(stops[0]):
                        sub_stops = taxi_stop.actType.split(",") # if more than 1 reservation in stop
                        for sub_stop in sub_stops:
                            current_route.append(sub_stop.split(" ")[2][1:-1])
                except:
                    current_route = ['None']
                if current_route == stops[1:]:
                    # route is the same
                    continue
                elif set(current_route) == set(stops[1:]) and len(current_route) == len(stops[1:]):
                    # if route serve same request, check if new is faster
                    tt_current_route = 0
                    edges = [taxi_stops.lane.split("_")[0] for taxi_stops in traci.vehicle.getStops(stops[0])] #TODO check next_act update
                    edges.insert(0, traci.vehicle.getLaneID(stops[0]).split("_")[0]) # add current edge
                    for idx, edge in enumerate(edges[:-1]):
                        tt_current_route += int(traci.simulation.findRoute(edge, edges[idx+1], v_type, step, routingMode=0).travelTime) + 60 # TODO default stop time ticket #6714
                    tt_new_route = rtv_dict[route_id][0]
                    if tt_new_route >= tt_current_route:
                        continue # current route better than new found
                if options.debug:
                    print('Dispatch: ', route_id)
                traci.vehicle.dispatchTaxi(stops[0], stops[1:])
                # assign vehicle to requests
                # TODO to avoid major changes in the pick-up time when assigning new passengers,
                # tw_pickup should be updated, whit some constant X seconds, e.g. 10 Minutes
                for x_id in set(stops[1:]):
                    x = r_all[x_id]
                    x.vehicle = stops[0]
        
        if step > options.end_time or (not traci.simulation.getMinExpectedNumber() > 0 and not traci.person.getIDList()): #TODO ticket #8385
            rerouting = False
            
        step += options.sim_step

    if options.rtv_algorithm == 0: # if exhaustive search
        if sum(memory_problems) == 0:
            print('Optimal solution found')
        else:
            print('The maximum specified time for the calculation with the exact method was exceeded. Solution could not be optimal')
    print('DRT simulation ended')
    traci.close()

if __name__ == "__main__":
    main()
