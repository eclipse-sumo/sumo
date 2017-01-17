# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011-06-23
@version $Id$

Python interface to SUMO especially for parsing output files.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
from __future__ import absolute_import
import sys
import re
import xml.etree.cElementTree as ET
from collections import namedtuple, defaultdict
from keyword import iskeyword
from functools import reduce
from xml.sax import make_parser
from xml.sax.handler import ContentHandler
from . import dump, inductionloop, convert


def _prefix_keyword(name, warn=False):
    result = name
    # create a legal identifier (xml allows '-', ':' and '.' ...)
    result = ''.join([c for c in name if c.isalnum() or c == '_'])
    if result != name:
        if result == '':
            result == 'attr_'
        if warn:
            print("Warning: Renaming attribute '%s' to '%s' because it contains illegal characters" % (
                name, result), file=sys.stderr)
    if iskeyword(name):
        result = 'attr_' + name
        if warn:
            print("Warning: Renaming attribute '%s' to '%s' because it conflicts with a python keyword" % (
                name, result), file=sys.stderr)
    return result


def compound_object(element_name, attrnames, warn=False):
    """return a class which delegates bracket access to an internal dict. 
       Missing attributes are delegated to the child dict for convenience.
       @note: Care must be taken when child nodes and attributes have the same names"""
    class CompoundObject():
        _original_fields = sorted(attrnames)
        _fields = [_prefix_keyword(a, warn) for a in _original_fields]

        def __init__(self, values, child_dict):
            for name, val in zip(self._fields, values):
                self.__dict__[name] = val
            self._child_dict = child_dict
            self.name = element_name

        def getAttributes(self):
            return [(k, getattr(self, k)) for k in self._fields]

        def hasAttribute(self, name):
            return name in self._fields

        def setAttribute(self, name, value):
            if name not in self._fields:
                self._original_fields.append(name)
                self._fields.append(_prefix_keyword(name, warn))
            self.__dict__[name] = value

        def hasChild(self, name):
            return name in self._child_dict

        def getChild(self, name):
            return self._child_dict[name]

        def __getattr__(self, name):
            if name[:2] != "__":
                return self._child_dict.get(name, None)
            raise AttributeError

        def __setattr__(self, name, value):
            if name != "_child_dict" and name in self._child_dict:
                self._child_dict[name] = value
            else:
                self.__dict__[name] = value

        def __delattr__(self, name):
            if name in self._child_dict:
                del self._child_dict[name]
            else:
                if name in self.__dict__:
                    del self.__dict__[name]
                self._original_fields.remove(name)
                self._fields.remove(_prefix_keyword(name, False))

        def __getitem__(self, name):
            return self._child_dict[name]

        def __str__(self):
            return "<%s,child_dict=%s>" % (self.getAttributes(), dict(self._child_dict))

        def toXML(self, initialIndent="", indent="    "):
            fields = ['%s="%s"' % (self._original_fields[i], getattr(self, k))
                      for i, k in enumerate(self._fields) if getattr(self, k) is not None]
            if not self._child_dict:
                return "%s<%s %s/>\n" % (initialIndent, element_name, " ".join(fields))
            else:
                s = "%s<%s %s>\n" % (
                    initialIndent, element_name, " ".join(fields))
                for l in self._child_dict.values():
                    for c in l:
                        s += c.toXML(initialIndent + indent)
                return s + "%s</%s>\n" % (initialIndent, element_name)

        def __repr__(self):
            return str(self)

    return CompoundObject


def parse(xmlfile, element_names, element_attrs={}, attr_conversions={},
          heterogeneous=False, warn=False):
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
    the subtree in which they occur (heterogeneous cases may be handled by
    setting heterogeneous=False (with reduced parsing speed)
    @Note: Attribute names may be modified to avoid name clashes
    with python keywords. (set warn=True to receive renaming warnings)
    @Note: The element_names may be either a single string or a list of strings.
    @Example: parse('plain.edg.xml', ['edge'])
    """
    if isinstance(element_names, str):
        element_names = [element_names]
    elementTypes = {}
    for event, parsenode in ET.iterparse(xmlfile):
        if parsenode.tag in element_names:
            yield _get_compound_object(parsenode, elementTypes,
                                       parsenode.tag, element_attrs,
                                       attr_conversions, heterogeneous, warn)
            parsenode.clear()


_NO_CHILDREN = defaultdict(lambda: [])
_IDENTITY = lambda x: x


def _get_compound_object(node, elementTypes, element_name, element_attrs, attr_conversions, heterogeneous, warn):
    if not element_name in elementTypes or heterogeneous:
        # initialized the compound_object type from the first encountered #
        # element
        attrnames = element_attrs.get(element_name, node.keys())
        if len(attrnames) != len(set(attrnames)):
            raise Exception(
                "non-unique attributes %s for element '%s'" % (attrnames, element_name))
        elementTypes[element_name] = compound_object(
            element_name, attrnames, warn)
    # prepare children
    child_dict = _NO_CHILDREN  # conserve space by reusing singleton
    if len(node) > 0:
        child_dict = defaultdict(lambda: [])
        for c in node:
            child_dict[c.tag].append(_get_compound_object(
                c, elementTypes, c.tag, element_attrs, attr_conversions,
                heterogeneous, warn))
    attrnames = elementTypes[element_name]._original_fields
    return elementTypes[element_name](
        [attr_conversions.get(a, _IDENTITY)(node.get(a)) for a in attrnames],
        child_dict)


def sum(elements, attrname):
    # for the given elements (as returned by method parse) compute the sum for attrname
    # attrname must be the name of a numerical attribute
    return reduce(lambda x, y: x + y, [float(getattr(e, attrname)) for e in elements])


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
        if name != self._element_name:
            return
        cid = float(attrs[self._idAttr])
        self._values[cid] = {}
        if self._attributes:
            for a in self._attributes:
                self._values[cid][a] = float(attrs[a])
        else:
            for a in attrs.keys():
                if a != self._idAttr:
                    self._values[cid][a] = float(attrs[a])


class AbstractHandler__asList(ContentHandler):

    def __init__(self, element_name, attributes):
        self._element_name = element_name
        self._attributes = attributes
        self._values = []

    def startElement(self, name, attrs):
        if name != self._element_name:
            return
        tmp = {}
        if self._attributes:
            for a in self._attributes:
                try:
                    tmp[a] = float(attrs[a])
                except:
                    tmp[a] = attrs[a]
        else:
            for a in attrs.keys():
                try:
                    tmp[a] = float(attrs[a])
                except:
                    tmp[a] = attrs[a]
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
    if minV != None:
        for i, v in enumerate(fv):
            fv[i] = max(v, minV)
    if maxV != None:
        for i, v in enumerate(fv):
            fv[i] = min(v, maxV)
