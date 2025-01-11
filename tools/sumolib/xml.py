# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    xml.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2011-06-23

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import re
import gzip
import io
import datetime
import fileinput
try:
    import xml.etree.cElementTree as ET
except ImportError as e:
    print("recovering from ImportError '%s'" % e)
    import xml.etree.ElementTree as ET
from collections import namedtuple, OrderedDict
from keyword import iskeyword
from functools import reduce
import xml.sax.saxutils

from . import version, miscutils

DEFAULT_ATTR_CONVERSIONS = {
    # shape-like
    'shape': lambda coords: map(lambda xy: map(float, xy.split(',')), coords.split()),
    # float
    'speed': float,
    'length': float,
    'width': float,
    'angle': float,
    'endOffset': float,
    'radius': float,
    'contPos': float,
    'visibility': float,
    'startPos': float,
    'endPos': float,
    'position': float,
    'x': float,
    'y': float,
    'lon': float,
    'lat': float,
    'freq': float,
    # int
    'priority': int,
    'numLanes': int,
    'index': int,
    'linkIndex': int,
    'linkIndex2': int,
    'fromLane': int,
    'toLane': int,
}


def supports_comments():
    return sys.version_info[0] >= 3 and sys.version_info[1] >= 8


def _prefix_keyword(name, warn=False):
    result = name
    # create a legal identifier (xml allows '-', ':' and '.' ...)
    result = ''.join([c for c in name if c.isalnum() or c == '_'])
    if result != name:
        if result == '':
            result = 'attr_'
        if warn:
            print("Warning: Renaming attribute '%s' to '%s' because it contains illegal characters" % (
                name, result), file=sys.stderr)
    if name == "name":
        result = 'attr_name'
        if warn:
            print("Warning: Renaming attribute '%s' to '%s' because it conflicts with a reserved field" % (
                name, result), file=sys.stderr)

    if iskeyword(name):
        result = 'attr_' + name
        if warn:
            print("Warning: Renaming attribute '%s' to '%s' because it conflicts with a python keyword" % (
                name, result), file=sys.stderr)
    return result


def compound_object(element_name, attrnames, warn=False, sort=True):
    """return a class which delegates bracket access to an internal dict.
       Missing attributes are delegated to the child dict for convenience.
       @note: Care must be taken when child nodes and attributes have the same names"""
    class CompoundObject():
        _original_fields = sorted(attrnames) if sort else tuple(attrnames)
        _fields = [_prefix_keyword(a, warn) for a in _original_fields]

        def __init__(self, values, child_dict=None, text=None, child_list=None):
            for name, val in zip(self._fields, values):
                self.__dict__[name] = val
            self._child_dict = child_dict if child_dict else {}
            self.name = element_name
            self._text = text
            self._child_list = child_list if child_list else []

        def getAttributes(self):
            return [(k, getattr(self, k)) for k in self._fields]

        def hasAttribute(self, name):
            return name in self._fields

        def getAttribute(self, name):
            if self.hasAttribute(name):
                return self.__dict__[name]
            raise AttributeError

        def getAttributeSecure(self, name, default=None):
            if self.hasAttribute(name):
                return self.__dict__[name]
            return default

        def setAttribute(self, name, value):
            if name not in self._original_fields:
                if isinstance(self._original_fields, tuple):
                    tempList = list(self._original_fields)
                    tempList.append(name)
                    self._original_fields = tuple(tempList)
                else:
                    self._original_fields.append(name)
                self._fields.append(_prefix_keyword(name, warn))
            self.__dict__[_prefix_keyword(name, warn)] = value

        def hasChild(self, name):
            return name in self._child_dict

        def getChild(self, name):
            return self._child_dict[name]

        def addChild(self, name, attrs=None, sortAttrs=True):
            if attrs is None:
                attrs = {}
            clazz = compound_object(name, attrs.keys(), sort=sortAttrs)
            child = clazz([attrs.get(a) for a in (sorted(attrs.keys()) if sortAttrs else attrs.keys())])
            self._child_dict.setdefault(name, []).append(child)
            self._child_list.append(child)
            return child

        def removeChild(self, child):
            self._child_dict[child.name].remove(child)
            self._child_list.remove(child)

        def setChildList(self, childs):
            for c in self._child_list:
                self._child_dict[c.name].remove(c)
            for c in childs:
                self._child_dict.setdefault(c.name, []).append(c)
            self._child_list = childs

        def getChildList(self, withComments=False):
            if withComments:
                return self._child_list
            else:
                return [c for c in self._child_list if not c.isComment()]

        def getText(self):
            return self._text

        def setText(self, text):
            self._text = text

        def isComment(self):
            return "function Comment" in str(self.name)

        def getComments(self):
            if not supports_comments:
                sys.stderr.write("Comment parsing is only supported with version 3.8 or higher by sumolib.xml\n")
            for name, children in self._child_dict.items():
                if "function Comment" in str(name):
                    return [c.getText() for c in children]
            return []

        def __getattr__(self, name):
            if name[:2] != "__":
                return self._child_dict.get(name, None)
            raise AttributeError

        def __setattr__(self, name, value):
            if name != "_child_dict" and name in self._child_dict:
                # this could be optimized by using the child_list only if there are different children
                for c in self._child_dict[name]:
                    self._child_list.remove(c)
                self._child_dict[name] = value
                for c in value:
                    self._child_list.append(c)
            else:
                self.__dict__[name] = value

        def __delattr__(self, name):
            if name in self._child_dict:
                for c in self._child_dict[name]:
                    self._child_list.remove(c)
                del self._child_dict[name]
            else:
                if name in self.__dict__:
                    del self.__dict__[name]
                self._original_fields.remove(name)
                self._fields.remove(_prefix_keyword(name, False))

        def __getitem__(self, name):
            return self._child_dict[name]

        def __str__(self):
            nodeText = '' if self._text is None else ",text=%s" % self._text
            return "<%s,child_dict=%s%s>" % (self.getAttributes(), dict(self._child_dict), nodeText)

        def toXML(self, initialIndent="", indent="    ", withComments=False):
            fields = ['%s="%s"' % (self._original_fields[i], getattr(self, k))
                      for i, k in enumerate(self._fields) if getattr(self, k) is not None and
                      # see #3454
                      '{' not in self._original_fields[i]]
            if self.isComment():
                if withComments:
                    return initialIndent + "<!-- %s -->\n" % self._text
                else:
                    return ""
            if not self._child_dict and self._text is None:
                return initialIndent + "<%s %s/>\n" % (self.name, " ".join(fields))
            else:
                s = initialIndent + "<%s %s>\n" % (self.name, " ".join(fields))
                for c in self._child_list:
                    s += c.toXML(initialIndent + indent, withComments=withComments)
                if self._text is not None and self._text.strip():
                    s += self._text.strip(" ")
                return s + "%s</%s>\n" % (initialIndent, self.name)

        def __repr__(self):
            return str(self)

        def __lt__(self, other):
            return str(self) < str(other)

    return CompoundObject


def parselines(xmlline, element_name, element_attrs=None, attr_conversions=None,
               heterogeneous=True, warn=False, addRoot="dummy"):
    tagStart1 = "<%s>" % element_name
    tagStart2 = "<%s " % element_name
    if tagStart1 in xmlline or tagStart2 in xmlline:
        if addRoot is not None:
            xmlline = "<%s>\n%s</%s>\n" % (addRoot, xmlline, addRoot)
        xmlfile = io.StringIO(xmlline)
        for x in parse(xmlfile, element_name, element_attrs, attr_conversions,
                       heterogeneous, warn):
            yield x


def _handle_namespace(tag, ignoreXmlns):
    if ignoreXmlns and "}" in tag:
        # see https://bugs.python.org/issue18304
        return tag.split("}")[1]
    return tag


def _check_file_like(xmlfile):
    if not hasattr(xmlfile, "read"):
        return miscutils.openz(xmlfile), True
    return xmlfile, False


def parse(xmlfile, element_names=None, element_attrs=None, attr_conversions=None,
          heterogeneous=True, warn=False, ignoreXmlns=False):
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
    setting heterogeneous=True (with reduced parsing speed)
    @Note: Attribute names may be modified to avoid name clashes
    with python keywords. (set warn=True to receive renaming warnings)
    @Note: The element_names may be either a single string or a list of strings.
    @Example: parse('plain.edg.xml', ['edge'])
    """
    if isinstance(element_names, str):
        element_names = [element_names]
    if element_attrs is None:
        element_attrs = {}
    if attr_conversions is None:
        attr_conversions = {}
    element_types = {}
    kwargs = {'parser': ET.XMLParser(target=ET.TreeBuilder(insert_comments=True))} if supports_comments() else {}
    xmlfile, close_source = _check_file_like(xmlfile)
    try:
        for _, parsenode in ET.iterparse(xmlfile, **kwargs):
            tag = _handle_namespace(parsenode.tag, ignoreXmlns)
            if element_names is None or tag in element_names:
                yield _get_compound_object(parsenode, element_types,
                                           tag, element_attrs,
                                           attr_conversions, heterogeneous, warn,
                                           ignoreXmlns)
                parsenode.clear()
    finally:
        if close_source:
            xmlfile.close()


def _IDENTITY(x):
    return x


def _get_compound_object(node, element_types, element_name, element_attrs, attr_conversions,
                         heterogeneous, warn, ignoreXmlns):
    if element_name not in element_types or heterogeneous:
        # initialized the compound_object type from the first encountered #
        # element
        attrnames = element_attrs.get(element_name, node.keys())
        if len(attrnames) != len(set(attrnames)):
            raise Exception(
                "non-unique attributes %s for element '%s'" % (attrnames, element_name))
        element_types[element_name] = compound_object(
            element_name, attrnames, warn)
    # prepare children
    child_dict = {}
    child_list = []
    if len(node) > 0:
        for c in node:
            tag = _handle_namespace(c.tag, ignoreXmlns)
            child = _get_compound_object(c, element_types, tag, element_attrs, attr_conversions,
                                         heterogeneous, warn, ignoreXmlns)
            child_dict.setdefault(tag, []).append(child)
            child_list.append(child)
    attrnames = element_types[element_name]._original_fields
    return element_types[element_name](
        [attr_conversions.get(a, _IDENTITY)(node.get(a)) for a in attrnames],
        child_dict, node.text, child_list)


def create_document(root_element_name, attrs=None, schema=None):
    if attrs is None:
        attrs = {}
    if schema is None:
        attrs["xmlns:xsi"] = "http://www.w3.org/2001/XMLSchema-instance"
        attrs["xsi:noNamespaceSchemaLocation"] = "http://sumo.dlr.de/xsd/" + root_element_name + "_file.xsd"
    clazz = compound_object(root_element_name, sorted(attrs.keys()))
    return clazz([attrs.get(a) for a in sorted(attrs.keys())], OrderedDict())


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


def _createRecordAndPattern(element_name, attrnames, warn, optional, extra=None):
    if isinstance(attrnames, str):
        attrnames = [attrnames]
    prefixedAttrnames = [_prefix_keyword(a, warn) for a in attrnames]
    if optional:
        pattern = ''.join(['<%s' % element_name] +
                          ['(\\s+%s="(?P<%s>[^"]*?)")?' % a for a in zip(attrnames, prefixedAttrnames)])
    else:
        pattern = '.*'.join(['<%s' % element_name] +
                            ['\\s+%s="([^"]*)"' % attr for attr in attrnames])
    if extra is not None:
        prefixedAttrnames += [_prefix_keyword(a, warn) for a in extra]
    Record = namedtuple(_prefix_keyword(element_name, warn), prefixedAttrnames)
    reprog = re.compile(pattern)
    return Record, reprog


def _open(xmlfile, encoding="utf8"):
    if isinstance(xmlfile, str):
        if xmlfile.endswith(".gz"):
            if encoding is None:
                return gzip.open(xmlfile, "r")
            return gzip.open(xmlfile, "rt")
        if encoding is not None:
            return io.open(xmlfile, encoding=encoding)
    return xmlfile


def _comment_filter(stream):
    """
    Filters given stream for comments. Is used by parse_fast and parse_fast_nested
    """
    in_comment = False
    for line in stream:
        if "<!--" in line or in_comment:
            if "-->" in line:
                yield re.sub(".*-->" if in_comment else "<!--.*-->", "", line)
                in_comment = False
            elif not in_comment:
                yield re.sub("<!--.*", "", line)
                in_comment = True
        else:
            yield line


def parse_fast(xmlfile, element_name, attrnames, warn=False, optional=False, encoding="utf8"):
    """
    Parses the given attrnames from all elements with element_name
    @Note: The element must be on its own line and the attributes must appear in
    the given order. If you set "optional=True", missing attributes will be set to None.
    Make sure that you list all (potential) attributes (even the ones you are not interested in)
    in this case. You can only leave out attributes at the end.
    @Example: parse_fast('plain.edg.xml', 'edge', ['id', 'speed'])
    """
    Record, reprog = _createRecordAndPattern(element_name, attrnames, warn, optional)
    xmlfile, close_source = _check_file_like(xmlfile)
    try:
        for line in _comment_filter(xmlfile):
            m = reprog.search(line)
            if m:
                if optional:
                    yield Record(**m.groupdict())
                else:
                    yield Record(*m.groups())
    finally:
        if close_source:
            xmlfile.close()


def parse_fast_nested(xmlfile, element_name, attrnames, element_name2, attrnames2,
                      warn=False, optional=False, encoding="utf8"):
    """
    Parses the given attrnames from all elements with element_name
    And attrnames2 from element_name2 where element_name2 is a child element of element_name
    If you set "optional=True", missing attributes will be set to None.
    Make sure that you list all (potential) attributes (even the ones you are not interested in)
    in this case. You can only leave out attributes at the end.
    @Note: The element must be on its own line and the attributes must appear in
    the given order.
    @Example: parse_fast_nested('fcd.xml', 'timestep', ['time'], 'vehicle', ['id', 'speed', 'lane']):
    """
    Record, reprog = _createRecordAndPattern(element_name, attrnames, warn, optional)
    Record2, reprog2 = _createRecordAndPattern(element_name2, attrnames2, warn, optional)
    record = None
    xmlfile, close_source = _check_file_like(xmlfile)
    try:
        for line in _comment_filter(xmlfile):
            m2 = reprog2.search(line)
            if record and m2:
                if optional:
                    yield record, Record2(**m2.groupdict())
                else:
                    yield record, Record2(*m2.groups())
            else:
                m = reprog.search(line)
                if m:
                    if optional:
                        record = Record(**m.groupdict())
                    else:
                        record = Record(*m.groups())
                elif element_name in line:
                    record = None
    finally:
        if close_source:
            xmlfile.close()


def parse_fast_structured(xmlfile, element_name, attrnames, nested,
                          warn=False, optional=False, encoding="utf8"):
    """
    Parses the given attrnames from all elements with element_name and nested elements of level 1.
    Unlike parse_fast_nested this function can handle multiple different child elements and
    returns objects where the child elements can be accessed by name (e.g. timestep.vehicle[0])
    as with the parse method. The returned object is not modifiable though.
    If you set "optional=True", missing attributes will be set to None.
    Make sure that you list all (potential) attributes (even the ones you are not interested in)
    in this case. You can only leave out attributes at the end.
    @Note: Every element must be on its own line and the attributes must appear in the given order.
    @Example: parse_fast_structured('fcd.xml', 'timestep', ['time'],
                                    {'vehicle': ['id', 'speed', 'lane'], 'person': ['id', 'speed', 'edge']}):
    """
    Record, reprog = _createRecordAndPattern(element_name, attrnames, warn, optional, nested.keys())
    re2 = [(elem,) + _createRecordAndPattern(elem, attr, warn, optional) for elem, attr in nested.items()]
    finalizer = "</%s>" % element_name
    record = None
    xmlfile, close_source = _check_file_like(xmlfile)
    try:
        for line in _comment_filter(xmlfile):
            if record:
                for name2, Record2, reprog2 in re2:
                    m2 = reprog2.search(line)
                    if m2:
                        if optional:
                            inner = Record2(**m2.groupdict())
                        else:
                            inner = Record2(*m2.groups())
                        getattr(record, name2).append(inner)
                        break
                else:
                    if finalizer in line:
                        yield record
                        record = None
            else:
                m = reprog.search(line)
                if m:
                    if optional:
                        args = dict(m.groupdict())
                        for name, _, __ in re2:
                            args[name] = []
                        record = Record(**args)
                    else:
                        args = list(m.groups())
                        for _ in range(len(re2)):
                            args.append([])
                        record = Record(*args)
    finally:
        if close_source:
            xmlfile.close()


def buildHeader(script=None, root=None, schemaPath=None, rootAttrs="", options=None, includeXMLDeclaration=False):
    """
    Builds an XML header with schema information and a comment on how the file has been generated
    (script name, arguments and datetime).
    If script name is not given, it is determined from the command line call.
    If root is not given, no root element is printed (and thus no schema).
    If schemaPath is not given, it is derived from the root element.
    If rootAttrs is given as a string, it can be used to add further attributes to the root element.
    If rootAttrs is set to None, the schema related attributes are not printed.
    """
    if script is None or script == "$Id$":
        script = os.path.basename(sys.argv[0])
    if options is None:
        optionString = u"  options: %s\n" % (' '.join(sys.argv[1:]).replace('--', '<doubleminus>'))
    else:
        optionString = options.config_as_string
    if includeXMLDeclaration:
        header = u'<?xml version="1.0" encoding="UTF-8"?>\n\n'
    else:
        header = u''
    header += u'<!-- generated on %s by %s %s\n%s-->\n\n' % (datetime.datetime.now(), script,
                                                             version.gitDescribe(), optionString)
    if root is not None:
        if rootAttrs is None:
            header += u'<%s>\n' % root
        else:
            if schemaPath is None:
                schemaPath = root + "_file.xsd"
            header += (u'<%s%s xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ' +
                       u'xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/%s">\n') % (root, rootAttrs, schemaPath)
    return header


def writeHeader(outf, script=None, root=None, schemaPath=None, rootAttrs="", options=None, includeXMLDeclaration=True):
    """
    Writes an XML header with schema information and a comment on how the file has been generated
    (script name, arguments and datetime). Please use this as first call whenever you open a
    SUMO related XML file for writing from your script.
    If script name is not given, it is determined from the command line call.
    If root is not given, no root element is printed (and thus no schema).
    If schemaPath is not given, it is derived from the root element.
    If rootAttrs is given as a string, it can be used to add further attributes to the root element.
    If rootAttrs is set to None, the schema related attributes are not printed.
    """
    outf.write(buildHeader(script, root, schemaPath, rootAttrs, options, includeXMLDeclaration))


def insertOptionsHeader(filename, options):
    """
    Inserts a comment header with the options used to call the script into an existing file.
    """
    header = buildHeader(options=options)
    fileToPatch = fileinput.FileInput(filename, inplace=True)
    for lineNbr, line in enumerate(fileToPatch):
        if lineNbr == 2:
            print(header, end='')
        print(line, end='')
    fileToPatch.close()


def quoteattr(val, ensureUnicode=False):
    # saxutils sometimes uses single quotes around the attribute
    # we can prevent this by adding an artificial single quote to the value and removing it again
    if ensureUnicode and type(val) is bytes:
        val = val.decode("utf-8")
    return '"' + xml.sax.saxutils.quoteattr("'" + val)[2:]
