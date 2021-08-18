# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2018-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    __init__.py
# @author  Michael Behrisch
# @date    2020-10-08

import sys
import os
if hasattr(os, "add_dll_directory"):
    # since Python 3.8 the DLL search path has to be set explicitly see https://bugs.python.org/issue43173
    if "SUMO_HOME" in os.environ:
        os.add_dll_directory(os.path.join(os.environ["SUMO_HOME"], "bin"))
    os.add_dll_directory(os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "bin")))

from traci import connection, constants, exceptions, _vehicle, _person, _trafficlight, _simulation, _gui  # noqa
from traci.connection import StepListener  # noqa
from .libtraci import vehicle, simulation, person, trafficlight, gui
from .libtraci import *  # noqa
from .libtraci import TraCIStage, TraCINextStopData, TraCIReservation, TraCILogic, TraCIPhase, TraCIException
from .libtraci import TraCICollision, TraCISignalConstraint

_DOMAINS = [
    busstop,  # noqa
    calibrator,  # noqa
    chargingstation,  # noqa
    edge,  # noqa
    gui,
    inductionloop,  # noqa
    junction,  # noqa
    lanearea,  # noqa
    lane,  # noqa
    meandata,  # noqa
    multientryexit,  # noqa
    overheadwire,  # noqa
    parkingarea,  # noqa
    person,
    poi,  # noqa
    polygon,  # noqa
    rerouter,  # noqa
    route,  # noqa
    routeprobe,  # noqa
    simulation,
    trafficlight,
    variablespeedsign,  # noqa
    vehicle,
    vehicletype,  # noqa
]

hasGUI = simulation.hasGUI
init = simulation.init
load = simulation.load
isLoaded = simulation.isLoaded
getVersion = simulation.getVersion
close = simulation.close
start = simulation.start
setOrder = simulation.setOrder
_stepListeners = {}
_nextStepListenerID = 0


def wrapAsClassMethod(func, module):
    def wrapper(*args, **kwargs):
        return func(module, *args, **kwargs)
    return wrapper


TraCIStage.__attr_repr__ = _simulation.Stage.__attr_repr__
TraCIStage.__repr__ = _simulation.Stage.__repr__
TraCIStage.toXML = _simulation.Stage.toXML
TraCICollision.__attr_repr__ = _simulation.Collision.__attr_repr__
TraCICollision.__repr__ = _simulation.Collision.__repr__

TraCINextStopData.__attr_repr__ = _vehicle.StopData.__attr_repr__
TraCINextStopData.__repr__ = _vehicle.StopData.__repr__

TraCIReservation.__attr_repr__ = _person.Reservation.__attr_repr__
TraCIReservation.__repr__ = _person.Reservation.__repr__

TraCILogic.getPhases = _trafficlight.Logic.getPhases
TraCILogic.__repr__ = _trafficlight.Logic.__repr__
TraCIPhase.__repr__ = _trafficlight.Phase.__repr__
TraCISignalConstraint.__repr__ = _trafficlight.Constraint.__repr__

exceptions.TraCIException = TraCIException
simulation.Stage = TraCIStage
vehicle.StopData = TraCINextStopData
person.Reservation = TraCIReservation
trafficlight.Phase = TraCIPhase
trafficlight.Logic = TraCILogic
vehicle.addFull = vehicle.add
vehicle.addLegacy = wrapAsClassMethod(_vehicle.VehicleDomain.addLegacy, vehicle)
vehicle.couldChangeLane = wrapAsClassMethod(_vehicle.VehicleDomain.couldChangeLane, vehicle)
vehicle.wantsAndCouldChangeLane = wrapAsClassMethod(_vehicle.VehicleDomain.wantsAndCouldChangeLane, vehicle)
vehicle.isStopped = wrapAsClassMethod(_vehicle.VehicleDomain.isStopped, vehicle)
vehicle.setBusStop = wrapAsClassMethod(_vehicle.VehicleDomain.setBusStop, vehicle)
vehicle.setParkingAreaStop = wrapAsClassMethod(_vehicle.VehicleDomain.setParkingAreaStop, vehicle)
vehicle.getRightFollowers = wrapAsClassMethod(_vehicle.VehicleDomain.getRightFollowers, vehicle)
vehicle.getRightLeaders = wrapAsClassMethod(_vehicle.VehicleDomain.getRightLeaders, vehicle)
vehicle.getLeftFollowers = wrapAsClassMethod(_vehicle.VehicleDomain.getLeftFollowers, vehicle)
vehicle.getLeftLeaders = wrapAsClassMethod(_vehicle.VehicleDomain.getLeftLeaders, vehicle)
vehicle.getLaneChangeStatePretty = wrapAsClassMethod(_vehicle.VehicleDomain.getLaneChangeStatePretty, vehicle)
person.removeStages = wrapAsClassMethod(_person.PersonDomain.removeStages, person)
_trafficlight.TraCIException = TraCIException
trafficlight.setLinkState = wrapAsClassMethod(_trafficlight.TrafficLightDomain.setLinkState, trafficlight)
addStepListener = wrapAsClassMethod(connection.Connection.addStepListener, sys.modules[__name__])
removeStepListener = wrapAsClassMethod(connection.Connection.removeStepListener, sys.modules[__name__])
_manageStepListeners = wrapAsClassMethod(connection.Connection._manageStepListeners, sys.modules[__name__])


def isLibsumo():
    return False


def isLibtraci():
    return True


vehicle._legacyGetLeader = True


def setLegacyGetLeader(enabled):
    _vehicle._legacyGetLeader = enabled


_libtraci_step = simulation.step


def simulationStep(step=0):
    _libtraci_step(step)
    result = []
    for domain in _DOMAINS:
        result += [(k, v) for k, v in domain.getAllSubscriptionResults().items()]
        result += [(k, v) for k, v in domain.getAllContextSubscriptionResults().items()]
    _manageStepListeners(step)
    return result


simulation.step = simulationStep
gui.DEFAULT_VIEW = _gui.GuiDomain.DEFAULT_VIEW
