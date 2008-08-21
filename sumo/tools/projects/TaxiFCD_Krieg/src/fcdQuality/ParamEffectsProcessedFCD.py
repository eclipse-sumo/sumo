# -*- coding: Latin-1 -*-
"""
@file    GenerateRawFCD.py
@author  Sascha.Krieg@dlr.de
@date    2008-07-26

Creates files with a comparison of speeds for each edge between the taxis and the average speed from the current edge.
Dependent of the frequency and the taxi quota.

Needed files (have a look at Path.py):
    FQedgeDump
    FQedgeDumpPickle
    FQvtype
    FQvtypePickle
    FQvehPickle
    FQoutput
Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

import random
import time
import os.path
import cProfile
import util.Path as path
from cPickle import dump
from cPickle import load
from util import BinarySearch
#global vars
simStartTime=21600 # =6 o'clock  ->begin in edgeDump
aggInterval=900 #aggregation Interval of the edgeDump
period=[1,2,5,10,20,50,100,200,500] #period in seconds | single element or a hole list
quota=[0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20., 50.] #how many taxis in percent of the total vehicles | single element or a hole list
iteration=10

outputTextList=[] #alle lines which should be written to the output-file 
edgeDumpDict=None
vtypeDict=None
vehList=None
vehSum=None



def main():
    global edgeDumpDict, vtypeDict, vehList, vehSum, period, quota 
        
    print "start program"
   
    edgeDumpDict=make(path.FQedgeDumpPickle,path.FQedgeDump,readEdgeDump)    
    vtypeDict=make(path.FQvtypePickle,path.FQvtype,readVtype)
    vehList=make(path.FQvehPickle,path.FQvtypePickle,getVehicleList,False,vtypeDict)
    vehSum=len(vehList)
    
    orgPath=path.FQoutput
    orgPeriod=period
    orgQuota=quota
    for i in range(iteration):
        print "iteration: ",i
        path.FQoutput=orgPath+"interval900s_iteration"+str(i)+".out.xml"
        period=orgPeriod
        quota=orgQuota
        createOutput()
    
    print "end"

def generatePeriodQuotaSets(stopByPeriod=False):
    global period, quota
    
    """Generates all period-quota-sets (with creation of new Taxis for each set). 
    You can iterate over that generator and gets for each step the period and quota.
    If stopByPeriod=True it stops not only in the quota block but in the period block to-> have a look at the code.
    """
    if type(period)!=list: period=[period]
    if type(quota)!=list: quota=[quota]
    pList=period    
    qList=quota
    for period in pList:
        if stopByPeriod:         
            yield (period,None,None,None)
        for quota in qList:           
            print "create output for: period ",period," quota ",quota 
            taxis=chooseTaxis(vehList)
            taxiSum=len(taxis)
            vtypeDictR=reduceVtype(taxis); del taxis
            #vSimFCD,drivenEdgesSet=reduceVtypeToMean(taxis); del taxis
            yield(period,quota,vtypeDictR,taxiSum)
            #createOutput(edgeDumpDict, vtypeDictR,vehSum, taxiSum)
            
def readEdgeDump():
    """Get for each interval all edges with corresponding speed."""
    edgeDumpDict={}
    begin=False
    interval=0
    inputFile=open(path.FQedgeDump,'r')    
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
            entered = int(words[15])
            if entered==0: #if no vehicle drove of the edge ignore the edge  
                continue
            edgeDumpDict.setdefault(interval,[]).append((edge,speed))
    inputFile.close()
    return edgeDumpDict


def readVtype():
    """Gets all necessary information of all vehicles."""
    vtypeDict={}
    timestep=0
    begin=False
    inputFile=open(path.FQvtype,'r')    
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
        print "load source: ",os.path.basename(source),"...",
        target=load(open(source,'rb'))        
    else:
       print "build source: ",os.path.basename(source),"..." ,
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


def reduceVtype(taxis):
    """Reduces the vtypeDict to the relevant information."""
    taxis.sort() #sort it for binary search
    newVtypeDict={}
    for timestep in vtypeDict:
        if timestep%period==0: #timesteps which are a multiple of the period 
            newVtypeDict[timestep]=([tup for tup in vtypeDict[timestep] if BinarySearch.isElmInList(taxis,tup[0])])
    return newVtypeDict

def reduceVtypeToMean(taxis):
    simpleTaxiMeanVList=[0,0]
    drivenEdgesSet=set()    
    vehList=[]
    i=0
    taxis.sort() #sort it for binary search
    for timestep in vtypeDict:
       if timestep%period==0: #timesteps which are a multiple of the period 
           #begin=time.time()
           for tup in vtypeDict[timestep]:
               i+=1
               #if tup[0] in taxis:
               if BinarySearch.isElmInList(taxis,tup[0]):
                   vehList.append(tup[0])
                   simpleTaxiMeanVList[0]+=float(tup[2])
                   simpleTaxiMeanVList[1]+=1
                   drivenEdgesSet.add(tup[1])
          # end=time.time()
          # print end-begin
    print i               
    vSimFCD=simpleTaxiMeanVList[0]/simpleTaxiMeanVList[1]    
    return vSimFCD,drivenEdgesSet


def createOutput():
    """Creates a file with a comparison of speeds for each edge 
    between the taxis and the average speed from the current edge."""
    
    firstPeriod=True
    #get edge No
    edgesNo=0
    edgesSet=set()
    for i in edgeDumpDict.keys(): #all intervals
        for edge,v in edgeDumpDict[i]: 
            edgesSet.add(edge)
    edgesNo=len(edgesSet)
    
        
    outputFile=open(path.FQoutput,'w')
    outputFile.write('<?xml version="1.0"?>\n')
    outputFile.write('<paramEffects aggregationInterval="%d" vehicles="%d" edges="%d">\n' %(aggInterval, vehSum, edgesNo))
    for period, quota, vtypeDictR, taxiSum in generatePeriodQuotaSets(True):         
        if quota==None:
            if not firstPeriod: outputFile.write("\t</periods>\n")                
            else: firstPeriod=False            
            outputFile.write('\t<periods period="%d">\n' %(period))
        else:
            simpleTaxiMeanVList=[0,0]
            simpleEdgeMeanVList=[0,0]
            drivenEdgesSet=set()    
            #create mean from all taxi speed values
            for timestep,taxiList in vtypeDictR.iteritems():
                for tup in taxiList: #all elements in this timestep 
                 simpleTaxiMeanVList[0]+=float(tup[2])
                 simpleTaxiMeanVList[1]+=1
                 drivenEdgesSet.add(tup[1])              
            #create mean from all edge speed values which are driven by the chosen taxis
            drivenEdgesList=list(drivenEdgesSet)
            drivenEdgesList.sort()
            for i in edgeDumpDict.keys(): #all intervals
                for edge,v in edgeDumpDict[i]: 
                   #if edge in drivenEdgesSet:
                   if BinarySearch.isElmInList(drivenEdgesList,edge):
                       simpleEdgeMeanVList[0]+=v
                       simpleEdgeMeanVList[1]+=1
                       
            #calc values for output               
            detectedEdges=len(drivenEdgesSet) 
            relDetectedEdges=detectedEdges*100.0/edgesNo            
            vSim=simpleEdgeMeanVList[0]/simpleEdgeMeanVList[1]
            vSimFCD=simpleTaxiMeanVList[0]/simpleTaxiMeanVList[1]
            vAbsDiff=vSimFCD-vSim
            vRelDiff=vAbsDiff/vSim*100
                       
            outputFile.write('\t\t<values taxiQuota="%f" taxis="%d" simMeanSpeed="%f" simFcdMeanSpeed="%f" ' %(quota,taxiSum, vSim,vSimFCD,))
            outputFile.write('detectedEdges="%d" notDetectedEdges="%d" ' %(detectedEdges,edgesNo-detectedEdges))
            outputFile.write('absSpeedDiff="%f" relSpeedDiff="%f" relDetectedEdges="%f" relNotDetectedEdges="%f"/>\n' %(vAbsDiff,vRelDiff,relDetectedEdges,100-relDetectedEdges))
    outputFile.write("\t</periods>\n</paramEffects>") 
    outputFile.close()
    
def getMean(list):
    """Returns a median value from the values in the given list."""
    return sum(list)/len(list)

#start the program
#cProfile.run('main()')
main()     