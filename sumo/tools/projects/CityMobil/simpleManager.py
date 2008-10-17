#!/usr/bin/env python
"""
@file    simpleManager.py
@author  Michael.Behrisch@dlr.de
@date    2008-10-09
@version $Id$

Control the CityMobil parking lot with a simple first come first serve approach.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
from optparse import OptionParser

import vehicleControl
from constants import *

class SimpleManager(vehicleControl.Manager):

    def __init__(self):
        self.cyberCarLoad = {}
        self.personsWaitingAt = {}

    def personArrived(self, personID, edge, target):
        if not edge in self.personsWaitingAt:
            self.personsWaitingAt[edge] = []
        self.personsWaitingAt[edge].append((personID, target))

    def cyberCarArrived(self, vehicleID, edge, step):
        footEdge = edge.replace("cyber", "footmain")
        wait = 0
        load = []
        for target in self.cyberCarLoad.get(vehicleID, []):
            if target == footEdge:
                wait += WAIT_PER_PERSON
            else:
                load.append(target)
        while self.personsWaitingAt.get(footEdge, []) and len(load) < CYBER_CAPACITY:
            person, target = self.personsWaitingAt[footEdge].pop(0)
            vehicleControl.leaveStop(person)
            load.append(target)
            wait += WAIT_PER_PERSON
        vehicleControl.leaveStop(vehicleID, delay=wait)
        if edge == "cyberout":
            row = -1
        else:
            row = int(edge[5])
        if row < DOUBLE_ROWS-1:
            vehicleControl.stopAt(vehicleID, "cyber%sto%s" % (row+1, row+2), ROW_DIST-15.)
        else:
            vehicleControl.stopAt(vehicleID, "cyberout", 90.)
        self.cyberCarLoad[vehicleID] = load

def main():
    vehicleControl.init(options.gui, SimpleManager(), options.verbose)
    while True:
        vehicleControl.doStep()
    close()


if __name__ == "__main__":
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-g", "--gui", action="store_true", dest="gui",
                         default=False, help="run with GUI")
    (options, args) = optParser.parse_args()
    main()
