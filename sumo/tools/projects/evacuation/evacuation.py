#!/usr/bin/env python
# -*- coding: cp1252 -*-
"""
@file EvacuationStart.py
@author Martin Taraz
@date 2015-09-09

Starts the creation of an Evacuation Scenario

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os
import sys
import subprocess
import math
import csv

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
sys.path += [os.path.join(SUMO_HOME, "tools", "trip"), os.path.join(SUMO_HOME, "tools", "import", "osm"), os.path.join(SUMO_HOME, "tools")]

import osmGet
import sumolib
import osmBuild
import PopulationExtraction
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from xml.dom import minidom
from shapely.geometry import Polygon
import generateTraffic

def merge():
    
    csvReader = csv.reader(open("xBevölkerungsdaten_2015-9-30.csv"), delimiter=',' , quotechar ='"')
    InhabitantArray = []
    for row in csvReader:
        InhabitantArray.append(row);

    Polygon = ET.ElementTree(file='GemeindePolygons.XML')
    root = Polygon.getroot()

    counter = 0
    fortschritt = 0
    inhabDict = {}
    for entry in InhabitantArray[3:]:
                while(len(entry[0]) < 2 ):
                    entry[0] = '0' + entry[0]  
                while(len(entry[2]) < 2 ):
                    entry[2] = '0' + entry[2]
                while(len(entry[3]) < 4 ):
                    entry[3] = '0' + entry[3]
                while(len(entry[4]) < 3 ):
                    entry[4] = '0' + entry[4]          
                inhabDict["".join(entry[:5])] = str(entry[6]).replace(' ','')
                
    for parents in root.findall("./*"):
        for elem in parents.findall("param[7]"):
            RSValue =  str(elem.attrib)[11:23]
            
            inhabitants = SubElement(parents, 'param')
                
            if RSValue in inhabDict:
                inhabitants.clear()
                inhabitants.attrib = { 'key':"INHABITANTS", 'value': inhabDict[RSValue] }
        counter = counter + 1    
    outstr =  minidom.parseString(ET.tostring(root)).toprettyxml(indent = "   ")
    with open("PopulationPolygons.xml", 'w') as out:
        out.write(outstr.encode('utf-8'))

from shapely.geometry import Polygon
import xml.etree.cElementTree as ET
def extract():
    PopulationXML = ET.ElementTree(file=("PopulationPolygons.xml"))
    EvacuationXML = ET.ElementTree(file=("ConvertedEvaSite.poly.xml"))
    PopRoot = PopulationXML.getroot()
    EvaRoot = EvacuationXML.getroot()
    Schnittmenge = []
    for evaArea in EvaRoot.findall("./poly[0]"):
        koordinatenStr = evaArea.attrib["shape"]
        koordinatenList =[tuple(map(float, x.split(','))) for x in koordinatenStr.split() ]
        evacuationArea = Polygon(koordinatenList)

    for PopArea in PopRoot.findall("./poly"):
        koordinatenStr = PopArea.attrib["shape"]
        koordinatenList =[tuple(map(float, x.split(','))) for x in koordinatenStr.split() ]
        Gemeinde = Polygon(koordinatenList)
        if Gemeinde.intersection(evacuationArea):
            for Inhab in PopArea.findall("./param"):
                if Inhab.attrib["key"] == "INHABITANTS":
                    Einwohner = int((Inhab.attrib["value"]))
                    Einwohner *= Gemeinde.intersection(evacuationArea).area / Gemeinde.area
                    Einwohner = str(Einwohner)
                    Schnittmenge.append((Gemeinde.intersection(evacuationArea) ,Einwohner))
                    print("merge!")
                    
    root = ET.Element('aditional')
    outTree = ET.ElementTree(root)
    i=1
    for G in Schnittmenge:
        poly = ET.SubElement(root, 'poly')
        exterior = G[0].exterior.coords
        OutExterior = ''
        for elem in exterior:
            OutExterior += str(elem[0])+"," +str(elem[1])+" "
        identity = "Schnittflache"+str(i)
        poly.attrib ={"id":identity , "shape":OutExterior, "inhabitants":G[1] }
        i += 1
    outstr =  minidom.parseString(ET.tostring(root)).toprettyxml(indent = "   ")
   # outTree = ET.ElementTree(root)
    with open("Schnittflachen.xml", 'w') as out:
        out.write(outstr)

def buildEvaSite():
    dots = 10.00
    import xml.etree.cElementTree as ET
    from xml.etree.ElementTree import Element, SubElement, Comment, tostring     
    InputXML = ET.ElementTree(file="Evakuierung_POI.xml")
    root = InputXML.getroot()
    #x = lon , y = lat
    lon = float(root[0].attrib["lon"])
    lat = float(root[0].attrib["lat"])
    radius = float(root[0].attrib["radius"])
    radius = radius * 360 / 4008 / 1000
    rootOut= ET.Element("additional")
    poly = SubElement(rootOut, 'poly')
    shape = ""
    i = 0
    while i < dots:
        templon = lon + radius * math.cos(math.radians(i/dots * 360))
        templat = lat + (radius * math.sin(math.radians(i/dots * 360)) )* math.cos(math.radians(lat))
        shape += str(templon)+","+str(templat)+ " "
        i += 1
    poly.attrib = {"id" : "Kreis" , "color":"255,0,0" , "fill":"1" , "layer":"-1.00" , "shape":shape}
    #schreiben der sammelplätze
    a = 1
    while a <=4:
        lon = float(root[a].attrib["lon"])
        lat = float(root[a].attrib["lat"])
        radius = float(root[a].attrib["radius"])
        radius = radius * 360 / 4008 / 1000
        poly = SubElement(rootOut, 'poly')
        shape = ""
        i = 0
        while i < dots:
            templon = lon + radius * math.cos(math.radians(i/dots * 360))
            templat = lat + (radius * math.sin(math.radians(i/dots * 360)) )* math.cos(math.radians(lat))
            shape += str(templon)+","+str(templat)+ " "
            i += 1
            poly.attrib = {"id" : "Sammelplatz"+str(a) , "color":"0,0,255" , "fill":"1" , "layer":"-1.00" , "shape":shape}
        a += 1
    outstr =  minidom.parseString(ET.tostring(rootOut)).toprettyxml(indent = "   ")
    with open("Evacuationsite.poly.xml", 'w') as out:
        out.write(outstr)
    
print("building Evacuation Site")
buildEvaSite()
print("osm Get")
osmGet.get(["-x", "Evacuationsite.poly.xml"])
print("osm Build")
osmOptions = ['-f', 'osm_bbox.osm.xml', '-p', 'testserveroutput','--vehicle-classes','road' ,
              '-m', os.path.join(SUMO_HOME, 'data', 'typemap', 'osmPolyconvert.typ.xml'),
              '--netconvert-options','--geometry.remove,--remove-edges.isolated,--roundabouts.guess,--ramps.guess,-v,--junctions.join,--tls.guess-signals,--tls.discard-simple,--tls.join,--junctions.corner-detail,5']
osmBuild.build(osmOptions)
print("polyconvert")
sys.stdout.flush()
subprocess.call([sumolib.checkBinary('polyconvert'), '-n', 'testserveroutput.net.xml', '--xml-files', 'Evacuationsite.poly.xml','-o', 'ConvertedEvaSite.poly.xml'])
print("merging")
merge()
print("extracting population data")
extract()
print("generating traffic")
generateTraffic.generate()
print("calling sumo")
sys.stdout.flush()
sumo= sumolib.checkBinary('sumo')
sumoOptions = [sumo, '-n', "testserveroutput.net.xml" , '-a' , "testserveroutput.poly.xml,Evakuierung_POI.xml,ConvertedEvaSite.poly.xml", '-r' , 'TRAFFIC.xml', '--ignore-route-errors', '--save-configuration', 'sumo.config']
subprocess.call(sumoOptions)
subprocess.call([sumolib.checkBinary('sumo-gui') ,'-c' , 'sumo.config'])
print("done")
