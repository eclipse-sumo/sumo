#!/usr/bin/env python
"""
@file    outputs.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-12-25
@version $Id$

This script is for generating the outputs from the choosed traffic assignment.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, random, string, sys, datetime, operator, math
from network import Net
from elements import Vehicle

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
def sortedVehOutput(vehicles, departtime, foutroute):                                   
    for veh in vehicles:                                                       
        if veh.depart == 0:
            veh.depart = random.randint(departtime, departtime + 3600)
    vehicles.sort(key=operator.attrgetter('depart'))                         # sorting by departure times 
    for veh in vehicles:                                                     # output the generated routes 
        foutroute.write('    <vehicle id="%s" depart="%d" departlane="free">\n' %(veh.label, veh.depart))
        foutroute.write('        <route>')
        for edge in veh.route[1:-1]:                       # for generating vehicle routes used in SUMO 
            foutroute.write('%s ' % edge.label)
        foutroute.write('</route>\n')
        foutroute.write('    </vehicle>\n') 
    
# output the result of the matrix estimation with the traffic counts
def outputMatrix(startVertices, endVertices, estMatrix, daytimeindex):
    filename = 'estimatedMatri-' + daytimeindex + '.fma'
    foutmtx = file(filename, 'w')
    
    foutmtx.write('$VMR;D2;estimated with the generalized least squares model\n')
    foutmtx.write('* Verkehrsmittelkennung\n') 
    foutmtx.write('   1\n')
    foutmtx.write('* Von  Bis\n\n')
    foutmtx.write('* Faktor\n')
    foutmtx.write('1.00\n')
    foutmtx.write('*\n')
    foutmtx.write('* Deutsches Zentrum fuer Luft- und Raumfahrt e.V.\n')
    foutmtx.write('* %s\n' %datetime.datetime.now())
    foutmtx.write('* Anzahl Bezirke\n')
    foutmtx.write('%s\n' %len(startVertices))
    foutmtx.write('*\n')
    for startVertex in startVertices:
        foutmtx.write('%s ' %startVertex.label)
    foutmtx.write('\n*')
    for start, startVertex in enumerate(startVertices):
        count = -1
        foutmtx.write('\n* from: %s\n' %startVertex.label)
        for end, endVertex in enumerate(endVertices):
            count += 1
            if operator.mod(count,12) != 0:
                foutmtx.write('%s ' %estMatrix[start][end])
            elif count > 12:
                foutmtx.write('\n%s '%estMatrix[start][end])
    foutmtx.close()