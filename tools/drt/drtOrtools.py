#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2021-2023 German Aerospace Center (DLR) and others.
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
# @author  Johannes Rummel
# @date    2021-12-16

"""
Prototype online DRT algorithm using ortools via TraCI.
"""
from __future__ import print_function
from enum import Enum

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

verbose = False


class CostType(Enum):
    DISTANCE = 1
    TIME = 2


def dispatch(reservations, fleet, time_limit, cost_type, drf, verbose):
    """Dispatch using ortools."""
    if verbose:
        print('Start creating the model.')
    data = create_data_model(reservations, fleet, cost_type, drf, verbose)
    if verbose:
        print('Start solving the problem.')
    solution_ortools = ortools_pdp.main(data, time_limit, verbose)
    if verbose:
        print('Start interpreting the solution for SUMO.')
    solution_requests = solution_by_requests(solution_ortools, reservations, data, verbose)
    return solution_requests


def create_data_model(reservations, fleet, cost_type, drf, verbose):
    """Creates the data for the problem."""
    n_vehicles = len(fleet)
    # use only reservations that haven't been picked up yet; reservation.state!=8 (not picked up)
    dp_reservations = [res for res in reservations if res.state != 8]
    n_dp_reservations = len(dp_reservations)
    if verbose:
        print('dp reservations: %s' % ([res.id for res in dp_reservations]))
    # use only reservations that already haven been picked up; reservation.state==8 (picked up)
    do_reservations = [res for res in reservations if res.state == 8]
    n_do_reservations = len(do_reservations)
    if verbose:
        print('do reservations: %s' % ([res.id for res in do_reservations]))

    # edges: [depot_id, res_from_id, ..., res_to_id, ..., res_dropoff_id, ..., veh_start_id, ...]
    edges = ['depot']
    for reservation in dp_reservations:
        from_edge = reservation.fromEdge
        edges.append(from_edge)
        # add new attribute 'from_node' to the reservation
        setattr(reservation, 'from_node', len(edges) - 1)
        if verbose:
            print('Reservation %s starts at edge %s' % (reservation.id, from_edge))
    for reservation in dp_reservations:
        to_edge = reservation.toEdge
        edges.append(to_edge)
        # add new attribute 'to_node' to the reservation
        setattr(reservation, 'to_node', len(edges) - 1)
        if verbose:
            print('Reservation %s ends at edge %s' % (reservation.id, to_edge))
    for reservation in dp_reservations:
        if reservation.state == 1 | reservation.state == 2:
            setattr(reservation, 'is_new', True)
    for reservation in do_reservations:
        to_edge = reservation.toEdge
        edges.append(to_edge)
        # add new attribute 'to_node' to the reservation
        setattr(reservation, 'to_node', len(edges) - 1)
        if verbose:
            print('Drop-off of reservation %s at edge %s' % (reservation.id, to_edge))

    starts = []
    types_vehicle = []
    vehicle_capacities = []
    for id_vehicle in fleet:
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
    pickup_indices = range(1, 1 + n_dp_reservations)
    dropoff_indices = range(1 + n_dp_reservations, 1 + 2*n_dp_reservations + n_do_reservations)
    cost_matrix, time_matrix = get_cost_matrix(edges, type_vehicle, cost_type, pickup_indices, dropoff_indices)

    # add "direct route cost" to the requests:
    for res in reservations:
        if hasattr(res, 'from_node'):
            setattr(res, 'direct_route_cost', cost_matrix[res.from_node][res.to_node])
        else:
            # TODO: use 'historical data' from dict in get_cost_matrix instead
            direct_route_cost = traci.simulation.findRoute(res.fromEdge, res.toEdge, vType=type_vehicle)
            setattr(res, 'direct_route_cost', direct_route_cost)

    # add "current route cost" to the already picked up reservations:
    for res in do_reservations:
        person_id = res.persons[0]
        stage = traci.person.getStage(person_id, 0)
        # stage type 3 is defined as 'driving'
        assert(stage.type == 3)
        # print("travel time: ", stage.travelTime)
        # print("travel length: ", stage.length)
        # print("travel cost: ", stage.cost)
        if cost_type == CostType.DISTANCE:
            setattr(res, 'current_route_cost', stage.length)
        elif cost_type == CostType.TIME:
            setattr(res, 'current_route_cost', stage.travelTime)
        else:
            raise ValueError("Cannot set given cost ('%s')." % (cost_type))

    # pd_nodes = list([from_node, to_node, is_new])
    # start from_node with 1 (0 is for depot)
    # pd_nodes = [[ii+1, n_dp_reservations+ii+1, (dp_reservations[ii].state == 1 | dp_reservations[ii].state == 2)]
    #             for ii in range(0, n_dp_reservations)]
    # do_node = list(dropoff_node)
    # do_nodes = [ii + 1 + 2*n_dp_reservations for ii in range(0, n_do_reservations)]
    ii = 1 + 2*n_dp_reservations + n_do_reservations
    # node to start from
    start_nodes = [jj for jj in range(ii, ii + n_vehicles)]

    # increase demand (load) of the vehicle for each outstanding drop off
    veh_demand = [0] * n_vehicles
    for v_i, id_vehicle in enumerate(fleet):
        for reservation in do_reservations:
            entered_persons = traci.vehicle.getPersonIDList(id_vehicle)
            if reservation.persons[0] in entered_persons:
                veh_demand[v_i] += 1
                setattr(reservation, 'vehicle', id_vehicle)  # id of assigned vehicle (from SUMO input)
                setattr(reservation, 'vehicle_index', v_i)  # index of assigned vehicle [0, ..., n_v -1]

    # get time windows
    time_windows = get_time_windows(reservations, fleet)

    data = {}
    data['cost_matrix'] = cost_matrix
    data['time_matrix'] = time_matrix
    data['pickups_deliveries'] = dp_reservations
    data['dropoffs'] = do_reservations
    data['num_vehicles'] = n_vehicles
    data['starts'] = start_nodes
    data['ends'] = n_vehicles * [0]  # end at 'depot', which is is anywere
    data['demands'] = [0] + n_dp_reservations*[1] + n_dp_reservations*[-1] + n_do_reservations*[-1] + veh_demand
    data['vehicle_capacities'] = vehicle_capacities
    data['drf'] = drf
    data['time_windows'] = time_windows
    return data


def get_time_windows(reservations, fleet):
    """returns a list of pairs with earliest and latest time"""
    # order must be the same as for the cost_matrix and demands
    # edges: [depot_id, res_from_id, ..., res_to_id, ..., res_dropoff_id, ..., veh_start_id, ...]
    time_windows = []
    # start at depot should be the current simulation time:
    current_time = round(traci.simulation.getTime())
    max_time = get_max_time()
    time_windows.append((current_time, max_time))
    # use reservations that haven't been picked up yet; reservation.state!=8 (not picked up)
    dp_reservations = [res for res in reservations if res.state != 8]
    for res in dp_reservations:
        person_id = res.persons[0]
        pickup_earliest = traci.person.getParameter(person_id, "pickup_earliest")
        if pickup_earliest:
            pickup_earliest = round(float(pickup_earliest))
        else:
            pickup_earliest = current_time
        time_windows.append((pickup_earliest, max_time))
    for res in dp_reservations:
        person_id = res.persons[0]
        dropoff_latest = traci.person.getParameter(person_id, "dropoff_latest")
        if dropoff_latest:
            dropoff_latest = round(float(dropoff_latest))
        else:
            dropoff_latest = max_time
        time_windows.append((current_time, dropoff_latest))
    # use reservations that already haven been picked up; reservation.state==8 (picked up)
    do_reservations = [res for res in reservations if res.state == 8]
    for res in do_reservations:
        person_id = res.persons[0]
        dropoff_latest = traci.person.getParameter(person_id, "dropoff_latest")
        if dropoff_latest:
            dropoff_latest = round(float(dropoff_latest))
        else:
            dropoff_latest = max_time
        time_windows.append((current_time, dropoff_latest))
    # start point of the vehicles (TODO: is that needed?)
    for _ in fleet:
        time_windows.append((current_time, max_time))
    return time_windows


def get_max_time():
    max_sim_time = traci.simulation.getEndTime()
    if max_sim_time == -1:
        return 90000
    else:
        return max_sim_time


# TODO: If cost_type is TIME, remove cost_matrix and cost_dict.
def get_cost_matrix(edges, type_vehicle, cost_type, pickup_indices, dropoff_indices):
    """Get cost matrix between edges.
    Index in cost matrix is the same as the node index of the constraint solver."""

    id_vehicle = traci.vehicle.getTaxiFleet(-1)[0]  # take a vehicle
    id_vtype = traci.vehicle.getTypeID(id_vehicle)  # take its vtype
    boardingDuration_param = traci.vehicletype.getBoardingDuration(id_vtype)
    boardingDuration = 0 if boardingDuration_param == '' else round(float(boardingDuration_param))
    pickUpDuration_param = traci.vehicle.getParameter(id_vehicle, 'device.taxi.pickUpDuration')
    pickUpDuration = 0 if pickUpDuration_param == '' else round(float(pickUpDuration_param))
    dropOffDuration_param = traci.vehicle.getParameter(id_vehicle, 'device.taxi.dropOffDuration')
    dropOffDuration = 0 if dropOffDuration_param == '' else round(float(dropOffDuration_param))
    n_edges = len(edges)
    time_matrix = np.zeros([n_edges, n_edges], dtype=int)
    cost_matrix = np.zeros([n_edges, n_edges], dtype=int)
    time_dict = {}
    cost_dict = {}
    # TODO initialize cost_dict and time_dict{} in run() and update for speed improvement
    for ii, edge_from in enumerate(edges):
        for jj, edge_to in enumerate(edges):
            if (edge_from, edge_to) in cost_dict:
                # get costs from previous call
                time_matrix[ii][jj] = time_dict[(edge_from, edge_to)]
                cost_matrix[ii][jj] = cost_dict[(edge_from, edge_to)]
                continue
            # cost to depot should be always 0
            # (means there is no way to depot in the end)
            if edge_from == 'depot' or edge_to == 'depot':
                time_matrix[ii][jj] = 0
                cost_matrix[ii][jj] = 0
                continue
            if ii == jj:
                time_matrix[ii][jj] = 0
                cost_matrix[ii][jj] = 0
                continue
            route = traci.simulation.findRoute(edge_from, edge_to, vType=type_vehicle)
            time_matrix[ii][jj] = round(route.travelTime)
            if ii in pickup_indices:
                time_matrix[ii][jj] += pickUpDuration  # add pickup_duration
                time_matrix[ii][jj] += boardingDuration  # add boarding_duration
            if jj in dropoff_indices:
                time_matrix[ii][jj] += dropOffDuration  # add dropoff_duration
            time_dict[(edge_from, edge_to)] = time_matrix[ii][jj]
            if cost_type == CostType.TIME:
                cost_matrix[ii][jj] = time_matrix[ii][jj]
                cost_dict[(edge_from, edge_to)] = time_dict[(edge_from, edge_to)]
            elif cost_type == CostType.DISTANCE:
                cost_matrix[ii][jj] = round(route.length)
                cost_dict[(edge_from, edge_to)] = round(route.length)
    return cost_matrix.tolist(), time_matrix.tolist()


def solution_by_requests(solution_ortools, reservations, data, verbose=False):
    """Translate solution from ortools to SUMO requests."""
    if solution_ortools is None:
        return None

    # dp_reservations = [res for res in reservations if res.state != 8]

    route2request = {}
    for res in data["pickups_deliveries"]:
        route2request[res.from_node] = res.id
        route2request[res.to_node] = res.id
    for res in data['dropoffs']:  # for each vehicle
        route2request[res.to_node] = res.id

    solution_requests = {}
    for key in solution_ortools:  # key is the vehicle number (0,1,...)
        solution = [[], []]  # request order and costs
        for i_route in solution_ortools[key][0][1:-1]:  # take only the routes ([0]) without the start node ([1:-1])
            if i_route in route2request:
                solution[0].append(route2request[i_route])  # add node to route
            else:
                if verbose:
                    print('!solution ignored: %s' % (i_route))
                continue
            solution[1] = solution_ortools[key][1]  # costs
            solution_requests[key] = solution
    return solution_requests


def run(end=90000, interval=30, time_limit=10, cost_type='distance', drf=1.5, verbose=False):
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

        # termination condition
        if timestep > end:
            running = False
            continue

        if not traci.vehicle.getTaxiFleet(-1) and timestep < end:
            timestep += interval
            continue

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

        fleet = traci.vehicle.getTaxiFleet(-1)
        reservations_new = traci.person.getTaxiReservations(2)
        reservations_all = traci.person.getTaxiReservations(0)

        if reservations_new:
            if verbose:
                print("Solve CPDP")
            solution_requests = dispatch(reservations_all, fleet, time_limit, cost_type, drf, verbose)
            if solution_requests is not None:
                for index_vehicle in solution_requests:  # for each vehicle
                    id_vehicle = fleet[index_vehicle]
                    reservations_order = [res_id for res_id in solution_requests[index_vehicle][0]]  # [0] for route
                    if verbose:
                        print("Dispatching %s with %s" % (id_vehicle, reservations_order))
                        print("Costs for %s: %s" % (id_vehicle, solution_requests[index_vehicle][1]))
                for index_vehicle in solution_requests:
                    id_vehicle = fleet[index_vehicle]
                    reservations_order = [res_id for res_id in solution_requests[index_vehicle][0]]
                    traci.vehicle.dispatchTaxi(id_vehicle, reservations_order)  # overwrite existing dispatch
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
    argument_parser.add_argument("-f", "--drf", type=float, default=1.5,
                                 help="direct route factor to calculate maximum cost "
                                      "for a single dropoff-pickup route (set to -1, if you do not need it)")
    arguments = argument_parser.parse_args()
    return arguments


def check_set_arguments(arguments):
    if arguments.nogui:
        arguments.sumoBinary = sumolib.checkBinary('sumo')
    else:
        arguments.sumoBinary = sumolib.checkBinary('sumo-gui')

    # set cost type
    if arguments.cost_type == "distance":
        arguments.cost_type = CostType.DISTANCE
    elif arguments.cost_type == "time":
        arguments.cost_type = CostType.TIME
    else:
        raise ValueError("Wrong cost type '%s'. Only 'distance' and 'time' are allowed." % (arguments.cost_type))

    if arguments.drf < 1 and arguments.drf != -1:
        raise ValueError("Wrong value for drf '%s'. Value must be equal or greater than 1. -1 means no drf is used.")


if __name__ == "__main__":

    arguments = get_arguments()
    check_set_arguments(arguments)
    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run

    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs
    traci.start([arguments.sumoBinary, "-c", arguments.sumo_config])
    run(arguments.end, arguments.interval,
        arguments.time_limit, arguments.cost_type, arguments.drf, arguments.verbose)
