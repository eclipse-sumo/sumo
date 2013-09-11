#!/usr/bin/env python
"""
@file    statistics.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2008-10-17
@version $Id$

Collecting statistics for the CityMobil parking lot

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
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

def evaluate(forTest=False):
    try:
        import numpy, math
    except ImportError:
        print "No numpy available, skipping statistics"
        return
    waitTimes = []
    routeTimes = {}
    for person in persons.itervalues():
        waitTimes.append(person.depart - person.waitStart)
        route = (person.source, person.target) 
        if not route in routeTimes:
            routeTimes[route] = [] 
        routeTimes[route].append(person.arrive - person.depart) 
    waitArray = numpy.array(waitTimes)
    if forTest:
        print "waiting time (max, mean, dev):", waitArray.max() < 1000, waitArray.mean() < 1000, math.sqrt(waitArray.var()) < 100
    else:
        print "waiting time (max, mean, dev):", waitArray.max(), waitArray.mean(), math.sqrt(waitArray.var())
   
    for route, times in sorted(routeTimes.iteritems()):
        timeArray = numpy.array(times)
        if forTest:
            print route, timeArray.max() < 1000, timeArray.mean() < 1000, math.sqrt(timeArray.var()) < 100
        else:       
            print route, timeArray.max(), timeArray.mean(), math.sqrt(timeArray.var())
    
    co2 = 0.
    for line in open("aggregated.xml"):
        if "cyber" in line:
            pos = line.find('CO2_abs="') + 9
            if pos >= 9:
                endpos = line.find('"', pos)
                co2 += float(line[pos:endpos])
    
    if forTest:
        print "CO2:", co2 < 10000000
    else:  
        print "CO2:", co2

if __name__ == "__main__":
    from pylab import *
    stats = open(sys.argv[1])
    demand = []
    simpleWaitMean = []
    agentWaitMean = []
    simpleWaitDev = []
    agentWaitDev = []
    simpleRouteMean = []
    agentRouteMean = []
    simpleRouteDev = []
    agentRouteDev = []
    for line in stats:
        if "simple" in line:
            mean = simpleWaitMean
            dev = simpleWaitDev
            rmean = simpleRouteMean
            rdev = simpleRouteDev
            demand.append(int(line.split()[-1]))
        if "agent" in line:
            mean = agentWaitMean
            dev = agentWaitDev
            rmean = agentRouteMean
            rdev = agentRouteDev
        if "waiting" in line:
            mean.append(float(line.split()[-2]))
            dev.append(float(line.split()[-1]))
        if line.startswith("('footmain0to1'"):
            rmean.append(float(line.split()[-2]))
            rdev.append(float(line.split()[-1]))
    stats.close()
    figure()
    errorbar(demand, simpleWaitMean, simpleWaitDev, lw=2, ms=10, fmt='o', label='standard bus scenario')
    errorbar(demand, agentWaitMean, agentWaitDev, lw=2, ms=10, color="red", fmt='o', label='agent controlled cyber cars')
    xlim(0, 50)
    ylim(0, 3300)
    xlabel('Repeater interval (s)')
    ylabel('Waiting time (s)')
    title('Mean and standard deviation of waiting time')
    legend(numpoints=1)
    savefig("waitingtime.png")
    figure()
    errorbar(demand, simpleRouteMean, simpleRouteDev, lw=2, ms=10, fmt='o', label='standard bus scenario')
    errorbar(demand, agentRouteMean, agentRouteDev, lw=2, ms=10, color="red", fmt='o', label='agent controlled cyber cars')
    xlim(0, 50)
    ylim(0, 300)
    xlabel('Repeater interval (s)')
    ylabel('Travel time (s)')
    title('Mean and standard deviation of travel time on the longest route')
    legend(numpoints=1)
    savefig("traveltime.png")
    show()
