"""
@file    calStatistics.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-03-18
@version $Id: calStatistics.py 2008-03-18$

This script is to execute the significance test for evaluating the results from different assignment methods.
The test includes:
- check the averages
- check the variances

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, string, sys, datetime, random, math, operator

from elements import Vertex, Edge, Vehicle, Assign, T_Value, H_Value
from network import Net
from tables import chiSquareTable, tTable
    
def getBasicStats(net, verbose, method, vehicles, totallist):
    vehicleslist = vehicles
    allvehlist = totallist
    vehlist = []
    totalVeh = 0.
    totalTravelTime = 0.
    totalTravelLength = 0.
    totalTravelSpeed = 0.
    totalWaitTime = 0.
    totalDiffTime = 0.
    totalDiffSpeed = 0.
    totalDiffLength = 0.
    totalDiffWaitTime = 0.
    totalDiffTravelTime = 0.
    
    for veh in vehicleslist:
        totalVeh += 1
        veh.method = method
        # unit: speed - m/s; traveltime - s; travel length - m    
        veh.speed = veh.travellength / veh.traveltime
        totalTravelTime += veh.traveltime
        totalTravelLength += veh.travellength
        totalWaitTime += veh.waittime
        totalTravelSpeed += veh.speed
        if method == "tripinfo_dua_24.xml" or method == "increm" or method == "clogit" or method == "tripinfo_oneshot_1800.xml":
            allvehlist.append(veh)
        else:
            vehlist.append(veh)
            
        if method == "tripinfo_oneshot_1800.xml" or method == "tripinfo_dua_24.xml":
            vehlist.append(veh)
  
    if verbose:    
        print 'totalVeh:', totalVeh
    avgTravelTime = totalTravelTime/totalVeh
    avgTravelLength = totalTravelLength/totalVeh
    avgTravelSpeed = totalTravelSpeed/totalVeh
    avgWaitTime = totalWaitTime/totalVeh
    
    for veh in vehicleslist:
        totalDiffTravelTime += (veh.traveltime - avgTravelTime)**2
        totalDiffSpeed += (veh.speed - avgTravelSpeed)**2
        totalDiffLength += (veh.travellength - avgTravelLength)**2
        totalDiffWaitTime += (veh.waittime - avgWaitTime)**2
    
    #SD: standard deviation
    SDTravelTime = (totalDiffTravelTime/totalVeh)**(0.5)
    SDLength = (totalDiffLength/totalVeh)**(0.5)
    SDSpeed = (totalDiffSpeed/totalVeh)**(0.5)
    SDWaitTime = (totalDiffWaitTime/totalVeh)**(0.5)

    net.addAssignment(Assign(method, totalVeh, totalTravelTime, totalTravelLength, totalWaitTime,
                     avgTravelTime, avgTravelLength, avgTravelSpeed, avgWaitTime, SDTravelTime, SDLength, SDSpeed, SDWaitTime))
    return allvehlist, vehlist
    
# The observations should be drawn from a normally distributed population. 
# For two independent samples, the t test has the additional requirement
# that the population standard deviations should be equal.
def doTTestForAvg(verbose, tValueAvg, assignments):
    if verbose:
       print 'begin the t test!'
       
    for A in assignments.itervalues():
        for B in assignments.itervalues():
            sdABTravelTime = 0.
            sdABSpeed = 0.
            sdABLength = 0.
            sdABWaitTime = 0.      
            if str(A.label) != str(B.label):
                sdABTravelTime = (float(A.totalVeh-1)*((A.SDTravelTime)**2) + float(B.totalVeh -1)*((B.SDTravelTime)**2))/float(A.totalVeh + B.totalVeh-2)
                sdABTravelTime = sdABTravelTime**(0.5)
                
                sdABSpeed = (float(A.totalVeh-1)*((A.SDSpeed)**2) + float(B.totalVeh -1)*((B.SDSpeed)**2))/float(A.totalVeh + B.totalVeh-2)
                sdABSpeed = sdABSpeed**(0.5)
                
                sdABLength = (float(A.totalVeh-1)*((A.SDLength)**2) + float(B.totalVeh -1)*((B.SDLength)**2))/float(A.totalVeh + B.totalVeh-2)
                sdABLength = sdABLength**(0.5)
                
                sdABWaitTime = (float(A.totalVeh-1)*((A.SDWaitTime)**2) + float(B.totalVeh -1)*((B.SDWaitTime)**2))/float(A.totalVeh + B.totalVeh-2)
                sdABWaitTime = sdABWaitTime**(0.5)

                tempvalue = (float(A.totalVeh * B.totalVeh)/float(A.totalVeh + B.totalVeh))**0.5
                
                avgtraveltime = abs(A.avgTravelTime - B.avgTravelTime)/sdABTravelTime * tempvalue
                
                avgtravelspeed = abs(A.avgTravelSpeed - B.avgTravelSpeed)/sdABSpeed * tempvalue
                
                avgtravellength = abs(A.avgTravelLength - B.avgTravelLength)/sdABLength * tempvalue
                
                if sdABWaitTime != 0.:
                    avgwaittime = abs(A.avgWaitTime - B.avgWaitTime)/sdABWaitTime * tempvalue
                else:
                    avgwaittime = 0.
                    print 'check if the information about veh.waittime exists!'
                freedomdegree = A.totalVeh + B.totalVeh - 2
                if freedomdegree > 30 and freedomdegree <= 40:
                    freedomdegree = 31
                if freedomdegree > 40 and freedomdegree <= 50:
                    freedomdegree = 32
                if freedomdegree > 50 and freedomdegree <= 60:
                    freedomdegree = 33
                if freedomdegree > 60 and freedomdegree <= 80:
                    freedomdegree = 34
                if freedomdegree > 80 and freedomdegree <= 100:
                    freedomdegree = 35
                if freedomdegree > 100:
                    freedomdegree = 36
                lowvalue = tTable[freedomdegree][6]
                highvalue = tTable[freedomdegree][9]
                
                tValueAvg[A][B] = T_Value(avgtraveltime, avgtravelspeed, avgtravellength, avgwaittime, lowvalue, highvalue)
                     
#def checkDistribution(parA, parB):

def doKruskalWallisTest(verbose, groups, combivehlist, assignments, label, hValues):
    assignlist = assignments
    csTable = chiSquareTable
    if verbose:
        print '\nbegin the Kruskal-Wallis test!'
        print 'methods:', label
        print 'number of samples:', len(combivehlist)
    adjlabel = label + '_' + "adjusted"
    if groups >= 100:
        groups = 100
    lowvalue = csTable[groups-1][2]
    highvalue = csTable[groups-1][4]
   
    H = H_Value(label, lowvalue, highvalue)
    adjH = H_Value(adjlabel, lowvalue, highvalue)
    hValues.append(H)
    hValues.append(adjH)
    
    for index in [("traveltime"),("speed"),("travellength"),("waittime")]:
        for veh in combivehlist:
            veh.rank = 0.
        for method in assignlist.itervalues():
            method.sumrank = 0.
            
        samecountlist = []
        current = 0
        lastrank = 0
        subtotal = 0.
        adjusted = 0.
        
        combivehlist.sort(key=operator.attrgetter(index))
        totalsamples = len(combivehlist)

        for i in range (0,len(combivehlist)):
            samecount = 0
            if i <= current:
                if index == "traveltime":
                    value = combivehlist[current].traveltime
                elif index == "speed":
                    value = combivehlist[current].speed
                elif index == "travellength":
                    value = combivehlist[current].travellength
                elif index == "waittime":
                    value = combivehlist[current].waittime
            else:
                print 'error!'

            for j in range (current,len(combivehlist)):
                if index == "traveltime":
                    if combivehlist[j].traveltime == value:
                        samecount += 1
                    else:
                        break
                elif index == "speed":
                    if combivehlist[j].speed == value:
                        samecount += 1
                    else:
                        break
                elif index == "travellength":
                    if combivehlist[j].travellength == value:
                        samecount += 1
                    else:
                        break
                elif index == "waittime":
                    if combivehlist[j].waittime == value:
                        samecount += 1
                    else:
                        break                    

            if samecount == 1.:
                lastrank += 1.
                combivehlist[current].rank = lastrank
            else:
                sumrank = 0.
                for j in range (0, samecount):
                    lastrank += 1.
                    sumrank += lastrank
                rank = sumrank/samecount
                for j in range (0, samecount):
                    combivehlist[current+j].rank = rank
                
                elem = (value, samecount)
                samecountlist.append(elem)
            
            current = current + samecount
            
            if current > (len(combivehlist) - 1):
                break
                print 'current:', current
                            
        for veh in combivehlist:
            for method in assignlist.itervalues():
                if veh.method == method.label:
                    method.sumrank += veh.rank

        for method in assignlist.itervalues():
            subtotal += (method.sumrank**2.) / method.totalVeh
            
        for elem in samecountlist:
            adjusted += (float(elem[1]**3) - float(elem[1]))
        
        c = 1. - (adjusted /float(totalsamples**3 - totalsamples))            

        if index == "traveltime":
            H.traveltime = 12./(totalsamples*(totalsamples+1)) * subtotal - 3.*(totalsamples+1)
            if c > 0.:
                adjH.traveltime = H.traveltime / c
        elif index == "speed":
            H.travelspeed = 12./(totalsamples*(totalsamples+1)) * subtotal - 3.*(totalsamples+1)
            if c > 0.:
                adjH.travelspeed = H.travelspeed / c
        elif index == "travellength":
            H.travellength = 12./(totalsamples*(totalsamples+1)) * subtotal - 3.*(totalsamples+1)
            if c > 0.:
                adjH.travellength = H.travellength / c
        elif index == "waittime":    
            H.waittime = 12./(totalsamples*(totalsamples+1)) * subtotal - 3.*(totalsamples+1)
            if c > 0.:
                adjH.waittime = H.waittime / c