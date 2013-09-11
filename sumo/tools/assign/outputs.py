"""
@file    outputs.py
@author  Yun-Pang Wang
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2007-12-25
@version $Id$

This script is for generating the outputs from the choosed traffic assignment.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
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
def outputStatistics(net, starttime, periods):
    totaltime = 0.0
    totalflow = 0.0
    assigntime = datetime.datetime.now() - starttime
    foutMOE = file('MOE.txt', 'w')
    foutMOE.write('Number of analyzed periods(hr):%s' %periods)
    for edge in net._edges:                                      # generate the output of the link travel times
        if edge.estcapacity > 0.:
            totaltime += edge.flow * edge.actualtime
            totalflow += edge.flow
            foutMOE.write('\nedge:%s \t from:%s \t to:%s \t freeflowtime(s):%2.2f \t traveltime(s):%2.2f \t traffic flow(veh):%2.2f \t v/c:%2.2f' \
            %(edge._id, edge._from, edge._to, edge.freeflowtime, edge.actualtime, edge.flow, (edge.flow/edge.estcapacity)))    
        if edge.flow > edge.estcapacity and edge.connection == 0:
            foutMOE.write('****overflow!')

    avetime = totaltime / totalflow
    foutMOE.write('\nTotal flow(veh):%2.2f \t average travel time(s):%2.2f\n' %(totalflow, avetime))
    foutMOE.write('\nTime for the traffic assignment and reading matrices:%s' %assigntime)    
    foutMOE.close()
    return assigntime

# output the releasing time and the route for each vehicle
def sortedVehOutput(vehicles, departtime, options, foutroute):
    random.seed(42)
    for veh in vehicles:                                                       
        if veh.depart == 0:
            veh.depart = random.randint(departtime, departtime + 3600*options.hours)
    vehicles.sort(key=operator.attrgetter('depart'))                         # sorting by departure times 
    for veh in vehicles:                                                     # output the generated routes 
        foutroute.write('    <vehicle id="%s" depart="%d" departLane="free">\n' %(veh.label, veh.depart))
        foutroute.write('        <route>')
        for edge in veh.route[1:-1]:                       # for generating vehicle routes used in SUMO 
            foutroute.write('%s ' % edge._id)
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
    
def linkChoicesOutput(net, startVertices, endVertices, matrixPshort, linkChoiceMap, odPairsMap, outputdir, starttime):
    foutchoice = file(os.path.join(outputdir, "linkchoices.xml"), 'w')
    print >> foutchoice, """<?xml version="1.0"?>
<!-- generated on %s by $Id$ -->
<edgechoices>""" % starttime
    for e in net._detectedEdges:
        if len(linkChoiceMap[e.detected])> 0:
            foutchoice.write('    <edge id="%s" flows="%s">\n' %(e.label, e.flow))
            foutchoice.write('        <choiceprobs>\n')
            for start, startVertex in enumerate(startVertices):
                for end, endVertex in enumerate(endVertices):
                    if startVertex.label != endVertex.label and matrixPshort[start][end] > 0.:
                        odIndex = odPairsMap[startVertex.label][endVertex.label]
                        foutchoice.write('            <choice origin="%s" destination="%s" choiceprob="%.5f"/>\n' \
                                      %(startVertex.label, endVertex.label, linkChoiceMap[e.detected][odIndex]/matrixPshort[start][end]))
            foutchoice.write('        </choiceprobs>\n')
            foutchoice.write('    </edge>\n')
    foutchoice.write('</edgechoices>\n')
    foutchoice.close()