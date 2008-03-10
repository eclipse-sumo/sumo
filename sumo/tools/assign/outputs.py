# python
# Six outputs can be generated. The outputs include the required time for reading input data,
# the lists of origins and destinations, the network geometry data, the traffic flows on all links, 
# the travel times on all links, the required time for executing the incremental traffic assignment.

import os, random, string, sys, datetime
from network import Net
from elements import Vehicle
import operator

def TimeforInput(inputreaderstart):
    fouttime = file('processtime.txt', 'w')
    inputreadtime = datetime.datetime.now() - inputreaderstart  
    fouttime.write('Time for reading input files:%s\n' %inputreadtime)
    fouttime.close()
    
#def OutputODZone(startVertices, endVertices, Pshort_EffCells, Plong_EffCells, Truck_EffCells):
def OutputODZone(startVertices, endVertices, Pshort_EffCells, MatrixCounter):
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

def OutputNetwork(net):
    foutnet = file('network.txt', 'w')
    net.printNet(foutnet)
    foutnet.close()

def OutputMOE(net, ODcontrol):
    totaltime = 0.0
    totalflow = 0.0
    foutMOE = file('MOE.txt', 'w')
    foutMOE.write('Number of analyzed periods(hr):%s' %(int(ODcontrol[(len(ODcontrol)-2)])))
    for edgeName, edgeObj in net._edges.iteritems():                                      # generate the output of the link travel times
        if str(edgeObj.source) != str(edgeObj.target) and edgeObj.estcapacity > 0.:
            totaltime += edgeObj.flow * edgeObj.actualtime
            totalflow += edgeObj.flow
            foutMOE.write('\nedge:%s \t from:%s \t to:%s \t freeflowtime(s):%s \t traveltime(s):%s \t traffic flow(veh):%s \t v/c:%s' \
            %(edgeName, edgeObj.source, edgeObj.target, edgeObj.freeflowtime, edgeObj.actualtime, edgeObj.flow, (edgeObj.flow/edgeObj.estcapacity)))    
        if edgeObj.flow > edgeObj.estcapacity and edgeObj.connection == 0:
            foutMOE.write('****overflow!')

    avetime = totaltime / totalflow
    foutMOE.write('\nTotal flow(veh):%s \t average travel time(s):%s\n' %(totalflow, avetime))
    
    foutMOE.close()

def TimeforAssign(starttime):
    fouttime = file('processtime.txt', 'a')
    assigntime = datetime.datetime.now() - starttime
    fouttime.write('\nTime for the traffic assignment and reading matrices:%s' %assigntime)
    fouttime.close()
    return assigntime

    
def SortedVehOutput(net, counter, ODcontrol):                                   
    net._vehicles.sort(key=operator.attrgetter('depart'))                         # sorting by departure times 
    
    if counter == 0:
        foutroute = file('routes.txt', 'w')                                           # initialize the file for recording the routes
        foutroute.write('<routes>\n')
    else:
        foutroute = file('routes.txt', 'a') 
    for veh in net._vehicles:                                                     # output the generated routes 
#        foutroute = file('routes.txt', 'a')
        foutroute.write('<vehicle id="%s" depart="%d">\n' %(veh.label, veh.depart))
        foutroute.write('<route>')
        for edge in veh.route[1:-1]:                       # for generating vehicle routes used in SUMO 
            foutroute.write('%s ' %edge.label)
        foutroute.write('</route>\n')
        foutroute.write('</vehicle>\n') 
    if int(ODcontrol[(len(ODcontrol)-2)]) == (counter - 1):
        foutroute.write('</routes>\n')
    foutroute.close()
    
def VehPoissonDistr(net, ODcontrol, begintime):
    foutpoisson = file('poisson.txt', 'w')                                        # check if the vehicles are distributed according to the poisson distribution
    if int(ODcontrol[(len(ODcontrol)-3)]) == 1:
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
    
def AveVehTravelTime(vehID, net):
    foutroute = file('AveSpeedTimeLength.txt', 'w')                                           # initialize the file for recording the routes
    foutroute.write('average vehicular travel time(s) = the sum of all vehicular travel time / the number of vehicles\n')
    foutroute.write('average vehicular travel length(m) = the sum of all vehicular travel length / the number of vehicles\n')
    foutroute.write('average vehicular travel speed(m/s) = the sum of all vehicular travel speed / the number of vehicles\n')
    TotalTime = 0.
    TotalLength = 0.
    TotalSpeed = 0.
    for veh in net._vehicles:                                                     # output the generated routes 
        TotalTime += veh.traveltime
        TotalLength += veh.travellength
        TotalSpeed += veh.speed

    avetime = TotalTime / vehID
    avelength = TotalLength / vehID
    avespeed = TotalSpeed / vehID
    foutroute.write('Total number of vehicles:%s\n' %vehID)
    foutroute.write('Total travel time(s):%s, ' %TotalTime)    
    foutroute.write('average vehicular travel time(s):%s\n' %avetime)
    foutroute.write('Total travel length(m):%s, ' %TotalLength)
    foutroute.write('average vehicular travel length(m):%s\n' %avelength)
    foutroute.write('average vehicular travel speed(m/s):%s\n' %avespeed)
    foutroute.close()
