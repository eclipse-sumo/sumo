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

import pulp as pl
import rtvAlgorithm

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
from sumolib import checkBinary  # noqa
import traci  # noqa
findRoute = traci.simulation.findRoute


def initOptions():
    ap = ArgumentParser()

    ap.add_argument("-n", "--network", dest="network", metavar="FILE",
                    help="SUMO network file")
    ap.add_argument("-r", "--reservations", metavar="FILE",
                    help="File with reservations (persons)")
    ap.add_argument("-v", "--taxis", metavar="FILE",
                    help="File with drt vehicles")
    ap.add_argument("-c", dest="sumocfg", metavar="FILE",
                    help="Sumo configuration file")
    ap.add_argument("-s", dest="sumo", default="sumo",
                    help="Run with sumo (default) or sumo-gui")
    ap.add_argument("-g", dest="gui_settings", metavar="FILE",
                    help="Load visualization settings from FILE")
    ap.add_argument("-o", dest="output", default='tripinfos.xml',
                    help="Name of output file")
    ap.add_argument("--rtv", dest="rtv_algorithm", default='0',
                    help="Method to search for possible routes (rtv). Available: 0: exhaustive search, 1: simple, 2: simple_rerouting")  # noqa
    ap.add_argument("--rtv-time", type=float, default=5,
                    help="Timeout for exhaustive search (default 5 seconds)")
    ap.add_argument("--ilp-time", type=float, default=5,
                    help="Timeout for ILP solver (default 5 seconds)")
    ap.add_argument("--c-ko", type=int, default=1000000000000,
                    help="Cost of ignoring a reservation")
    ap.add_argument("--cost-per-trip", type=int, default=600,
                    help="Cost to avoid using multiple vehicles if the travel time of trips is similar (default 600 seconds)")  # noqa
    ap.add_argument("--drf", dest="drf", type=float, default=2,
                    help="Factor by which the DRT travel time should not exceed the one of a direct connection (default 2)")  # noqa
    ap.add_argument("--drf-min", type=int, default=600,
                    help="Minimum time difference allowed between DRT travel time and direct connection for the cases of short trips (default 600 seconds)")  # noqa
    ap.add_argument("--max-wait", type=int, default=900,
                    help="Maximum waiting time for pickup (default 900 seconds)")  # noqa
    ap.add_argument("--sim-step", type=int, default=30,
                    help="Step time to collect new reservations (default 30 seconds)")  # noqa
    ap.add_argument("--end-time", type=int, default=90000,
                    help="Maximum simulation time to close Traci (default 90000 sec - 25h)")  # noqa
    ap.add_argument("--routing-algorithm", default='dijkstra',
                    help="Algorithm for shortest path routing. Support: dijkstra (default), astar, CH and CHWrapper")  # noqa
    ap.add_argument("--routing-mode", type=int, default=0,
                    help="Mode for shortest path routing. Support: 0 (default) for routing with loaded or default speeds and 1 for routing with averaged historical speeds")  # noqa
    ap.add_argument("--debug", action='store_true')

    return ap


def res_res_pair(options, res1, res2, veh_type, rv_dict):
    """
    Search all combinations between two reservations. Notation: the first of
    the reservations is defined as 'res1' and the second as 'res2' and 'p'
    refers to pick up and 'd' to drop off a reservation. Then 'res2p_res1d'
    means pickup reservation 2 and then drop off reservation 1.
    """

    # combination 1p2p 2p2d 2d1d
    if (res1.tw_pickup[0] <= res2.tw_pickup[1] and
       res2.tw_dropoff[0] <= res1.tw_dropoff[1]):
        # if earliest pick up of req 1 before latest pick up of req 2 and
        # if earliest drop off of req 2 before latest drop off of req 1

        # calculate travel times for each pair
        res1p_res2p = int(findRoute(res1.fromEdge, res2.fromEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa
        res2p_res2d = rv_dict.get('%sy_%sz' % (res2.id, res2.id), False)
        if not res2p_res2d:
            res2p_res2d = res2.direct + 60  # TODO default stop time ticket #6714 # noqa
            pair = '%sy_%sz' % (res2.id, res2.id)  # 2p2d
            rv_dict[pair] = [res2p_res2d, -1, [res2.id, res2.id]]
        else:
            res2p_res2d = res2p_res2d[0]
        res2d_res1d = int(findRoute(res2.toEdge, res1.toEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa

        # check if time windows constrains are fulfilled
        time_res2p = res1.tw_pickup[0] + res1p_res2p
        if time_res2p > res2.tw_pickup[1]:
            pass  # not possible
        elif (time_res2p + res2p_res2d) > res2.tw_dropoff[1]:
            pass  # not possible
        elif (time_res2p + res2p_res2d + res2d_res1d) > res1.tw_dropoff[1]:
            pass  # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (res1.id, res2.id)  # 1p2p
            rv_dict[pair] = [res1p_res2p, 1, [res1.id, res2.id]]
            pair = '%sz_%sz' % (res2.id, res1.id)  # 2d1d
            rv_dict[pair] = [res2d_res1d, -1, [res2.id, res1.id]]

    # combination 1p2p 2p1d 1d2d
    if (res1.tw_pickup[0] <= res2.tw_pickup[1] and
       res1.tw_dropoff[0] <= res2.tw_dropoff[1]):
        # if earliest pick up of req 1 before latest pick up of req 2 and
        # if earliest drop off of req 1 before latest drop off of req 2

        # calculate travel times for each pair
        res1p_res2p = rv_dict.get('%sy_%sy' % (res1.id, res2.id), False)
        if not res1p_res2p:
            res1p_res2p = int(findRoute(res1.fromEdge, res2.fromEdge, veh_type,
                              routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time #6714 # noqa
        else:
            res1p_res2p = res1p_res2p[0]
        res2p_res1d = int(findRoute(res2.fromEdge, res1.toEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa
        res1d_res2d = int(findRoute(res1.toEdge, res2.toEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa

        # check if time windows constrains are fulfilled
        time_res2p = res1.tw_pickup[0] + res1p_res2p
        if time_res2p > res2.tw_pickup[1]:
            pass  # not possible
        elif (time_res2p + res2p_res1d) > res1.tw_dropoff[1]:
            pass  # not possible
        elif (time_res2p + res2p_res1d + res1d_res2d) > res2.tw_dropoff[1]:
            pass  # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (res1.id, res2.id)  # 1p2p
            if not rv_dict.get(pair, False):
                rv_dict[pair] = [res1p_res2p, 1, [res1.id, res2.id]]
            pair = '%sy_%sz' % (res2.id, res1.id)  # 2p1d
            rv_dict[pair] = [res2p_res1d, -1, [res2.id, res1.id]]
            pair = '%sz_%sz' % (res1.id, res2.id)  # 1d2d
            rv_dict[pair] = [res1d_res2d, -1, [res1.id, res2.id]]

    # combination 2p1p 1p2d 2d1d
    if (res2.tw_pickup[0] <= res1.tw_pickup[1] and
       res2.tw_dropoff[0] <= res1.tw_dropoff[1]):
        # if earliest pick up of req 2 before latest pick up of req 1 and
        # if earliest drop off of req 2 before latest drop off of req 1

        # calculate travel times for each pair
        res2p_res1p = int(findRoute(res2.fromEdge, res1.fromEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa
        res1p_res2d = int(findRoute(res1.fromEdge, res2.toEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa
        res2d_res1d = rv_dict.get('%sz_%sz' % (res2.id, res1.id), False)
        if not res2d_res1d:
            # if 2d1d not added to dict in steps before
            res2d_res1d = int(findRoute(res2.toEdge, res1.toEdge, veh_type,
                              routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time #6714 # noqa
        else:
            res2d_res1d = res2d_res1d[0]

        # check if time windows constrains are fulfilled
        time_res1p = res2.tw_pickup[0] + res2p_res1p
        if time_res1p > res1.tw_pickup[1]:
            pass  # not possible
        elif (time_res1p + res1p_res2d) > res2.tw_dropoff[1]:
            pass  # not possible
        elif (time_res1p + res1p_res2d + res2d_res1d) > res1.tw_dropoff[1]:
            pass  # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (res2.id, res1.id)  # 2p1p
            rv_dict[pair] = [res2p_res1p, 1, [res2.id, res1.id]]
            pair = '%sy_%sz' % (res1.id, res2.id)  # 1p2d
            rv_dict[pair] = [res1p_res2d, -1, [res1.id, res2.id]]
            pair = '%sz_%sz' % (res1.id, res2.id)  # 2d1d
            if not rv_dict.get(pair, False):
                rv_dict[pair] = [res2d_res1d, -1, [res2.id, res1.id]]

    # combination 2p1p 1p1d 1d2d
    if (res2.tw_pickup[0] <= res1.tw_pickup[1] and
       res1.tw_dropoff[0] <= res2.tw_dropoff[1]):
        # if earliest pick up of req 2 before latest pick up of req 1 and
        # if earliest drop off of req 1 before latest drop off of req 2

        # calculate travel times for each pair
        res2p_res1p = rv_dict.get('%sy_%sy' % (res2.id, res1.id), False)
        res1d_res2d = rv_dict.get('%sz_%sz' % (res1.id, res2.id), False)
        res1p_res1d = rv_dict.get('%sy_%sz' % (res1.id, res1.id), False)
        if not res1p_res1d:
            res1p_res1d = res1.direct + 60  # TODO default stop time ticket #6714 # noqa
            pair = '%sy_%sz' % (res1.id, res1.id)  # 1p1d
            rv_dict[pair] = [res1p_res1d, -1, [res1.id, res1.id]]
        else:
            res1p_res1d = res1p_res1d[0]

        if not res2p_res1p or not res1d_res2d:
            # if 2p1p 1d2d not added to dict in steps before
            if not res2p_res1p:
                res2p_res1p = int(findRoute(res2.fromEdge, res1.fromEdge,
                                  veh_type, routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time #6714 # noqa
            else:
                res2p_res1p = res2p_res1p[0]
            if not res1d_res2d:
                res1d_res2d = int(findRoute(res1.toEdge, res2.toEdge, veh_type,
                                  routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time #6714 # noqa
            else:
                res1d_res2d = res1d_res2d[0]
            res1p_res1d = rv_dict.get('%sy_%sz' % (res1.id, res1.id))[0]

            # check if time windows constrains are fulfilled
            time_res1p = res2.tw_pickup[0] + res2p_res1p
            if time_res1p > res1.tw_pickup[1]:
                pass  # not possible
            elif (time_res1p + res1p_res1d) > res1.tw_dropoff[1]: # noqa
                pass  # not possible
            elif (time_res1p + res1p_res1d + res1d_res2d) > res2.tw_dropoff[1]:
                pass  # not possible
            else:
                # pairs are possible
                pair = '%sy_%sy' % (res2.id, res1.id)  # 2p1p
                if not rv_dict.get(pair, False):
                    rv_dict[pair] = [res2p_res1p, 1, [res2.id, res1.id]]
                pair = '%sz_%sz' % (res1.id, res2.id)  # 1d2d
                if not rv_dict.get(pair, False):
                    rv_dict[pair] = [res1d_res2d, -1, [res1.id, res2.id]]

    # pair 1d2p
    if res1.tw_dropoff[0] <= res2.tw_pickup[1]:
        # if earliest drop off of req 1 before latest pick up of req 2

        # calculate travel times for each pair
        res1d_res2p = int(findRoute(res1.toEdge, res2.fromEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa

        # check if time windows constrains are fulfilled
        if (res1.tw_dropoff[0] + res1d_res2p) < res2.tw_pickup[1]:
            pair = '%sz_%sy' % (res1.id, res2.id)  # 1d2p
            rv_dict[pair] = [res1d_res2p, 1, [res1.id, res2.id]]

    # pair 2d1p
    if res2.tw_dropoff[0] <= res1.tw_pickup[1]:
        # if earliest drop off of req 2 before latest pick up of req 1

        # calculate travel times for each pair
        res2d_res1p = int(findRoute(res2.toEdge, res1.fromEdge, veh_type,
                          routingMode=options.routing_mode).travelTime) + 60  # TODO default stop time ticket #6714 # noqa

        # check if time windows constrains are fulfilled
        if (res2.tw_dropoff[0] + res2d_res1p) < res1.tw_pickup[1]:
            pair = '%sz_%sy' % (res2.id, res1.id)  # 2d1p
            rv_dict[pair] = [res2d_res1p, 1, [res2.id, res1.id]]


def get_rv(options, res_id_new, res_id_picked, res_id_served, res_all, fleet, veh_type, rv_dict, step, veh_edges):  # noqa
    """
    Generates the reservation-vehicle graph, which has the possible
    combinations between two reservations and between a reservation and a
    vehicle with the required travel time, the number of passengers picked up
    or dropped off and the reservation id and/or vehicle id of the pair.
    """
    res_rv_remove = []
    res_all_remove = []
    for res_id, res in res_all.items():
        if res_id in res_id_new:
            # if reservation is new

            # check if reservation can be serve for a vehicle on time
            res_possible = False
            # add vehicle-reservation pairs
            for edge, vehicles in veh_edges.items():
                pickup_time = findRoute(edge, res.fromEdge, veh_type,
                                        routingMode=options.routing_mode).travelTime
                if step+pickup_time > res.tw_pickup[1]:
                    # if vehicle arrives to late
                    continue
                # if vehicle on time, add to rv graph
                for veh_id in vehicles:
                    route_id = '%s_%sy' % (veh_id, res_id)
                    rv_dict[route_id] = [pickup_time+60, 1, [veh_id, res_id]]  # TODO default stop time ticket #6714 # noqa
                    res_possible = True

            if not res_possible:
                # reject reservation and remove person from simulation
                # TODO no rejection option should be implemented in future
                print("Reservation %s (person %s) cannot be served" %
                      (res_id, res.persons))
                res_all_remove.append(res_id)
                res_rv_remove.extend(['%sy' % res_id, '%sz' % res_id])
                for person in res.persons:
                    traci.person.removeStages(person)
                continue

            # add direct route pair
            route_id = '%sy_%sz' % (res_id, res_id)  # y: pickup / z: drop off
            rv_dict[route_id] = [res.direct+60, -1, [res_id, res_id]]  # TODO default stop time ticket #6714 # noqa

            # add reservation-reservation pairs
            reservations2 = set(res_all.keys()) ^ set(res_all_remove)
            for res2 in reservations2:
                if res2 != res_id:
                    res_res_pair(options, res, res_all.get(res2), veh_type, rv_dict)

        elif not res.vehicle:
            # if reservation not assigned
            # check if pick-up still possible
            if res.tw_pickup[1] < step:
                # latest pickup time exceed simulation time, reject reservation
                # TODO no rejection option should be implemented in future
                print("Reservation %s (person %s) cannot be served" %
                      (res_id, res.persons))
                res_all_remove.append(res_id)
                res_rv_remove.extend(['%sy' % res_id, '%sz' % res_id])
                for person in res.persons:
                    traci.person.removeStages(person)
                continue

            remove = True
            for edge, vehicles in veh_edges.items():
                pickup_time = findRoute(edge, res.fromEdge, veh_type,
                                        routingMode=options.routing_mode).travelTime
                if step+pickup_time <= res.tw_pickup[1]:
                    # if vehicle on time, add to rv graph
                    for veh_id in vehicles:
                        route_id = '%s_%sy' % (veh_id, res_id)
                        if rv_dict.get(route_id, False):
                            rv_dict[route_id][0] = pickup_time+60
                            remove = False
                else:
                    # remove pair if pick-up not possible
                    for veh_id in vehicles:
                        route_id = '%s_%sy' % (veh_id, res_id)
                        if rv_dict.get(route_id, False):
                            rv_dict.pop(route_id)

            if remove:
                # if no vehicles can pick up on time -> reject reservation
                print("Reservation %s (person %s) cannot be served" %
                      (res_id, res.persons))
                res_all_remove.append(res_id)
                res_rv_remove.extend(['%sy' % res_id, '%sz' % res_id])
                for person in res.persons:
                    traci.person.removeStages(person)

        elif res_id not in res_id_picked:
            # if reservation assigned but not picked up
            route_id = '%s_%sy' % (res.vehicle, res_id)
            if rv_dict.get(route_id, False):
                # update travel time to pickup
                if traci.vehicle.getRoadID(res.vehicle).startswith(':'):
                    # avoid routing error when in intersection TODO #5829
                    edge_index = traci.vehicle.getRouteIndex(res.vehicle) + 1
                    from_edge = traci.vehicle.getRoute(res.vehicle)[edge_index]
                else:
                    from_edge = traci.vehicle.getRoadID(res.vehicle)
                pickup_time = findRoute(from_edge, res.fromEdge, veh_type,
                                        routingMode=options.routing_mode).travelTime
                rv_dict[route_id][0] = pickup_time+60  # TODO default stop time ticket #6714 # noqa
                if options.debug and step+pickup_time > res.tw_pickup[1]:  # Debug only # noqa
                    print("Time window surpassed by", step+pickup_time - res.tw_pickup[1]) # noqa

        elif res_id in res_id_picked:
            # if reservation picked up, delete pick-up pair
            res_rv_remove.append('%sy' % res_id)
            # update travel time to drop off
            if traci.vehicle.getRoadID(res.vehicle).startswith(':'):
                # avoid routing error when in intersection TODO #5829
                edge_index = traci.vehicle.getRouteIndex(res.vehicle) + 1
                from_edge = traci.vehicle.getRoute(res.vehicle)[edge_index]
            else:
                from_edge = traci.vehicle.getRoadID(res.vehicle)
            dropoff_time = int(findRoute(from_edge, res.toEdge, veh_type,
                               routingMode=options.routing_mode).travelTime)
            route_id = '%s_%sz' % (res.vehicle, res_id)
            rv_dict[route_id] = [dropoff_time+60, -1, [res.vehicle, res_id]]  # TODO default stop time ticket #6714 # noqa
        else:
            print("Error: Reservation state not considered")

    # remove rejected, served and picked up reservations from rv graph
    if res_rv_remove:
        [rv_dict.pop(key) for key in list(rv_dict) if set(res_rv_remove) & set(key.split("_"))] # noqa

    # remove rejected reservations from res_all
    if res_all_remove:
        [res_all.pop(key) for key in res_all_remove]


def ilp_solve(options, veh_num, res_num, costs, veh_constraints, res_constraints):  # noqa
    """
    Solves the integer linear programming to define the best routes for each
    vehicle. Only implemented for problems with multiple vehicles.
    """
    # founds the combination of trips that minimize the costs function

    # req_costs = [reservation.cost for reservation in reservations] # TODO to implement req with diff costs/priorities # noqa
    order_trips = costs.keys()

    ILP_result = []

    # Create the 'prob' variable to contain the problem data
    prob = pl.LpProblem("DARP", pl.LpMinimize)

    # 'Trips_vars' dict with the referenced Variables (all possible trips)
    Trips_vars = pl.LpVariable.dicts("Trip", order_trips, cat='Binary')

    # add objective function
    # solution cost = sum(cost of each trip) -
    #             sum(reservation rejection cost * reservations served in trip)
    prob += pl.lpSum([costs[i] * Trips_vars[i] for i in order_trips]) - \
        options.c_ko * pl.lpSum([sum(res_constraints[i]) * Trips_vars[i]
                                for i in order_trips]), "Trips_travel_time"

    # add constraints
    for index in range(veh_num):
        prob += pl.lpSum([veh_constraints[i][index] * Trips_vars[i]
                         for i in order_trips]) <= 1, \
                         "Max_1_trip_per_vehicle_%s" % index

    for index in range(res_num):
        prob += pl.lpSum([res_constraints[i][index] * Trips_vars[i]
                         for i in order_trips]) <= 1, \
                         "Max_1_trip_per_reservation_%s" % index

    prob += pl.lpSum([sum(veh_constraints[i]) * Trips_vars[i]
                     for i in order_trips]) >= 1, "Assing_at_least_one_vehicle"

    # The problem data is written into following file
    prob.writeLP("DRT_ilp.txt")  # TODO write as temporary

    # The problem is solved using PuLP's Solver choice
    prob.solve(pl.PULP_CBC_CMD(msg=0, timeLimit=options.ilp_time))

    if pl.LpStatus[prob.status] != 'Optimal':
        sys.exit("No optimal solution found: %s" % pl.LpStatus[prob.status])
    else:  # if optimal solution was found
        for variable in prob.variables():
            if variable.varValue == 1:
                result = variable.name.split("Trip_")[1]
                ILP_result.append(result)

    return ILP_result


def main():
    # read inputs
    ap = initOptions()
    options = ap.parse_args()

    res_all = {}
    rv_dict = {}
    exact_sol = [0]

    if options.sumo == 'sumo':
        SUMO = checkBinary('sumo')
    else:
        SUMO = checkBinary('sumo-gui')

    # start traci
    if options.sumocfg:
        run_traci = [SUMO, "-c", options.sumocfg,
                     '--tripinfo-output.write-unfinished', "--no-step-log",
                     '--routing-algorithm', options.routing_algorithm]
    else:
        run_traci = [SUMO, '--net-file', '%s' % options.network, '-r',
                     '%s,%s' % (options.reservations, options.taxis), '-l',
                     'log.txt', '--device.taxi.dispatch-algorithm', 'traci',
                     '--tripinfo-output', '%s' % options.output,
                     '--tripinfo-output.write-unfinished', "--no-step-log",
                     '--routing-algorithm', options.routing_algorithm]
        if options.gui_settings:
            run_traci.extend(['-g', '%s' % options.gui_settings])

    traci.start(run_traci)

    # execute the TraCI control loop
    step = traci.simulation.getTime() + 10
    veh_type = None
    rerouting = True
    while rerouting:
        traci.simulationStep(step)

        if not traci.vehicle.getTaxiFleet(-1):
            step += options.sim_step
            continue

        # get vType for route calculation
        if not veh_type:
            fleet = traci.vehicle.getTaxiFleet(-1)
            veh_type = traci.vehicle.getTypeID(fleet[0])

        # get new reservations
        res_id_new = []
        for res in traci.person.getTaxiReservations(1):

            # search direct travel time
            direct = int(findRoute(res.fromEdge, res.toEdge, veh_type,
                         res.depart, routingMode=options.routing_mode).travelTime)

            # add new reservation attributes
            setattr(res, 'direct', direct)  # direct travel time
            setattr(res, 'vehicle', False)  # id of assigned vehicle
            setattr(res, 'delay', 0)  # real pick up time - assigned time
            # pickup time window
            setattr(res, 'tw_pickup',
                    [res.depart, res.depart+options.max_wait])
            # drop off time window
            if res.direct*options.drf < options.drf_min:
                setattr(res, 'tw_dropoff', [res.tw_pickup[0]+direct,
                        res.tw_pickup[1]+direct+options.drf_min])
            else:
                setattr(res, 'tw_dropoff', [res.tw_pickup[0]+direct,
                        res.tw_pickup[1]+direct*options.drf])

            # add reservation id to new reservations
            res_id_new.append(res.id)
            # add reservation object to list
            res_all[res.id] = res

        # unassigned reservations
        res_id_unassigned = [res.id for res_id, res in res_all.items()
                             if not res.vehicle]

        # if reservations pending
        if res_id_unassigned:
            if options.debug:
                print('\nRun dispatcher')
                if res_id_new:
                    print('New reservations: ', res_id_new)
                print('Unassigned reservations: ',
                      list(set(res_id_unassigned)-set(res_id_new)))

            # get fleet
            fleet = traci.vehicle.getTaxiFleet(-1)
            if set(fleet) != set(fleet) & set(traci.vehicle.getIDList()):  # TODO manage teleports # noqa
                print("\nVehicle %s is being teleported, skip to next step" %
                      (set(fleet) - set(traci.vehicle.getIDList())))
                step += options.sim_step
                continue  # if a vehicle is being teleported skip to next step

            # find unique vehicles to avoid same calculation if vehicle are equal
            veh_edges = {}
            for veh_id in fleet:
                if traci.vehicle.getRoadID(veh_id).startswith(':'):
                    # avoid routing error when in intersection TODO #5829
                    edge_index = traci.vehicle.getRouteIndex(veh_id) + 1
                    veh_edge = traci.vehicle.getRoute(veh_id)[edge_index]
                else:
                    veh_edge = traci.vehicle.getRoadID(veh_id)
                if veh_edges.get(veh_edge):
                    veh_edges[veh_edge].append(veh_id)
                else:
                    veh_edges[veh_edge] = [veh_id]

            # remove reservations already served
            res_id_current = [res.id for res in traci.person.getTaxiReservations(0)]  # noqa
            res_id_served = list(set(res_all.keys()) - set(res_id_current))
            [res_all.pop(key) for key in res_id_served]
            [rv_dict.pop(key) for key in list(rv_dict)
             if set(res_id_served) & set(rv_dict[key][2])]

            # reservations already picked up
            res_id_picked = [res.id for res in traci.person.getTaxiReservations(8)]  # noqa

            # search reservation-vehicles pairs
            if options.debug:
                print('Calculate RV-Graph')
            get_rv(options, res_id_new, res_id_picked, res_id_served, res_all,
                   fleet, veh_type, rv_dict, step, veh_edges)

            # search trips (rtv graph)
            if options.debug:
                print('Calculate RTV-Graph')
            if options.rtv_algorithm == '0':
                rtv_dict, rtv_res, exact_sol = rtvAlgorithm.exhaustive_search(
                    options, res_id_unassigned, res_id_picked, res_all, fleet,
                    veh_type, rv_dict, step, exact_sol, veh_edges)
            elif options.rtv_algorithm == '1':
                rtv_dict, rtv_res = rtvAlgorithm.simple(
                    options, res_id_unassigned, res_id_picked, res_id_served,
                    res_all, fleet, veh_type, rv_dict, step)
            elif options.rtv_algorithm == '2':
                rtv_dict, rtv_res = rtvAlgorithm.simple_rerouting(
                    options, res_id_unassigned, res_id_picked, res_id_served,
                    res_all, fleet, veh_type, rv_dict, step)

            if len(rtv_dict) == 0:
                step += options.sim_step
                continue  # if no routes found

            elif len(rtv_dict.keys()) == 1:
                # if one vehicle darp, assign route
                best_routes = list(rtv_dict.keys())

            else:
                # if multiple vehicle darp, solve ILP with pulp
                veh_constraints = {}
                res_constraints = {}
                costs = {}
                trips = list(rtv_dict.keys())  # trips for parsing ILP solution

                # add bonus_cost to trip cost (makes trips with more served
                # reservations cheaper than splitting the reservations to more
                # vehicles with smaller trips if both strategies would yield
                # a similar cost)
                for idx, trip_id in enumerate(trips):
                    # rtv_dict[route] = [travel_time, veh_bin, res_bin, value]
                    # TODO specific cost for vehicle can be consider here
                    bonus_cost = (sum(rtv_dict[trip_id][2]) + 1) * \
                                  options.cost_per_trip
                    # generate dict with costs
                    costs.update({idx: rtv_dict[trip_id][0] + bonus_cost})
                    # generate dict with vehicle used in the trip
                    veh_constraints.update({idx: rtv_dict[trip_id][1]})
                    # generate dict with served reservations in the trip
                    res_constraints.update({idx: rtv_dict[trip_id][2]})

                if options.debug:
                    print('Solve ILP')
                ilp_result = ilp_solve(options, len(fleet), len(rtv_res),
                                       costs, veh_constraints, res_constraints)

                # parse ILP result
                best_routes = [trips[int(route_index)] for route_index in ilp_result]  # noqa

            # assign routes to vehicles
            for route_id in best_routes:
                stops = route_id.replace('y', '')
                stops = stops.replace('z', '')
                stops = stops.split("_")
                veh_id = stops[0]
                # first check if new route is better than the current one
                current_route = []
                if len(traci.vehicle.getStops(veh_id)) > 1:
                    for taxi_stop in traci.vehicle.getStops(veh_id):
                        # get reservations served at each stop
                        sub_stops = taxi_stop.actType.split(",")
                        # if more than 1 reservation in stop
                        for sub_stop in sub_stops:
                            current_route.append(sub_stop.split(" ")[2][1:-1])
                if current_route == stops[1:]:
                    # route is the same
                    continue
                elif (set(current_route) == set(stops[1:]) and
                      len(current_route) == len(stops[1:])):
                    # if route serve same reservations and have the same stops
                    # get travel time of current route
                    tt_current_route = 0
                    edges = [taxi_stop.lane.split("_")[0] for taxi_stop
                             in traci.vehicle.getStops(veh_id)]
                    # add current edge to list
                    if traci.vehicle.getRoadID(veh_id).startswith(':'):
                        # avoid routing error when in intersection TODO #5829
                        edge_index = traci.vehicle.getRouteIndex(veh_id) + 1
                        veh_edge = traci.vehicle.getRoute(veh_id)[edge_index]
                    else:
                        veh_edge = traci.vehicle.getRoadID(veh_id)

                    edges.insert(0, veh_edge)  # noqa
                    # calculate travel time
                    for idx, edge in enumerate(edges[:-1]):
                        # TODO default stop time ticket #6714
                        tt_current_route += int(findRoute(edge, edges[idx+1],
                                                veh_type, step,
                                                routingMode=options.routing_mode).travelTime) + 60
                    # get travel time of the new route
                    tt_new_route = rtv_dict[route_id][0]
                    if tt_new_route >= tt_current_route:
                        continue  # current route better than new found
                if options.debug:
                    print('Dispatch: ', route_id)
                traci.vehicle.dispatchTaxi(veh_id, stops[1:])
                # assign vehicle to reservations
                # TODO to avoid major changes in the pick-up time when assigning new passengers,  # noqa
                # tw_pickup should be updated, whit some constant X seconds, e.g. 10 Minutes  # noqa
                for res_id in set(stops[1:]):
                    res = res_all[res_id]
                    res.vehicle = veh_id

        # TODO ticket #8385
        if step > options.end_time or (traci.simulation.getMinExpectedNumber()
                                       <= 0 and not traci.person.getIDList()):
            rerouting = False

        step += options.sim_step

    if options.rtv_algorithm == 0:  # if exhaustive search
        if sum(exact_sol) == 0:
            print('Optimal solution found.')
        else:
            print('The maximal specified calculation time has been exceeded. Solution could be not optimal.')  # noqa
    print('DRT simulation ended')
    traci.close()


if __name__ == "__main__":
    main()
