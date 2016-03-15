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
Copyright (C) 2008-2015 DLR (http://www.dlr.de/) and contributors

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
try:
    import traciemb
    _embedded = True
except ImportError:
    _embedded = False

from .domain import _defaultDomains
from .connection import Connection
from .exceptions import TraCIException
from . import _inductionloop, _multientryexit, _trafficlights
from . import _lane, _vehicle, _vehicletype, _person, _route, _areal
from . import _poi, _polygon, _junction, _edge, _simulation, _gui

_connections = {}


def isEmbedded():
    return _embedded


def _STEPS2TIME(step):
    """Conversion from time steps in milliseconds to seconds as float"""
    return step / 1000.


def init(port=8813, numRetries=10, host="localhost", label="default"):
    if _embedded:
        return getVersion()
    for wait in range(1, numRetries + 2):
        try:
            sock = socket.socket()
            sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            sock.connect((host, port))
            _connections[""] = _connections[label] = Connection(sock)
            for domain in _defaultDomains:
                domain._setConnection(_connections[""])
            break
        except socket.error:
            time.sleep(wait)
    return getVersion()


def connect(port=8813, numRetries=10, host="localhost", label="default"):
    init(port, numRetries, host, label)
    return _connections.get("")


def simulationStep(step=0):
    """
    Make a simulation step and simulate up to the given millisecond in sim time.
    If the given value is 0 or absent, exactly one step is performed.
    Values smaller than or equal to the current sim time result in no action.
    """
    return _connections[""].simulationStep(step)


def getVersion():
    return _connections[""].getVersion()


def close():
    _connections[""].close()


def switch(label):
    _connections[""] = _connections[label]
    for domain in _defaultDomains:
        domain._setConnection(_connections[""])
