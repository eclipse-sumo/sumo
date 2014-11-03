#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    CompareVelocitys.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-29
@version $Id$

Creates a file with a comparison of velocities between Taxi-FCD and vtypeprobe.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import util.Path as path
import util.Reader  as reader
from analysis.Taxi import * 

#global vars
fcdEdgeDict={}
vtypeEdgeDict={}
WEE=True

def main():
    print "start program"   
    #getVtypeV()
    #getFcdV()
    
    getSpeeds()
    writeSelLanesOutput()
    writeOutput()
    print "end"


def getSpeeds():
    """Reads the speeds from the analysis file"""
    
    taxis=reader.readAnalysisInfo(WEE)   
    #read speeds for every edge
    for taxi in taxis:
        for step in taxi.getSteps():
            if step.source==SOURCE_SIMFCD:            
                vtypeEdgeDict.setdefault(step.edge,[]).append(float(step.speed))    
            elif step.source==SOURCE_FCD:
                fcdEdgeDict.setdefault(step.edge,[]).append(float(step.speed))
    #calc avg speed for each edge
    #print fcdEdgeDict["558300689"]
    #print vtypeEdgeDict["558300689"]
    for edge in fcdEdgeDict:        
        fcdEdgeDict[edge]=sum(fcdEdgeDict[edge])/len(fcdEdgeDict[edge])
    print len(fcdEdgeDict)
    
    for edge in vtypeEdgeDict:        
        vtypeEdgeDict[edge]=sum(vtypeEdgeDict[edge])/len(vtypeEdgeDict[edge])
    print len(vtypeEdgeDict)
     
            
#deprecated            
def getVtypeV():
    """Reads the vyteprobe-File and creates a dict of edges with list of velocities.
       In a second step generates for each edge a average speed in km/h.  
    """
    vtypeEdgeDictSpeedList={}
    inputFile=open(path.vtypeprobe,'r')
    for line in inputFile:        
        words=line.split('"')
        if words[0].find("<vehicle id=")!=-1 and words[3][0]!=':':            
            vtypeEdgeDictSpeedList.setdefault(words[3][:-2],[]).append(float(words[15])*3.6)           
    inputFile.close()
    for edge in vtypeEdgeDictSpeedList:        
        vtypeEdgeDict[edge]=sum(vtypeEdgeDictSpeedList[edge])/len(vtypeEdgeDictSpeedList[edge])
    print len(vtypeEdgeDict)
    
#deprecated      
def getFcdV():
    """Reads the fcd-File and creates a dict of edges with list of velocities.
       In a second step generates for each edge a average speed in km/h.  
    """
    fcdEdgeDictSpeedList={}
    inputFile=open(path.vls,'r')
    for line in inputFile:        
        words=line.split('\t')
        fcdEdgeDictSpeedList.setdefault(words[1],[]).append(float(words[2])) 
    inputFile.close()
    for edge in fcdEdgeDictSpeedList:        
        fcdEdgeDict[edge]=sum(fcdEdgeDictSpeedList[edge])/len(fcdEdgeDictSpeedList[edge])
    print len(fcdEdgeDict)

def writeSelLanesOutput():
    outputFile=open(path.taxiVsFCDSpeedSelLanes,'w') 
    i=0
    for edge in fcdEdgeDict:#each edge
        if edge in vtypeEdgeDict:
            #clac average speed             
            absDeviation=vtypeEdgeDict[edge]-fcdEdgeDict[edge]   
            relDeviation=absDeviation/fcdEdgeDict[edge]*100   
            #write output only if Taxi speed for this edge exists
            #print relDeviation
            if relDeviation<-50 or relDeviation>50: 
                i+=1
                print "relDev ",relDeviation," edge ",edge
                outputFile.write("lane:"+edge+"_0\n")
    print "total",i
    outputFile.close()
              
def writeOutput():
    """Writes the collected results to a file."""
    outputFile=open(path.taxiVsFCDSpeed,'w') 
    outputFile.write('edge;fcdSpeed;simSpeed;absDeviation;relDeviation\n')
    for edge in fcdEdgeDict:#each edge
        if edge in vtypeEdgeDict:
            #clac average speed 
            absDeviation=vtypeEdgeDict[edge]-fcdEdgeDict[edge]   
            relDeviation=absDeviation/fcdEdgeDict[edge]*100   
            #write output only if Taxi speed for this edge exists
            outputFile.write('%s;%.2f;%.2f;%.2f;%.2f\n' %(edge,fcdEdgeDict[edge],vtypeEdgeDict[edge],absDeviation,relDeviation))
    outputFile.close()
    
#start the program
main()
