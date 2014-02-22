#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    Traveltime.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-21
@version $Id$

Compares the travel time of vtypeprobe and FCD. (avg=False)

Secondly the average travel time will be calculate (avg=True). 

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from pylab import *
from analysis.Taxi import * 
import util.Reader  as reader
from util import CalcTime 
from math import sqrt

PERC=True #sets the view (absolute or relative)
colorTupel=('#ff4500','#7fff00','#dc143c','#ffd700','#1e90ff','#9932cc')
traveltimeList=[]
avg=False
WEE=True

def main(): 
    print "start program"
    if avg: 
        clacAvg()
    else:    
        if PERC:   
            #drawPieChart()
            drawBarChart()
        else:
            drawBarChart()
        show()
    print "end"
    
def getPiePieces():
    """Classifies the relative time difference into pieces (intervals) used for drawing the pie chart."""
    taxis=reader.readAnalysisInfo()
    pieces=[0,0,0,0,0,0]
    for taxi in taxis:        
        try:
            diff=getTimeDiff(taxi.getSteps())
        except TypeError, e:
            print "Error by taxi %s : %s"  %(taxi.id,e.message) 
               
        #classify the relative time difference
        #<10%', '10%-30%', '30%-50%', '50%-70%', '70%-90%', '>90%
        if diff<10:
            pieces[0]+=1 
        elif diff<30:
            pieces[1]+=1  
        elif diff<50:
            pieces[2]+=1
        elif diff<70:
            pieces[3]+=1
        elif diff<90:
            pieces[4]+=1
        else:            
            pieces[5]+=1        
    print pieces    
    print sum(pieces)
    return pieces
      
def getBars():
   """Classifies the time difference in single bars."""
   taxis=reader.readAnalysisInfo(WEE)   
   barsDict={}
   barsDictSim={}
   stdDev=[]
   mw=[]
   for taxi in taxis:
        if len(taxi.getSteps())<1:
            continue        
        try:
            #diff=getTimeDiff(taxi.getSteps(),False)
            diffSim,fcd,sim,no=getTimeDiff(taxi.getSteps())
            
            #anna     
            if diffSim>150:  
                print diffSim," ",taxi.id," ",no," ",fcd," ",sim                 
            
            #standard deviation 
            stdDev.append((diffSim-9.46)*(diffSim-9.46))   
            mw.append(diffSim) 
            #classify the absolute time difference
            #barsDict[(diff/10)*10]=barsDict.setdefault((diff/10)*10,0)+1   
            barsDictSim[(diffSim/10)*10]=barsDictSim.setdefault((diffSim/10)*10,0)+1 
        except TypeError, e:
            tueNichts=True
            #print "Error by taxi %s : %s"  %(taxi.id,e.message) 
   print "mw",sum(mw)/(len(mw)+0.0) #9.46
   print "standard deviation ",sqrt(sum(stdDev)/(len(stdDev)+0.0))     
   return  (barsDictSim, barsDict)
def getTimeDiff(steps,sim=True):
    """Calculates the travel time for each source found in the steps."""
    global traveltimeList
    fcdDataNo=0
    times=[None,None,None,None]
    getSecondEdgeSim=True
    getSecondEdgeFcd=True
    for step in steps: 
        #add first and last times of fcd and vtypeprobe to the timesList    
        if sim:
            source=SOURCE_SIMFCD            
        else:
             source=SOURCE_VTYPE
        if step.source==source:            
            if times[0]!=None:
                times[1]=step.time
            else:                
                if getSecondEdgeSim:
                    times[0]=step.time
                getSecondEdgeSim=True
        elif step.source==SOURCE_FCD:
            fcdDataNo+=1
            if times[2]!=None:
                times[3]=step.time
            else:                              
                if getSecondEdgeFcd:                    
                    times[2]=step.time
                getSecondEdgeFcd=True   
    if fcdDataNo<=6:
        raise TypeError, "not enough values"
    
    if (None in times and not avg) or (avg and times[2]==None or times[3]==None):
        raise TypeError, "Can't calculate time difference because not all needed values are available. %s Source:%s" %(times,source)
    
    if not avg:
        #clac travel time diff
        traveltimeList.append(times[3]-times[2])
        #traveltimeList.append(times[1]-times[0])
        fcd=times[3]-times[2]
        sim=(times[1]-times[0])
        diff=sim-fcd
        if PERC:
            """
            if (times[3]-times[2])>0 and (100*diff)/(times[3]-times[2])<-1000:     
                print  "%%",(100*diff)/(times[3]-times[2])         
                print "Diff",diff
                print "fcd",(times[3]-times[2])  
                print "vtype",times[1]-times[0]
            """    
            if fcd==0:                
                return 0
            else:
                return (int(round((100.0*diff)/fcd)),fcd,sim,fcdDataNo)
        else:    
            return diff
    else: #for calc of avg    
        return fcd  #FCD duration 
    
def clacAvg():
   durationList=[]
   taxis=reader.readAnalysisInfo()   
   for taxi in taxis:
       try:    
           dur=getTimeDiff(taxi.getSteps())
           durationList.append(dur)
           if dur >=1479:
               print "maxtaxi", taxi
       except TypeError, e:
            print "Error by taxi %s : %s"  %(taxi.id,e.message) 
   print "no",len(durationList)
   print "avg", sum(durationList)/(len(durationList)+0.0),"s =",
   CalcTime.getSecsInTime(int(round(sum(durationList)/(len(durationList)+0.0)))) 
   print "min", min(durationList),"s =",
   CalcTime.getSecsInTime(min(durationList))
   print "max", max(durationList),"s =",
   CalcTime.getSecsInTime(max(durationList)) 
        
def drawPieChart():
    """Draws a pie chart with the relative travel time aberrance."""
    #sets the window size
    figure(1, figsize=(10,10))
    
    labels = 'Diff <10%', 'Diff 10%-30%', 'Diff 30%-50%', 'Diff 50%-70%', 'Diff 70%-90%', 'Diff >90%'
    #fracs = [15,30,45, 10,66,7]
    
    pie(getPiePieces(), labels=labels, autopct='%1.1f%%', shadow=True, colors=colorTupel, labeldistance=1.2 )
    title("Relative Differenz der Reisezeit zwischen simulierten Taxis und Taxi-FCD\n(nicht simulierten Taxi-FCD!)\n")
     
    
     
def drawBarChart():
    """Draws a bar chart with the relative travel time aberrance."""
    barsDictSim, barsDict=getBars()
    xList=[]
    yList=[]
    xListSim=[]
    yListSim=[]
    under100=0    
    over100=0
    simFaster=0
    simSlower=0
    """
    for k in sorted(barsDict.keys()):
       
       if k >100:
          over100+=0    
       elif k<-100:
           under100+=0
       else:
           xList.append(k)
           yList.append(barsDict[k])
    """  
    for k in sorted(barsDictSim.keys()):       
       if k >=100:
          over100+=barsDictSim[k]              
       elif k<-100:
           under100+=barsDictSim[k]
       else:
           xListSim.append(k)
           yListSim.append(barsDictSim[k]) 
           if k<0:             
             simSlower+=barsDictSim[k]
           else:    
             simFaster+=barsDictSim[k]  
        
   
    
    textsize=18
    subplots_adjust(left=0.10, right=0.60, bottom=0.10, top=0.90)
    xticks(range(-110,130,20), size=textsize)
    yticks(size=textsize)
    #b=bar(xList,yList, width=10, alpha=0.5)
    bSim=bar(xListSim,yListSim, width=10, color="red", alpha=0.5)    
    legend((None,),('Taxis gesamt: '+str(sum(barsDictSim.values())),'> 0 Sim. schneller', '< 0 Sim. langsammer'), shadow=True)    
    #u'\u00f8'+' Reisezeit: '+str(sum(traveltimeList)/len(traveltimeList))+'s'
    title("Abweichung der Reisezeit zwischen realen und simulierten FCD", size=textsize)
    xlabel('\nrelative Abweichung der Reisezeiten [%] (bei '+str(over100)+' Taxis > 100%)', size=textsize)
    ylabel('Anzahl der Taxis', size=textsize)
    
    
#start the program
#profile.run('main()')
main()
