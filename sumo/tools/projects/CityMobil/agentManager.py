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

class Task:
    def __init__(self, person, source, target):
        self.person = person
        self.source = source
        self.target = target
        self.startStep = None

class CyberAgent:
    def __init__(self, id):
        self.id = id
        self.load = 0
        self.pending = {}
        self.tasks = []
        self.running = []
        self.costMatrix = {}
        self.totalEstimatedCost = 0
        self.position = None
        
    def request(self, person, source, target):
        self.pending[person] = Task(person, source, target)
        if (source, target) in self.costMatrix:
            return self.totalEstimatedCost + self.costMatrix[(source, target)]
        else:
            return self.totalEstimatedCost + 2 * DOUBLE_ROWS * ROW_DIST / CYBER_SPEED

    def accept(self, person):
        task = self.pending[person]
        self.tasks.append(task)
        if len(self.tasks) == 1:
            if self.position == task.source:
                self.checkBoarding(task.source, 0)
            elif self.position:
                vehicleControl.leaveStop(self.id)
                vehicleControl.stopAt(self.id, task.source, ROW_DIST-15.)
        del self.pending[person]

    def reject(self, person):
        del self.pending[person]

    def _findNextTarget(self):
        minTarget = "z"
        minDownstreamTarget = "z"
        minSource = "z"
        minDownstreamSource = "z"
        for task in self.running:
            if task.target < minTarget:
                minTarget = task.target
            if task.target < minDownstreamTarget and task.target > self.position:
                minDownstreamTarget = task.target
            if task.source < minSource:
                minSource = task.source
            if task.source < minDownstreamSource and task.source > self.position:
                minDownstreamSource = task.source
        if minDownstreamTarget != "z":
            minTarget = minDownstreamTarget 
        if self.load < CYBER_CAPACITY:
            if minDownstreamSource < minTarget:
                return minDownstreamSource
            if minTarget < self.position and minSource < minTarget:
                return minSource
        return minTarget

    def checkBoarding(self, edge, step):
        self.position = edge
        wait = 0
        running = []
        for task in self.running:
            if task.target == edge:
                self.load -= 1
                wait += WAIT_PER_PERSON
                self.costMatrix[(task.source, task.target)] = step - task.startStep
            else:
                running.append(task)
        self.running = running
        if self.load < CYBER_CAPACITY: 
            tasks = []
            for task in self.tasks:
                if task.source == edge:
                    self.load += 1
                    wait += WAIT_PER_PERSON
                    task.startStep = step
                    self.running.append(task)
                else:
                    tasks.append(task)
            self.tasks = tasks
        if self.running:
            vehicleControl.leaveStop(self.id, delay=wait)
            vehicleControl.stopAt(self.id, self._findNextTarget(), ROW_DIST-15.)

class AgentManager(vehicleControl.Manager):

    def __init__(self):
        self.agents = {}
        self.cyberCars = []

    def personArrived(self, personID, edge):
        if not personID in self.agents:
            person = PersonAgent(personID)
            self.agents[personID] = person
            person.startRequest(edge.replace("footmain", "cyber"), "cyberout", self.cyberCars)

    def cyberCarArrived(self, vehicleID, edge, step):
        if vehicleID in self.agents:
            cyberCar = self.agents[vehicleID] 
        else:
            cyberCar = CyberAgent(vehicleID)
            self.agents[vehicleID] = cyberCar
            self.cyberCars.append(cyberCar)
        cyberCar.checkBoarding(edge, step)

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
