# -*- coding: Latin-1 -*-
"""
@file    VelocityCurve.py
@author  Sascha.Krieg@dlr.de
@date    2008-05-13

Shows a velocity time-variation curve for a given Taxi or for the hole day all average values.

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

from pylab import *
from analysis.Taxi import * 
import util.Reader  as reader

#global Vars
WEE=False #=withoutEmptyEdges decide which analysis file should be used

def main(): 
    print "start program"
        
    taxiId='154_2'
    interval=900     
    plotCurve(interval)
    
    show()
    print "end"

def plotCurve(interval=10, taxiId=None):
    """plots the  velocity time-variation curve for a single taxi or averaged values of the hole day."""
    if taxiId==None:
        values, interval=getAveragedValues(interval) 
        legendText=('FCD','simFCD','intervall: '+str(interval)+'s')
    else:    
        values, starttime=getDataForTaxi(taxiId) 
        legendText=('FCD','simFCD','vtype','start: '+str(starttime))
       
    subplot(211)    
    plot(values[0], values[1], values[0], values[2])
    legend(legendText)   
    title("Geschwindigkeitsganglinie")
    xticks([])   
    ylabel("v (km/h)")
    subplot(212)
    plot(values[0], values[4], values[0], values[5])
    xlabel("t (s)")
    ylabel("q")
    legend(('FCD','simFCD'))
    title("Anzahl der Fahrzeuge")
    #set that the axis begin at 0
    #axis([axis()[0],axis()[1],0,axis()[3]]) 
    
   
        
def getDataForTaxi(taxiId):
     """Gets the Data for a single Taxi"""     
     values=[[],[],[],[],[],[]] #x1,y1,x2,y2,x3,y3
     starttime=0
     
     taxis=reader.readAnalysisInfo(WEE) 
     for step in taxis[taxis.index(taxiId)].getSteps():
        if step.source==SOURCE_FCD:
            values[0].append(step.time-starttime)
            values[1].append(step.speed)  
        elif step.source==SOURCE_SIMFCD:
            values[2].append(step.time-starttime)
            values[3].append(step.speed)           
        elif step.source==SOURCE_VTYPE: 
            if starttime==0:             
                starttime=step.time
            values[4].append(step.time-starttime)
            values[5].append(step.speed)
     return (values,starttime)       

def getAveragedValues(interval):    
    """catches all data in the given interval steps and calculates the average speed for each interval."""
    timeValues=range(0,86410,interval)
    fcdValues=[[] for i in range(0,86410,interval)]
    simFcdValues=[[] for i in range(0,86410,interval)]      
    vtypeValues=[[] for i in range(0,86410,interval)]
    fcdValuesNo=[set() for i in range(0,86410,interval)] 
    simFcdValuesNo=[set() for i in range(0,86410,interval)]
    vtypeValuesNo=[set() for i in range(0,86410,interval)] 
    taxis=reader.readAnalysisInfo(WEE) 
    
    #helper function
    def calcAverageOrLen(list,no=False):        
        for i in range(len(list)):
            if len(list[i])>0:
                   if no: #if no True clac Len                    
                      list[i]=len(list[i])
                   else:      
                      list[i]=sum(list[i])/len(list[i])                   
            else:       
               list[i]=None                
        return list
        
    for taxi in taxis:
       for step in taxi.getSteps(): 
           if step.source==SOURCE_FCD:
               #add the speed to the corresponding time interval
               fcdValues[step.time/interval].append(step.speed) 
               fcdValuesNo[step.time/interval].add(taxi.id) 
           elif step.source==SOURCE_SIMFCD:
               #add the speed to the corresponding time interval
               simFcdValues[step.time/interval].append(step.speed)
               simFcdValuesNo[step.time/interval].add(taxi.id)                    
           elif step.source==SOURCE_VTYPE: 
               #add the speed to the corresponding time interval               
               vtypeValues[step.time/interval].append(step.speed)
               vtypeValuesNo[step.time/interval].add(taxi.id) 
     
    vtypeValues=calcAverageOrLen(vtypeValues)
    fcdValues=calcAverageOrLen(fcdValues)
    simFcdValues=calcAverageOrLen(simFcdValues)
    vtypeValuesNo=calcAverageOrLen(vtypeValuesNo,True)
    fcdValuesNo=calcAverageOrLen(fcdValuesNo,True)
    simFcdValuesNo=calcAverageOrLen(simFcdValuesNo,True)
    return ([timeValues, fcdValues,simFcdValues,vtypeValues, fcdValuesNo, simFcdValuesNo, vtypeValuesNo],interval)
    
        
#start the program
#profile.run('main()')
main()