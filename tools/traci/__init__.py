# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    __init__.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09
# @version $Id$

from __future__ import print_function
from __future__ import absolute_import
import socket
import time
import subprocess
import warnings
import abc

import sumolib  # noqa
from sumolib.miscutils import getFreeSocketPort

from .domain import _defaultDomains
from .connection import Connection, _embedded
from .exceptions import FatalTraCIError, TraCIException
from . import _inductionloop, _lanearea, _multientryexit, _trafficlight
from . import _lane, _vehicle, _vehicletype, _person, _route
from . import _poi, _polygon, _junction, _edge, _simulation, _gui

_connections = {}
_stepListeners = []


def _STEPS2TIME(step):
    """Conversion from time steps in milliseconds to seconds as float"""
    return step / 1000.


def connect(port=8813, numRetries=10, host="localhost", proc=None):
    """
    Establish a connection to a TraCI-Server and return the
    connection object. The connection is not saved in the pool and not
    accessible via traci.switch. It should be safe to use different
    connections established by this method in different threads.
    """
    for wait in range(1, numRetries + 2):
        try:
            return Connection(host, port, proc)
        except socket.error as e:
            print("Could not connect to TraCI server at %s:%s" %
                  (host, port), e)
            if wait < numRetries + 1:
                print(" Retrying in %s seconds" % wait)
                time.sleep(wait)
    raise FatalTraCIError(str(e))


def init(port=8813, numRetries=10, host="localhost", label="default"):
    """
    Establish a connection to a TraCI-Server and store it under the given
    label. This method is not thread-safe. It accesses the connection
    pool concurrently.
    """
    _connections[label] = connect(port, numRetries, host)
    switch(label)
    return getVersion()


def start(cmd, port=None, numRetries=10, label="default"):
    """
    Start a sumo server using cmd, establish a connection to it and
    store it under the given label. This method is not thread-safe.
    """
    if port is None:
        port = sumolib.miscutils.getFreeSocketPort()
    sumoProcess = subprocess.Popen(cmd + ["--remote-port", str(port)])
    _connections[label] = connect(port, numRetries, "localhost", sumoProcess)
    switch(label)
    return getVersion()


def isEmbedded():
    return _embedded


def load(args):
    """load([optionOrParam, ...])
    Let sumo load a simulation using the given command line like options 
    Example:
      load(['-c', 'run.sumocfg'])
      load(['-n', 'net.net.xml', '-r', 'routes.rou.xml'])
    """
    return _connections[""].load(args)


def simulationStep(step=0):
    """
    Make a simulation step and simulate up to the given millisecond in sim time.
    If the given value is 0 or absent, exactly one step is performed.
    Values smaller than or equal to the current sim time result in no action.
    """
    global _stepListeners
    responses = _connections[""].simulationStep(step)
    for listener in _stepListeners:
        listener.step(step)
    return responses


class StepListener(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def step(self, s=0):
        """step(int) -> None

        After adding a StepListener 'listener' with traci.addStepListener(listener),
        TraCI will call listener.step(s) after each call to traci.simulationStep(s)
        """
        pass


def addStepListener(listener):
    """addStepListener(traci.StepListener) -> bool

    Append the step listener (its step function is called at the end of every call to traci.simulationStep())
    Returns True if the listener was added successfully, False otherwise.
    """
    if issubclass(type(listener), StepListener):
        _stepListeners.append(listener)
        return True
    warnings.warn(
        "Proposed listener's type must inherit from traci.StepListener. Not adding object of type '%s'" % type(listener))
    return False


def removeStepListener(listener):
    """removeStepListener(traci.StepListener) -> bool

    Remove the step listener from traci's step listener container.
    Returns True if the listener was removed successfully, False if it wasn't registered.
    """
    if listener in _stepListeners:
        _stepListeners.remove(listener)
        return True
    warnings.warn(
        "removeStepListener(listener): listener %s not registered as step listener" % str(listener))
    return False


def getVersion():
    return _connections[""].getVersion()


def setOrder(order):
    return _connections[""].setOrder(order)


def close(wait=True):
    _connections[""].close(wait)


def switch(label):
    _connections[""] = _connections[label]
    for domain in _defaultDomains:
        domain._setConnection(_connections[""])

def getConnection(label ="default"):
    if not label in _connections:
        raise TraCIException("connection with label '%s' is not known")
    return _connections[label]

if _embedded:
    # create the default dummy connection
    init()
