#!/usr/bin/env python
"""
@file    simpleManager.py
@author  Michael.Behrisch@dlr.de
@date    2008-10-09
@version $Id$

Control the CityMobil parking lot via TraCI.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
from optparse import OptionParser

import vehicleControl, traciControl
from constants import *

class CyberCar:
    def __init__(self):
        self.parking = False
        self.load = 0

class SimpleManager(vehicleControl.Manager):

    def __init__(self):
        self.cyberCars = {}
        self.waiting = {}

    def personArrived(self, vehicleID, edge):
        if not edge in self.waiting:
            self.waiting[edge] = []
        self.waiting[edge].append(vehicleID)

    def cyberCarArrived(self, vehicleID, edge, pos):
        if not vehicleID in self.cyberCars:
            self.cyberCars[vehicleID] = CyberCar()
        cyberCar = self.cyberCars[vehicleID]
        if edge != "cyberin":
            if pos < 70:
                footEdge = edge.replace("cyber", "footmain")
                if not cyberCar.parking:
                    if cyberCar.load < CYBER_CAPACITY:                
                        if footEdge in self.waiting and self.waiting[footEdge]:
                            vehicleControl.stopAt(vehicleID, edge, ROW_DIST-15.)
                            cyberCar.parking = True
                else:
                    if pos >= ROW_DIST - 20:
                        while self.waiting[footEdge] and cyberCar.load < CYBER_CAPACITY:
                            person = self.waiting[footEdge].pop(0)
                            vehicleControl.leaveStop(person)
                            cyberCar.load += 1
                        vehicleControl.leaveStop(vehicleID, "cyberout")
                        cyberCar.parking = False
        if edge == "cyberout" and pos >= 70 and not cyberCar.parking:
            traciControl.changeTarget("cyberin", vehicleID)
            traciControl.stopObject(edge, vehicleID, 90., cyberCar.load * 5.)
            cyberCar.parking = True
        if edge == "cyberin" and cyberCar.parking:
            cyberCar.parking = False
            cyberCar.load = 0
            traciControl.changeTarget("cyberout", vehicleID)

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
