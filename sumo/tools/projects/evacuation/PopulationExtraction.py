# -*- coding: cp1252 -*-
#move to EvacuationStart
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
    outTree = ET.ElementTree(root)
    outTree.write("Schnittflachen.xml")
if __name__ == "__main__":
    extract()
