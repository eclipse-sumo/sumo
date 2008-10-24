#!/usr/bin/env python
"""
@file    statistics.py
@author  Michael.Behrisch@dlr.de
@date    2008-10-17
@version $Id$

Collecting statistics for the CityMobil parking lot

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import numpy, math

persons = {}
personsRunning = 0

class Person:
    def __init__(self, id, source, target, step):
        self.id = id
        self.source = source
        self.target = target
        self.waitStart = step
        self.depart = None
        self.arrive = None

def personArrived(personID, edge, target, step):
    global personsRunning
    persons[personID] = Person(personID, edge, target, step)
    personsRunning += 1

def personLoaded(personID, step):
    persons[personID].depart = step

def personUnloaded(personID, step):
    global personsRunning
    persons[personID].arrive = step
    personsRunning -= 1

def evaluate():
    waitTimes = []
    routeTimes = {}
    for person in persons.itervalues():
        waitTimes.append(person.depart - person.waitStart)
        route = (person.source, person.target) 
        if not route in routeTimes:
            routeTimes[route] = [] 
        routeTimes[route].append(person.arrive - person.depart) 
    waitArray = numpy.array(waitTimes)
    print "waiting time (max, mean, dev):", waitArray.max(), waitArray.mean(), math.sqrt(waitArray.var())
    for route, times in sorted(routeTimes.iteritems()):
        timeArray = numpy.array(times)
        print route, timeArray.max(), timeArray.mean(), math.sqrt(timeArray.var())

if __name__ == "__main__":
    from pylab import *
    stats = open(sys.argv[1])
    demand = []
    simpleWaitMean = []
    agentWaitMean = []
    simpleWaitDev = []
    agentWaitDev = []
    for line in stats:
        if "simple" in line:
            mean = simpleWaitMean
            dev = simpleWaitDev
            demand.append(int(line.split()[-1]))
        if "agent" in line:
            mean = agentWaitMean
            dev = agentWaitDev
        if "waiting" in line:
            mean.append(float(line.split()[-2]))
            dev.append(float(line.split()[-1]))
    stats.close()
    figure()
    demand = demand[:len(simpleWaitMean)]
    errorbar(demand, simpleWaitMean, simpleWaitDev, fmt='o')
    demand = demand[:len(agentWaitMean)]
    errorbar(demand, agentWaitMean, agentWaitDev, fmt='o')
    xlim(0, 50)
    show()
