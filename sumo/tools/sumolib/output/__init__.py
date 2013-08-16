# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011-06-23
@version $Id$

Python interface to SUMO especially for parsing output files.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sys
import dump, inductionloop
import re
import xml.dom
from xml.dom import pulldom
from collections import namedtuple, defaultdict
from keyword import iskeyword


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
    parses the given element_names from xmlfile and yield compound objects for
    their xml subtrees (no extra objects are returned if element_names appear in
    the subtree) The compound objects follow provide all element attributes of
    the first read element as attributes unless attr_names are supplied. In this
    case attr_names maps element names to a list of attributes which are
    supplied. If attr_conversions is not empty it must map attribute names to
    callables which will be called upon the attribute value before storing under
    the attribute name (attribute names may be modified to avoid name clashes
    with python keywords). 
    The compound objects gives dictionary style access to list of compound
    objects o for any children with the given element name 
    o['child_element_name'] = [osub0, osub1, ...]
    @Note: all elements with the same name must have the same type regardless of
    the subtree in which they occur
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
            print >>sys.stderr, "Warning: Renaming attribute '%s' to '%s' because it contains illegal characters" % (name, result)
    if iskeyword(name):
        result = 'attr_' + name
        if warn:
            print >>sys.stderr, "Warning: Renaming attribute '%s' to '%s' because it conflicts with a python keyword" % (name, result)
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
        raise "average of 0 elements is not defined"


def parse_fast(xmlfile, element_name, attrnames, warn=False):
    # parses the given attribute from all elements with element_name
    # note that the element must be on its own line and 
    # the attributes must appear in the given order
    # example: parse_fast('plain.edg.xml', 'edge', ['id', 'speed'])
    pattern = '.*'.join(['<%s' % element_name] +
        ['%s="([^"]*)"' % attr for attr in attrnames])
    attrnames = [_prefix_keyword(a, warn) for a in attrnames]
    Record = namedtuple(element_name, attrnames)
    reprog = re.compile(pattern)
    for line in open(xmlfile):
        m = reprog.search(line)
        if m:
            yield Record(*m.groups())

