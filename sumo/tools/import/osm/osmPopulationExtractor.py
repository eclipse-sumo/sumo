#!/usr/bin/env python
"""
@file    osmPopulationExactor.py
@author  Yun-Pang Floetteroed
@author  Melanie Knocke
@date    2013-02-08
@version $Id$

This script is to extract the popoulation data from a given Open Street Map (OSM).

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, sys
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

class Net():
    def __init__(self):
        self._relations = []
        self._nodes = []
        self._nodeMap = {}
        self._relationMap = {}
        self._uidNodeMap = {}
        self._uidRelationMap = {}
        
    def addNode(self, id, lat, lon, population):
        if id not in self._nodeMap:
            node = Node(id, lat, lon, population)
            self._nodes.append(node)
            self._nodeMap[id] = node
        return self._nodeMap[id]
        
    def addRelation(self, id, uid, population):
        if id not in self._relationMap:
            relation = Relation(id, uid, population)
            self._relations.append(relation)
            self._relationMap[id] = relation

        return self._relationMap[id]
        
class Node:
    """
    This class is to store node information.
    """
    def __init__(self, id, lat, lon, population):
        self.id = id
        self.lat = lat
        self.lon = lon
        self.uid = None
        self.place = None
        self.name = None
        self.population = population
    def __repr__(self):
        return "<%s|%s_%s_%s>" % (self.id, self.name, self.uid, self.population)
        
class Relation:
    """
    This class is to store relation information.
    """
    def __init__(self, id, uid, population):
        self.id = id
        self.uid = uid
        self.name = None
        self.type = None
        self.population = population
    def __repr__(self):
        return "%s|%s_%s" % (self.id, self.name, self.uid, self.population)
        
class PopulationReader(handler.ContentHandler):
    """The class is for parsing the OSM XML file.
       The data parsed is written into the net.
    """
    def __init__(self, net):
        self._net = net
        self._nodeId = None
        self._nodeObj = None
        self._nodeLat = None
        self._nodeLon = None
        self._nodeuid = None
        self._place = None
        self._relationId = None
        self._relationObj = None        
        self._relationuid = None
        self._type = None
        self._name = None
        self._population = None

    def startElement(self, name, attrs):
        if name =='node':
            self._nodeId = attrs['id']
            self._nodeLat = attrs['lat']
            self._nodeLon = attrs['lon']
            if attrs.has_key('uid'):
                self._nodeuid = attrs['uid']
        if self._nodeId and name == 'tag':
            if attrs['k'] == 'name':
                self._name = attrs['v'] #unicode(attrs['v']).encode("utf-8")
            if attrs['k'] == 'place':
                self._place = attrs['v']
            if not self._population and attrs['k'] == 'openGeoDB:population':
                self._population = attrs['v']
            if not self._population and attrs['k'] == 'population':
                self._population = attrs['v']
        if name =='relation':
            self._relationId = attrs['id']
            self._uid = attrs['uid']
            if attrs.has_key('uid'):
                self._relationuid = attrs['uid']
        if self._relationId and name == 'tag':
            if attrs['k'] == 'name':
                self._name = attrs['v'] #unicode(attrs['v']).encode("utf-8")
            if attrs['k'] == 'type':
                self._type = attrs['v']
            if not self._population and attrs['k'] == 'openGeoDB:population':
                self._population = attrs['v']
            if not self._population and attrs['k'] == 'population':
                self._population = attrs['v']

    def endElement(self, name):
        if name == 'node':
            if self._population:
                self._nodeObj = self._net.addNode(self._nodeId, self._nodeLat, self._nodeLon, self._population)
                if self._nodeuid:
                    self._nodeObj.uid = self._nodeuid
                    if self._nodeuid not in self._net._uidNodeMap:
                        self._net._uidNodeMap[self._nodeuid] = []
                    self._net._uidNodeMap[self._nodeuid].append(self._nodeObj)

                if self._name:
                    self._nodeObj.name = self._name
                if self._place:
                    self._nodeObj.place = self._place
            self._nodeId = None
            self._nodeObj = None
            self._nodeLat = None
            self._nodeLon = None
            self._nodeuid = None
            self._place = None
            self._name = None
            self._population = None
        if name == 'relation':
            if self._population:
                self._relationObj = self._net.addRelation(self._relationId, self._relationuid, self._population)
                self._relationObj.population = self._population
                if self._relationuid not in self._net._uidRelationMap:
                    self._net._uidRelationMap[self._relationuid] = []
                self._net._uidRelationMap[self._relationuid].append(self._relationObj)

                if self._name:
                    self._relationObj.name = self._name
                if self._type:
                    self._relationObj.place = self._type
            self._relationId = None
            self._relationObj = None        
            self._relationuid = None
            self._type = None
            self._name = None
            self._population = None

def main():
    print 'begin the program'
    parser = make_parser()
    osmFile = sys.argv[1]
    print 'osmFile:', osmFile
    #osmFile = 'bremen.xml'
    prefix = osmFile.split('.')[0]
    net = Net()
    parser.setContentHandler(PopulationReader(net))
    parser.parse(osmFile)
    
    print 'finish with data parsing'
    print 'write output files'
    outputfile = '%s_nodesWithPopulation.txt' %prefix
    fout = open(outputfile, 'w')
    fout.write('nodeId\tname\tlat\tlon\tuid\tpopulation\n')
    fout.close()
    for n in net._nodes:
        if os.path.exists(outputfile):
            fout = open(outputfile, 'a')
        print n.id, n.name, n.lat, n.lon, n.uid, n.population
        fout.write(('%s\t%s\t%s\t%s\t%s\t%s\n' % (n.id, n.name, n.lat, n.lon, n.uid, n.population)).encode("latin1"))
        fout.close()

    fout = open('%s_relationsWithPopulation.txt' %prefix, 'w')
    fout.write('relationId\tname\tuid\tpopulation\n')
    for r in net._relations:
        fout.write('%s\t%s\t%s\t%s\n' % (r.id, r.name, r.uid, r.population))
    fout.close()
    
    fout = open('%s_nodesWithSameUid.txt' %prefix, 'w')
    fout.write('nodeUid\tnodeId\tname\n')
    for r in net._uidNodeMap:
        fout.write('%s' % r)
        for n in net._uidNodeMap[r]:
            fout.write(('\t%s\t%s' % (n.id,n.name)).encode("latin1"))
        fout.write('\n')
    fout.close()
    
    fout = open('%s_uidRelations.txt' %prefix, 'w')
    fout.write('relationUid\trelationId\tname\n')
    for r in net._uidRelationMap:
        fout.write('%s' % r)
        for n in net._uidRelationMap[r]:
            fout.write('\t%s\t%s' % (n.id, n.name))
        fout.write('\n')
    fout.close()
    print 'finished'

main()