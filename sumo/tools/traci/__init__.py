# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael Behrisch
@author  Lena Kalleske
@author  Mario Krumnow
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
from __future__ import absolute_import
import socket
import time
import subprocess

import sumolib
from .domain import _defaultDomains
from .connection import Connection, _embedded
from .exceptions import FatalTraCIError, TraCIException
from . import _inductionloop, _lanearea, _multientryexit, _trafficlights
from . import _lane, _vehicle, _vehicletype, _person, _route
from . import _poi, _polygon, _junction, _edge, _simulation, _gui

_connections = {}


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


def simulationStep(step=0):
    """
    Make a simulation step and simulate up to the given millisecond in sim time.
    If the given value is 0 or absent, exactly one step is performed.
    Values smaller than or equal to the current sim time result in no action.
    """
    return _connections[""].simulationStep(step)


def getVersion():
    return _connections[""].getVersion()


def close(wait=True):
    _connections[""].close(wait)


def switch(label):
    _connections[""] = _connections[label]
    for domain in _defaultDomains:
        domain._setConnection(_connections[""])


if _embedded:
    # create the default dummy connection
    init()
