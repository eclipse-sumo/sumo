# -*- coding: utf-8 -*-
"""
@file    exceptions.py
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


class TraCIException(Exception):

    """Exception class for all TraCI errors which keep the connection intact"""

    def __init__(self, command, errorType, desc):
        Exception.__init__(self, desc)
        self._command = command
        self._type = errorType

    def getCommand(self):
        return self._command

    def getType(self):
        return self._type


class FatalTraCIError(Exception):

    """Exception class for all TraCI errors which do not allow for continuation"""

    def __init__(self, desc):
        Exception.__init__(self, desc)
