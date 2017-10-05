# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _reporting.py
# @author Leonhard Luecken
# @date   2017-04-09
# @version $Id$


import sys
import traci

# control level of verbosity
VERBOSITY = 1

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

        if minVerbosityLevel <= VERBOSITY:
            if omitReportTime:
                sys.stderr.write("WARNING: " + str(msg) + " (" + self._domain + ")\n")
            else:
                sys.stderr.write(str(simTime()) + ": WARNING: " + str(msg) + " (" + self._domain + ")\n")


class Reporter(object):

    def __init__(self, domain):
        self._domain = domain

    def __call__(self, msg, minVerbosityLevel=0, omitReportTime=False):
        if minVerbosityLevel <= VERBOSITY:
            if omitReportTime:
                print(str(msg) + " (" + self._domain + ")")
            else:
                print(str(simTime()) + ": " + str(msg) + " (" + self._domain + ")")
