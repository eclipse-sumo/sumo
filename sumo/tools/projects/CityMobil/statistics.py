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
    for route, times in routeTimes.iteritems():
        timeArray = numpy.array(times)
        print route, timeArray.max(), timeArray.mean(), math.sqrt(timeArray.var())
