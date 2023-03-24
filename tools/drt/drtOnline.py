#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
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
# @date    2021-02-15

"""
Simulate Demand Responsive Transport via TraCi
Track progress https://github.com/eclipse/sumo/issues/8256
"""

from __future__ import print_function
import os
import sys
from argparse import ArgumentParser
from itertools import combinations
import subprocess
import shutil

import pulp as pl
import darpSolvers

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
from sumolib import checkBinary  # noqa
from sumolib.xml import parse_fast_nested  # noqa
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
    ap.add_argument("--darp-solver", default='exhaustive_search',
                    help="Method to solve the DARP problem. Available: exhaustive_search and simple_rerouting")
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
                    help="Maximum waiting time for pickup (default 900 seconds)")
    ap.add_argument("--max-processing", type=int,
                    help="Maximum number of attempts to process a request (default unlimited)")
    ap.add_argument("--sim-step", type=int, default=30,
                    help="Step time to collect new reservations (default 30 seconds)")
    ap.add_argument("--end-time", type=int, default=90000,
                    help="Maximum simulation time to close Traci (default 90000 sec - 25h)")
    ap.add_argument("--routing-algorithm", default='dijkstra',
                    help="Algorithm for shortest path routing. Support: dijkstra (default), astar, CH and CHWrapper")
    ap.add_argument("--routing-mode", type=int, default=0,
                    help="Mode for shortest path routing. Support: 0 (default) for routing with loaded or default speeds and 1 for routing with averaged historical speeds")  # noqa
    ap.add_argument("--dua-times", action='store_true',
                    help="Calculate travel time between edges with duarouter")
    ap.add_argument("--tracefile",
                    help="log traci commands to the given FILE")
    ap.add_argument("--tracegetters", action='store_true',
                    help="include get-methods in tracefile")
    ap.add_argument("--verbose", action='store_true')

    return ap


def find_dua_times(options):
    """
    Get all combinations between start and end reservation edges and calculates
    the travel time between all combinations with duarouter in an empty net.
    """

    edge_pair_time = {}
    os.mkdir('temp_dua')

    # define trips between all edge combinations
    route_edges = []
    for person, reservation in parse_fast_nested(options.reservations,
                                                 "person", "depart", "ride",
                                                 ("from", "to", "lines")):
        route_edges.extend((reservation.attr_from, reservation.to))

    combination_edges = combinations(set(route_edges), 2)
    with open("temp_dua/dua_file.xml", "w+") as dua_file:
        dua_file.write("<routes>\n")
        for comb_edges in list(combination_edges):
            dua_file.write('\t<trip id="%s_%s" depart="0" from="%s" to="%s"/>\n'
                           % (comb_edges[0], comb_edges[1], comb_edges[0], comb_edges[1]))
            dua_file.write('\t<trip id="%s_%s" depart="0" from="%s" to="%s"/>\n'
                           % (comb_edges[1], comb_edges[0], comb_edges[1], comb_edges[0]))
        dua_file.write("</routes>\n")

    # run duarouter:
    duarouter = checkBinary('duarouter')

    subprocess.call([duarouter, "-n", options.network, "--route-files",
                     "temp_dua/dua_file.xml", "-o", "temp_dua/dua_output.xml",
                     "--ignore-errors", "true", "--no-warnings", "true",
                     "--bulk-routing", "true"])

    # parse travel time between edges
    with open("edges_pair_graph.xml", "w+") as pair_file:
        for trip, route in parse_fast_nested("temp_dua/dua_output.alt.xml",
                                             "vehicle", "id", "route", "cost",
                                             optional=True):
            if route.cost:
                edge_pair_time[trip.id] = float(route.cost)
                pair_file.write('<pair id="%s" cost="%s"/>\n'
                                % (trip.id, float(route.cost)))

    # remove extra dua files
    shutil.rmtree('temp_dua')

    return edge_pair_time


def ilp_solve(options, veh_num, res_num, costs, veh_constraints,
              res_constraints):
    """
    Solves the integer linear programming to define the best routes for each
    vehicle. Only implemented for problems with multiple vehicles.
    """
    # founds the combination of trips that minimize the costs function

    # req_costs = [reservation.cost for reservation in reservations] # TODO to implement req with diff costs/priorities
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

    # The problem is solved using PuLP's Solver choice
    try:
        prob.solve(pl.PULP_CBC_CMD(msg=0, timeLimit=options.ilp_time))
    except pl.apis.core.PulpSolverError:
        prob.solve(pl.COIN_CMD(msg=0, timeLimit=options.ilp_time, path="/usr/bin/cbc"))

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

    if options.sumo == 'sumo':
        SUMO = checkBinary('sumo')
    else:
        SUMO = checkBinary('sumo-gui')

    # start traci
    if options.sumocfg:
        run_traci = [SUMO, "-c", options.sumocfg,
                     '--tripinfo-output.write-unfinished',
                     '--routing-algorithm', options.routing_algorithm]
    else:
        run_traci = [SUMO, '--net-file', '%s' % options.network, '-r',
                     '%s,%s' % (options.reservations, options.taxis), '-l',
                     'log.txt', '--device.taxi.dispatch-algorithm', 'traci',
                     '--tripinfo-output', '%s' % options.output,
                     '--tripinfo-output.write-unfinished',
                     '--routing-algorithm', options.routing_algorithm,
                     '--stop-output', 'stops_%s' % options.output]
        if options.gui_settings:
            run_traci.extend(['-g', '%s' % options.gui_settings])

    if options.dua_times:
        if options.verbose:
            print('Calculate travel time between edges with duarouter')
            if not options.reservations:
                sys.exit("please specify the reservation file with the option '--reservations'")
            if not options.network:
                sys.exit("please specify the sumo network file with the option '--network'")
        pairs_dua_times = find_dua_times(options)
    else:
        pairs_dua_times = {}

    traci.start(run_traci, traceFile=options.tracefile, traceGetters=options.tracegetters)

    # execute the TraCI control loop
    step = traci.simulation.getTime() + 10
    veh_type = None
    rerouting = True
    while rerouting:
        traci.simulationStep(step)

        # TODO ticket #8385
        if not traci.vehicle.getTaxiFleet(-1) and step < options.end_time:
            step += options.sim_step
            continue

        # get vType and its parameters for route calculation
        if not veh_type:
            fleet = traci.vehicle.getTaxiFleet(-1)
            veh_type = traci.vehicle.getTypeID(fleet[0])
            veh_time_pickup = float(traci.vehicle.getParameter(fleet[0],
                                    'device.taxi.pickUpDuration'))
            veh_time_dropoff = float(traci.vehicle.getParameter(fleet[0],
                                     'device.taxi.dropOffDuration'))

        # get new reservations
        res_id_new = []
        for res in traci.person.getTaxiReservations(1):

            # search direct travel time
            direct = pairs_dua_times.get("%s_%s" % (res.fromEdge, res.toEdge))
            if direct is None:
                direct = int(findRoute(res.fromEdge, res.toEdge, veh_type,
                             res.depart, routingMode=options.routing_mode).travelTime)

            # add new reservation attributes
            setattr(res, 'direct', direct)  # direct travel time
            setattr(res, 'vehicle', False)  # id of assigned vehicle
            setattr(res, 'delay', 0)  # real pick up time - assigned time

            # read extra attributes
            person_id = res.persons[0]
            pickup_earliest = traci.person.getParameter(person_id,
                                                        "pickup_earliest")
            if pickup_earliest:
                pickup_earliest = float(pickup_earliest)
            dropoff_latest = traci.person.getParameter(person_id,
                                                       "dropoff_latest")
            if dropoff_latest:
                dropoff_latest = float(dropoff_latest)
            max_waiting = traci.person.getParameter(person_id, "max_waiting")
            if max_waiting:
                max_waiting = float(max_waiting)

            # calculates time windows
            if not max_waiting:
                # take global value
                max_waiting = options.max_wait
            if pickup_earliest and dropoff_latest:
                # set latest pickup based on waiting time or latest drop off
                pickup_latest = min(pickup_earliest + max_waiting,
                                    dropoff_latest - direct)
                # if drop off time given, set time window based on waiting time
                dropoff_earliest = max(pickup_earliest + direct,
                                       dropoff_latest - max_waiting)
            elif dropoff_latest:
                # if latest drop off given, calculate pickup window based
                # on max. travel time with drf
                if res.direct*options.drf < options.drf_min:
                    pickup_earliest = max(res.depart,
                                          dropoff_latest - options.drf_min)
                else:
                    pickup_earliest = max(res.depart,
                                          dropoff_latest - direct*options.drf)
                pickup_latest = max(pickup_earliest, dropoff_latest - direct)
                dropoff_earliest = max(pickup_earliest + direct,
                                       dropoff_latest - max_waiting)
            else:
                if not pickup_earliest:
                    # if no time was given
                    pickup_earliest = res.depart
                # set earliest drop off based on pickup and max. travel time
                dropoff_earliest = pickup_earliest + direct
                # check if min travel time or drf must be applied
                if res.direct*options.drf < options.drf_min:
                    dropoff_latest = pickup_earliest + direct + options.drf_min
                else:
                    dropoff_latest = pickup_earliest + direct*options.drf
                # set latest pickup based on waiting time
                pickup_latest = min(dropoff_latest - direct,
                                    pickup_earliest + max_waiting)

            # add time window attributes
            setattr(res, 'tw_pickup', [pickup_earliest, pickup_latest])
            setattr(res, 'tw_dropoff', [dropoff_earliest, dropoff_latest])

            # time out for request processing
            if options.max_processing:
                setattr(res, 'max_processing',
                        step + options.max_processing*options.sim_step)
            else:
                setattr(res, 'max_processing', pickup_latest+options.sim_step)

            # add reservation id to new reservations
            res_id_new.append(res.id)
            # add reservation object to list
            res_all[res.id] = res

        # find unassigned reservations and
        # remove reservations which have exceeded the processing time
        res_id_unassigned = []
        res_id_proc_exceeded = []
        for res_key, res_values in res_all.items():
            if not res_values.vehicle:
                if step >= res_values.max_processing:
                    res_id_proc_exceeded.append(res_key)
                    print("\nProcessing time for reservation %s -person %s- was exceeded. Reservation can not be served" % (res_key, res_values.persons))  # noqa
                    for person in res_values.persons:
                        traci.person.removeStages(person)
                else:
                    res_id_unassigned.append(res_key)

        # remove reservations
        [res_all.pop(key) for key in res_id_proc_exceeded]

        # if reservations pending
        if res_id_unassigned:
            if options.verbose:
                print('\nRun dispatcher')
                if res_id_new:
                    print('New reservations:', sorted(res_id_new))
                print('Pending reservations:',
                      sorted(set(res_id_unassigned)-set(res_id_new)))

            # get fleet
            fleet = traci.vehicle.getTaxiFleet(-1)
            if set(fleet) != set(fleet) & set(traci.vehicle.getIDList()):  # TODO manage teleports
                print("\nVehicle %s is being teleported, skip to next step" %
                      (set(fleet) - set(traci.vehicle.getIDList())))
                step += options.sim_step
                continue  # if a vehicle is being teleported skip to next step

            # find vehicle in same edge, avoid calculating same routes twice
            veh_edges = {}
            for veh_id in fleet:
                if traci.vehicle.getRoadID(veh_id).startswith(':'):
                    # avoid routing error when in intersection TODO #5829
                    edge_index = traci.vehicle.getRouteIndex(veh_id) + 1
                    veh_edge = traci.vehicle.getRoute(veh_id)[edge_index]
                else:
                    veh_edge = traci.vehicle.getRoadID(veh_id)
                if veh_edges.get(veh_edge) is not None:
                    veh_edges[veh_edge].append(veh_id)
                else:
                    veh_edges[veh_edge] = [veh_id]

            # remove reservations already served
            res_id_current = [res.id for res in traci.person.getTaxiReservations(0)]
            res_id_served = list(set(res_all.keys()) - set(res_id_current))
            [res_all.pop(key) for key in res_id_served]

            # reservations already picked up
            res_id_picked = [res.id for res in traci.person.getTaxiReservations(8)]

            # call DARP solver to find the best routes
            if options.verbose:
                print('Solve DARP with %s' % options.darp_solver)

            darp_solution = darpSolvers.main(options, step, fleet, veh_type,
                                             veh_time_pickup, veh_time_dropoff,
                                             res_all, res_id_new,
                                             res_id_unassigned, res_id_picked,
                                             veh_edges,
                                             pairs_dua_times)
            routes, ilp_res_cons, exact_sol = darp_solution

            if len(routes) == 0:
                step += options.sim_step
                continue  # if no routes found

            elif ilp_res_cons is None:
                # if DARP solver found optimal routes, this is, no ILP needed
                best_routes = list(routes.keys())

            else:
                # if DARP solver not optimized routes, run Integer Linear
                # Programming with pulp
                veh_constraints = {}
                res_constraints = {}
                costs = {}
                trips = list(routes.keys())  # trips for parsing ILP solution

                # add bonus_cost to trip cost (makes trips with more served
                # reservations cheaper than splitting the reservations to more
                # vehicles with smaller trips if both strategies would yield
                # a similar cost)
                for idx, trip_id in enumerate(trips):
                    # routes[route] = [travel_time, veh_bin, res_bin, value]
                    # TODO specific cost for vehicle can be consider here
                    bonus_cost = (sum(routes[trip_id][2]) + 1) * \
                                  options.cost_per_trip
                    # generate dict with costs
                    costs.update({idx: routes[trip_id][0] + bonus_cost})
                    # generate dict with vehicle used in the trip
                    veh_constraints.update({idx: routes[trip_id][1]})
                    # generate dict with served reservations in the trip
                    res_constraints.update({idx: routes[trip_id][2]})

                if options.verbose:
                    print('Solve ILP')
                ilp_result = ilp_solve(options, len(fleet), len(ilp_res_cons),
                                       costs, veh_constraints, res_constraints)

                # parse ILP result
                best_routes = [trips[int(route_index)]
                               for route_index in ilp_result]

            # assign routes to vehicles
            for route_id in best_routes:
                stops = route_id.replace('y', '')
                stops = stops.replace('z', '')
                stops = stops.split("_")
                veh_id = stops[0]
                # first check if new route is better than the current one
                current_route = []
                if traci.vehicle.getStops(veh_id):
                    for taxi_stop in traci.vehicle.getStops(veh_id):
                        next_act = taxi_stop.actType.split(",")[0].split(" ")[0]
                        if not next_act:
                            # vehicle doesn't have a current route
                            continue
                        next_id = taxi_stop.actType.split(",")[0].split(" ")[-1][1:-1]
                        if next_act == 'pickup' and next_id in res_id_picked:
                            # person already picked up, consider next stop
                            continue
                        elif next_act == 'dropOff' and next_id not in res_all.keys():
                            # person already dropped off, consider next stop
                            continue
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
                    tt_current_route = step
                    edges = [taxi_stop.lane.split("_")[0] for taxi_stop
                             in traci.vehicle.getStops(veh_id)]
                    stop_types = [taxi_stop.actType for taxi_stop
                                  in traci.vehicle.getStops(veh_id)]
                    # add current edge to list
                    if traci.vehicle.getRoadID(veh_id).startswith(':'):
                        # avoid routing error when in intersection TODO #5829
                        edge_index = traci.vehicle.getRouteIndex(veh_id) + 1
                        veh_edge = traci.vehicle.getRoute(veh_id)[edge_index]
                    else:
                        veh_edge = traci.vehicle.getRoadID(veh_id)

                    edges.insert(0, veh_edge)
                    # calculate travel time
                    for idx, edge in enumerate(edges[:-1]):
                        tt_pair = pairs_dua_times.get("%s_%s" % (edge,
                                                      edges[idx+1]))
                        if tt_pair is None:
                            tt_pair = int(findRoute(edge, edges[idx+1],
                                          veh_type, step, routingMode=options.routing_mode).travelTime)

                        if 'pickup' in stop_types[idx]:
                            tt_current_route += tt_pair + veh_time_pickup
                        else:
                            tt_current_route += tt_pair + veh_time_dropoff
                    # get travel time of the new route
                    tt_new_route = routes[route_id][0]
                    if tt_new_route >= tt_current_route:
                        continue  # current route better than new found
                if options.verbose:
                    print('Dispatch:', route_id)
                traci.vehicle.dispatchTaxi(veh_id, stops[1:])
                # assign vehicle to reservations
                # TODO to avoid major changes in the pick-up time when assigning new passengers,
                # tw_pickup should be updated, whit some constant X seconds, e.g. 10 Minutes
                for res_id in set(stops[1:]):
                    res = res_all[res_id]
                    res.vehicle = veh_id

        # TODO ticket #8385
        if step > options.end_time:
            rerouting = False

        step += options.sim_step

    if all(exact_sol):
        print('\nExact solution found.')
    else:
        print('\nApproximate solution found.')
    print('DRT simulation ended')
    traci.close()


if __name__ == "__main__":
    main()
