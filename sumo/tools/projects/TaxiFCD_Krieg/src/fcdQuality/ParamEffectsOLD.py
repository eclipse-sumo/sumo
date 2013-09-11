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
outputPath=mainPath+"output/simResult_"

simStartTime=21600 # =6 o'clock  ->begin in edgeDump
period=[5,10,20,30,40,50,60,90,120] #period in seconds | single element or a hole list
quota=[0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 5.0, 10.0] #how many taxis in percent of the total vehicles | single element or a hole list


def main():
    global period, quota 
        
    print "start program"
    
    edgeDumpDict=make(edgeDumpPicklePath,edgeDumpPath,readEdgeDump)    
    vtypeDict=make(vtypePicklePath,vtypePath,readVtype)
    vehList=make(vehPicklePath,vtypePicklePath,getVehicleList,False,vtypeDict)
    vehSum=len(vehList)
    if type(period)!=list: period=[period]
    if type(quota)!=list: quota=[quota]
    pList=period    
    qList=quota
    for period in pList:        
        for quota in qList:           
            print "create output for: period ",period," quota ",quota 
            taxis=chooseTaxis(vehList)
            taxiSum=len(taxis)
            vtypeDictR=reduceVtype(vtypeDict,taxis); del taxis
            createOutput(edgeDumpDict, vtypeDictR,vehSum, taxiSum)
    
    print "end"


def readEdgeDump():
    """Get for each interval all edges with corresponding speed."""
    edgeDumpDict={}
    begin=False
    interval=0
    inputFile=open(edgeDumpPath,'r')    
    for line in inputFile:
        words=line.split('"')
        if not begin and words[0].find("<end>")!=-1:
            words=words[0].split(">")
            interval=int(words[1][:-5])
            edgeDumpDict.setdefault(interval,[])            
        elif words[0].find("<interval")!=-1 and int(words[1])>=simStartTime:           
            interval=int(words[1])
            begin=True            
        if begin and words[0].find("<edge id")!=-1:
            edge=words[1]
            speed=float(words[13])
            edgeDumpDict.setdefault(interval,[]).append((edge,speed))
    inputFile.close()
    return edgeDumpDict


def readVtype():
    """Gets all necessary information of all vehicles."""
    vtypeDict={}
    timestep=0
    begin=False
    inputFile=open(vtypePath,'r')    
    for line in inputFile:
        words=line.split('"')
        if words[0].find("<timestep ")!=-1 and int(words[1])>=simStartTime:
            timestep=int(words[1])
            begin=True  
        if begin and words[0].find("<vehicle id=")!=-1:            
            #                       time                 id    edge           speed 
            vtypeDict.setdefault(timestep,[]).append((words[1],words[3][:-2],words[15]))
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


def make(source, dependentOn, builder, buildNew=False,*builderParams):
    """Fills the target (a variable) with Information of source (pickelt var).
       It Checks if the pickle file is up to date in comparison to the dependentOn file.
       If not the builder function is called.
       If buildNew is True the builder function is called anyway.     
    """
    #check if pickle file exists
    if not os.path.exists(source):
        buildNew=True        
    #check date
    if not buildNew and os.path.getmtime(source)>os.path.getmtime(dependentOn): #if source is newer
        print "load source: ",os.path.basename(source),"..."
        target=load(open(source,'rb'))        
    else:
       print "build source: ",os.path.basename(source),"..." 
       target=builder(*builderParams)    
       #pickle the target 
       dump(target, open(source,'wb'),1)
    print "Done!"       
    return target


def chooseTaxis(vehList):
    """ Chooses from the vehicle list random vehicles with should act as taxis."""
    #calc absolute amount of taxis
    taxiNo=int(round(quota*len(vehList)/100))
    
    random.shuffle(vehList)            
    return vehList[:taxiNo]


def reduceVtype(vtypeDict,taxis):
    """Reduces the vtypeDict to the relevant information."""
    newVtypeDict={}
    for timestep in vtypeDict:
        if timestep%period==0: #timesteps which are a multiple of the period            
            newVtypeDict[timestep]=([tup for tup in vtypeDict[timestep] if tup[0] in taxis])
    return newVtypeDict


def createOutput(edgeDumpDict, vtypeDict,vehSum, taxiSum):
    """Creates a file with a comparison of speeds for each edge 
    between the taxis and the average speed from the current edge."""
    
    intervalList=edgeDumpDict.keys()
    intervalList.sort()
    interval=intervalList[1]-intervalList[0]
    
    outputFile=open(outputPath+str(period)+"s_"+str(quota)+"%.out.xml",'w')
    outputFile.write('<?xml version="1.0"?>\n')
    outputFile.write('<results simStart="%d" interval="%d" taxiQuota="%.3f" period="%d" vehicles="%d" taxis="%d">\n' %(simStartTime, interval, quota, period, vehSum, taxiSum))
    for i in intervalList[:-1]: #each interval
       outputFile.write('\t<interval begin="%d" end="%d">\n' %(i,i+interval-1))
       intEdges={}
       for timestep,taxiList in vtypeDict.iteritems():
           if  i<timestep<intervalList[intervalList.index(i)+1]: #for each timestep in the interval
               for tup in taxiList: #all elements in this timestep 
                   intEdges.setdefault(tup[1],[]).append(float(tup[2])) #add speed entry to the relevant edge
           
       #wirte results for every founded edge
       for edge,v in edgeDumpDict[i]: 
           if edge in intEdges:
               vList=intEdges[edge]   
               meanV=sum(vList)/len(vList)
               abs=meanV-v
               rel=abs/v*100
               outputFile.write('\t\t<edge id="%s" simSpeed="%.2f" fcdSpeed="%.2f" absDeviation="%.2f" relDeviation="%.2f"/>\n' %(edge, v, meanV, abs,rel))
       outputFile.write('\t</interval>\n')    
    outputFile.write('</results>')
    outputFile.close()


#start the program
#profile.run('main()')
main()     