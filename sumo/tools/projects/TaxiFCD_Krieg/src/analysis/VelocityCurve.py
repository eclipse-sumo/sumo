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



def main(): 
    print "start program"
        
    taxiId='154_2'
    interval=600 
    plotCurve(interval)
    
    show()
    print "end"

def plotCurve(interval=10, taxiId=None):
    if taxiId==None:
        values, interval=getAveragedValues(interval) 
        legendText=('FCD','vtype','intervall: '+str(interval)+'s')
    else:    
        values, starttime=getDataForTaxi(taxiId) 
        legendText=('FCD','vtype','start: '+str(starttime))
       
    plot(values[0], values[1], values[2], values[3])
    #set that the axis begin at 0
    axis([axis()[0],axis()[1],0,axis()[3]]) 
    legend(legendText)
    title("Geschwindigkeitsganglinie")
    xlabel("t (s)")
    ylabel("v (km/h)")
        
def getDataForTaxi(taxiId):     
     values=[[],[],[],[]] #x1,y1,x2,y2
     starttime=0
     
     taxis=reader.readAnalysisInfo() 
     for step in taxis[taxis.index(taxiId)].getSteps():
        if step.source==SOURCE_FCD:
            values[0].append(step.time-starttime)
            values[1].append(step.speed)            
        elif step.source==SOURCE_VTYPE: 
            if starttime==0:
                starttime=step.time
            values[2].append(step.time-starttime)
            values[3].append(step.speed)
     return (values,starttime)       

def getAveragedValues(interval):    
    timeValues=range(0,86410,interval)
    fcdValues=[[] for i in range(0,86410,interval)]    
    vtypeValues=[[] for i in range(0,86410,interval)] 
    taxis=reader.readAnalysisInfo() 
    
    #helper function
    def calcAverage(list):
       
        for i in range(len(list)):
            if len(list[i])>0:
                   list[i]=sum(list[i])/len(list[i])
            else:       
               list[i]=None 
        return list   
    
    for taxi in taxis:
       for step in taxi.getSteps(): 
           if step.source==SOURCE_FCD:
               #add the speed to the corresponding time interval
               fcdValues[step.time/interval].append(step.speed) 
           elif step.source==SOURCE_VTYPE: 
               #add the speed to the corresponding time interval
               
               vtypeValues[step.time/interval].append(step.speed)
     
    vtypeValues=calcAverage(vtypeValues)
    fcdValues=calcAverage(fcdValues)
    return ([timeValues, fcdValues,timeValues, vtypeValues],interval)
    
        
#start the program
#profile.run('main()')
main()