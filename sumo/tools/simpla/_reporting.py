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

from collections import deque
import sys
import traci



def initDefaults():
    global VERBOSITY, MAX_LOG_SIZE, WARNING_LOG, REPORT_LOG
    # control level of verbosity
    # 0 - silent (only errors)
    # 1 - standard (errors and warnings)
    # 2 - log (additional status messages for platoons)
    # 3 - extended log (more status information)
    # 4 - insane (all kind of single vehicle detailed state infos)
    VERBOSITY = 1

    # log storage
    MAX_LOG_SIZE = 1000
    WARNING_LOG = deque()
    REPORT_LOG = deque()

# perform default init
initDefaults()

def simTime():
    return traci.simulation.getCurrentTime() / 1000.


def array2String(a):
    if len(a) > 0 and hasattr(a[0], "getID"):
        return str([e.getID() for e in a])
    return str([str(e) for e in a])


class Warner(object):

    def __init__(self, domain):
        self._domain = domain

    def __call__(self, msg, omitReportTime=False):
        global MAX_LOG_SIZE, WARNING_LOG
        if len(WARNING_LOG) >= MAX_LOG_SIZE:
            WARNING_LOG.popleft()
        rep = "WARNING: " + str(msg) + " (" + self._domain + ")"
        if not omitReportTime:
            rep = str(simTime()) + ": " + rep
        sys.stderr.write(rep+"\n")
        WARNING_LOG.append(rep)

class Reporter(object):

    def __init__(self, domain):
        self._domain = domain

    def __call__(self, msg, omitReportTime=False):
        global MAX_LOG_SIZE, REPORT_LOG
        if len(REPORT_LOG) >= MAX_LOG_SIZE:
            REPORT_LOG.popleft()
        rep = str(msg) + " (" + self._domain + ")"
        if not omitReportTime:
            rep = str(simTime()) + ": " + rep
        sys.stdout.write(rep+"\n")
        REPORT_LOG.append(rep)
