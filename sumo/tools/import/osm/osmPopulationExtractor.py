#!/usr/bin/env python
"""
@file    osmPopulationExactor.py
@author  Yun-Pang Floetteroed
@author  Melanie Knocke
@date    2013-02-08
@version $Id$

This script is to extract the popoulation data from a given Open Street Map (OSM).

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
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
        self.attribute = "node"
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
        self.attribute = "relation"
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
                self._name = attrs['v']
            if not self._name and attrs['k'] == 'name:de':
                self._name = attrs['v']
            if not self._name and attrs['k'] == 'openGeoDB:name':
                self._name = attrs['v']
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
                self._name = attrs['v']
            if attrs['k'] == 'type':
                self._type = attrs['v']
            if not self._population and attrs['k'] == 'openGeoDB:population':
                self._population = attrs['v']
            if not self._population and attrs['k'] == 'population':
                self._population = attrs['v']

    def endElement(self, name):
        if name == 'node' and self._population:
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

        if name == 'relation' and self._population:
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
    parser = make_parser()
    osmFile = options.osmfile
    print 'osmFile:', osmFile
    if options.bsafile:
        bsaFile = options.bsafile
        print 'bsaFile:', bsaFile
    if options.outputfile:
        prefix = options.outputfile
    else:
        prefix = osmFile.split('.')[0]
    net = Net()
    parser.setContentHandler(PopulationReader(net))
    parser.parse(osmFile)
    
    print 'finish with data parsing'
    if options.generateoutputs:
        print 'write the population to the output file'
        outputfile = '%s_populations.txt' %prefix
        fout = open(outputfile, 'w')
        fout.write("attribute\tid\tname\tuid\tpopulation\tlat\tlon\n")
        for n in net._nodes:
            fout.write(("%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (n.attribute, n.id, n.name, n.uid, n.population, n.lat, n.lon)).encode(options.encoding))
        fout.close()
        
        if os.path.exists(outputfile):
            fout = open(outputfile, 'a')
        else:
            print "there is no file named %s", outputfile
            print "A new file will be open."
            fout = open(outputfile, 'w')

        for r in net._relations:
            print r.attribute, r.id, r.name, r.uid, r.population
            print type(r.attribute), type(r.id), type(r.name), type(r.uid), type(r.population)
            fout.write(("%s\t%s\t%s\t%s\t%s\tNone\tNone\n" % (r.attribute, r.id, r.name, r.uid, r.population)).encode(options.encoding))
        fout.close()
        
        fout = open('%s_nodesWithSameUid.txt' %prefix, 'w')
        fout.write('nodeUid\tnodeId\tname\n')
        for r in net._uidNodeMap:
            fout.write('%s' % r)
            for n in net._uidNodeMap[r]:
                fout.write(('\t%s\t%s' % (n.id,n.name)).encode(options.encoding))
            fout.write('\n')
        fout.close()
        
        fout = open('%s_uidRelations.txt' %prefix, 'w')
        fout.write('relationUid\trelationId\tname\n')
        for r in net._uidRelationMap:
            fout.write('%s' % r)
            for n in net._uidRelationMap[r]:
                fout.write(('\t%s\t%s' % (n.id, n.name)).encode(options.encoding))
            fout.write('\n')
        fout.close()
        
    if options.bsafile:        
        print 'compare the data with the data from BSA'
        bsaTotalCount = 0
        matchedCount = 0
        bsaNamesList = []
        foutnone = open("%s_nameNone.txt" %prefix, 'w')
        foutnone.write("nodeId\tnodeName\tPopulation\tLat\tLon\n")
        fout = open("%s_matchedAreas.txt" %prefix, 'w')
        fout.write("#bsaName\tbsaArea\tbsaPop\tbsaLat\tbsaLon\tosmName\tosmAtt\tosmPop\tosmLat\tosmLon\n")
        noneList = []
        areasList =[] # names list of the found areas
        for line in open(options.bsafile):
            if '#' not in line:
                line = line.split('\n')[0]
                line = line.split(';')
                name = line[0].decode("latin1")
                area = float(line[1])
                pop = int(line[2])
                lon = line[3]
                lat = line[4]
                bsaTotalCount += 1
                if name in bsaNamesList:
                    print 'duplicated name:', name
                else:
                    bsaNamesList.append(name)
                    for n in net._nodes:
                        if n.name == None and n not in noneList:
                            noneList.append(n)
                        elif n.name != None and name == n.name and n.name not in areasList:
                            matchedCount += 1
                            fout.write(("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" %(name, area, pop, lat, lon, n.name, n.attribute, n.population, n.lat, n.lon)).encode(options.encoding))
                            areasList.append(n.name)
                        
                    for r in net._relations:
                        if r.name == None and r not in noneList:
                            noneList.append(r)
                        elif r.name != None and name == r.name and r.name not in areasList:
                            matchedCount += 1
                            fout.write(("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\tNone\tNone\n" %(name, area, pop, lat, lon, r.name, r.attribute, r.population)).encode(options.encoding))
                            areasList.append(r.name)
        fout.close()
        for n in noneList:
            foutnone.write(("%s\t%s\t%s\t%s\t%s\n" % (n.id, n.name, n.population, n.lat, n.lon)).encode(options.encoding))
        foutnone.close()
        osmTotalCount = len(net._nodes) + len(net._relations)  #Duplicated data could exist.
        duplicated = osmTotalCount - len(areasList)
        print prefix, '_matched count:', matchedCount
        print 'bsaTotalCount:', bsaTotalCount
        print 'osmTotalCount:', osmTotalCount
        print 'duplicated areas in osm:', duplicated
        print 'len(bsaNamesList):', len(bsaNamesList)

optParser = OptionParser()
optParser.add_option("-s", "--osm-file", dest="osmfile", 
                     help="read OSM file from FILE (mandatory)", metavar="FILE")
optParser.add_option("-b", "--bsa-file", dest="bsafile", 
                     help="read population (in csv form) provided by German federal statistic authority (Bundesstatistikamt) from FILE", metavar="FILE")
optParser.add_option("-o", "--output-file", dest="outputfile", 
                     help="define the output file name prefix")
optParser.add_option("-e", "--encoding", help="output file encoding (default: %default)", default="utf8")
optParser.add_option("-g", "--generate-outputs", dest="generateoutputs", action="store_true", 
                     default=False, help="generate output files")
(options, args) = optParser.parse_args()

if not options.osmfile:
    optParser.print_help()
    sys.exit()
main()
