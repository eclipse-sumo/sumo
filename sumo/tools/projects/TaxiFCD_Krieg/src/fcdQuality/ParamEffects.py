# -*- coding: Latin-1 -*-
"""
@file    GenerateRawFCD.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-07-26

Creates files with a comparison of speeds for each edge between the taxis and the average speed from the current edge.
Dependent of the frequency and the taxi quota.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys
import random
import os.path
import profile
from cPickle import dump
from cPickle import load


#global vars
mainPath="D:/Krieg/Projekte/Diplom/Daten/fcdQualitaet/"
#mainPath="F:/DLR/Projekte/Diplom/Daten/fcdQualitaet/"
edgeDumpPath=mainPath+"edgedumpFcdQuality_900_6Uhr.xml"
edgeDumpPicklePath=mainPath+"edgedumpFcdPickleDict.pickle"
vtypePath=mainPath+"vtypeprobeFcdQuality_1s_6Uhr.out.xml"
vtypePicklePath=mainPath+"vtypeprobePickleDict.pickle"
vehPicklePath=mainPath+"vehiclePickleList.pickle"
outputPath=mainPath+"readPlotData/out900_9.txt"
aggInterval=900
simStartTime=21600 # =6 o'clock  ->begin in edgeDump
period=[1,2,5,10,20,50,100,200,500] #period in seconds | single element or a hole list
quota=[0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20., 50.] #how many taxis in percent of the total vehicles | single element or a hole list

class EdgeInfo:
    def __init__(self):
        self._agg = 0
        self._veh = 0
        self._vehNo = 0

class Stat:
    def __init__(self):
        self._agg = 0
        self._aggSamples = 0
        self._veh = 0
        self._vehNo = 0



def main():
    global period, quota     
    stats = {}
    eiDict = {}
    print "Reading edges..."
    edgeDumpDict = readEdgeDump(edgeDumpPath)

    print "Reading vehicles..."
    vtypeDict = readVtype(vtypePath)

    print "Processing..."
    for e in edgeDumpDict:
        eiDict[e] = EdgeInfo()
        eiDict[e]._agg = edgeDumpDict[e][0]
        eiDict[e]._aggSamples = edgeDumpDict[e][1]
    for p in period:        
        print "Period: " + str(p)
        stats[p] = {}
        for q in quota:           
            print "Quota: " + str(q)
            for e in edgeDumpDict:
                eiDict[e]._veh = 0
                eiDict[e]._vehNo = 0
            taxis=chooseTaxis(vtypeDict,q)
            taxiSum=len(taxis)
            vtypeDictR=reduceVtype(vtypeDict,taxis,p)
            noProb = 0;
            for c in vtypeDictR:
                for e in vtypeDictR[c]:
                    eiDict[e[0]]._veh = eiDict[e[0]]._veh + e[1]
                    eiDict[e[0]]._vehNo = eiDict[e[0]]._vehNo + 1
                    noProb = noProb + 1
            
            s = Stat()
            s._noTaxis = len(taxis)
            s._noProb = noProb
            # number of known / unknown edges
            s._noKnown = 0
            s._noUnknown = 0
            setEdges = 0
            for e in eiDict:
                if eiDict[e]._aggSamples==0:
                    continue
                setEdges = setEdges + 1
                if eiDict[e]._vehNo==0:
                    s._noUnknown = s._noUnknown + 1
                else:
                    s._noKnown = s._noKnown + 1
            # mean aggregated and reported (taxi) speeds 
            s._oV = 0
            s._oVNo = 0
            s._tV = 0
            s._tVNo = 0
            for e in eiDict:
                if eiDict[e]._aggSamples==0:
                    continue
                if eiDict[e]._vehNo!=0:
                    s._tV = s._tV + eiDict[e]._veh
                    s._tVNo = s._tVNo + eiDict[e]._vehNo
                s._oV = s._oV + eiDict[e]._agg
                s._oVNo = s._oVNo + 1
            s._tV = s._tV / float(s._tVNo)
            s._oV = s._oV / float(s._oVNo)
            # speed deviations
            s._absDiff = s._tV-s._oV
            s._relDiff = s._absDiff/s._oV*100.
            stats[p][q] = s
    print "end"

    fd = open(outputPath, "w")
    for p in period:        
        for q in quota:           
            s = stats[p][q]
            fd.write(str(p) + ";" + str(q) + ";")
            fd.write(str(s._noTaxis) + ";" + str(s._noProb) + ";")
            fd.write(str(s._noKnown) + ";" + str(s._noUnknown) + ";")
            fd.write(str(s._oVNo) + ";" + str(s._tVNo) + ";")
            fd.write(str(s._oV) + ";" + str(s._tV) + ";")
            fd.write(str(s._absDiff) + ";" + str(s._relDiff) + ";")
            fd.write(str(float(s._noKnown)/float(setEdges)*100) + ";" + str(float(s._noUnknown)/float(setEdges)*100) + ";")
            fd.write("\n")
    fd.close()

def readEdgeDump(fileName):
    """Get for each interval all edges with corresponding speed."""
    edgeDumpDict={}
    begin=False
    interval=0
    noEmpty = 0
    inputFile=open(fileName,'r')    
    for line in inputFile:
        words=line.split('"')
        if not begin and words[0].find("<end>")!=-1:
            words=words[0].split(">")
            interval=int(words[1][:-5])
        elif words[0].find("<interval")!=-1 and int(words[1])>=simStartTime:           
            interval=int(words[1])
            begin=True            
        if begin and words[0].find("<edge id")!=-1:
            edge=words[1]
            if edge[0]!=":":
                speed=float(words[13])
                samples = float(words[5])
                edgeDumpDict[edge] = ( speed, samples )
    inputFile.close()
    return edgeDumpDict



def readVtype(fileName):
    """Gets all necessary information of all vehicles."""
    vtypeDict={}
    timestep=0
    begin=False
    inputFile=open(fileName,'r')    
    for line in inputFile:

        words=line.split('"')
        if words[0].find("<timestep ")!=-1:
            t = int(words[1])
            if t>=simStartTime and t<simStartTime+aggInterval:
                timestep=t
                begin=True  
        if begin and words[0].find("<vehicle id=")!=-1:            
            #                       time                 id    edge           speed 
#            vtypeDict.setdefault(timestep,[]).append((words[1],words[3][:-2],words[15]))
            if words[3][0]!=":":
                if words[1] not in vtypeDict:
                    vtypeDict[words[1]] = []
                vtypeDict[words[1]].append( ( words[3][:-2], float(words[15]) ) )
            #break 
    inputFile.close()        
    return vtypeDict


def getVehicleList(vtypeDict):
    """Collects all vehicles used in the simulation."""    
    vehSet=set()
    for timestepList in vtypeDict.values():       
        for elm in timestepList:            
            vehSet.add(elm[0])            
    return list(vehSet)


def chooseTaxis(vtypeDict,quota):
    """ Chooses from the vehicle list random vehicles with should act as taxis."""
    #calc absolute amount of taxis
    vehList = []
    for t in vtypeDict:
        vehList.append(t)
    random.shuffle(vehList)            
    taxiNo=int(round(quota*len(vehList)/100))
    print " " + str(len(vtypeDict)) + " taxis reduced to " + str(taxiNo)
    return vehList[:taxiNo]


def reduceVtype(vtypeDict,taxis,period):
    """Reduces the vtypeDict to the relevant information."""
    newVtypeDict={}
    for t in vtypeDict:
         if t in taxis:
             newVtypeDict[t] = []
             index = 0
             for e in vtypeDict[t]:
                 if index%period==0:
                     newVtypeDict[t].append(e)
                 index = index + 1
    return newVtypeDict



#start the program
#profile.run('main()')
main()     