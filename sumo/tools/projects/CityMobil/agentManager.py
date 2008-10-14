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
                if minCar:
                    minCar.reject(self)
                minCar = car
                minCost = cost
            else:
                car.reject(self)
        minCar.accept(self)

class CyberAgent:
    def __init__(self, id):
        self.id = id
        self.load = 0
        self.pending = {}
        self.tasks = []
        self.costMatrix = {}
        self.totalEstimatedCost = 0
        
    def request(self, person, source, target):
        self.pending[person] = (person, source, target)
        if (source, target) in self.costMatrix:
            return self.totalEstimatedCost + self.costMatrix[(source, target)]
        else:
            return self.totalEstimatedCost + 2 * DOUBLE_ROWS * ROW_DIST / CYBER_SPEED

    def accept(self, person, source, target):
        self.tasks.append(self.pending[person])

    def reject(self, person, source, target):
        del self.pending[person]

class AgentManager(vehicleControl.Manager):

    def __init__(self):
        self.agents = {}
        self.cyberCars = []

    def personArrived(self, personID, edge):
        if not personID in self.agents:
            person = PersonAgent(personID)
            self.agents[personID] = person
            person.startRequest(edge, "cyberout", self.cyberCars)

    def cyberCarArrived(self, vehicleID, edge, step):
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
