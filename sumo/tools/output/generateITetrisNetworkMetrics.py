#!/usr/bin/env python
"""
@file    generateITetrisNetworkMetrics.py 
@author  Daniel.Krajzewicz@dlr.de
@date    2007-10-25
@version $Id$

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
from optparse import OptionParser
import os
from pylab import *
from xml.sax import saxutils, make_parser, handler
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


def quantil(a, alpha):
    asort = sort(a)
    n = len(asort)
    j = int(n * alpha)
    if (j*alpha == n):
        return 0.5 * (asort[j-1] + asort[j])
    else:
        return asort[j]
    
# This class is for storing vehicle information, such as departure time, route and travel time.
class Vehicle:
    def __init__(self, label):
        self.label = label
        self.depart = 0.
        self.arrival = 0.       
        self.speed = 0.
        self.traveltime = 0.
        self.travellength = 0.
        self.departdelay = 0.
        self.waittime = 0.
        self.fuel_consumption = 0.
        self.co = 0.
        self.co2 = 0.
        self.hc = 0.
        self.pmx = 0.
        self.nox = 0.
        self.vtype = None

    def __repr__(self):
        return "%s_%s_%s_%s_%s_%s<%s>" % (self.label, self.depart, self.arrival, self.speed, self.traveltime, self.travellength)
     
class VehInformationReader(handler.ContentHandler):
    def __init__(self, vehList):
        self._vehList = vehList
        self._Vehicle = None

            
    def startElement(self, name, attrs):
        if name == 'tripinfo':
            self._Vehicle = Vehicle(attrs['id'])
            self._Vehicle.vtype = attrs['vtype']
            self._Vehicle.traveltime = float(attrs['duration'])
            self._Vehicle.travellength = float(attrs['routeLength'])
            self._Vehicle.departdelay = float(attrs['departDelay'])
            self._Vehicle.waittime = float(attrs['departDelay']) + float(attrs['waitSteps']) 
            self._Vehicle.depart = float(attrs['depart'])
            self._vehList.append(self._Vehicle)
        if name == 'emissions':
            self._Vehicle.fuel_consumption = float(attrs['fuel_abs'])
            self._Vehicle.co = float(attrs['CO_abs'])
            self._Vehicle.co2 = float(attrs['CO2_abs'])
            self._Vehicle.hc = float(attrs['HC_abs'])
            self._Vehicle.pmx = float(attrs['PMx_abs'])
            self._Vehicle.nox = float(attrs['NOx_abs'])

class VehRoutesReader(handler.ContentHandler):
    def __init__(self):
        self._routes = []
        self._depart = 0
        self._arrival = 0
            
    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._depart = int(attrs['depart'])
            self._arrival = int(attrs['arrival'])
        if name == 'route':
            self._routes.append((attrs['edges'], self._arrival-self._depart))

def getAvgNrLanesPerStreet(netfile):
    parser = make_parser()
    net = sumonet.NetReader()
    parser.setContentHandler(net)
    parser.parse(netfile)
    net = net.getNet()
    
    nrLanes = 0
    for edge in net._edges:
        nrLanes += len(edge._lanes)
        
    return 1.0 * nrLanes / len(net._edges)

def getRouteDistributions(vehroutesFile):
    vehicles = []
    parser = make_parser()
    viReader = VehRoutesReader()
    parser.setContentHandler(viReader)
    parser.parse(vehroutesFile)
    
    routes = viReader._routes
    
    startEnd = {}
    for route in routes:
        routeSplit = route[0].split(' ')
        if (not startEnd.has_key((routeSplit[0], routeSplit[-1]))):
            startEnd[routeSplit[0], routeSplit[-1]] = {}
        if (not startEnd[routeSplit[0], routeSplit[-1]].has_key(route[0])):
            startEnd[routeSplit[0], routeSplit[-1]][route[0]] = [0, 0]   
        startEnd[routeSplit[0], routeSplit[-1]][route[0]][0] += 1
        startEnd[routeSplit[0], routeSplit[-1]][route[0]][1] += route[1]    
    return startEnd
           
def getBasicStats(vehicles):
    totalVeh = 0.
    aTravelTime = []
    aTravelLength = []
    aTravelSpeed = []
    aWaitTime = []
    aDepartDelay = []
    aFuelConsumption = []
    aCO = []
    aCO2 = []
    aHC = []
    aPMx = []
    aNOx = []
          
    for veh in vehicles:
        totalVeh += 1
        # unit: speed - m/s; traveltime - s; travel length - m    
        veh.speed = veh.travellength / veh.traveltime
        aTravelTime.append(veh.traveltime)
        aTravelLength.append(veh.travellength)
        aWaitTime.append(veh.waittime)
        aTravelSpeed.append(veh.speed)
        aDepartDelay.append(veh.departdelay)
        aFuelConsumption.append(veh.fuel_consumption)
        aCO.append(veh.co)
        aCO2.append(veh.co2)
        aHC.append(veh.hc)
        aPMx.append(veh.pmx)
        aNOx.append(veh.nox)


    assignments = {}
    assignments['totalVeh'] = totalVeh
    assignments['totalTravelTime'] = sum(aTravelTime)
    assignments['totalTravelLength'] = sum(aTravelLength)
    assignments['totalDepartDelay'] = sum(aDepartDelay)
    assignments['totalWaitTime'] = sum(aWaitTime)
    assignments['totalFuelConsumption'] = sum(aFuelConsumption)
    assignments['totalCO'] = sum(aCO)
    assignments['totalCO2'] = sum(aCO2)
    assignments['totalHC'] = sum(aHC)
    assignments['totalPMx'] = sum(aPMx)
    assignments['totalNOx'] = sum(aNOx)

    assignments['avgTravelTime'] = mean(aTravelTime)
    assignments['avgTravelLength'] = mean(aTravelLength)
    assignments['avgTravelSpeed'] = mean(aTravelSpeed)
    assignments['avgDepartDelay'] = mean(aDepartDelay)
    assignments['avgWaitTime'] = mean(aWaitTime)
    assignments['avgFuelConsumption'] = mean(aFuelConsumption)
    assignments['avgCO'] = mean(aCO)
    assignments['avgCO2'] = mean(aCO2)
    assignments['avgHC'] = mean(aHC)
    assignments['avgPMx'] = mean(aPMx)
    assignments['avgNOx'] = mean(aNOx)

    assignments['SDTravelTime'] = std(aTravelTime)
    assignments['SDLength'] = std(aTravelLength)
    assignments['SDSpeed'] = std(aTravelSpeed)
    assignments['SDWaitTime'] = std(aWaitTime)
    assignments['SDFuelConsumption'] = std(aFuelConsumption)
    assignments['SDCO'] = std(aCO)
    assignments['SDCO2'] = std(aCO2)
    assignments['SDHC'] = std(aHC)
    assignments['SDPMx'] = std(aPMx)
    assignments['SDNOx'] = std(aNOx)
    
    assignments['quartil25TravelTime'] = quantil(aTravelTime,0.25)
    assignments['quartil25Length'] = quantil(aTravelLength,0.25)
    assignments['quartil25Speed'] = quantil(aTravelSpeed,0.25)
    assignments['quartil25WaitTime'] = quantil(aWaitTime,0.25)
    assignments['quartil25FuelConsumption'] = quantil(aFuelConsumption,0.25)
    assignments['quartil25CO'] = quantil(aCO,0.25)
    assignments['quartil25CO2'] = quantil(aCO2,0.25)
    assignments['quartil25HC'] = quantil(aHC,0.25)
    assignments['quartil25PMx'] = quantil(aPMx,0.25)
    assignments['quartil25NOx'] = quantil(aNOx,0.25)
    
    assignments['quartil75TravelTime'] = quantil(aTravelTime,0.75)
    assignments['quartil75Length'] = quantil(aTravelLength,0.75)
    assignments['quartil75Speed'] = quantil(aTravelSpeed,0.75)
    assignments['quartil75WaitTime'] = quantil(aWaitTime,0.75)
    assignments['quartil75FuelConsumption'] = quantil(aFuelConsumption,0.75)
    assignments['quartil75CO'] = quantil(aCO,0.75)
    assignments['quartil75CO2'] = quantil(aCO2,0.75)
    assignments['quartil75HC'] = quantil(aHC,0.75)
    assignments['quartil75PMx'] = quantil(aPMx,0.75)
    assignments['quartil75NOx'] = quantil(aNOx,0.75)
    
    assignments['medianTravelTime'] = quantil(aTravelTime,0.5)
    assignments['medianLength'] = quantil(aTravelLength,0.5)
    assignments['medianSpeed'] = quantil(aTravelSpeed,0.5)
    assignments['medianWaitTime'] = quantil(aWaitTime,0.5)
    assignments['medianFuelConsumption'] = quantil(aFuelConsumption,0.5)
    assignments['medianCO'] = quantil(aCO,0.5)
    assignments['medianCO2'] = quantil(aCO2,0.5)
    assignments['medianHC'] = quantil(aHC,0.5)
    assignments['medianPMx'] = quantil(aPMx,0.5)
    assignments['medianNOx'] = quantil(aNOx,0.5)
    
    return assignments


# output the network statistics based on the sumo-simulation results
def getStatisticsOutput(assignments, outputfile):
    foutveh = file(outputfile, 'w')
    foutveh.write('mean number of lanes per street : %s\n\n' % assignments['avgNrLanes'])
    
    foutveh.write('total number of vehicles : %s\n\n' % assignments['totalVeh'])
    
    foutveh.write('travel time\n')
    foutveh.write('===========\n')
    foutveh.write('total       : %s\n' % assignments['totalTravelTime'])
    foutveh.write('mean        : %s\n' % assignments['avgTravelTime'])
    foutveh.write('std         : %s\n' % assignments['SDTravelTime'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25TravelTime'])
    foutveh.write('median      : %s\n' % assignments['medianTravelTime'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75TravelTime'])
    
    foutveh.write('travel speed\n')
    foutveh.write('============\n')
    foutveh.write('mean        : %s\n' % assignments['avgTravelSpeed'])
    foutveh.write('std         : %s\n' % assignments['SDSpeed'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25Speed'])
    foutveh.write('median      : %s\n' % assignments['medianSpeed'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75Speed'])
    
    foutveh.write('waiting time\n')
    foutveh.write('============\n')
    foutveh.write('total       : %s\n' % assignments['totalWaitTime'])
    foutveh.write('mean        : %s\n' % assignments['avgWaitTime'])
    foutveh.write('std         : %s\n' % assignments['SDWaitTime'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25WaitTime'])
    foutveh.write('median      : %s\n' % assignments['medianWaitTime'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75WaitTime'])
    
    foutveh.write('distance travelled\n')
    foutveh.write('==================\n')
    foutveh.write('total       : %s\n' % assignments['totalTravelLength'])
    foutveh.write('mean        : %s\n' % assignments['avgTravelLength'])
    foutveh.write('std         : %s\n' % assignments['SDLength'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25Length'])
    foutveh.write('median      : %s\n' % assignments['medianLength'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75Length'])    
    
    foutveh.write('fuel consumption\n')
    foutveh.write('================\n')
    foutveh.write('total       : %s\n' % assignments['totalFuelConsumption'])
    foutveh.write('mean        : %s\n' % assignments['avgFuelConsumption'])
    foutveh.write('std         : %s\n' % assignments['SDFuelConsumption'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25FuelConsumption'])
    foutveh.write('median      : %s\n' % assignments['medianFuelConsumption'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75FuelConsumption'])      
    
    foutveh.write('CO emissions\n')
    foutveh.write('============\n')
    foutveh.write('total       : %s\n' % assignments['totalCO'])
    foutveh.write('mean        : %s\n' % assignments['avgCO'])
    foutveh.write('std         : %s\n' % assignments['SDCO'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25CO'])
    foutveh.write('median      : %s\n' % assignments['medianCO'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75CO'])      
    
    foutveh.write('CO2 emissions\n')
    foutveh.write('=============\n')
    foutveh.write('total       : %s\n' % assignments['totalCO2'])
    foutveh.write('mean        : %s\n' % assignments['avgCO2'])
    foutveh.write('std         : %s\n' % assignments['SDCO2'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25CO2'])
    foutveh.write('median      : %s\n' % assignments['medianCO2'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75CO2'])   
    
    foutveh.write('HC emissions\n')
    foutveh.write('============\n')
    foutveh.write('total       : %s\n' % assignments['totalHC'])
    foutveh.write('mean        : %s\n' % assignments['avgHC'])
    foutveh.write('std         : %s\n' % assignments['SDHC'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25HC'])
    foutveh.write('median      : %s\n' % assignments['medianHC'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75HC']) 
    
    foutveh.write('PMx emissions\n')
    foutveh.write('=============\n')
    foutveh.write('total       : %s\n' % assignments['totalPMx'])
    foutveh.write('mean        : %s\n' % assignments['avgPMx'])
    foutveh.write('std         : %s\n' % assignments['SDPMx'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25PMx'])
    foutveh.write('median      : %s\n' % assignments['medianPMx'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75PMx'])     
    
    foutveh.write('NOx emissions\n')
    foutveh.write('=============\n')
    foutveh.write('total       : %s\n' % assignments['totalNOx'])
    foutveh.write('mean        : %s\n' % assignments['avgNOx'])
    foutveh.write('std         : %s\n' % assignments['SDNOx'])
    foutveh.write('1/4-quantil : %s\n' % assignments['quartil25NOx'])
    foutveh.write('median      : %s\n' % assignments['medianNOx'])
    foutveh.write('3/4-quantil : %s\n\n' % assignments['quartil75NOx'])
    
    foutveh.write('\n\nRoute distribution:\n')
    foutveh.write('==================:\n')
    startEnd = assignment['routeDistr']
    for se in startEnd.iterkeys():
        for r in startEnd[se].iterkeys():
            foutveh.write('%s: Number of vehicles: %d, Avg Travel Time: %d\n' % (r, startEnd[se][r][0], 1.0 * startEnd[se][r][1] / startEnd[se][r][0]))
        foutveh.write('\n')
    foutveh.close()
    
    print 'mean number of lanes per street : %s\n' % assignments['avgNrLanes']
    
    print 'total number of vehicles : %s\n' % assignments['totalVeh']
    
    print 'travel time'
    print '==========='
    print 'total       : %s' % assignments['totalTravelTime']
    print 'mean        : %s' % assignments['avgTravelTime']
    print 'std         : %s' % assignments['SDTravelTime']
    print '1/4-quantil : %s' % assignments['quartil25TravelTime']
    print 'median      : %s' % assignments['medianTravelTime']
    print '3/4-quantil : %s\n' % assignments['quartil75TravelTime']
    
    print 'travel speed'
    print '============'
    print 'mean        : %s' % assignments['avgTravelSpeed']
    print 'std         : %s' % assignments['SDSpeed']
    print '1/4-quantil : %s' % assignments['quartil25Speed']
    print 'median      : %s' % assignments['medianSpeed']
    print '3/4-quantil : %s\n' % assignments['quartil75Speed']
    
    print 'waiting time'
    print '============'
    print 'total       : %s' % assignments['totalWaitTime']
    print 'mean        : %s' % assignments['avgWaitTime']
    print 'std         : %s' % assignments['SDWaitTime']
    print '1/4-quantil : %s' % assignments['quartil25WaitTime']
    print 'median      : %s' % assignments['medianWaitTime']
    print '3/4-quantil : %s\n' % assignments['quartil75WaitTime']
    
    print 'distance travelled'
    print '=================='
    print 'total       : %s' % assignments['totalTravelLength']
    print 'mean        : %s' % assignments['avgTravelLength']
    print 'std         : %s' % assignments['SDLength']
    print '1/4-quantil : %s' % assignments['quartil25Length']
    print 'median      : %s' % assignments['medianLength']
    print '3/4-quantil : %s\n' % assignments['quartil75Length']    
    
    print 'fuel consumption'
    print '================'
    print 'total       : %s' % assignments['totalFuelConsumption']
    print 'mean        : %s' % assignments['avgFuelConsumption']
    print 'std         : %s' % assignments['SDFuelConsumption']
    print '1/4-quantil : %s' % assignments['quartil25FuelConsumption']
    print 'median      : %s' % assignments['medianFuelConsumption']
    print '3/4-quantil : %s\n' % assignments['quartil75FuelConsumption']      
    
    print 'CO emissions'
    print '============'
    print 'total       : %s' % assignments['totalCO']
    print 'mean        : %s' % assignments['avgCO']
    print 'std         : %s' % assignments['SDCO']
    print '1/4-quantil : %s' % assignments['quartil25CO']
    print 'median      : %s' % assignments['medianCO']
    print '3/4-quantil : %s\n' % assignments['quartil75CO']      
    
    print 'CO2 emissions'
    print '============='
    print 'total       : %s' % assignments['totalCO2']
    print 'mean        : %s' % assignments['avgCO2']
    print 'std         : %s' % assignments['SDCO2']
    print '1/4-quantil : %s' % assignments['quartil25CO2']
    print 'median      : %s' % assignments['medianCO2']
    print '3/4-quantil : %s\n' % assignments['quartil75CO2']   
    
    print 'HC emissions'
    print '============'
    print 'total       : %s' % assignments['totalHC']
    print 'mean        : %s' % assignments['avgHC']
    print 'std         : %s' % assignments['SDHC']
    print '1/4-quantil : %s' % assignments['quartil25HC']
    print 'median      : %s' % assignments['medianHC']
    print '3/4-quantil : %s\n' % assignments['quartil75HC'] 
    
    print 'PMx emissions'
    print '============='
    print 'total       : %s' % assignments['totalPMx']
    print 'mean        : %s' % assignments['avgPMx']
    print 'std         : %s' % assignments['SDPMx']
    print '1/4-quantil : %s' % assignments['quartil25PMx']
    print 'median      : %s' % assignments['medianPMx']
    print '3/4-quantil : %s\n' % assignments['quartil75PMx']     
    
    print 'NOx emissions'
    print '============='
    print 'total       : %s' % assignments['totalNOx']
    print 'mean        : %s' % assignments['avgNOx']
    print 'std         : %s' % assignments['SDNOx']
    print '1/4-quantil : %s' % assignments['quartil25NOx']
    print 'median      : %s' % assignments['medianNOx']
    print '3/4-quantil : %s\n' % assignments['quartil75NOx']
    
def getCSVOutput(assignments, path, veh_types, interval):
    f_mean_travel_time = file(os.path.join(options.path, 'mean_travel_time.csv'), 'w')
    f_mean_speed = file(os.path.join(options.path, 'mean_speed.csv'), 'w')
    f_mean_waiting_time = file(os.path.join(options.path, 'mean_waiting_time.csv'), 'w')
    f_mean_distance_travelled = file(os.path.join(options.path, 'mean_distance_travelled.csv'), 'w')
    f_mean_fuel_consumption = file(os.path.join(options.path, 'mean_fuel_consumption.csv'), 'w')
    f_mean_CO_emissions = file(os.path.join(options.path, 'mean_CO_emissions.csv'), 'w')
    f_mean_CO2_emissions = file(os.path.join(options.path, 'mean_CO2_emissions.csv'), 'w')
    f_mean_HC_emissions = file(os.path.join(options.path, 'mean_HC_emissions.csv'), 'w')
    f_mean_PMx_emissions = file(os.path.join(options.path, 'mean_PMx_emissions.csv'), 'w')
    f_mean_NOx_emissions = file(os.path.join(options.path, 'mean_NOx_emissions.csv'), 'w')
    f_abs_travel_time = file(os.path.join(options.path, 'abs_travel_time.csv'), 'w')
    f_abs_waiting_time = file(os.path.join(options.path, 'abs_waiting_time.csv'), 'w')
    f_abs_distance_travelled = file(os.path.join(options.path, 'abs_distance_travelled.csv'), 'w')
    f_abs_fuel_consumption = file(os.path.join(options.path, 'abs_fuel_consumption.csv'), 'w')
    f_abs_CO_emissions = file(os.path.join(options.path, 'abs_CO_emissions.csv'), 'w')
    f_abs_CO2_emissions = file(os.path.join(options.path, 'abs_CO2_emissions.csv'), 'w')
    f_abs_HC_emissions = file(os.path.join(options.path, 'abs_HC_emissions.csv'), 'w')
    f_abs_PMx_emissions = file(os.path.join(options.path, 'abs_PMx_emissions.csv'), 'w')
    f_abs_NOx_emissions = file(os.path.join(options.path, 'abs_NOx_emissions.csv'), 'w')
    
    files = []
    files.append(f_mean_travel_time)
    files.append(f_mean_speed)
    files.append(f_mean_waiting_time)
    files.append(f_mean_distance_travelled)
    files.append(f_mean_fuel_consumption)
    files.append(f_mean_CO_emissions)
    files.append(f_mean_CO2_emissions)
    files.append(f_mean_HC_emissions)
    files.append(f_mean_PMx_emissions)
    files.append(f_mean_NOx_emissions)
    files.append(f_abs_travel_time)
    files.append(f_abs_waiting_time)
    files.append(f_abs_distance_travelled)
    files.append(f_abs_fuel_consumption)
    files.append(f_abs_CO_emissions)
    files.append(f_abs_CO2_emissions)
    files.append(f_abs_HC_emissions)
    files.append(f_abs_PMx_emissions)
    files.append(f_abs_NOx_emissions)
    
    for f in files:
        f.write(';')
    
    for veh_type in veh_types:
        head = veh_type + '(mean);' + veh_type + '(std);' + veh_type + '(1/4-quantil);' + veh_type + '(median);' + veh_type + '(3/4-quantil);'
        f_mean_travel_time.write(head)
        f_mean_speed.write(head)
        f_mean_waiting_time.write(head)
        f_mean_distance_travelled.write(head)
        f_mean_fuel_consumption.write(head)
        f_mean_CO_emissions.write(head)
        f_mean_CO2_emissions.write(head)
        f_mean_HC_emissions.write(head)
        f_mean_PMx_emissions.write(head)
        f_mean_NOx_emissions.write(head)
        f_abs_travel_time.write(veh_type + ';')
        f_abs_waiting_time.write(veh_type + ';')
        f_abs_distance_travelled.write(veh_type + ';')
        f_abs_fuel_consumption.write(veh_type + ';')
        f_abs_CO_emissions.write(veh_type + ';')
        f_abs_CO2_emissions.write(veh_type + ';')
        f_abs_HC_emissions.write(veh_type + ';')
        f_abs_PMx_emissions.write(veh_type + ';')
        f_abs_NOx_emissions.write(veh_type + ';')
    
    for f in files:
        f.write('\n')
    
    t = 0
    while assignments.has_key(t):
        for f in files:
            f.write('[' + str(t) + ':' + str(t+interval-1) + '];')
        for veh_type in assignments[t].itervalues():
            f_mean_travel_time.write(str(veh_type['avgTravelTime']) + ";" + str(veh_type['SDTravelTime']) + ";" + str(veh_type['quartil25TravelTime']) + ";" + str(veh_type['medianTravelTime']) + ";" + str(veh_type['quartil75TravelTime']) + ";")
            f_mean_speed.write(str(veh_type['avgTravelSpeed']) + ";" + str(veh_type['SDSpeed']) + ";" + str(veh_type['quartil25Speed']) + ";" + str(veh_type['medianSpeed']) + ";" + str(veh_type['quartil75Speed']) + ";")
            f_mean_waiting_time.write(str(veh_type['avgWaitTime']) + ";" + str(veh_type['SDWaitTime']) + ";" + str(veh_type['quartil25WaitTime']) + ";" + str(veh_type['medianWaitTime']) + ";" + str(veh_type['quartil75WaitTime']) + ";")
            f_mean_distance_travelled.write(str(veh_type['avgTravelLength']) + ";" + str(veh_type['SDLength']) + ";" + str(veh_type['quartil25Length']) + ";" + str(veh_type['medianLength']) + ";" + str(veh_type['quartil75Length']) + ";")
            f_mean_fuel_consumption.write(str(veh_type['avgFuelConsumption']) + ";" + str(veh_type['SDFuelConsumption']) + ";" + str(veh_type['quartil25FuelConsumption']) + ";" + str(veh_type['medianFuelConsumption']) + ";" + str(veh_type['quartil75FuelConsumption']) + ";")
            f_mean_CO_emissions.write(str(veh_type['avgCO']) + ";" + str(veh_type['SDCO']) + ";" + str(veh_type['quartil25CO']) + ";" + str(veh_type['medianCO']) + ";" + str(veh_type['quartil75CO']) + ";")
            f_mean_CO2_emissions.write(str(veh_type['avgCO2']) + ";" + str(veh_type['SDCO2']) + ";" + str(veh_type['quartil25CO2']) + ";" + str(veh_type['medianCO2']) + ";" + str(veh_type['quartil75CO2']) + ";")
            f_mean_HC_emissions.write(str(veh_type['avgHC']) + ";" + str(veh_type['SDHC']) + ";" + str(veh_type['quartil25HC']) + ";" + str(veh_type['medianHC']) + ";" + str(veh_type['quartil75HC']) + ";")
            f_mean_PMx_emissions.write(str(veh_type['avgPMx']) + ";" + str(veh_type['SDPMx']) + ";" + str(veh_type['quartil25PMx']) + ";" + str(veh_type['medianPMx']) + ";" + str(veh_type['quartil75PMx']) + ";")
            f_mean_NOx_emissions.write(str(veh_type['avgNOx']) + ";" + str(veh_type['SDNOx']) + ";" + str(veh_type['quartil25NOx']) + ";" + str(veh_type['medianNOx']) + ";" + str(veh_type['quartil75NOx']) + ";")
            f_abs_travel_time.write(str(veh_type['totalTravelTime']) + ";")
            f_abs_waiting_time.write(str(veh_type['totalWaitTime']) + ";")
            f_abs_distance_travelled.write(str(veh_type['totalTravelLength']) + ";")
            f_abs_fuel_consumption.write(str(veh_type['totalFuelConsumption']) + ";")
            f_abs_CO_emissions.write(str(veh_type['totalCO']) + ";")
            f_abs_CO2_emissions.write(str(veh_type['totalCO2']) + ";")
            f_abs_HC_emissions.write(str(veh_type['totalHC']) + ";")
            f_abs_PMx_emissions.write(str(veh_type['totalPMx']) + ";")
            f_abs_NOx_emissions.write(str(veh_type['totalNOx']) + ";")   
        for f in files:
            f.write('\n')
        t += interval
    
    for f in files:
        f.close()
        
# initialise 
optParser = OptionParser()
optParser.add_option("-n", "--netfile", dest="netfile",
                     help="name of the netfile (f.e. 'inputs\\pasubio\\a_costa.net.xml')", metavar="<FILE>", type="string")
optParser.add_option("-p", "--path", dest="path",
                     help="name of folder to work with (f.e. 'outputs\\a_costa\\')", metavar="<FOLDER>", type="string")
optParser.add_option("-t", "--vehicle-types", dest="vehicle_types",
                     help="vehicle-types for which the values shall be generated", metavar="<VEHICLE_TYPE>[,<VEHICLE_TYPE>]", type="string")
optParser.add_option("-i", "--intervals", dest="interval",
                     help="intervals to be generated ([0:<TIME>-1], [<TIME>:2*<TIME>-1], ...)", metavar="<TIME>", type="int")

optParser.set_usage('\ngenerateITetrisNetworkMetrics.py -n inputs\\a_costa\\acosta.net.xml -p outputs\\a_costa\\ -t passenger2a,passenger5,passenger1,passenger2b,passenger3,passenger4 -i 500 \n' + 'generateITetrisNetworkMetrics.py -n inputs\\a_costa\\acosta.net.xml -p outputs\\a_costa\\ -i 500\n' + 'generateITetrisNetworkMetrics.py -n inputs\\a_costa\\acosta.net.xml -p outputs\\a_costa\\')
# parse options
(options, args) = optParser.parse_args()
if not options.netfile or not options.path:
    print "Missing arguments"
    optParser.print_help()
    exit()

interval = options.interval
netfile = options.netfile
vehroutefile = os.path.join(options.path, 'vehroutes.xml')

if interval == None:
    interval = 10000000


vehicles = []
parser = make_parser()
parser.setContentHandler(VehInformationReader(vehicles))
parser.parse(os.path.join(options.path, 'tripinfos.xml'))

if options.vehicle_types == None:
    vehicle_types = ['all']
else:
    vehicle_types = options.vehicle_types.split(',')

vehicles_of_type_interval = {}
for veh in vehicles:
    t = int(veh.depart / interval) * interval
    if (not vehicles_of_type_interval.has_key(t)):
        vehicles_of_type_interval[t] = {}
        for veh_type in vehicle_types:
            vehicles_of_type_interval[t][veh_type] = []
    if vehicle_types[0] == 'all':
        vehicles_of_type_interval[t]['all'].append(veh)
    else:
        if (veh.vtype in vehicle_types):
            vehicles_of_type_interval[t][veh.vtype].append(veh)
assignments = {}
for t in vehicles_of_type_interval.keys():
    assignments[t] = {}
    for veh_type in vehicle_types:
        assignments[t][veh_type] = getBasicStats(vehicles_of_type_interval[t][veh_type])
        
getCSVOutput(assignments, options.path, vehicle_types, interval)    
    
assignment = getBasicStats(vehicles)
assignment['avgNrLanes'] = getAvgNrLanesPerStreet(netfile)
assignment['routeDistr'] = getRouteDistributions(vehroutefile)
getStatisticsOutput(assignment, os.path.join(options.path, "network_metrics_summary.txt"))
    
print 'The calculation is done!'
