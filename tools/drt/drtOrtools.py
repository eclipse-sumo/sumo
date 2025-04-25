#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2021-2025 German Aerospace Center (DLR) and others.
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
# @author  Mirko Barthauer
# @date    2021-12-16

"""
Prototype online DRT algorithm using ortools via TraCI.
"""
# needed for type alias in python < 3.9
from __future__ import annotations
from typing import List, Dict, Tuple

import os
import sys
import argparse

import ortools_pdp
import orToolsDataModel

# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))

# SUMO modules
import sumolib  # noqa
import traci  # noqa

PENALTY_FACTOR = 'dynamic'  # factor on penalty for rejecting requests


# use 'type' statement in python version 3.12 or higher
RequestOrder = List[str]
Cost = int
NodeOrder = List[int]
TranslatedSolutions = Dict[int, Tuple[RequestOrder, Cost, NodeOrder]]

# TODO: solution_requests is not needed as input


def dispatch(time_limit: int, solution_requests: TranslatedSolutions,
             data: orToolsDataModel.ORToolsDataModel, verbose: bool) -> TranslatedSolutions | None:
    """Dispatch using ortools."""
    if verbose:
        print('Start solving the problem.')
    solution_ortools = ortools_pdp.main(data, time_limit, verbose)
    if verbose:
        print('Start interpreting the solution for SUMO.')
    solution_requests = solution_by_requests(solution_ortools, data, verbose)
    return solution_requests


def create_data_model(sumo_fleet: list[str], cost_type: orToolsDataModel.CostType,
                      drf: float, waiting_time: int, end: int,
                      fix_allocation: bool, solution_requests: TranslatedSolutions | None, penalty_factor: str | int,
                      data_reservations: list[orToolsDataModel.Reservation],
                      timestep: float, verbose: bool) -> orToolsDataModel.ORToolsDataModel:
    """Creates the data for the problem."""
    vehicles = orToolsDataModel.create_vehicles(sumo_fleet)
    updated_reservations = orToolsDataModel.update_reservations(data_reservations)
    new_reservations = orToolsDataModel.create_new_reservations(updated_reservations)
    reservations = new_reservations + updated_reservations
    reservations, rejected_reservations = orToolsDataModel.reject_late_reservations(
        reservations, waiting_time, timestep)
    orToolsDataModel.map_vehicles_to_reservations(vehicles, reservations)
    node_objects = orToolsDataModel.create_nodes(reservations, vehicles)

    n_vehicles = len(vehicles)
    if verbose:
        if rejected_reservations:
            print(f"Reservations rejected: {[res.get_id() for res in rejected_reservations]}")
        print(f'dp reservations: {[res.get_id() for res in reservations if not res.is_picked_up()]}')
        print(f'do reservations: {[res.get_id() for res in reservations if res.is_picked_up()]}')
        for reservation in reservations:
            if not reservation.is_picked_up():
                print(f'Reservation {reservation.get_id()} starts at edge {reservation.get_from_edge()}')
        for reservation in reservations:
            if not reservation.is_picked_up():
                print(f'Reservation {reservation.get_id()} ends at edge {reservation.get_to_edge()}')
        for reservation in reservations:
            if reservation.is_picked_up():
                print(f'Drop-off of reservation {reservation.get_id()} at edge {reservation.get_to_edge()}')

    vehicle_capacities = [veh.get_person_capacity() for veh in vehicles]

    types_vehicle = [veh.get_type_ID() for veh in vehicles]
    types_vehicles_unique = list(set(types_vehicle))
    if len(types_vehicles_unique) > 1:
        raise Exception("Only one vehicle type is supported.")
        # TODO support more than one vehicle type
    else:
        type_vehicle = types_vehicles_unique[0]
    cost_matrix, time_matrix = orToolsDataModel.get_cost_matrix(node_objects, cost_type)

    # safe cost and time matrix
    if verbose:
        import csv
        with open("cost_matrix.csv", 'a') as cost_file:
            wr = csv.writer(cost_file)
            wr.writerows(cost_matrix)
        with open("time_matrix.csv", 'a') as time_file:
            wr = csv.writer(time_file)
            wr.writerows(time_matrix)

    # add "direct route cost" to the requests:
    for reservation in reservations:
        reservation.update_direct_route_cost(type_vehicle, cost_matrix, cost_type)
        if verbose:
            print(f'Reservation {reservation.get_id()} has direct route costs {reservation.direct_route_cost}')

    # add "current route cost" to the already picked up reservations:
    for reservation in reservations:
        if reservation.is_picked_up():
            reservation.update_current_route_cost(cost_type)

    start_nodes = [veh.start_node for veh in vehicles]

    demands = [orToolsDataModel.get_demand_of_node_object(node_object, node)
               for node, node_object in enumerate(node_objects)]

    # get time windows
    time_windows = [orToolsDataModel.get_time_window_of_node_object(node_object, node, end)
                    for node, node_object in enumerate(node_objects)]

    penalty = orToolsDataModel.get_penalty(penalty_factor, cost_matrix)
    if verbose:
        print(f'Penalty factor is {penalty}')

    data = orToolsDataModel.ORToolsDataModel(
        depot=0,
        cost_matrix=cost_matrix,
        time_matrix=time_matrix,
        pickups_deliveries=[res for res in reservations if not res.is_picked_up()],  # dp_reservations
        dropoffs=[res for res in reservations if res.is_picked_up()],  # do_reservations
        num_vehicles=n_vehicles,
        starts=start_nodes,
        ends=n_vehicles * [0],  # end at 'depot', which is is anywere
        demands=demands,  # [0] + n_dp_reservations*[1] + n_dp_reservations*[-1] + n_do_reservations*[-1] + veh_demand
        vehicle_capacities=vehicle_capacities,
        drf=drf,
        waiting_time=waiting_time,
        time_windows=time_windows,
        fix_allocation=fix_allocation,
        max_time=end,
        initial_routes=solution_requests,
        penalty=penalty,
        reservations=reservations,
        vehicles=vehicles,
        cost_type=cost_type
    )
    return data


def get_max_time() -> int:
    max_sim_time = traci.simulation.getEndTime()
    if max_sim_time == -1:
        return 90000
    else:
        return max_sim_time


def solution_by_requests(solution_ortools: ortools_pdp.ORToolsSolution | None,
                         data: orToolsDataModel.ORToolsDataModel, verbose: bool = False) -> TranslatedSolutions | None:
    """Translate solution from ortools to SUMO requests."""
    if solution_ortools is None:
        return None

    # dp_reservations = [res for res in reservations if res.state != 8]

    route2request = {}
    for res in data.pickups_deliveries:
        route2request[res.from_node] = res.get_id()
        route2request[res.to_node] = res.get_id()
    for res in data.dropoffs:  # for each vehicle
        route2request[res.to_node] = res.get_id()

    solution_requests: TranslatedSolutions = {}
    for key in solution_ortools:  # key is the vehicle number (0,1,...)
        request_order: RequestOrder = []
        node_order: NodeOrder = []
        for i_route in solution_ortools[key][0][1:-1]:  # take only the routes ([0]) without the start node ([1:-1])
            if i_route in route2request:
                request_order.append(route2request[i_route])  # add request id to route
                # [res for res in data["pickups_deliveries"]+data['dropoffs']
                #      if res.get_id() == route2request[i_route]][0]  # get the reservation
                res = orToolsDataModel.get_reservation_by_node(data.pickups_deliveries+data.dropoffs, i_route)
                res.vehicle = orToolsDataModel.get_vehicle_by_vehicle_index(data.vehicles, key)
            else:
                if verbose:
                    print(f'!solution ignored: {i_route}')
                continue
            node_order.append(i_route)  # node
        cost: Cost = solution_ortools[key][1]  # cost
        solution_requests[key] = (request_order, cost, node_order)
    return solution_requests


def run(penalty_factor: str | int, end: int = None, interval: int = 30, time_limit: float = 10,
        cost_type: orToolsDataModel.CostType = orToolsDataModel.CostType.DISTANCE,
        drf: float = 1.5, waiting_time: int = 900, fix_allocation: bool = False, verbose: bool = False):
    """
    Execute the TraCI control loop and run the scenario.

    Parameters
    ----------
    penalty_factor: 'dynamic' | int
        Penalty for rejecting requests or exceeding drf, time windows and waiting times.
        If 'dynamic' then the factor is set depending on the maximum cost.
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
    if not end:
        end = get_max_time()

    if verbose:
        print('Simulation parameters:')
        print(f'  end: {end}')
        print(f'  interval: {interval}')
        print(f'  time_limit: {time_limit}')
        print(f'  cost_type: {cost_type}')
        print(f'  drf: {drf}')
        print(f'  waiting_time: {waiting_time}')
        print(f'  fix_allocation: {fix_allocation}')

    solution_requests = None
    data_reservations = list()
    while running:

        traci.simulationStep(timestep)

        # termination condition
        if timestep > end:
            running = False
            continue

        if not traci.vehicle.getTaxiFleet(-1) and timestep < end:
            timestep += interval
            continue

        if verbose:
            print(f"timestep: {timestep}")
            res_waiting = [res.id for res in traci.person.getTaxiReservations(3)]
            res_pickup = [res.id for res in traci.person.getTaxiReservations(4)]
            res_transport = [res.id for res in traci.person.getTaxiReservations(8)]
            if res_waiting:
                print(f"Reservations waiting: {res_waiting}")
            if res_pickup:
                print(f"Reservations being picked up: {res_pickup}")
            if res_transport:
                print(f"Reservations en route: {res_transport}")
            fleet_empty = traci.vehicle.getTaxiFleet(0)
            fleet_pickup = traci.vehicle.getTaxiFleet(1)
            fleet_occupied = traci.vehicle.getTaxiFleet(2)
            fleet_occupied_pickup = traci.vehicle.getTaxiFleet(3)
            if fleet_empty:
                print(f"Taxis empty: {fleet_empty}")
            if fleet_pickup:
                print(f"Taxis picking up: {fleet_pickup}")
            if fleet_occupied:
                print(f"Taxis occupied: {fleet_occupied}")
            if fleet_occupied_pickup:
                print(f"Taxis occupied and picking up: {fleet_occupied_pickup}")

        fleet = traci.vehicle.getTaxiFleet(-1)
        # take reservations, that are not assigned to a taxi (state 1: new + state 2: already retrieved)
        reservations_not_assigned = traci.person.getTaxiReservations(3)

        # if reservations_all:  # used for debugging
        if reservations_not_assigned:
            if verbose:
                print("Solve CPDP")
            if verbose:
                print('Start creating the model.')
            data = create_data_model(fleet, cost_type, drf, waiting_time, int(end),
                                     fix_allocation, solution_requests, penalty_factor,
                                     data_reservations, timestep, verbose)
            data_reservations = data.reservations
            solution_requests = dispatch(time_limit, solution_requests, data, verbose)
            if solution_requests is not None:
                for index_vehicle, vehicle_requests in solution_requests.items():  # for each vehicle
                    id_vehicle = fleet[index_vehicle]
                    reservations_order = [res_id for res_id in vehicle_requests[0]]  # [0] for route
                    if verbose:
                        print(f"Dispatching {id_vehicle} with {reservations_order}")
                        print(f"Costs for {id_vehicle}: {vehicle_requests[1]}")
                    if fix_allocation and not reservations_order:  # ignore empty reservations if allocation is fixed
                        continue
                    traci.vehicle.dispatchTaxi(id_vehicle, reservations_order)  # overwrite existing dispatch
            else:
                if verbose:
                    print("Found no solution, continue...")

        timestep += interval

    # Finish
    traci.close()
    sys.stdout.flush()


def dynamic_or_int(value):
    if value == 'dynamic':
        return value
    try:
        return int(value)
    except ValueError:
        raise ValueError(f"Wrong value for penalty factor '{value}'. Must be 'dynamic' or an integer.")


def get_arguments() -> argparse.Namespace:
    """Get command line arguments."""
    ap = sumolib.options.ArgumentParser()
    ap.add_argument("-s", "--sumo-config", required=True, category="input", type=ap.file,
                    help="sumo config file to run")
    ap.add_argument("-e", "--end", type=ap.time,
                    help="time step to end simulation at")
    ap.add_argument("-i", "--interval", type=ap.time, default=30,
                    help="dispatching interval in s")
    ap.add_argument("-n", "--nogui", action="store_true", default=False,
                    help="run the commandline version of sumo")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="print debug information")
    ap.add_argument("-t", "--time-limit", type=ap.time, default=10,
                    help="time limit for solver in s")
    ap.add_argument("-d", "--cost-type", default="distance",
                    help="type of costs to minimize (distance or time)")
    ap.add_argument("-f", "--drf", type=float, default=1.5,
                    help="direct route factor to calculate maximum cost "
                         "for a single dropoff-pickup route (set to -1, if you do not need it)")
    ap.add_argument("-a", "--fix-allocation", action="store_true", default=False,
                    help="if true: after first solution the allocation of reservations to vehicles" +
                    "does not change anymore")
    ap.add_argument("-w", "--waiting-time", type=ap.time, default=900,
                    help="maximum waiting time to serve a request in s")
    ap.add_argument("-p", "--penalty-factor", type=dynamic_or_int, default=PENALTY_FACTOR,
                    help="factor on penalty for rejecting requests, must be 'dynamic' or an integer (e.g. 100000)")
    ap.add_argument("--trace-file", type=ap.file,
                    help="log file for TraCI debugging")
    return ap.parse_args()


def check_set_arguments(arguments: argparse.Namespace):
    if arguments.nogui:
        arguments.sumoBinary = sumolib.checkBinary('sumo')
    else:
        arguments.sumoBinary = sumolib.checkBinary('sumo-gui')

    # set cost type
    if arguments.cost_type == "distance":
        arguments.cost_type = orToolsDataModel.CostType.DISTANCE
    elif arguments.cost_type == "time":
        arguments.cost_type = orToolsDataModel.CostType.TIME
    else:
        raise ValueError(f"Wrong cost type '{arguments.cost_type}'. Only 'distance' and 'time' are allowed.")

    if arguments.drf < 1 and arguments.drf != -1:
        raise ValueError(
            f"Wrong value for drf '{arguments.drf}'. Value must be equal or greater than 1. -1 means no drf is used.")

    if arguments.waiting_time < 0:
        raise ValueError(
            f"Wrong value for waiting time '{arguments.waiting_time}'. Value must be equal or greater than 0.")


if __name__ == "__main__":

    arguments = get_arguments()
    check_set_arguments(arguments)
    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run

    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs
    traci.start([arguments.sumoBinary, "-c", arguments.sumo_config], traceFile=arguments.trace_file)

    run(arguments.penalty_factor, arguments.end, arguments.interval,
        arguments.time_limit, arguments.cost_type, arguments.drf,
        arguments.waiting_time, arguments.fix_allocation, arguments.verbose)
