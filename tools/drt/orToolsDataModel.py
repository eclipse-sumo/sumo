# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2021-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    orToolsDataModel.py
# @author  Johannes Rummel
# @date    2024-03-13

"""
Data model for drtOrtools.py to solve a drt problem with the ortools routing solver.
"""
# needed for type alias in python < 3.9
from __future__ import annotations
import os
import sys
import typing
from dataclasses import dataclass
from enum import Enum
import numpy as np
import math

# SUMO modules
# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import traci._person
import traci._simulation

SPEED_DEFAULT = 20  # default vehicle speed in m/s


class CostType(Enum):
    DISTANCE = 1
    TIME = 2


@dataclass
class Vehicle:
    """
    Represents a vehicle/route for the routing problem.
    """
    id_vehicle: str  # vehicle id/name from SUMO
    vehicle_index: int
    start_node: int = None
    end_node: int = None

    def get_person_capacity(self) -> int:
        return traci.vehicle.getPersonCapacity(self.id_vehicle)

    def get_type_ID(self) -> str:
        return traci.vehicle.getTypeID(self.id_vehicle)

    def get_edge(self) -> str:
        return traci.vehicle.getRoadID(self.id_vehicle)

    def get_person_id_list(self) -> list[str]:
        return traci.vehicle.getPersonIDList(self.id_vehicle)


@dataclass
class Reservation:
    """
    Represents a request for a transportation.
    """
    reservation: traci._person.Reservation
    from_node: int = None
    to_node: int = None
    direct_route_cost: int = None
    current_route_cost: int = None
    vehicle: Vehicle = None

    def is_new(self) -> bool:
        if self.reservation.state == 1 or self.reservation.state == 2:
            return True
        else:
            return False

    def is_picked_up(self) -> bool:
        return self.reservation.state == 8

    def is_from_node(self, node: int) -> bool:
        return (not self.is_picked_up() and self.from_node == node)

    def is_to_node(self, node: int) -> bool:
        return self.to_node == node

    def get_from_edge(self) -> str:
        return self.reservation.fromEdge

    def get_to_edge(self) -> str:
        return self.reservation.toEdge

    def get_id(self) -> str:
        return self.reservation.id

    def get_persons(self) -> list[str]:
        return self.reservation.persons

    def get_earliest_pickup(self) -> int:
        person_id = self.get_persons()[0]
        pickup_earliest = (traci.person.getParameter(person_id, "pickup_earliest")
                           or traci.person.getParameter(person_id, "earliestPickupTime"))
        if pickup_earliest:
            pickup_earliest = round(float(pickup_earliest))
        return pickup_earliest

    def get_dropoff_latest(self) -> int:
        person_id = self.get_persons()[0]
        dropoff_latest = (traci.person.getParameter(person_id, "dropoff_latest")
                          or traci.person.getParameter(person_id, "latestDropoffTime"))
        if dropoff_latest:
            dropoff_latest = round(float(dropoff_latest))
        return dropoff_latest

    def update_direct_route_cost(self, type_vehicle: str, cost_matrix: list[list[int]] = None,
                                 cost_type: CostType = CostType.DISTANCE):
        if self.direct_route_cost:
            return
        if not self.is_picked_up():
            self.direct_route_cost = cost_matrix[self.from_node][self.to_node]
        else:
            # TODO: use 'historical data' from dict in get_cost_matrix instead
            route: traci._simulation.Stage = traci.simulation.findRoute(
                self.get_from_edge(), self.get_to_edge(), vType=type_vehicle)
            if cost_type == CostType.TIME:
                self.direct_route_cost = round(route.travelTime)
            elif cost_type == CostType.DISTANCE:
                self.direct_route_cost = round(route.length)
            else:
                raise ValueError(f"Cannot set given cost ({cost_type}).")

    def update_current_route_cost(self, cost_type: CostType = CostType.DISTANCE):
        person_id = self.reservation.persons[0]
        stage: traci._simulation.Stage = traci.person.getStage(person_id, 0)
        # stage type '3' is defined as 'driving'
        assert stage.type == 3
        if cost_type == CostType.DISTANCE:
            self.current_route_cost = round(stage.length)
        elif cost_type == CostType.TIME:
            self.current_route_cost = round(stage.travelTime)
        else:
            raise ValueError(f"Cannot set given cost ({cost_type}).")


@dataclass
class ORToolsDataModel:
    """
    Data model class used by constrains of the OR-tools lib.
    """
    # nodeID of the depot
    depot: int
    cost_matrix: list[list[int]]
    time_matrix: list[list[int]]
    pickups_deliveries: list[Reservation]
    dropoffs: list[Reservation]
    num_vehicles: int
    starts: list[int]
    ends: list[int]
    demands: list[int]
    vehicle_capacities: list[int]
    drf: float
    waiting_time: int
    time_windows: list[(int, int)]
    fix_allocation: bool
    max_time: int
    initial_routes: dict[int: list[list[int]]]
    penalty: int
    reservations: list[Reservation]
    vehicles: list[Vehicle]
    cost_type: CostType

    def __str__(self):
        return f'number of vehicles: {self.num_vehicles}, ...'

    def get_penalty(self, explicitly_time_related: bool = False) -> int:
        """Returns penalty. If explicitly time related, it depends on the CostType of the data."""
        if not explicitly_time_related:
            return self.penalty
        if self.cost_type == CostType.DISTANCE:
            return round(self.penalty * SPEED_DEFAULT)
        else:
            return self.penalty


@dataclass
class Node:
    """
    Connects an object of the routing problem with a nodeID.
    """
    class NodeType(Enum):
        FROM_EDGE = 1
        TO_EDGE = 2
        VEHICLE = 3
        DEPOT = 4

    # id: int = field(default_factory=...)
    node_type: NodeType


# use 'type' statement in python version 3.12 or higher
NodeObject = typing.Union[str, Vehicle, Reservation]


def create_nodes(reservations: list[Reservation], vehicles: list[Vehicle]) -> list[NodeObject]:
    """
    Sets the node ids from 0...n for the locations of the start and
    end points of the reservations and vehicles.
    """
    n = 0  # reserved for depot (which can also be a free location)
    node_objects = ['depot']
    n += 1
    for res in reservations:
        if not res.is_picked_up():
            node_objects.append(res)
            res.from_node = n
            n += 1
        node_objects.append(res)
        res.to_node = n
        n += 1
    for veh in vehicles:
        node_objects.append(veh)
        veh.start_node = n
        n += 1
        veh.end_node = 0  # currently all vehicles end at depot
        # TODO: to generalize the end nodes, separate nodes are needed
    return node_objects


def create_vehicles(fleet: list[str]) -> list[Vehicle]:
    vehicles = []
    for i, veh_id in enumerate(fleet):
        veh = Vehicle(veh_id, i)
        vehicles.append(veh)
    return vehicles


# list[traci.Person.Reservation]
def create_new_reservations(data_reservations: list[Reservation]) -> list[Reservation]:
    """create Reservations that not already exist"""
    sumo_reservations = traci.person.getTaxiReservations(0)  # TODO: state 1 should be enough

    data_reservations_ids = [res.get_id() for res in data_reservations]
    new_reservations = []
    for res in sumo_reservations:
        if res.id not in data_reservations_ids:
            new_reservations.append(Reservation(res))
    return new_reservations


def update_reservations(data_reservations: list[Reservation]) -> list[Reservation]:
    """update the Reservation.reservation and also remove Reservations that are completed"""
    sumo_reservations: tuple[traci._person.Reservation] = traci.person.getTaxiReservations(0)
    updated_reservations = []
    for data_reservation in data_reservations:
        new_res = [res for res in sumo_reservations if res.id == data_reservation.get_id()]
        if new_res:
            data_reservation.reservation = new_res[0]
            updated_reservations.append(data_reservation)
    return updated_reservations


def reject_late_reservations(data_reservations: list[Reservation], waiting_time: int,
                             timestep: float) -> tuple[list[Reservation], list[Reservation]]:
    """
    rejects reservations that are not assigned to a vehicle and cannot be served by time

    Returns a cleared list and a list of the removed reservations.
    """
    new_data_reservations = []
    rejected_reservations = []
    for data_reservation in data_reservations:
        if not data_reservation.vehicle and data_reservation.reservation.reservationTime + waiting_time < timestep:
            for person in data_reservation.get_persons():
                traci.person.removeStages(person)
            rejected_reservations.append(data_reservation)
        else:
            new_data_reservations.append(data_reservation)
    return new_data_reservations, rejected_reservations


def map_vehicles_to_reservations(vehicles: list[Vehicle], reservations: list[Reservation]) -> None:
    """
    Sets the vehicle attribute of the reservations with the vehicle that contains the same persons.
    """
    for vehicle in vehicles:
        persons_in_vehicle = vehicle.get_person_id_list()
        for reservation in reservations:
            if reservation.get_persons()[0] in persons_in_vehicle:
                reservation.vehicle = vehicle


def get_edge_of_node_object(node_object: NodeObject, node: int) -> str | None:
    """
    Returns the edge of the given NodeObject. "node" is needed for Reservations,
    to make clear if the edge of the departure or destination is searched.
    Returns "None" if an edge cannot be found.
    """
    if isinstance(node_object, Vehicle):
        return node_object.get_edge()
    if isinstance(node_object, Reservation):
        if node_object.is_from_node(node):
            return node_object.get_from_edge()
        if node_object.is_to_node(node):
            return node_object.get_to_edge()
    return None


def get_demand_of_node_object(node_object: NodeObject, node: int) -> int | None:
    """
    Returns "None" if node is not from_node or to_node of a reservation.
    """
    if isinstance(node_object, str) and node_object == 'depot':
        return 0
    if isinstance(node_object, Vehicle):
        return traci.vehicle.getPersonNumber(node_object.id_vehicle)
    if isinstance(node_object, Reservation):
        if node_object.is_from_node(node):
            return 1
        if node_object.is_to_node(node):
            return -1
    return None


# TODO: If cost_type is TIME, remove cost_matrix and cost_dict.
def get_cost_matrix(node_objects: list[NodeObject], cost_type: CostType):
    """Get cost matrix between edges.
    Index in cost matrix is the same as the node index of the constraint solver."""

    # get vehicle type of one vehicle (we suppose all vehicles are of the same type)
    type_vehicle, id_vehicle = next(((x.get_type_ID(), x.id_vehicle)
                                    for x in node_objects if isinstance(x, Vehicle)), None)
    boardingDuration_param = traci.vehicletype.getBoardingDuration(type_vehicle)
    boardingDuration = 0 if boardingDuration_param == '' else round(float(boardingDuration_param))
    # TODO: pickup and dropoff duration of first vehicle is used for all vehicles!!!
    pickUpDuration_param = traci.vehicle.getParameter(id_vehicle, 'device.taxi.pickUpDuration')
    pickUpDuration = 0 if pickUpDuration_param == '' else round(float(pickUpDuration_param))
    dropOffDuration_param = traci.vehicle.getParameter(id_vehicle, 'device.taxi.dropOffDuration')
    dropOffDuration = 0 if dropOffDuration_param == '' else round(float(dropOffDuration_param))
    n_edges = len(node_objects)
    time_matrix = np.zeros([n_edges, n_edges], dtype=int)
    cost_matrix = np.zeros([n_edges, n_edges], dtype=int)
    time_dict = {}
    cost_dict = {}
    # TODO initialize cost_dict and time_dict{} in run() and update for speed improvement
    for ii, from_node_object in enumerate(node_objects):
        edge_from = get_edge_of_node_object(from_node_object, ii)
        for jj, to_node_object in enumerate(node_objects):
            edge_to = get_edge_of_node_object(to_node_object, jj)
            # cost to depot should be always 0
            # (means there is no way to depot in the end)
            if from_node_object == 'depot' or to_node_object == 'depot':
                time_matrix[ii][jj] = 0
                cost_matrix[ii][jj] = 0
                continue
            if ii == jj:
                time_matrix[ii][jj] = 0
                cost_matrix[ii][jj] = 0
                continue
            if (edge_from, edge_to) in cost_dict:
                # get costs from previous call
                time_matrix[ii][jj] = time_dict[(edge_from, edge_to)]
                cost_matrix[ii][jj] = cost_dict[(edge_from, edge_to)]
                continue
            # TODO: findRoute is not needed between two vehicles
            route: traci._simulation.Stage = traci.simulation.findRoute(edge_from, edge_to, vType=type_vehicle)
            time_matrix[ii][jj] = round(route.travelTime)
            if isinstance(from_node_object, Reservation) and from_node_object.is_from_node(ii):
                time_matrix[ii][jj] += pickUpDuration  # add pickup_duration
                time_matrix[ii][jj] += boardingDuration  # add boarding_duration
            if isinstance(to_node_object, Reservation) and to_node_object.is_to_node(jj):
                time_matrix[ii][jj] += dropOffDuration  # add dropoff_duration
            time_dict[(edge_from, edge_to)] = time_matrix[ii][jj]
            if cost_type == CostType.TIME:
                cost_matrix[ii][jj] = time_matrix[ii][jj]
                cost_dict[(edge_from, edge_to)] = time_dict[(edge_from, edge_to)]
            elif cost_type == CostType.DISTANCE:
                cost_matrix[ii][jj] = round(route.length)
                cost_dict[(edge_from, edge_to)] = round(route.length)
    return cost_matrix.tolist(), time_matrix.tolist()


def get_time_window_of_node_object(node_object: NodeObject, node: int, end: int) -> tuple[int, int]:
    """returns a pair with earliest and latest service time"""
    current_time = round(traci.simulation.getTime())
    max_time = round(end)

    time_window = None
    if isinstance(node_object, str) and node_object == 'depot':
        time_window = (current_time, max_time)
    elif isinstance(node_object, Vehicle):
        # TODO: throws an exception if not set: traci.vehicle.getParameter(node_object.id_vehicle, 'device.taxi.end')
        device_taxi_end = max_time
        time_window_end = max_time if device_taxi_end == '' else round(float(device_taxi_end))
        time_window = (current_time, time_window_end)
    elif isinstance(node_object, Reservation):
        if node_object.is_from_node(node):
            pickup_earliest = node_object.get_earliest_pickup() or current_time
            time_window = (pickup_earliest, max_time)
        if node_object.is_to_node(node):
            dropoff_latest = node_object.get_dropoff_latest() or max_time
            time_window = (current_time, dropoff_latest)
    else:
        raise ValueError(f"Cannot set time window for node {node}.")
    return time_window


def get_vehicle_by_vehicle_index(vehicles: list[Vehicle], index: int) -> Vehicle:
    for vehicle in vehicles:
        if vehicle.vehicle_index == index:
            return vehicle
    return None


def get_reservation_by_node(reservations: list[Reservation], node: int) -> Reservation:
    for reservation in reservations:
        if reservation.is_from_node(node) or reservation.is_to_node(node):
            return reservation
    return None


def get_penalty(penalty_factor: str | int, cost_matrix: list[list[int]]) -> int:
    if penalty_factor == 'dynamic':
        max_cost = max(max(sublist) for sublist in cost_matrix)
        return round_up_to_next_power_of_10(max_cost)
    else:
        return penalty_factor


def round_up_to_next_power_of_10(n: int) -> int:
    if n < 0:
        raise ValueError(f"Input '{n}' must be a positive integer")
    if n == 0:
        return 1
    # Determine the number of digits of the input value
    num_digits = math.floor(math.log10(n)) + 1
    scale = 10 ** (num_digits - 1)
    leading_digit = n // scale
    # If the input value is not already a power of 10, increase the leading digit by 1
    if n % scale != 0:
        leading_digit += 1
    rounded_value = leading_digit * scale
    return rounded_value
