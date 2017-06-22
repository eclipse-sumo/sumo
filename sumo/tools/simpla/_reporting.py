"""
@author Leonhard Luecken
@date   2017-04-09

-----------------------------------
SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
-----------------------------------
"""

import simpla._config as cfg
import sys
import traci


def simTime():
    return traci.simulation.getCurrentTime() / 1000.


def array2String(a):
    if len(a) > 0 and hasattr(a[0], "getID"):
        return str([e.getID() for e in a])
    return str([str(e) for e in a])


class Warner(object):

    def __init__(self, domain):
        self._domain = domain

    def __call__(self, msg, minVerbosityLevel=0, omitReportTime=False):

        if minVerbosityLevel <= cfg.VERBOSITY:
            if omitReportTime:
                sys.stderr.write("WARNING: " + str(msg) + " (" + self._domain + ")\n")
            else:
                sys.stderr.write(str(simTime()) + ": WARNING: " + str(msg) + " (" + self._domain + ")\n")


class Reporter(object):

    def __init__(self, domain):
        self._domain = domain

    def __call__(self, msg, minVerbosityLevel=0, omitReportTime=False):
        if minVerbosityLevel <= cfg.VERBOSITY:
            if omitReportTime:
                print(str(msg) + " (" + self._domain + ")")
            else:
                print(str(simTime()) + ": " + str(msg) + " (" + self._domain + ")")
