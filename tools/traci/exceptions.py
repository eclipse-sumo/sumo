# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    exceptions.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09
# @version $Id$

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
