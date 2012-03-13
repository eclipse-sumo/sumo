#!/usr/bin/env python
"""
@file    routeChoices.py
@author  Evamarie Wiessner
@author  Yun-Pang Floetteroed
@date    2007-02-27
@version $Id$

This script is to calculate the route choice probabilities based on different methods.
- Gawron
- step-size (TBD)
- ......

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import string, glob, os, sys, optparse, random,shutil
from optparse import OptionParser
from xml.sax import saxutils, make_parser, handler
from xml.sax import parse
from numpy import *
#from matplotlib import pyplot as plt
from math import *


class Vehicle:
    def __init__(self,label,depart, departlane, departpos, departspeed):
        self.label = label
        self.CO_abs = 0.
        self.CO2_abs = 0.
        self.HC_abs = 0.
        self.PMx_abs = 0.
        self.NOx_abs = 0.
        self.fuel_abs = 0.
        self.routesList = []
        #self.speed = 0.
        self.depart = float(depart)
        self.departlane= departlane
        self.departpos= departpos
        self.departspeed = departspeed
        self.selectedRoute = None
        
class Edge:
    def __init__(self,label):
        self.label = label
        self.length = 0.
        self.freespeed = 0.
        self.CO_abs = 0.
        self.CO2_abs = 0.
        self.HC_abs = 0.
        self.PMx_abs = 0.
        self.NOx_abs = 0.
        self.fuel_abs = 0.
        self.traveltime = 0.
        self.CO_perVeh = 0.
        self.CO2_perVeh = 0.
        self.HC_perVeh = 0.
        self.PMx_perVeh = 0.
        self.NOx_perVeh = 0.
        self.fuel_perVeh = 0.
        # only one veh on the edge
        self.fuel_perVeh_default = 0.
        self.CO_perVeh_default = 0.
        self.CO2_perVeh_default = 0.
        self.HC_perVeh_default = 0.
        self.PMx_perVeh_default = 0.
        self.NOx_perVeh_default = 0.
        self.fuel_perVeh_default = 0.
        self.freetraveltime = 0.

pathNum = 0
        
class Route:
    def __init__(self, edges):
        global pathNum
        self.label = "%s" % pathNum
        pathNum += 1
        self.edges = edges
        self.ex_probability = None
        self.act_probability = 0.
        self.selected = False
        self.ex_cost = 0.
        self.act_cost = 0.

class netReader(handler.ContentHandler):
    def __init__(self, edgesList, edgesMap):
        self._edgesList = edgesList
        self._edgesMap = edgesMap
        self._edgObj = None
        
    def startElement(self, name, attrs):
        if name == 'edge' and not attrs.has_key('function'):
            if attrs['id'] not in self._edgesMap:
                self._edgeObj = Edge(attrs['id'])
                self._edgesList.append(self._edgeObj)
                self._edgesMap[attrs['id']] = self._edgeObj
            if self._edgeObj and name == 'lane':
                self._edgeObj.length = float(attrs['length'])
                self._edgeObj.freespeed = float(attrs['speed'])  #todo when no data in dump. default for fuel consumption?
                self._edgeObj.freetraveltime = self._edgeObj.length/self._edgeObj.speed
    def endElement(self, name):
        if name == 'edge':
            self._edgObj = None
                
class addweightsReader(handler.ContentHandler):
    def __init__(self, edgesList, edgesMap):
        self._edgesList = edgesList
        self._edgesMap = edgesMap
        self._edgObj = None
        
    def startElement(self, name, attrs):
        if name == 'edge':
            if attrs['id'] in self._edgesMap:
                self._edgeObj = self._edgesMap[attrs['id']]
            if attrs.has_key('traveltime'):
                self._edgeObj.freetraveltime = float(attrs['traveltime'])
            if attrs.has_key('CO_perVeh'):
                self._edgeObj.CO_perVeh_default = float(attrs['CO_perVeh'])
            if attrs.has_key('CO2_perVeh'):
                self._edgeObj.CO2_perVeh_default = float(attrs['CO2_perVeh'])
            if attrs.has_key('HC_perVeh'):
                self._edgeObj.HC_perVeh_default = float(attrs['HC_perVeh'])
            if attrs.has_key('PMx_perVeh'):
                self._edgeObj.PMx_perVeh_default = float(attrs['PMx_perVeh'])
            if attrs.has_key('NOx_perVeh'):
                self._edgeObj.NOx_perVeh_default = float(attrs['NOx_perVeh'])
            if attrs.has_key('fuel_perVeh'):
                self._edgeObj.fuel_perVeh_default = float(attrs['fuel_perVeh'])
            if attrs.has_key('fuel_abs'):
                self._edgeObj.fuel_abs_default = float(attrs['fuel_abs'])
            if attrs.has_key('NOx_abs'):
                self._edgeObj.NOx_abs_default = float(attrs['NOx_abs'])
            if attrs.has_key('PMx_abs'):
                self._edgeObj.PMx_abs_default = float(attrs['PMx_abs'])
            if attrs.has_key('HC_abs'):
                self._edgeObj.HC_abs_default = float(attrs['HC_abs'])
            if attrs.has_key('CO2_abs'):
                self._edgeObj.CO2_abs_default = float(attrs['CO2_abs'])
            if attrs.has_key('CO_abs'):
                self._edgeObj.CO_abs_default = float(attrs['CO_abs'])                


class routeReader(handler.ContentHandler):
    def __init__(self, vehList, vehMap):
        self._vehList = vehList
        self._vehMap = vehMap
        self._vehObj = None
        self._routObj = None

    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vehObj = Vehicle(attrs['id'], attrs['depart'], attrs['departLane'], attrs['departPos'], attrs['departSpeed'])
            self._vehMap[attrs['id']] = self._vehObj
            self._vehList.append(self._vehObj)

        if self._vehObj and name == 'route':
            edgesList = attrs['edges'].split(' ')
            self._routObj = Route(" ".join(edgesList))
            self._vehObj.routesList.append(self._routObj)

    def endElement(self, name):
        if name == 'vehicle':
            self._vehObj = None
            self._routObj = None
                    
class vehrouteReader(handler.ContentHandler):
    def __init__(self, vehList, vehMap, edgesMap, fout, foutrout,ecoMeasure, alpha, beta):
        self._vehList = vehList
        self._vehMap = vehMap
        self._edgesMap = edgesMap
        self._fout = fout
        self._foutrout = foutrout
        self._ecoMeasure = ecoMeasure
        self._newroutesList = []
        self._alpha = alpha
        self._beta = beta
        self._vehObj = None
        self._routObj = None
        self._selected = None
        self._currentSelected = None
        self._count = 0
        self._existed = False

    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vehObj = Vehicle(attrs['id'], attrs['depart'], attrs['departLane'], attrs['departPos'], attrs['departSpeed'])
            self._vehMap[attrs['id']] = self._vehObj
            self._vehList.append(self._vehObj)
            
        if self._vehObj and name == 'routeDistribution':
             self._currentSelected = attrs['last']

        if self._vehObj and name == 'route':
            if self._count == int(self._currentSelected):
                self._vehObj.selectedRouteEdges = attrs['edges']
            self._count += 1
            for r in self._vehObj.routesList:
                if r.edges == attrs['edges']:
                    self._existed = True
                    self._routObj = r
                    break
            if not self._existed:
                self._routObj = Route(attrs['edges'])
                self._vehObj.routesList.append(self._routObj)
            if attrs.has_key('probability'):
                self._routObj.ex_probability = float(attrs['probability'])
            if attrs.has_key('cost'):
                self._routObj.ex_cost = float(attrs['cost'])
            for e in attrs['edges'].split(' '):
                eObj = self._edgesMap[e]
                if self._ecoMeasure != 'fuel' and eObj.traveltime == 0.:
                    self._routObj.act_cost += eObj.freetraveltime
                elif self._ecoMeasure != 'fuel' and eObj.traveltime > 0.:
                    self._routObj.act_cost += eObj.traveltime
                elif self._ecoMeasure == 'fuel' and eObj.fuel_perVeh == 0.:
                    self._routObj.act_cost += eObj.fuel_perVeh_default
                elif self._ecoMeasure == 'fuel' and eObj.fuel_perVeh > 0.:
                    self._routObj.act_cost += eObj.fuel_perVeh

    def endElement(self, name):
        if name == 'vehicle':
            if len(self._vehObj.routesList) == 1:
                self._vehObj.routesList[0].ex_probability = 1.
            for r in self._vehObj.routesList:
                if not r.ex_probability:
                    r.ex_probability = 1./float(len(self._vehObj.routesList))
                    print 'new probability is calculated.'
                    self._newroutesList.append(r)
            if len(self._newroutesList) > 0:
                addProb = 0.
                origProbSum  = 0.
                for r in self._vehObj.routesList:
                    if r in self._newroutesList:
                        addProb += r.ex_probability
                    else:
                        origProbSum += r.ex_probability
                for r in self._vehObj.routesList:
                    if r not in self._newroutesList:
                        r.ex_probability = r.ex_probability/origProbSum * (1. - addProb)
                        
            for r1 in self._vehObj.routesList:     # todo: add "one used route to all routes"
                for r2 in self._vehObj.routesList:
                    if r1.label != r2.label:
                        gawron(r1, r2, self._alpha, self._beta, self._vehObj.selectedRouteEdges)

            randProb = random.random()
            if len(self._vehObj.routesList) == 1:
                self._vehObj.routesList[0].act_probability = 1.
                self._selected = 0
            else:
                for i, r in enumerate(self._vehObj.routesList):
                    if i == 0 and r.act_probability >= randProb:
                        self._selected = i
                        break
                    elif i > 0 and (self._vehObj.routesList[i-1].act_probability + self._vehObj.routesList[i].act_probability) >= randProb:
                        self._selected = i
                        break
            # generate the *.rou.xml
            self._foutrout.write('    <vehicle id="%s" depart="%.2f" departLane="%s" departPos="%s" departSpeed="%s">\n' 
                              % (self._vehObj.label, self._vehObj.depart, self._vehObj.departlane, self._vehObj.departpos, self._vehObj.departspeed))
            self._foutrout.write('        <route edges="%s"/>\n'% self._vehObj.routesList[self._selected].edges)
            self._foutrout.write('    </vehicle> \n')
            
            #generate the *.rou.alt.xml
            self._fout.write('    <vehicle id="%s" depart="%.2f" departLane="%s" departPos="%s" departSpeed="%s">\n' 
                              % (self._vehObj.label, self._vehObj.depart, self._vehObj.departlane, self._vehObj.departpos, self._vehObj.departspeed))    
            self._fout.write('        <routeDistribution last="%s">\n' % self._selected)
                        
            for route in self._vehObj.routesList:
                self._fout.write('            <route cost="%s" probability="%s" edges="%s"/>\n' % (route.act_cost, route.act_probability, route.edges))
            self._fout.write('        </routeDistribution>\n')
            self._fout.write('    </vehicle> \n')
            
            self._newroutesList = []
            self._vehObj = None
            self._selected = None
            self._currentSelected = None
            self._count = 0
        if name == 'route':
            self._routObj = None
        if name == 'route-alternatives':
            self._fout.write('</route-alternatives>\n')
            self._fout.close()
            self._foutrout.write('</routes>\n')
            self._foutrout.close()
                
class dumpsReader(handler.ContentHandler):
    def __init__(self, edgesList,edgesMap):
        self._edgesList = edgesList
        self._edgeObj = None
        self._edgesMap = edgesMap

    def startElement(self, name, attrs):
        if name == 'edge':
            if attrs['id'] not in self._edgesMap:
                self._edgeObj = Edge(attrs['id'])
                self._edgesList.append(self._edgeObj)
                self._edgesMap[attrs['id']] = self._edgeObj
            else:
                self._edgeObj = self._edgesMap[attrs['id']]

            if attrs.has_key('traveltime'):
                self._edgeObj.traveltime = float(attrs['traveltime'])
            if attrs.has_key('CO_perVeh'):
                self._edgeObj.CO_perVeh = float(attrs['CO_perVeh'])
            if attrs.has_key('CO2_perVeh'):
                self._edgeObj.CO2_perVeh = float(attrs['CO2_perVeh'])
            if attrs.has_key('HC_perVeh'):
                self._edgeObj.HC_perVeh = float(attrs['HC_perVeh'])
            if attrs.has_key('PMx_perVeh'):
                self._edgeObj.PMx_perVeh = float(attrs['PMx_perVeh'])
            if attrs.has_key('NOx_perVeh'):
                self._edgeObj.NOx_perVeh = float(attrs['NOx_perVeh'])
            if attrs.has_key('fuel_perVeh'):
                self._edgeObj.fuel_perVeh = float(attrs['fuel_perVeh'])
            if attrs.has_key('fuel_abs'):
                self._edgeObj.fuel_abs = float(attrs['fuel_abs'])
            if attrs.has_key('NOx_abs'):
                self._edgeObj.NOx_abs = float(attrs['NOx_abs'])
            if attrs.has_key('PMx_abs'):
                self._edgeObj.PMx_abs = float(attrs['PMx_abs'])
            if attrs.has_key('HC_abs'):
                self._edgeObj.HC_abs = float(attrs['HC_abs'])
            if attrs.has_key('CO2_abs'):
                self._edgeObj.CO2_abs = float(attrs['CO2_abs'])
            if attrs.has_key('CO_abs'):
                self._edgeObj.CO_abs = float(attrs['CO_abs'])                
                
    def endElement(self, name):
        if name == 'edge':
            self._edgeObj = None

def resetEdges(edgesList,edgesMap):
    for eid in edgesList:
        e = edgesMap[eid]
        e.traveltime = 0.
        e.CO_abs = 0.
        e.CO2_abs = 0.
        e.HC_abs = 0.
        e.PMx_abs = 0.
        e.NOx_abs = 0.
        e.fuel_abs = 0.
        e.CO_perVeh = 0.
        e.CO2_perVeh = 0.
        e.HC_perVeh = 0.
        e.PMx_perVeh = 0.
        e.NOx_perVeh = 0.
        e.fuel_perVeh = 0.

def getRouteChoices(edgesMap, dumpfile, routeAltfile, netfile, addWeightsfile, alpha, beta, step, ecoMeasure=None):  # check with Micha
    random.seed(42)   # check with micha
    edgesList = []
    vehList = []
    vehMap = {}
    print 'run getRouteChoices'
    print 'ecoMeasure:', ecoMeasure
    outputPath = os.path.abspath(routeAltfile)
    outputPath = os.path.dirname(outputPath)
    prefix = os.path.basename(routeAltfile)
    prefix = prefix[:prefix.find('.')]
    print 'outputPath:', outputPath
    print 'prefix:', prefix
    outputAltfile = os.path.join(outputPath, prefix + '.rou.galt.xml')
    outputRoufile = os.path.join(outputPath, prefix + '.grou.xml')
    print outputAltfile
    print outputRoufile

    if step == 1:
        print 'parse network file'
        parse(netfile, netReader(edgesList, edgesMap))
        parse(addWeightsfile, addweightsReader(edgesList,edgesMap))
    else:
        resetEdges(edgesList, edgesMap)
        
    fout = open(outputAltfile, 'w')
    foutrout = open(outputRoufile, 'w')
    fout.write('<?xml version="1.0"?>\n')
    fout.write('<!--\n')
    fout.write('route choices are generated with use of %s' % os.path.join(os.getcwd(), 'routeChoices.py'))
    fout.write('-->\n')    
    fout.write('<route-alternatives>\n')
    foutrout.write('<?xml version="1.0"?>\n')
    foutrout.write('<!--\n')
    foutrout.write('route choices are generated with use of %s' % os.path.join(os.getcwd(), 'routeChoices.py'))
    foutrout.write('-->\n')
    foutrout.write('<routes>')

    print 'parse dumpfile'
    print dumpfile
    parse(dumpfile, dumpsReader(edgesList, edgesMap))
    print 'parse routeAltfile'
    print routeAltfile
    parse(routeAltfile, routeReader(vehList, vehMap))   # parse routeAltfile from SUMO
    print 'parse routeAltfile from externalGawron'
    ex_outputAltFile = prefix[:prefix.rfind('_')] + '_%03i' %(step-1) + '.rou.galt.xml'
    ex_outputAltFile = os.path.join(outputPath,ex_outputAltFile)
    print 'ex_outputAltFile:', ex_outputAltFile
    parse(ex_outputAltFile, vehrouteReader(vehList, vehMap, edgesMap,fout, foutrout,ecoMeasure,alpha, beta))
    
    return outputRoufile, edgesMap

def gawron(r1, r2, alpha, beta,selectedRouteEdges):
    a = alpha
    #print 'run gawron'
    if r1.edges != selectedRouteEdges:
        r1.act_cost = beta * r1.act_cost + (1. - beta) * r1.ex_cost
    if r2.edges != selectedRouteEdges:
        r2.act_cost = beta * r2.act_cost + (1. - beta) * r2.ex_cost
    
    delta = (r2.act_cost - r1.act_cost)/(r2.act_cost + r1.act_cost)

    g = math.exp(a*delta/(1-delta*delta))

    r1.act_probability = (r1.ex_probability*(r1.ex_probability + r2.ex_probability)*g)/(r1.ex_probability*g + r2.ex_probability)  # check together with Eva
    r2.act_probability = r1.ex_probability + r2.ex_probability - r1.act_probability
    
#Eva
# gegeben: 3 Routen
# fuel consumption

#def get_costs(p, l):
#    C = 1000
#    v = 10
#    F = 5000
#    Vi = 30
#    v0 = 16.67
#    c = 1
#    k = 0.5
#    q = 1000

#    Ti =  l/v*(1+(p*F/(2*C))*(p*F/(2*C)))
#    return c*Ti*(1 + k*p*F/q + 0.5*math.pow(Vi/v0,3)/math.pow(1+k*p*F/q,2))

#def gawron(p1, p2, c1, c2, l1, l2):
#    a = 0.5
#    b = 0.9
#    delta = (c2-c1)/(c2 + c1)
#    g = math.exp(a*delta/(1-delta*delta))
#    tmp = p1
#
#    p1 = (p1*(p1 + p2)*g)/(p1*g + p2)
#    p2 = tmp+p2-p1
#    c1 = get_costs(p1, l1)
#    c2 = b*get_costs(p2, l2) + (1-b)*c2
#
#    return p1, p2, c1, c2
#
#anz = 100
#
#l1 = 100
#l2 = 150
#l3 = 200
#
#c1 = get_costs(0,l1) # fuel consumuption
#c2 = get_costs(0,l2) # fuel consumuption
#c3 = get_costs(0,l3) # fuel consumuption
#
#p1 = 1
#p2 = 0
#p3 = 0
#
#for i in arange(anz):
#
#    if (i==1):
#        p1 = 0.5
#        p2 = 0.5
#
#    if (i==2):
#        p1 = p1-(0.33*p1)
#        p2 = 0.67-p1
#        p3 = 0.33
#
#    p1, p2, c1, c2 = gawron (p1, p2, c1, c2, l1, l2)
#    p1, p3, c1, c3 = gawron (p1, p3, c1, c3, l1, l3)
#    # p2, p3, c2, c3 = gawron (p2, p3, c2, c3, l2, l3)
#    plt.plot(i, p1, ls='', marker='o', mew=0, ms=2, color = 'b')
#    plt.plot(i, p2, ls='', marker='o', mew=0, ms=2, color = 'r')
#    plt.plot(i, p3, ls='', marker='o', mew=0, ms=2, color = 'g')

#plt.draw()
#plt.show()