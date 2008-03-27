"""
@file    outputs.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-12-25
@version $Id$

This script is for generating the outputs from the choosed traffic assignment.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime
from network import Net
from elements import Vehicle
import operator

# calculate the time for reading the input data (matrix data are excluded.)
def timeForInput(inputreaderstart):
    fouttime = file('timeforinput.txt', 'w')
    inputreadtime = datetime.datetime.now() - inputreaderstart  
    fouttime.write('Time for reading input files:%s\n' %inputreadtime)
    fouttime.close()
    
# output the input matrices, origins, destinations and the number of OD pairsdemand > 0)
def outputODZone(startVertices, endVertices, Pshort_EffCells, MatrixCounter):
    foutmatrixstart = file('origins.txt', 'a')
    foutmatrixend = file('destinations.txt', 'a')
    foutmatrixstart.write('Interval =%s\n' %(MatrixCounter))
    foutmatrixstart.write('number of origins=%s\n' %len(startVertices))
    foutmatrixstart.write('number of effective OD cells for Passenger vehicles=%s\n' %(Pshort_EffCells))
#    foutmatrixstart.write('number of effective OD cells for Passenger vehicles(long distances)=%s\n' %(Plong_EffCells))
#    foutmatrixstart.write('number of effective OD cells for trucks=%s\n' %(Truck_EffCells))
    for i in range (0, len(startVertices)):
        foutmatrixstart.write('%s\n' %startVertices[i])

    foutmatrixend.write('number of destinations=%s\n' %len(endVertices))    
    for j in range (0, len(endVertices)):
        foutmatrixend.write('%s\n' %endVertices[j])

    foutmatrixstart.close()
    foutmatrixend.close()

# output the network data which is based on the SUMO-network
def outputNetwork(net):
    foutnet = file('network.txt', 'w')
    net.printNet(foutnet)
    foutnet.close()

# ouput the required CPU time for the assignment and the assignment results (e.g. link flows, link travel times)
def outputStatistics(net, starttime, Parcontrol):
    totaltime = 0.0
    totalflow = 0.0
    assigntime = datetime.datetime.now() - starttime
    foutMOE = file('MOE.txt', 'w')
    foutMOE.write('Number of analyzed periods(hr):%s' %(int(Parcontrol[(len(Parcontrol)-2)])))
    for edgeName, edgeObj in net._edges.iteritems():                                      # generate the output of the link travel times
        if str(edgeObj.source) != str(edgeObj.target) and edgeObj.estcapacity > 0.:
            totaltime += edgeObj.flow * edgeObj.actualtime
            totalflow += edgeObj.flow
            foutMOE.write('\nedge:%s \t from:%s \t to:%s \t freeflowtime(s):%2.2f \t traveltime(s):%2.2f \t traffic flow(veh):%2.2f \t v/c:%2.2f' \
            %(edgeName, edgeObj.source, edgeObj.target, edgeObj.freeflowtime, edgeObj.actualtime, edgeObj.flow, (edgeObj.flow/edgeObj.estcapacity)))    
        if edgeObj.flow > edgeObj.estcapacity and edgeObj.connection == 0:
            foutMOE.write('****overflow!')

    avetime = totaltime / totalflow
    foutMOE.write('\nTotal flow(veh):%2.2f \t average travel time(s):%2.2f\n' %(totalflow, avetime))
    foutMOE.write('\nTime for the traffic assignment and reading matrices:%s' %assigntime)    
    foutMOE.close()
    return assigntime

# output the releasing time and the route for each vehicle
def sortedVehOutput(vehicles, foutroute):                                   
    vehicles.sort(key=operator.attrgetter('depart'))                         # sorting by departure times 
    for veh in vehicles:                                                     # output the generated routes 
        foutroute.write('    <vehicle id="%s" depart="%d">\n' %(veh.label, veh.depart))
        foutroute.write('        <route>')
        for edge in veh.route[1:-1]:                       # for generating vehicle routes used in SUMO 
            foutroute.write('%s ' % edge.label)
        foutroute.write('</route>\n')
        foutroute.write('    </vehicle>\n') 
    
# output the number of the released vehicles in the defined interval (when the Poisson distribution is used for generating vehicular releasing times)
def vehPoissonDistr(net, Parcontrol, begintime):
    foutpoisson = file('poisson.txt', 'w')
    if int(Parcontrol[(len(Parcontrol)-3)]) == 1:
        zaehler = 0
        interval = 10
        count = 0
        for veh in net._vehicles:
            if veh.depart <= float(begintime + interval):
                zaehler += 1
            else:
                foutpoisson.write('interval:%s, count:%s, %s\n' %(begintime+ interval, count, zaehler))
                zaehler = 1
                interval += 10
            count += 1
        foutpoisson.write('interval:%s, count:%s, %s\n' %(begintime+ interval, count, zaehler))
        foutpoisson.close()
    else:
        foutpoisson.write('The vehicular releasing times are generated randomly(uniform). ')
        foutpoisson.close()
        
# output the results of the significance tests
def getSignificanceTestOutput(net, tValueAvg, methods, normal, hValues):
    foutSGtest = file('SG_Test.txt', 'w')
    if normal:
        foutSGtest.write('The significances of the performance averages among the used assignment models are examined with the t test.\n')
        for A in net._assignments.itervalues():
            for B in net._assignments.itervalues():
                if str(A.label) != str(B.label):
                    foutSGtest.write('\nmethod:%s' %A.label)
                    foutSGtest.write('\nmethod:%s' %B.label)
                    foutSGtest.write('\n   t-value for the avg. travel time:%s' %tValueAvg[A][B].avgtraveltime)
                    foutSGtest.write('\n   t-value for the avg. travel length:%s'%tValueAvg[A][B].avgtravellength)
                    foutSGtest.write('\n   t-value for the avg.travel speed:%s' %tValueAvg[A][B].avgtravelspeed)
                    foutSGtest.write('\n   t-value for the avg. stop time:%s\n' %tValueAvg[A][B].avgstoptime)
    else:
        foutSGtest.write('The samples are not normal distributed.\n')
        foutSGtest.write('The significance test among the different assignment methods is therefore done with the Kruskal-Wallis test.\n')
        for h in hValues:
            foutSGtest.write('\n\nmethods:%s' %h.label)
            foutSGtest.write('\nH_traveltime:%s' %h.traveltime)
            foutSGtest.write('\nH_travelspeed:%s' %h.travelspeed)
            foutSGtest.write('\nH_travellength:%s' %h.travellength)
            foutSGtest.write('\nH_stoptime:%s\n' %h.stoptime)
            foutSGtest.write('\n95 chi-square value:%s' %h.lowchivalue)
            foutSGtest.write('\n99 chi-square value:%s\n' %h.highchivalue)
    foutSGtest.close()