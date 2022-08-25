#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2021-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    drtOrtools.py
# @author  Philip Ritzer
# @date    2021-12-16

"""
Prototype online DRT algorithm using ortools via TraCi.
"""
from __future__ import print_function

import os
import sys

import numpy as np
import ortools_pdp

# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

# SUMO modules
import sumolib  # noqa
import traci  # noqa


def dispatch(reservations, fleet_empty, time_limit, cost_type='distance'):
    """Dispatch using ortools."""
    data = create_data_model(reservations, fleet_empty, cost_type)
    solution_ortools = ortools_pdp.main(data, time_limit)
    solution_requests = solution_by_requests(solution_ortools, data)
    return solution_requests


def create_data_model(reservations, fleet_empty, cost_type='distance'):
    """Creates the data for the problem."""
    n_vehicles = len(fleet_empty)
    n_reservations = len(reservations)

    edges = ['depot']
    for reservation in reservations:
        from_edge = reservation.fromEdge
        edges.append(from_edge)

    for reservation in reservations:
        to_edge = reservation.toEdge
        edges.append(to_edge)

    starts = []
    types_vehicle = []
    vehicle_capacities = []
    for id_vehicle in fleet_empty:
        edge_vehicle = traci.vehicle.getRoadID(id_vehicle)
        starts.append(edge_vehicle)
        edges.append(edge_vehicle)
        vehicle_capacities.append(traci.vehicle.getPersonCapacity(id_vehicle))
        types_vehicle.append(traci.vehicle.getTypeID(id_vehicle))
    types_vehicles_unique = list(set(types_vehicle))
    if len(types_vehicles_unique) > 1:
        raise Exception("Only one vehicle type is supported.")
        # TODO support more than one vehicle type
    else:
        type_vehicle = types_vehicles_unique[0]
    cost_matrix = get_cost_matrix(edges, type_vehicle, cost_type)

    pd_numeric = [[ii, n_reservations+ii] for ii in range(1, n_reservations+1)]
    ii = 1 + 2 * n_reservations
    starts_numeric = [jj for jj in range(ii, ii + n_vehicles)]

    data = {}
    data['cost_matrix'] = cost_matrix
    data['pickups_deliveries'] = pd_numeric
    data['num_vehicles'] = n_vehicles
    data['starts'] = starts_numeric
    data['ends'] = n_vehicles * [0]
    data['demands'] = [0] + n_reservations * [1] + n_reservations * [-1] + n_vehicles * [0]
    data['vehicle_capacities'] = vehicle_capacities
    return data


def get_cost_matrix(edges, type_vehicle, cost_type='distance'):
    """Get cost matrix between edges."""
    n_edges = len(edges)
    # index 0 is depot
    # cost to depot should be always 0
    cost_matrix = np.zeros([n_edges, n_edges])
    cost_dict = {}
    # TODO initialize cost_dict in run() and update for speed improvement
    for ii, edge_from in enumerate(edges):
        for jj, edge_to in enumerate(edges):
            if (edge_from, edge_to) in cost_dict:
                # get costs from previous call
                cost_matrix[ii][jj] = cost_dict[(edge_from, edge_to)]
                continue
            if edge_from == 'depot' or edge_to == 'depot':
                cost_matrix[ii][jj] = 0
                continue
            if ii == jj:
                cost_matrix[ii][jj] = 0
                continue
            route = traci.simulation.findRoute(edge_from, edge_to, vType=type_vehicle)
            if cost_type == 'time':
                cost_matrix[ii][jj] = route.travelTime
                cost_dict[(edge_from, edge_to)] = route.travelTime
            else:  # default is distance
                cost_matrix[ii][jj] = route.length
                cost_dict[(edge_from, edge_to)] = route.length
    return cost_matrix.tolist()


def solution_by_requests(solution_ortools, data):
    """Translate solution from ortools to SUMO requests."""
    if solution_ortools is None:
        return None
    route2request = {}
    for i_request, [i_pickup, i_delivery] in enumerate(data["pickups_deliveries"]):
        route2request[i_pickup] = i_request
        route2request[i_delivery] = i_request
    solution_requests = {}
    for key in solution_ortools:
        solution = [[],[]] # request order and costs
        for i_route in solution_ortools[key][0][1:-1]:
            if i_route in route2request:
                solution[0].append(route2request[i_route])
            else:
                continue
            solution[1] = solution_ortools[key][1] # costs
            solution_requests[key] = solution
    return solution_requests


def run(end=90000, interval=30, time_limit=10, cost_type='distance', verbose=False):
    """
    Execute the TraCI control loop and run the scenario.

    Parameters
    ----------
    end : int, optional
        Final time step of simulation. The default is 90000.
        This option can be ignored by giving a negative value.
    interval : int, optional
        Dispatching interval in s. The default is 30.
    time_limit: float, optional
        Time limit for solver in s. The default is 10.
    cost_type: str, optional
        Type of costs. The default is 'distance'. Another option is 'time'.
    verbose : bool, optional
        Controls whether debug information is printed. The default is True.
    """
    running = True
    timestep = traci.simulation.getTime()
    while running:

        traci.simulationStep(timestep)

        if timestep > end:
            running = False
            continue

        if not traci.vehicle.getTaxiFleet(-1) and timestep < end:
            timestep += interval
            continue

        # TODO why is this needed prior to getTaxiReservations(2/4/8)?
        traci.person.getTaxiReservations(0)
        if verbose:
            print("timestep: ", timestep)
            res_waiting = [res.id for res in traci.person.getTaxiReservations(2)]
            res_pickup = [res.id for res in traci.person.getTaxiReservations(4)]
            res_transport = [res.id for res in traci.person.getTaxiReservations(8)]
            if res_waiting:
                print("Reservations waiting:", res_waiting)
            if res_pickup:
                print("Reservations being picked up:", res_pickup)
            if res_transport:
                print("Reservations en route:", res_transport)
            fleet_empty = traci.vehicle.getTaxiFleet(0)
            fleet_pickup = traci.vehicle.getTaxiFleet(1)
            fleet_occupied = traci.vehicle.getTaxiFleet(2)
            fleet_occupied_pickup = traci.vehicle.getTaxiFleet(3)
            if fleet_empty:
                print("Taxis empty:", fleet_empty)
            if fleet_pickup:
                print("Taxis picking up:", fleet_pickup)
            if fleet_occupied:
                print("Taxis occupied:", fleet_occupied)
            if fleet_occupied_pickup:
                print("Taxis occupied and picking up:", fleet_occupied_pickup)

        fleet_empty = traci.vehicle.getTaxiFleet(0)
        reservations = traci.person.getTaxiReservations(2)

        if reservations and fleet_empty:
            if verbose:
                print("Solve CPDP")
            solution_requests = dispatch(reservations, fleet_empty, time_limit, cost_type)
            if solution_requests is not None:
                for index_vehicle in solution_requests:
                    id_vehicle = fleet_empty[index_vehicle]
                    reservations_order = [reservations[index].id for index in solution_requests[index_vehicle][0]]
                    traci.vehicle.dispatchTaxi(id_vehicle, reservations_order)
                    if verbose:
                        print("Dispatching %s with %s" % (id_vehicle, reservations_order))
                        print("Costs for %s: %s" % (id_vehicle, solution_requests[index_vehicle][1]))
            else:
                if verbose:
                    print("Found no solution, continue...")

        timestep += interval

    # Finish
    traci.close()
    sys.stdout.flush()


def get_arguments():
    """Get command line arguments."""
    argument_parser = sumolib.options.ArgumentParser()
    argument_parser.add_argument("-s", "--sumo-config", required=True, help="sumo config file to run")
    argument_parser.add_argument("-e", "--end", type=float, default=90000,
                                 help="time step to end simulation at")
    argument_parser.add_argument("-i", "--interval", type=float, default=30,
                                 help="dispatching interval in s")
    argument_parser.add_argument("-n", "--nogui", action="store_true", default=False,
                                 help="run the commandline version of sumo")
    argument_parser.add_argument("-v", "--verbose", action="store_true", default=False,
                                 help="print debug information")
    argument_parser.add_argument("-t", "--time-limit", type=float, default=10,
                                 help="time limit for solver in s")
    argument_parser.add_argument("-d", "--cost-type", default="distance",
                                 help="type of costs to minimize (distance or time)")
    arguments = argument_parser.parse_args()
    return arguments


if __name__ == "__main__":

    arguments = get_arguments()

    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run
    if arguments.nogui:
        sumoBinary = sumolib.checkBinary('sumo')
    else:
        sumoBinary = sumolib.checkBinary('sumo-gui')

    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs

    traci.start([sumoBinary, "-c", arguments.sumo_config])

    run(arguments.end, arguments.interval,
        arguments.time_limit, arguments.cost_type, arguments.verbose)
