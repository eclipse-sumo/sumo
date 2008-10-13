#!/usr/bin/env python
"""
@file    agentManager.py
@author  Michael.Behrisch@dlr.de
@date    2008-10-09
@version $Id$

Control the CityMobil parking lot with a multi agent system.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import sys
from optparse import OptionParser

import vehicleControl
from constants import *

class PersonAgent:
    def __init__(self, id):
        self.id = id
        
    def startRequest(self, source, target, cybers):
        minCost = INFINITY
        minCar = None
        for car in cybers:
            cost = car.request(self, source, target) 
            if (cost < minCost):
                minCar = car
                minCost = cost
        for car in cybers:
            if car == minCar:
                car.accept(self, source, target)
            else: 
                car.reject(self, source, target)

class CyberAgent:
    def __init__(self, id):
        self.id = id
        self.load = 0
        self.tasks = []
        
    def request(self, person, source, target):
        return INFINITY

    def accept(self, person, source, target):
        self.tasks.append((person, source, target))

    def reject(self, person, source, target):
        pass

class AgentManager(vehicleControl.Manager):

    def __init__(self):
        self.agents = {}
        self.cyberCars = []

    def personArrived(self, personID, edge):
        if not personID in self.agents:
            person = PersonAgent(personID)
            self.agents[personID] = person
            person.startRequest(edge, "cyberout", self.cyberCars)

    def cyberCarArrived(self, vehicleID, edge, pos):
        if not vehicleID in self.agents:
            cyberCar = CyberAgent(vehicleID)
            self.agents[vehicleID] = cyberCar
            self.cyberCars.append(cyberCar)

def main():
    vehicleControl.init(options.gui, AgentManager(), options.verbose)
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
