#!/usr/bin/env python
"""
@file    elements.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-10-25
@version $Id$

This script is to define the classes and functions for
- reading network geometric, 
- calculating link characteristics, such as capacity, travel time and link cost function,
- recording vehicular and path information, and 
- conducting statistic tests.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, math

# This class is used for finding the k shortest paths.
class Predecessor:

    def __init__(self, edge, pred, distance):
        self.edge = edge
        self.pred = pred
        self.distance = distance

# This class is used to build the nodes in the investigated network and 
# includes the update-function for searching the k shortest paths.
class Vertex:

    def __init__(self, num):
        self.inEdges = set()
        self.outEdges = set()
        self.label = "temp_%s" % num
        self.preds = []
        self.wasUpdated = False
    
    def __repr__(self):
        return self.label
        
    def _addNewPredecessor(self, edge, updatePred, newPreds):
        for pred in newPreds:
            if pred.pred == updatePred:
                return
        pred = updatePred
        if edge.kind == "real":
            while pred.edge != None:
                if pred.edge.kind == "real" and pred.edge.source.label == edge.target.label:
                    return
                pred = pred.pred
        newPreds.append(Predecessor(edge, updatePred,
                                    updatePred.distance + edge.actualtime))

    def update(self, KPaths, edge):
        updatePreds = edge.source.preds
        if len(self.preds) == KPaths\
           and updatePreds[0].distance + edge.actualtime >= self.preds[KPaths-1].distance:
            return False
        newPreds = []
        updateIndex = 0
        predIndex = 0
        while len(newPreds) < KPaths\
              and (updateIndex < len(updatePreds)\
                   or predIndex < len(self.preds)):
            if predIndex == len(self.preds):
                self._addNewPredecessor(edge, updatePreds[updateIndex], newPreds)
                updateIndex += 1
            elif updateIndex == len(updatePreds):
                newPreds.append(self.preds[predIndex])
                predIndex += 1
            elif updatePreds[updateIndex].distance + edge.actualtime < self.preds[predIndex].distance:
                self._addNewPredecessor(edge, updatePreds[updateIndex], newPreds)
                updateIndex += 1
            else:
                newPreds.append(self.preds[predIndex])
                predIndex += 1
        if predIndex == len(newPreds):
            return False
        self.preds = newPreds
        returnVal = not self.wasUpdated
        self.wasUpdated = True
        return returnVal

# This class is uesed to store link information and estimate 
# as well as flow and capacity for the flow computation and some parameters
# read from the net.
class Edge:
    def __init__(self, label, source, target, kind="junction"):
        self.label = label
        self.source = source
        self.target = target
        self.capacity = sys.maxint
        # paramerter for estimating capacities according to signal timing plans
        self.junction = None
        self.junctiontype = None
        self.rightturn = None
        self.straight = None
        self.leftturn = None
        self.uturn = None
        self.flow = 0.0
        self.kind = kind
        self.maxspeed = 1.0
        self.length = 0.0
        self.numberlane = 0.
        self.freeflowtime = 0.0
        self.queuetime = 0.0                          
        self.estcapacity = 0.0                             
        self.CRcurve = None
        self.actualtime = 0.0
        self.weight = 0.0
        self.connection = 0
        self.edgetype = None
        self.helpflow = 0.0
        # parameter in the Lohse traffic assignment
        self.helpacttime = 0.
        # parameter in the Lohse traffic assignment
        self.fTT = 0.
        # parameter in the Lohse traffic assignment
        self.TT = 0.
        # parameter in the Lohse traffic assignment                
        self.delta = 0.
        # parameter in the Lohse traffic assignment 
        self.helpacttimeEx = 0.
        # parameter in the matrix estimation
        self.detected = False
        self.detectorNum = 0.
        self.detecteddata = {}
        self.detectedlanes = 0.
        
    def init(self, speed, length, laneNumber):
        self.maxspeed = speed
        self.length = length
        self.numberlane = laneNumber
        if str(self.source) == str(self.target):
            self.freeflowtime = 0.0
        else:
            self.freeflowtime = self.length / self.maxspeed
            self.helpacttime = self.freeflowtime
        
    def __repr__(self):
        cap = str(self.capacity)
        if self.capacity == sys.maxint or self.connection != 0:
            cap = "inf"
        return "%s_%s_%s<%s|%s|%s|%s|%s|%s|%s|%s|%s>" % (self.kind, self.label, self.source, self.target, self.junctiontype,
                                                      self.flow, self.length, self.numberlane,
                                                      self.CRcurve, self.estcapacity, cap, self.weight)

    def getFreeFlowTravelTime(self):
        return self.freeflowtime
                
    def getDefaultCapacity(self):
        if (self.numberlane > 0. or self.detectorNum > 0.) and self.CRcurve == "None":
            self.estcapacity = max(self.numberlane, self.detectorNum) * 1500.
    
    def addDetectedData(self, detecteddataObj):
        self.detecteddata[detecteddataObj.label] = detecteddataObj
        
    # modified CR-curve database, defined in the PTV-Validate files
    def getCapacity(self):
        if self.numberlane > 0. or self.detectorNum > 0.:
            laneNum = max(self.detectorNum, self.numberlane)
            if self.maxspeed > 38.0:
                self.estcapacity = laneNum * 1500.
                if laneNum <= 2.:
                    self.edgetype = '14'
                elif laneNum == 3.:
                    self.edgetype = '10'
                elif laneNum >= 4.:
                    self.edgetype = '6'
            elif self.maxspeed > 34.0 and self.maxspeed <= 38.0:
                self.estcapacity = laneNum * 1500.
                if laneNum == 2.:
                    self.edgetype = '15'
                elif laneNum == 3.:
                    self.edgetype = '11'
                elif laneNum >= 4.:
                    self.edgetype = '7'
            elif self.maxspeed > 33.0 and self.maxspeed <= 34.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '26'
                if laneNum == 2.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '23'
                elif laneNum >= 3.:
                    self.estcapacity = laneNum * 1500.
                    self.edgetype = '20'
            elif self.maxspeed > 30.0 and self.maxspeed <= 33.0:
                self.estcapacity = laneNum * 1400.
                if laneNum <= 2.:        
                    self.edgetype = '16'
                if laneNum == 3.:
                    self.edgetype = '12'
                elif laneNum >= 3.:
                    self.edgetype = '8'
            elif self.maxspeed > 29.0 and self.maxspeed <= 30.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1350.   
                    self.edgetype = '18'
                if laneNum == 2.:
                    self.estcapacity = laneNum * 1500.
                    self.edgetype = '24'
                elif laneNum >= 3.:
                    self.estcapacity = laneNum * 1500.
                    self.edgetype = '21'
            elif self.maxspeed > 27.0 and self.maxspeed <= 29.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1350.
                    self.edgetype = '64'
                if laneNum >= 2.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '61'
            elif self.maxspeed >= 25.0 and self.maxspeed <= 27.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1350.     
                    self.edgetype = '19'
                if laneNum == 2.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '17'
                if laneNum == 3.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '13'
                if laneNum >= 4.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '9'
            elif self.maxspeed > 22.0 and self.maxspeed < 25.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1300.
                    self.edgetype = '29'
                if laneNum == 2.:
                    self.estcapacity = laneNum * 1300.
                    self.edgetype = '33'
                if slaneNum >= 3.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '30'
            elif self.maxspeed > 19.0 and self.maxspeed <= 22.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1200.
                    self.edgetype = '37'
                if laneNum == 2.:
                    self.estcapacity = laneNum * 1350.
                    self.edgetype = '34'
                if laneNum >= 3.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '31'
            elif self.maxspeed > 18.0 and self.maxspeed <= 19.0:
                self.estcapacity = laneNum * 1300.
                self.edgetype = '84'
            elif self.maxspeed > 16.0 and self.maxspeed <= 18.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1200.
                    self.edgetype = '38'
                if laneNum == 2.:
                    self.estcapacity = laneNum * 1300.
                    self.edgetype = '35'
                if laneNum == 3.:
                    self.estcapacity = laneNum * 1300.
                    self.edgetype = '32'
                if laneNum >= 4.:
                    self.estcapacity = laneNum * 1100.
                    self.edgetype = '40'
            elif self.maxspeed > 15.0 and self.maxspeed <= 16.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1000.
                    self.edgetype = '47'
                if laneNum >= 2.:
                    self.estcapacity = laneNum * 1100.
                    self.edgetype = '44'
            elif self.maxspeed > 13.0 and self.maxspeed <= 15.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 1200.
                    self.edgetype = '39'
                if laneNum == 2.:
                    self.estcapacity = laneNum * 1050.
                    self.edgetype = '45'
                if laneNum >= 3.:
                    self.estcapacity = laneNum * 1110.
                    self.edgetype = '42'
            elif self.maxspeed > 12.0 and self.maxspeed <= 13.0:
                self.estcapacity = laneNum * 800.
                if laneNum == 1.: 
                    self.edgetype = '49'
                if laneNum == 2.:
                    self.edgetype = '86'
            elif self.maxspeed > 11.0 and self.maxspeed <= 12.0:
                if laneNum == 1.: 
                    self.estcapacity = laneNum * 800.
                    self.edgetype = '83'
                if laneNum >= 2.:
                    self.estcapacity = laneNum * 1000.
                    self.edgetype = '75'
            elif self.maxspeed > 9.0 and self.maxspeed <= 11.0:
                if laneNum == 1.:
                    self.estcapacity = laneNum * 800.
                    self.edgetype = '89'
                if laneNum >= 2.:
                    self.estcapacity = laneNum * 1400.
                    self.edgetype = '87'
            elif self.maxspeed > 8.0 and self.maxspeed <= 9.0:
                self.estcapacity = laneNum * 800.
                self.edgetype = '79'
            elif self.maxspeed <= 8.0:
                self.estcapacity = laneNum * 200.
                self.edgetype = '94'
    
    def getCRcurve(self):
        self.CRcurve =''
        if self.edgetype != None:
            if int(self.edgetype) <= 5:
                    self.CRcurve = 'CR12'
                    self.estcapacity = 0.                    
            elif int(self.edgetype) >= 6 and int(self.edgetype) <= 19:
                    self.CRcurve = 'CR1'
            elif int(self.edgetype) >= 20 and int(self.edgetype) <= 49:
                    self.CRcurve = 'CR2'
            elif int(self.edgetype) >= 50 and int(self.edgetype) <= 79:
                    self.CRcurve = 'CR3'
            elif int(self.edgetype) >= 80 and int(self.edgetype) <= 89:
                    self.CRcurve = 'CR4'
            elif int(self.edgetype) >= 90 and int(self.edgetype) <= 93:
                    self.CRcurve = 'CR5'
            elif int(self.edgetype) >= 94 and int(self.edgetype) <= 99:
                    self.CRcurve = 'CR6'
    
    def getAdjustedCapacity(self, net):
        straightGreen = 0.
        rightGreen = 0.
        leftGreen = 0.
        greentime = 0.
        straightSymbol = -1
        rightSymbol = -1
        leftSymbol = -1
        cyclelength = 0.
        count = 0
        if self.numberlane > 0. or self.detectorNum > 0.:
            laneNum = max(self.detectorNum, self.numberlane)
            if self.junctiontype == 'signalized':
                junction = net._junctions[self.junction]
                if self.rightturn != None:
                    rightSymbol = int(self.rightturn)
                if self.leftturn != None:
                    leftSymbol = int(self.leftturn)
                if self.straight != None:
                    straightSymbol = int(self.straight)
    
                for phase in junction.phases[:]:
                    count += 1
                    cyclelength += phase.duration
                    if straightSymbol != -1 and phase.green[straightSymbol] == "1":
                        straightGreen += phase.duration
                    if rightSymbol != -1 and phase.green[rightSymbol] == "1":
                        rightGreen += phase.duration
                    if leftSymbol != -1 and phase.green[leftSymbol] == "1":
                        leftGreen += phase.duration
    
                if self.straight != None:
                    self.estcapacity = (straightGreen*(3600./cyclelength))/1.5 * laneNum
                else:
                    greentime = max(rightGreen, leftGreen)
                    self.estcapacity = (greentime*(3600./cyclelength))/1.5 * laneNum
            else:
                if self.straight == "m":
                    self.estcapacity *= 1.0
                elif self.straight == "None" and (self.rightturn == "m" or self.leftturn == "m"):
                     self.estcapacity = self.estcapacity * 1.0
                     
    # Function for calculating/updating link travel time
    def getActualTravelTime(self, curvefile, lamda):        
        foutcheck = file('queue_info.txt', 'a')
        f = file(curvefile)
        for line in f:
            itemCR = line.split()
            # get the parameters for the respective cost function
            if itemCR[0] == self.CRcurve:
                if self.flow == 0.0 or self.connection > 0 or self.numberlane == 0 or str(self.source) == str(self.target):
                    self.actualtime = self.freeflowtime
                else:
                    if self.estcapacity == 0.0:
                        foutcheck.write('edge.label=%s: estcapacity=0\n' %(self.label))
                    else:
                        self.actualtime = self.freeflowtime*(1+(float(itemCR[1])*(self.flow/(self.estcapacity*float(itemCR[3])))**float(itemCR[2])))
                if self.flow > self.estcapacity and self.connection == 0 and str(self.source) != str(self.target):
                    self.queuetime = self.queuetime + lamda*(self.actualtime - self.freeflowtime*(1+(float(itemCR[1]))))
                    foutcheck.write('edge.label= %s: queuing time= %s.\n' %(self.label, self.queuetime))
                    foutcheck.write('travel time at capacity: %s; actual travel time: %s.\n' %(self.freeflowtime*(1+(float(itemCR[1]))), self.actualtime))

                    if self.queuetime < 1.:
                        self.queuetime = 0.

                elif self.flow <= self.estcapacity and self.connection == 0 and str(self.source) != str(self.target):
                    self.queuetime = 0.
        f.close()
        foutcheck.close()        
        return self.actualtime
    

    # reset link flows
    def cleanFlow(self):
        self.flow = 0.
        self.helpflow = 0.

    # reset the parameter used in the Lohse-assignment (learning method - Lernverfahren)   
    def resetLohseParameter(self):
        self.fTT = 0.
        self.TT = 0.               
        self.delta = 0.
        self.helpacttimeEx = 0.  
    # update the parameter used in the Lohse-assignment (learning method - Lernverfahren)          
    def getLohseParUpdate(self, options):
        if self.helpacttime > 0.:
            self.TT = abs(self.actualtime - self.helpacttime) / self.helpacttime
            self.fTT = options.v1/(1 + math.exp(options.v2-options.v3*self.TT))
            self.delta = options.under + (options.upper - options.under)/((1+self.TT)**self.fTT)
            self.helpacttimeEx = self.helpacttime
            self.helpacttime = self.helpacttime + self.delta*(self.actualtime - self.helpacttime)    
            
    # check if the convergence reaches in the Lohse-assignment        
    def stopCheck(self, options):
        stop = False
        criteria = 0.
        criteria = options.cvg1 * self.helpacttimeEx**(options.cvg2/options.cvg3)

        if abs(self.actualtime - self.helpacttimeEx) <= criteria:
            stop = True
        return stop
   
# This class is for storing vehicle information, such as departure time, route and travel time.
class Vehicle:
    def __init__(self, label):
        self.label = label
        self.method = None
        self.depart = 0.
        self.arrival = 0.       
        self.speed = 0.
        self.route = []
        self.traveltime = 0.
        self.travellength = 0.
        self.departdelay = 0.
        self.waittime = 0.
        self.rank = 0.

    def __repr__(self):
        return "%s_%s_%s_%s_%s_%s<%s>" % (self.label, self.depart, self.arrival, self.speed, self.traveltime, self.travellength, self.route)
        
pathNum = 0
        
# This class is for storing path information which is mainly for the C-logit model.
class Path:
    def __init__(self, source, target, edges):
        self.source = source
        self.target = target
        global pathNum
        self.label = "%s" % pathNum
        pathNum += 1
        self.edges = edges
        self.freepathtime = 0.0
        self.actpathtime = 0.0
        self.pathflow = 0.0
        self.helpflow = 0.0
        self.commfactor = 0.0
        self.choiceprob = 0.0
        # parameter used in the Lohse traffic assignment
        self.usedcounts = 1
        # parameter used in the Lohse traffic assignment          
        self.pathhelpacttime = 0.
        # record if this path is the currrent shortest one.
        self.currentshortest = True
    
    def __repr__(self):
        return "%s_%s_%s<%s|%s|%s|%s>" % (self.label, self.source, self.target, self.freepathtime, self.pathflow, self.actpathtime, self.edges)

    def updatePathActTime(self, net):
        self.actpathtime = 0.
        for edge in self.edges:
            self.actpathtime += edge.actualtime
            self.actpathtime += edge.queuetime
        self.actpathtime = self.actpathtime/3600.
        
    # only used in the Lohse traffic assignment        
    def getPathTimeUpdate(self, net): 
        self.actpathtime = 0.
        self.pathhelpacttime = 0.
        for edge in self.edges:
            self.actpathtime += edge.actualtime
            self.actpathtime += edge.queuetime
            self.pathhelpacttime += edge.helpacttime
        self.pathhelpacttime = self.pathhelpacttime/3600.
        self.actpathtime = self.actpathtime/3600.

class TLJunction:
    def __init__(self):
        self.label = None
        self.phaseNum = 0
        self.phases = []
        
    def __repr__(self):
        return "%s_%s<%s>" % (self.label, self.phaseNum, self.phases)
        
class Signalphase:
    def __init__(self, duration, phase, brake, yellow):
        self.label= None
        self.duration = duration
        self.green = phase[::-1]
        self.brake = brake[::-1]
        self.yellow = yellow[::-1]
    
    def __repr__(self):
        return "%s_%s<%s|%s|%s>" % (self.label, self.duration, self.green, self.brake, self.yellow)

class DetectedFlows:
    def __init__(self, label, passengercars, truckflows):
        self.label = label             # wDay+hour, eg. MON-06
        self.flowPger = passengercars
        self.flowTruck = truckflows


    def __repr__(self):
        return "%s_<%s|%s|%s|%s|%s>" % (self.label, self.flowPger, self.flowTruck)

        
class VarianceFlowMatrix:
    def __init__(self, label, passengerarray, truckarray):
        self.label = label
        self.passengerarray = passengerarray
        self.truckarray = truckarray