# -*- coding: utf-8 -*-
"""
@file    xsd.py
@author  Marek Heinrich
@author  Michael Behrisch
@date    2014-01-20
@version $Id$

Helper classes for parsing xsd schemata.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os
from xml.dom import minidom

class XmlElement:
    def __init__(self, entity):
        self.name = entity.getAttribute('name')
        self.ref = entity.getAttribute('ref')
        self.type = entity.getAttribute('type')
        self.attributes = []
        self.children = []

    def __repr__(self):
        childList = [c.name for c in self.children]
        return "name '%s' ref '%s' type '%s' attrs %s %s"  % (self.name, self.ref, self.type, self.attributes, childList)

class XsdStructure():
    def __init__(self, xsdFile):        
        xmlDoc = minidom.parse(open(xsdFile))
        self.root = None
        self._namedElements = {}
        self._namedTypes = {}
        for btEntity in xmlDoc.getElementsByTagName('xsd:include'):
            path = btEntity.getAttribute('schemaLocation')
            fullPath = os.path.join(os.path.dirname(xsdFile), path)
            subStruc = XsdStructure(fullPath)
            self._namedElements.update(subStruc._namedElements)
            self._namedTypes.update(subStruc._namedTypes)
        for btEntity in xmlDoc.getElementsByTagName('xsd:element'):
            if btEntity.nodeType == 1 and btEntity.hasAttribute('name'):
                el = self.getElementStructure(btEntity, True)
                self._namedElements[el.name] = el
                if self.root is None:
                    self.root = el
        for btEntity in xmlDoc.getElementsByTagName('xsd:complexType'):
            if btEntity.nodeType == 1 and btEntity.hasAttribute('name'):
                el = self.getElementStructure(btEntity)
                self._namedTypes[el.name] = el
        self.resolveRefs()
        #print self._namedElements
        #print self._namedTypes

    def getElementStructure(self, entity, checkNestedType=False):
        eleObj = XmlElement(entity)        
        if checkNestedType:
            nestedTypes = entity.getElementsByTagName('xsd:complexType')
            if nestedTypes:
                entity = nestedTypes[0] # skip xsd:complex-tag
        for aa in entity.childNodes:
            if aa.nodeName =='xsd:attribute':
                for attrss in range(aa.attributes.length):
                    if aa.attributes.item(attrss).nodeName == 'name':
                        eleObj.attributes.append(aa.attributes.item(attrss).value)
                        break
            elif aa.nodeName =='xsd:sequence' or aa.nodeName =='xsd:choice':            
                for aae in aa.getElementsByTagName('xsd:element'):
                    seqObj = XmlElement(aae)
                    eleObj.children.append(seqObj)
        return eleObj

    def resolveRefs(self):
        for ele in self._namedElements.itervalues():
            if ele.type and ele.type in self._namedTypes:
                t = self._namedTypes[ele.type]
                ele.attributes = t.attributes
                ele.children = t.children
        for ele in self._namedElements.itervalues():
            newChildren = []
            for child in ele.children:
                if child.ref:
                    newChildren.append(self._namedElements[child.ref])
                elif child.type and child.type in self._namedTypes:
                    t = self._namedTypes[child.type]
                    child.attributes = t.attributes
                    child.children = t.children
                    newChildren.append(child)
            ele.children = newChildren
