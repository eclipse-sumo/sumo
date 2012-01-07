# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011-06-23
@version $Id$

Python interface to SUMO especially for parsing output files.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2011-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import dump, inductionloop

from xml.dom import pulldom
from collections import namedtuple
from keyword import iskeyword


def parse(xmlfile, element_name):
    # parses the attributes of all nodes with element_name and returns a list of namedtuples
    # @note the first node in xmlfile will determine the set of attributes 
    # @note attribute names which are also python keywords will be prefixed with 'attr_' 
    elementType = [] # mutable, will be [namedtuple]
    xml_doc = pulldom.parse(xmlfile)
    return [get_attrs(parsenode, elementType, element_name) for event, parsenode in xml_doc 
            if event == pulldom.START_ELEMENT and parsenode.localName == element_name]

def get_attrs(node, elementType, element_name):
    if not elementType:
        # why NamedNodeMap does not support iteration is a mystery
        attrnames = [node.attributes.item(i).localName for i in range(node.attributes.length)]
        attrnames = [prefix_keyword(a) for a in attrnames]
        elementType.append(namedtuple(element_name, attrnames))
    return elementType[0](*[node.getAttribute(a) for a in elementType[0]._fields])


def prefix_keyword(name):
    return 'attr_' + name if iskeyword(name) else name


def sum(elements, attrname):
    # for the given elements (as returned by method parse) compute the sum for attrname 
    # attrname must be the name of a numerical attribute
    return reduce(lambda x,y: x+y, [float(e.__getattribute__(attrname)) for e in elements])


def average(elements, attrname):
    # for the given elements (as returned by method parse) compute the average for attrname 
    # attrname must be the name of a numerical attribute
    if elements:
        return sum(elements, attrname) / len(elements)
    else:
        raise "average of 0 elements is not defined"

