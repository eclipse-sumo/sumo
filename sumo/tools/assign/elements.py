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
        self.flow = 0.0
        self.kind = kind
        self.maxspeed = 1.0
        self.length = 0.0
        self.numberlane = 0
        self.freeflowtime = 0.0                            
        self.estcapacity = 0.0                             
        self.CRcurve = ''
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
        return "%s_%s<%s|%s|%s|%s|%s|%s|%s|%s|%s>" % (self.kind, self.label, self.source, self.target,
                                                      self.flow, self.length, self.numberlane,
                                                      self.CRcurve, self.estcapacity, cap, self.weight)

    def getFreeFlowTravelTime(self):
        return self.freeflowtime
                
    def getDefaultCapacity(self, parfile):
        f = file(parfile)
        for line in f:
            p = line.split()
            periods = int(p[len(p)-2])
        # The respective rules will be developed accroding to the HBS. 
        self.estcapacity = float(self.numberlane * 1500) * periods           

        return self.estcapacity

    # modified CR-curve database, defined in the PTV-Validate files
    def getCapacity(self, parfile):
        f = file(parfile)
        for line in f:
            p = line.split()
            periods = int(p[(len(p)-2)])
        if self.numberlane > 0:
            if self.maxspeed > 38.0:
                self.estcapacity = float(self.numberlane * 1500) * periods
                if self.numberlane <= 2:
                    self.edgetype = '14'
                elif self.numberlane == 3:
                    self.edgetype = '10'
                elif self.numberlane >= 4:
                    self.edgetype = '6'
            elif self.maxspeed > 34.0 and self.maxspeed <= 38.0:
                self.estcapacity = float(self.numberlane * 1500) * periods
                if self.numberlane == 2:
                    self.edgetype = '15'
                elif self.numberlane == 3:
                    self.edgetype = '11'
                elif self.numberlane >= 4:
                    self.edgetype = '7'
            elif self.maxspeed > 33.0 and self.maxspeed <= 34.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '26'
                if self.numberlane == 2:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '23'
                elif self.numberlane >= 3:
                    self.estcapacity = float(self.numberlane * 1500) * periods
                    self.edgetype = '20'
            elif self.maxspeed > 30.0 and self.maxspeed <= 33.0:
                self.estcapacity = float(self.numberlane * 1400) * periods
                if self.numberlane <= 2:        
                    self.edgetype = '16'
                if self.numberlane == 3:
                    self.edgetype = '12'
                elif self.numberlane >= 3:
                    self.edgetype = '8'
            elif self.maxspeed > 29.0 and self.maxspeed <= 30.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1350) * periods       
                    self.edgetype = '18'
                if self.numberlane == 2:
                    self.estcapacity = float(self.numberlane * 1500) * periods
                    self.edgetype = '24'
                elif self.numberlane >= 3:
                    self.estcapacity = float(self.numberlane * 1500) * periods
                    self.edgetype = '21'
            elif self.maxspeed > 27.0 and self.maxspeed <= 29.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1350) * periods       
                    self.edgetype = '64'
                if self.numberlane >= 2:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '61'
            elif self.maxspeed >= 25.0 and self.maxspeed <= 27.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1350) * periods       
                    self.edgetype = '19'
                if self.numberlane == 2:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '17'
                if self.numberlane == 3:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '13'
                if self.numberlane >= 4:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '9'
            elif self.maxspeed > 22.0 and self.maxspeed < 25.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1300) * periods
                    self.edgetype = '29'
                if self.numberlane == 2:
                    self.estcapacity = float(self.numberlane * 1300) * periods
                    self.edgetype = '33'
                if self.numberlane >= 3:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '30'
            elif self.maxspeed > 19.0 and self.maxspeed <= 22.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1200) * periods
                    self.edgetype = '37'
                if self.numberlane == 2:
                    self.estcapacity = float(self.numberlane * 1350) * periods
                    self.edgetype = '34'
                if self.numberlane >= 3:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '31'
            elif self.maxspeed > 18.0 and self.maxspeed <= 19.0:
                self.estcapacity = float(self.numberlane * 1300) * periods
                self.edgetype = '84'
            elif self.maxspeed > 16.0 and self.maxspeed <= 18.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1200) * periods
                    self.edgetype = '38'
                if self.numberlane == 2:
                    self.estcapacity = float(self.numberlane * 1300) * periods
                    self.edgetype = '35'
                if self.numberlane == 3:
                    self.estcapacity = float(self.numberlane * 1300) * periods
                    self.edgetype = '32'
                if self.numberlane >= 4:
                    self.estcapacity = float(self.numberlane * 1100) * periods
                    self.edgetype = '40'
            elif self.maxspeed > 15.0 and self.maxspeed <= 16.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1000) * periods
                    self.edgetype = '47'
                if self.numberlane >= 2:
                    self.estcapacity = float(self.numberlane * 1100) * periods
                    self.edgetype = '44'
            elif self.maxspeed > 13.0 and self.maxspeed <= 15.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 1200) * periods
                    self.edgetype = '39'
                if self.numberlane == 2:
                    self.estcapacity = float(self.numberlane * 1050) * periods
                    self.edgetype = '45'
                if self.numberlane >= 3:
                    self.estcapacity = float(self.numberlane * 1110) * periods
                    self.edgetype = '42'
            elif self.maxspeed > 12.0 and self.maxspeed <= 13.0:
                self.estcapacity = float(self.numberlane * 800) * periods
                if self.numberlane == 1: 
                    self.edgetype = '49'
                if self.numberlane == 2:
                    self.edgetype = '86'
            elif self.maxspeed > 11.0 and self.maxspeed <= 12.0:
                if self.numberlane == 1: 
                    self.estcapacity = float(self.numberlane * 800) * periods
                    self.edgetype = '83'
                if self.numberlane >= 2:
                    self.estcapacity = float(self.numberlane * 1000) * periods
                    self.edgetype = '75'
            elif self.maxspeed > 9.0 and self.maxspeed <= 11.0:
                if self.numberlane == 1:
                    self.estcapacity = float(self.numberlane * 800) * periods
                    self.edgetype = '89'
                if self.numberlane >= 2:
                    self.estcapacity = float(self.numberlane * 1400) * periods
                    self.edgetype = '87'
            elif self.maxspeed > 8.0 and self.maxspeed <= 9.0:
                self.estcapacity = float(self.numberlane * 800) * periods
                self.edgetype = '79'
            elif self.maxspeed <= 8.0:
                self.estcapacity = float(self.numberlane * 200) * periods
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
                    
# Function for calculating/updating link travel time
    def getActualTravelTime(self, curvefile):        
        foutcheck = file('time_flow.txt', 'a')
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
                    # travel time penalty 20% (can/should be modified)
                    self.actualtime = self.actualtime*1.2
                    foutcheck.write('****edge.label="%s": acutaltime is timed by 1.2.\n' %(self.label))
        f.close()
        foutcheck.close()        
        return self.actualtime

# reset link flows
    def cleanFlow(self):
        self.flow = 0.

# update the parameter used in the Lohse-assignment (learning method - Lernverfahren)          
    def getLohseParUpdate(self, under, upper, v1, v2, v3):
        if self.helpacttime > 0.:
            self.TT = abs(self.actualtime - self.helpacttime) / self.helpacttime
            self.fTT = v1/(1 + math.exp(v2-v3*self.TT))
            self.delta = under + (upper - under)/((1+self.TT)**self.fTT)
            self.helpacttimeEx = self.helpacttime
            self.helpacttime = self.helpacttime + self.delta*(self.actualtime - self.helpacttime)    
            
# check if the convergence reaches in the Lohse-assignment        
    def stopCheck(self, verbose, cvg1, cvg2, cvg3):
        stop = False
        criteria = 0.
        criteria = cvg1 * self.helpacttimeEx**(cvg2/cvg3)

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
        self.waittime = 0.
        self.rank = 0.

    def __repr__(self):
        return "%s_%s_%s_%s_%s_%s<%s>" % (self.label, self.depart, self.arrival, self.speed, self.traveltime, self.travellength, self.route)
        
pathNum = 0
        
# This class is for storing path information which is mainly for the C-logit model.
class Path:
    def __init__(self):
        self.source = None
        self.target = None
        self.label = "%s" % pathNum
        global pathNum
        pathNum += 1
        self.Edges = []
        self.freepathtime = 0.0
        self.actpathtime = 0.0
        self.pathflow = 0.0
        self.helpflow = 0.0
        self.commfactor = 0.0
        self.choiceprob = 0.0
        # parameter used in the Lohse traffic assignment
        self.usedcounts = 0
        # parameter used in the Lohse traffic assignment          
        self.pathhelpacttime = 0.    
    
    def __repr__(self):
        return "%s_%s_%s<%s|%s|%s|%s>" % (self.label, self.source, self.target, self.freepathtime, self.pathflow, self.actpathtime, self.Edges)

    def updatePathActTime(self, net):
        self.actpathtime = 0.
        for edge in self.Edges:
            self.actpathtime += edge.actualtime
        self.actpathtime = self.actpathtime/3600.
        
    # only used in the Lohse traffic assignment        
    def getPathTimeUpdate(self, net): 
        self.actpathtime = 0.
        self.pathhelpacttime = 0.
        for edge in self.Edges:
            self.actpathtime += edge.actualtime
            self.pathhelpacttime += edge.helpacttime
        self.pathhelpacttime = self.pathhelpacttime/3600.
        self.actpathtime = self.actpathtime/3600.
         
# This cloass is used in the significance test.
class Assign:
    def __init__(self, method, totalVeh, totalTravelTime, totalTravelLength, totalWaitTime, avgTravelTime, avgTravelLength, avgTravelSpeed, avgWaitTime, SDTravelTime, SDLength, SDSpeed, SDWaitTime):
        self.label = method
        self.totalVeh = totalVeh
        self.totalTravelTime = totalTravelTime
        self.totalTravelLength = totalTravelLength
        self.totalWaitTime = totalWaitTime
        self.avgTravelTime = avgTravelTime
        self.avgTravelLength = avgTravelLength
        self.avgTravelSpeed = avgTravelSpeed
        self.avgWaitTime = avgWaitTime
        self.SDTravelTime = SDTravelTime
        self.SDLength = SDLength      
        self.SDSpeed = SDSpeed
        self.SDWaitTime = SDWaitTime
        self.sumrank = 0.
        
    def __repr__(self):
        return "%s_<%s|%s|%s|%s|%s|%s|%s|%s|%s>" % (self.label, self.totalVeh, self.avgTravelTime, self.avgTravelLength, self.avgTravelSpeed,
                                                     self.avgWaitTime, self.SDTravelTime, self.SDLength, self.SDSpeed, self.SDWaitTime)

# This cloass is used for the t test in the significance test.
class T_Value:
    def __init__(self, avgtraveltime, avgtravelspeed, avgtravellength, avgwaittime, lowvalue, highvalue):
        self.avgtraveltime = avgtraveltime
        self.avgtravelspeed = avgtravelspeed
        self.avgtravellength = avgtravellength
        self.avgwaittime = avgwaittime
        self.lowtvalue = lowvalue
        self.hightvalue = highvalue
        
    def __repr__(self):
        return "%<%s|%s|%s|%s>" % (self.avgtraveltime, self.avgtravelspeed, self.avgtravellength, self.avgwaittime)

# This class is used for the Kruskal-Wallis test in the significance test.
class H_Value:
    def __init__(self, label, lowvalue, highvalue):
        self.label = label
        self.traveltime = 0.
        self.travelspeed = 0.
        self.travellength = 0.
        self.waittime = 0.
        # 95% confidence interval
        self.lowchivalue = lowvalue 
        # 99% confidence interval
        self.highchivalue = highvalue
        
    def __repr__(self):
        return "%<%s|%s|%s|%s|%s|%s>" % (self.traveltime, self.travelspeed, self.travellength, self.waittime, self.lowchivalue, self.highchivalue)