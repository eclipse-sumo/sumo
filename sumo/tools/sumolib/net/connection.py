"""
@file    connection.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Karol Stosiek
@author  Michael Behrisch
@date    2011-11-28
@version $Id$

This file contains a Python-representation of a single connection.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
class Connection:
    """edge connection for a sumo network"""
    def __init__(self, fromEdge, toEdge, fromLane, toLane, direction, tls, tllink):
        self._from = fromEdge
        self._to = toEdge
        self._fromLane = fromLane
        self._toLane = toLane
        self._tls = tls
        self._tlLink = tllink
