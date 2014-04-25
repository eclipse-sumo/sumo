#!/usr/bin/env python
"""
@file    agentManager.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2008-10-09
@version $Id$

Control the CityMobil parking lot with a multi agent system.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import vehicleControl, statistics
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
    def __init__(self, person, source, target, estCost):
        self.person = person
        self.source = source
        self.target = target
        self.estCost = estCost
        self.startStep = None
        
    def __repr__(self):
        return "<%s %s %s %s>" % (self.person.id, self.source, self.target, self.startStep)

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
        self.broken = False
        
    def request(self, person, source, target):
        if self.broken:
            estCost = INFINITY
        elif (source, target) in self.costMatrix:
            estCost = self.costMatrix[(source, target)]
        else:
            estCost = 2 * DOUBLE_ROWS * ROW_DIST / CYBER_SPEED
        self.pending[person] = Task(person, source, target, estCost)
        return self.totalEstimatedCost + estCost

    def accept(self, person):
        task = self.pending[person]
        self.tasks.append(task)
        self.totalEstimatedCost += task.estCost
        del self.pending[person]

    def reject(self, person):
        del self.pending[person]
                    

    def _findNextTarget(self, wait):
        minTarget = "z"
        minDownstreamTarget = "z"
        minSource = "z"
        minDownstreamSource = "z"
        edge = vehicleControl.getPosition(self.id)
        if edge == "cyberin":
            edge = "cyber"
        for task in self.running:
            if task.target < minTarget:
                minTarget = task.target
            if task.target < minDownstreamTarget and task.target > edge:
                minDownstreamTarget = task.target
        for task in self.tasks[:vehicleControl.getCapacity() - self.load]:
            if task.source < minSource:
                minSource = task.source
            if task.source < minDownstreamSource and task.source > edge:
                minDownstreamSource = task.source
        if minDownstreamTarget != "z":
            minTarget = minDownstreamTarget 
        elif minDownstreamSource < minTarget:
            minTarget = minDownstreamSource
        elif minTarget < edge and minSource < minTarget:
            minTarget = minSource
        if minTarget == "z":
            minTarget = "cyberin"
        if minTarget != vehicleControl.getPosition(self.id):
            vehicleControl.leaveStop(self.id, delay=wait)
            vehicleControl.stopAt(self.id, minTarget)

    def setPosition(self, edge):
        self.position = edge
        self.broken = False

    def checkBoarding(self):
        step = vehicleControl.getStep()
        wait = 0
        running = []
        for task in self.running:
            if task.target == self.position:
                statistics.personUnloaded(task.person.id, step)
                self.load -= 1
                wait += WAIT_PER_PERSON
                self.costMatrix[(task.source, task.target)] = step - task.startStep
                self.totalEstimatedCost -= task.estCost
            else:
                running.append(task)
        self.running = running
        if self.load < vehicleControl.getCapacity(): 
            tasks = []
            for task in self.tasks:
                if task.source == self.position and self.load < vehicleControl.getCapacity():
                    vehicleControl.leaveStop(task.person.id)
                    statistics.personLoaded(task.person.id, step)
                    self.load += 1
                    wait += WAIT_PER_PERSON
                    task.startStep = step
                    self.running.append(task)
                else:
                    tasks.append(task)
            self.tasks = tasks
        self._findNextTarget(wait)

    def reallocateTasks(self, cybers):
        self.broken = True
        tasks = self.tasks
        self.tasks = []
        for task in tasks:
            task.person.startRequest(task.source, task.target, cybers)

class AgentManager(vehicleControl.Manager):

    def __init__(self):
        self.agents = {}
        self.cyberCars = []

    def personArrived(self, personID, edge, target):
        if not personID in self.agents:
            person = PersonAgent(personID)
            self.agents[personID] = person
            person.startRequest(edge.replace("footmain", "cyber"),
                                target.replace("footmain", "cyber"), self.cyberCars)

    def cyberCarArrived(self, vehicleID, edge):
        if vehicleID in self.agents:
            cyberCar = self.agents[vehicleID] 
        else:
            cyberCar = CyberAgent(vehicleID)
            self.agents[vehicleID] = cyberCar
            self.cyberCars.append(cyberCar)
        cyberCar.setPosition(edge)

    def cyberCarBroken(self, vehicleID, edge):
        self.agents[vehicleID].reallocateTasks(self.cyberCars)

    def setNewTargets(self):
        for car in self.cyberCars:
            if not car.broken:
                car.checkBoarding()


if __name__ == "__main__":
    vehicleControl.init(AgentManager())
