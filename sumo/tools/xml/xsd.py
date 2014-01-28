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

import os, pprint
from xml.dom import minidom

class XmlAttribute:
    def __init__(self, entity):
        if hasattr(entity, "getAttribute"):
            self.name = entity.getAttribute('name')
            self.use = entity.getAttribute('use')
            self.type = entity.getAttribute('type')
        else:
            self.name = entity
        self.resolved = False

    def __repr__(self):
        return self.name

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
#        pp = pprint.PrettyPrinter(indent=4)
#        pp.pprint(self._namedElements)
#        pp.pprint(self._namedTypes)

    def getElementStructure(self, entity, checkNestedType=False):
        eleObj = XmlElement(entity)
        if checkNestedType:
            nestedTypes = entity.getElementsByTagName('xsd:complexType')
            if nestedTypes:
                entity = nestedTypes[0] # skip xsd:complex-tag
            extension = entity.getElementsByTagName('xsd:extension')
            if extension:
                eleObj.type = extension[0].getAttribute('base')
                entity = extension[0]
        for aa in entity.childNodes:
            if aa.nodeName =='xsd:attribute':
                eleObj.attributes.append(XmlAttribute(aa))
            elif aa.nodeName =='xsd:sequence' or aa.nodeName =='xsd:choice':
                for aae in aa.getElementsByTagName('xsd:element'):
                    eleObj.children.append(XmlElement(aae))
        return eleObj

    def resolveRefs(self):
        for ele in self._namedElements.itervalues():
            if ele.type and ele.type in self._namedTypes and not ele.resolved:
                t = self._namedTypes[ele.type]
                ele.attributes += t.attributes
                ele.children += t.children
                ele.resolved = True
        for ele in self._namedElements.itervalues():
            newChildren = []
            for child in ele.children:
                if child.ref:
                    newChildren.append(self._namedElements[child.ref])
                else:
                    newChildren.append(self._namedElements[child.name])
            ele.children = newChildren
