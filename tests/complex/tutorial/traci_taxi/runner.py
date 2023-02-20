#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Melanie Weber
# @author  Lara Kaiser
# @date    2021-03-29


from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import optparse


# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

from sumolib import checkBinary  # noqa
import traci  # noqa
import sumolib  # noqa


# Creating 4 taxis
def createTaxi(time):
    for i in range(0, 20, 5):
        # declaring the name(unique), route(from demand.route.xml), type of vehicle(declared in demand.route.xml),
        # depart time, and line
        traci.vehicle.add('taxiV%i' % i, 'route_0',
                          'taxi', depart=time, line='taxi')


# Dispatching taxis to cater to people waiting at a busstop
def emergencyTaxi(busstopID):
    # getting a Id-list of people waiting at the busstop
    peopleWaiting = traci.busstop.getPersonIDs(busstopID)
    print("PersonId: \n", peopleWaiting)
    pickup = []
    # creating a list with the taxi resevations
    for i, val in enumerate(peopleWaiting):
        pickup.append(traci.person.getTaxiReservations(0)[i].id)
    # if one Taxi should pick up all customers, the list needs to clarify the drop off
    # hence the pickup is extendet by the order of drop offs
    # pickup.extend(pickup)
    print("PickupList:\n", pickup)

    try:
        fleet = traci.vehicle.getTaxiFleet(0)
    except (traci.exceptions.FatalTraCIError):
        print("No unoccupied taxi-fleet!")
    # dispatching the unoccupied taxis to pick up their designated customers
    for i, val in enumerate(peopleWaiting):
        print("Taxifleet-Status")
        print("empty: ", traci.vehicle.getTaxiFleet(0))
        print("pickup: ", traci.vehicle.getTaxiFleet(1))
        traci.vehicle.dispatchTaxi(fleet[i], pickup[i])


# checks if busstop is "overflowing"
def busstopCheck():
    # getting all bus stops on the map
    busstops = traci.busstop.getIDList()
    # checking the personcount of waiting people
    for i in busstops:
        if traci.busstop.getPersonCount(i) >= 4:
            emergencyTaxi(i)


def run():
    """execute the TraCI control loop"""
    step = 0
    time = 90
    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep()
        step += 1
        if step == time:
            createTaxi(time)
        if step == 150:
            busstopCheck()
        if step == 5000:
            break
    traci.close()
    sys.stdout.flush()


def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("--nogui", action="store_true",
                         default=False, help="run the commandline version of sumo")
    options, args = optParser.parse_args()
    return options


if __name__ == "__main__":
    # first, generate the route file for this simulation
    options = get_options()
    # If you want to run this tutorial please uncomment following lines, that define the sumoBinary
    # and delete the line before traci.start, to use the gui
    # if options.nogui:
    #    sumoBinary = checkBinary('sumo')
    # else:
    #    sumoBinary = checkBinary('sumo-gui')
    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs
    sumoBinary = checkBinary('sumo')
    traci.start([sumoBinary, "-c", "data/sumo.sumocfg",
                 "--tripinfo-output", "data/tripinfo.xml"])
    run()
