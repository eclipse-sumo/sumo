# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011-06-23
@version $Id$

Python interface to SUMO especially for parsing output files.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
import sys
import re
import xml.dom
from xml.dom import pulldom
from collections import namedtuple, defaultdict
from keyword import iskeyword
from functools import reduce
from xml.sax import make_parser
from xml.sax.handler import ContentHandler
from . import dump, inductionloop, convert


def compound_object(element_name, attrnames):
    """return a class which delegates attribute acces to a namedtuple instance and
       bracket access to an internal dict. Missing attributes are deletegated to
       the child dict for convenience
       @note: Care must be taken when child nodes and attributes have the same names"""
    nt = namedtuple(element_name, attrnames)
    class CompoundObject():
        _fields = attrnames
        def __init__(self, values, child_dict):
            self.nt_instance = nt(*values)
            self.child_dict = child_dict
        def __coerce__(self, other):
            return None
        def __cmp__(self, other):
            if (self.nt_instance == other.nt_instance and
                    self.child_dict == other.child_dict):
                return 0
            elif (self.nt_instance < other.nt_instance or
                    (self.nt_instance == other.nt_instance and
                        self.child_dict < other.child_dict)):
                return -1
            else:
                return 1
        def __getattr__(self, name):
            try:
                return getattr(self.nt_instance, name)
            except AttributeError:
                return self.child_dict.get(name, None)
        def __getitem__(self, name):
            return self.child_dict[name]
        def __str__(self):
            return "<%s,child_dict=%s>" % (self.nt_instance, dict(self.child_dict))
        def __repr__(self):
            return str(self)

    return CompoundObject


def parse(xmlfile, element_names, element_attrs={}, attr_conversions={}):
    """
    Parses the given element_names from xmlfile and yield compound objects for
    their xml subtrees (no extra objects are returned if element_names appear in
    the subtree) The compound objects provide all element attributes of
    the root of the subtree as attributes unless attr_names are supplied. In this
    case attr_names maps element names to a list of attributes which are
    supplied. If attr_conversions is not empty it must map attribute names to
    callables which will be called upon the attribute value before storing under
    the attribute name. 
    The compound objects gives dictionary style access to list of compound
    objects o for any children with the given element name 
    o['child_element_name'] = [osub0, osub1, ...]
    As a shorthand, attribute style access to the list of child elements is
    provided unless an attribute with the same name as the child elements
    exists (i.e. o.child_element_name = [osub0, osub1, ...])
    @Note: All elements with the same name must have the same type regardless of
    the subtree in which they occur
    @Note: Attribute names may be modified to avoid name clashes
    with python keywords.
    @Example: parse('plain.edg.xml', ['edge'])
    """
    elementTypes = {}
    xml_doc = pulldom.parse(xmlfile)
    for event, parsenode in xml_doc:
        if event == pulldom.START_ELEMENT and parsenode.localName in element_names:
            xml_doc.expandNode(parsenode)
            yield _get_compound_object(parsenode, elementTypes,
                    parsenode.localName, element_attrs, attr_conversions)


_NO_CHILDREN = defaultdict(lambda:[])
_IDENTITY = lambda x:x
def _get_compound_object(node, elementTypes, element_name, element_attrs, attr_conversions):
    if not element_name in elementTypes:
        # initialized the compound_object type from the first encountered # element
        attrnames = element_attrs.get(element_name, 
                [node.attributes.item(i).localName for i in range(node.attributes.length)])
        if len(attrnames) != len(set(attrnames)):
            raise Exception("non-unique attributes %s for element '%s'" % (attrnames, element_name))
        attrnames = [_prefix_keyword(a) for a in attrnames]
        elementTypes[element_name] = compound_object(element_name, attrnames)
    # prepare children
    child_dict = _NO_CHILDREN # conserve space by reusing singleton
    child_elements = [c for c in node.childNodes if c.nodeType == xml.dom.Node.ELEMENT_NODE]
    if child_elements:
        child_dict = defaultdict(lambda:[])
        for c in child_elements:
            child_dict[c.localName].append(_get_compound_object(
                c, elementTypes, c.localName, element_attrs, attr_conversions))
    attrnames = elementTypes[element_name]._fields
    return elementTypes[element_name](
            [(attr_conversions.get(a, _IDENTITY)(node.getAttribute(a)) if node.hasAttribute(a) else None) for a in attrnames],
            child_dict)


def _prefix_keyword(name, warn=False):
    result = name
    # create a legal identifier (xml allows '-', ':' and '.' ...)
    result = ''.join([c for c in name if c.isalnum() or c=='_'])
    if result != name:
        if result == '':
            result == 'attr_'
        if warn:
            print("Warning: Renaming attribute '%s' to '%s' because it contains illegal characters" % (name, result), file=sys.stderr)
    if iskeyword(name):
        result = 'attr_' + name
        if warn:
            print("Warning: Renaming attribute '%s' to '%s' because it conflicts with a python keyword" % (name, result), file=sys.stderr)
    return result


def sum(elements, attrname):
    # for the given elements (as returned by method parse) compute the sum for attrname 
    # attrname must be the name of a numerical attribute
    return reduce(lambda x,y: x+y, [float(getattr(e, attrname)) for e in elements])


def average(elements, attrname):
    # for the given elements (as returned by method parse) compute the average for attrname 
    # attrname must be the name of a numerical attribute
    if elements:
        return sum(elements, attrname) / len(elements)
    else:
        raise Exception("average of 0 elements is not defined")


def parse_fast(xmlfile, element_name, attrnames, warn=False):
    """
    Parses the given attrnames from all elements with element_name
    @Note: The element must be on its own line and the attributes must appear in
    the given order.
    @Example: parse_fast('plain.edg.xml', 'edge', ['id', 'speed'])
    """
    pattern = '.*'.join(['<%s' % element_name] +
        ['%s="([^"]*)"' % attr for attr in attrnames])
    attrnames = [_prefix_keyword(a, warn) for a in attrnames]
    Record = namedtuple(element_name, attrnames)
    reprog = re.compile(pattern)
    for line in open(xmlfile):
        m = reprog.search(line)
        if m:
            yield Record(*m.groups())


class AbstractHandler__byID(ContentHandler):
    def __init__(self, element_name, idAttr, attributes):
        self._element_name = element_name
        self._attributes = attributes
        self._idAttr = idAttr
        self._values = {}
        
    def startElement(self, name, attrs):
        if name!=self._element_name:
            return
        cid = float(attrs[self._idAttr])
        self._values[cid] = {}
        if self._attributes:
            for a in self._attributes:
                self._values[cid][a] = float(attrs[a])
        else:
            for a in attrs.keys():
                if a!=self._idAttr:
                    self._values[cid][a] = float(attrs[a])

class AbstractHandler__asList(ContentHandler):
    def __init__(self, element_name, attributes):
        self._element_name = element_name
        self._attributes = attributes
        self._values = []
        
    def startElement(self, name, attrs):
        if name!=self._element_name:
            return
        tmp = {}
        if self._attributes:
            for a in self._attributes:
                try: tmp[a] = float(attrs[a])
                except: tmp[a] = attrs[a]
        else:
            for a in attrs.keys():
                try: tmp[a] = float(attrs[a])
                except: tmp[a] = attrs[a]
        self._values.append(tmp)            
            

def parse_sax(xmlfile, handler):
    myparser = make_parser()
    myparser.setContentHandler(handler)
    myparser.parse(xmlfile)


def parse_sax__byID(xmlfile, element_name, idAttr, attrnames):
    h = AbstractHandler__byID(element_name, idAttr, attrnames)
    parse_sax(xmlfile, h)
    return h._values
    
def parse_sax__asList(xmlfile, element_name, attrnames):
    h = AbstractHandler__asList(element_name, attrnames)
    parse_sax(xmlfile, h)
    return h._values
    
def toList(mapList, attr):
    ret = []
    for a in mapList:
        ret.append(a[attr])
    return ret

def prune(fv, minV, maxV):
    if minV!=None:
        for i,v in enumerate(fv):
            fv[i] = max(v, minV)
    if maxV!=None:
        for i,v in enumerate(fv):
            fv[i] = min(v, maxV)
    
