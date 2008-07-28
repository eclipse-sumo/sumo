# -*- coding: Latin-1 -*-
"""
@file    Traveltime.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-21

Compares the travel time of vtypeprobe and FCD. (avg=False)

Secondly the average travel time will be calculate (avg=True). 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

from pylab import *
from analysis.Taxi import * 
import util.Reader  as reader
from util import CalcTime 

PERC=True #sets the view (absolute or relative)
colorTupel=('#ff4500','#7fff00','#dc143c','#ffd700','#1e90ff','#9932cc')
traveltimeList=[]
avg=True

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
   taxis=reader.readAnalysisInfo()   
   barsDict={}
   barsDictSim={}
   for taxi in taxis:        
        try:
            diff=getTimeDiff(taxi.getSteps(),False)
            diffSim=getTimeDiff(taxi.getSteps())
            if diff<-1000:  
                print taxi.id
                print "\n"
        except TypeError, e:
            print "Error by taxi %s : %s"  %(taxi.id,e.message) 
        #classify the absolute time difference
        barsDict[(diff/10)*10]=barsDict.setdefault((diff/10)*10,0)+1   
        barsDictSim[(diffSim/10)*10]=barsDictSim.setdefault((diffSim/10)*10,0)+1  
         
   return  (barsDictSim, barsDict)
def getTimeDiff(steps,sim=True):
    """Calculates the travel time for each source found in the steps."""
    global traveltimeList
    
    times=[None,None,None,None]
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
                times[0]=step.time
        elif step.source==SOURCE_FCD:
            if times[2]!=None:
                times[3]=step.time
            else:
                times[2]=step.time   
    
    if (None in times and not avg) or (avg and times[2]==None or times[3]==None):
        raise TypeError, "Can't calculate time difference because not all needed values are available. %s" %times
    
    if not avg:
        #clac travel time diff
        traveltimeList.append(times[3]-times[2])
        #traveltimeList.append(times[1]-times[0])
        diff=(times[3]-times[2])-(times[1]-times[0])
        if PERC:
            if (100*diff)/(times[3]-times[2])<-1000:     
                print  "%%",(100*diff)/(times[3]-times[2])         
                print "Diff",diff
                print "fcd",(times[3]-times[2])  
                print "vtype",times[1]-times[0]
                
            return (100*diff)/(times[3]-times[2])
        else:    
            return diff
    else: #for calc of avg    
        return (times[3]-times[2])  #FCD duration 
    
def clacAvg():
   durationList=[]
   taxis=reader.readAnalysisInfo()   
   for taxi in taxis:
       try:    
           dur=getTimeDiff(taxi.getSteps())
           durationList.append(dur)
           if dur >10000:
               print taxi
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
    under200=0    
    over200=0
    for k in sorted(barsDict.keys()):
       
       if k >200:
          over200+=0    
       elif k<-200:
           under200+=0
       else:
           xList.append(k)
           yList.append(barsDict[k])
      
    for k in sorted(barsDictSim.keys()):
       
       if k >200:
          over200+=1    
       elif k<-200:
           under200+=1
       else:
           xListSim.append(k)
           yListSim.append(barsDictSim[k])
           

   
    
    
    b=bar(xList,yList, width=10, alpha=0.5)
    bSim=bar(xListSim,yListSim, width=10, color="red", alpha=0.5)    
    legend((b[0], bSim[0]),('FCD-vtype', 'FCD-simFCD', '> 0 Sim schneller', '< 0 Sim langsammer','Taxis gesamt: '+str(sum(barsDict.values())),
             'Diff < -200%: '+str(under200),'Diff >  200%: '+str(over200),
             u'\u00f8'+' Reisezeit: '+str(sum(traveltimeList)/len(traveltimeList))+'s',), 
             shadow=True, loc=2)    
    title("Differenz der Reisezeit zwischen simulierten Taxis und Taxi-FCD)")
    xlabel('Reisezeit-Differenz in %')
    ylabel('Taxis')
#start the program
#profile.run('main()')
main()