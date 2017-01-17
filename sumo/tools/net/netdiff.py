#!/usr/bin/env python
"""
@file    netdiff.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011-10-04
@version $Id$

Reads two networks (source, dest) and tries to produce the minimal plain-xml input
which can be loaded with netconvert alongside source to create dest

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import codecs
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO
from xml.dom import pulldom
from xml.dom import Node
from optparse import OptionParser
from subprocess import call
from collections import namedtuple, defaultdict

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib
from OrderedMultiSet import OrderedMultiSet

INDENT = 4

# file types to compare
TYPE_NODES = '.nod.xml'
TYPE_EDGES = '.edg.xml'
TYPE_CONNECTIONS = '.con.xml'
TYPE_TLLOGICS = '.tll.xml'
PLAIN_TYPES = [
    TYPE_NODES,
    TYPE_EDGES,
    TYPE_CONNECTIONS,
    TYPE_TLLOGICS
]

# traffic lights have some peculiarities
# CAVEAT1 - ids are not unique (only in combination with programID)
# CAVEAT2 - the order of their children (phases) is important.
#     this makes partial diffs unfeasible. The easiest solution is to forgo diffs and always export the whole new traffic light
# CAVEAT3 - deletes need not be written because they are also signaled by a changed node type
#     (and they complicate the handling of deleted tl-connections)
# CAVEAT4 - deleted connections must be written with their tlID and tlIndex, otherwise
#     parsing in netconvert becomes tedious
# CAVEAT5 - phases must maintain their order
# CAVEAT6 - identical phases may occur multiple times, thus OrderedMultiSet

TAG_TLL = 'tlLogic'
TAG_CONNECTION = 'connection'
TAG_CROSSING = 'crossing'
TAG_ROUNDABOUT = 'roundabout'

# see CAVEAT1
IDATTRS = defaultdict(lambda: ('id',))
IDATTRS[TAG_TLL] = ('id', 'programID')
IDATTRS[TAG_CONNECTION] = ('from', 'to', 'fromLane', 'toLane')
IDATTRS[TAG_CROSSING] = ('node', 'edges')
IDATTRS[TAG_ROUNDABOUT] = ('edges',)
IDATTRS['interval'] = ('begin', 'end')

DELETE_ELEMENT = 'delete'  # the xml element for signifying deletes

# provide an order for the attribute names
ATTRIBUTE_NAMES = {
    #'.nod.xml' : ()
    #'.edg.xml' : ()
    #'.con.xml' : ()
}

# default values for the given attribute (needed when attributes appear in
# source but do not appear in dest)
DEFAULT_VALUES = defaultdict(lambda: "")
DEFAULT_VALUES['width'] = "-1"
DEFAULT_VALUES['offset'] = "0"
DEFAULT_VALUES['spreadType'] = "right"
DEFAULT_VALUES['customShape'] = "false"
DEFAULT_VALUES['keepClear'] = "true"
DEFAULT_VALUES['contPos'] = "-1"
DEFAULT_VALUES['visibility'] = "-1"
RESET = 0


# stores attributes for later comparison
class AttributeStore:

    def __init__(self, type, copy_tags, level=1):
        # xml type being parsed
        self.type = type
        # tag names to copy even if unchanged
        self.copy_tags = copy_tags
        # indent level
        self.level = level
        # dict of names-tuples
        self.attrnames = {}
        # sets of (tag, id) preserve order to avoid dangling references during
        # loading
        self.ids_deleted = OrderedMultiSet()
        self.ids_created = OrderedMultiSet()
        self.ids_copied = OrderedMultiSet()
        # dict from (tag, id) to (names, values, children)
        self.id_attrs = {}
        # dict from tag to (names, values)-sets, need to preserve order
        # (CAVEAT5)
        self.idless_deleted = defaultdict(OrderedMultiSet)
        self.idless_created = defaultdict(OrderedMultiSet)
        self.idless_copied = defaultdict(OrderedMultiSet)

    # getAttribute returns "" if not present
    def getValue(self, node, name):
        if node.hasAttribute(name):
            return node.getAttribute(name)
        else:
            return None

    def getNames(self, xmlnode):
        idattrs = IDATTRS[xmlnode.localName]
        a = xmlnode.attributes
        all = [a.item(i).localName for i in range(a.length)]
        instance = tuple([n for n in all if n not in idattrs])
        if not instance in self.attrnames:
            self.attrnames[instance] = instance
        # only store a single instance of this tuple to conserve memory
        return self.attrnames[instance]

    def getAttrs(self, xmlnode):
        names = self.getNames(xmlnode)
        values = tuple([self.getValue(xmlnode, a) for a in names])
        children = None
        if any([c.nodeType == Node.ELEMENT_NODE for c in xmlnode.childNodes]):
            children = AttributeStore(
                self.type, self.copy_tags, self.level + 1)
        tag = xmlnode.localName
        id = tuple([xmlnode.getAttribute(a)
                    for a in IDATTRS[tag] if xmlnode.hasAttribute(a)])
        return tag, id, children, (names, values, children)

    def store(self, xmlnode):
        tag, id, children, attrs = self.getAttrs(xmlnode)
        tagid = (tag, id)
        if id != ():
            self.ids_deleted.add(tagid)
            self.ids_copied.add(tagid)
            self.id_attrs[tagid] = attrs
            if children:
                for child in xmlnode.childNodes:
                    if child.nodeType == Node.ELEMENT_NODE:
                        children.store(child)
        else:
            self.no_children_supported(children, tag)
            self.idless_deleted[tag].add(attrs)

    def compare(self, xmlnode):
        tag, id, children, attrs = self.getAttrs(xmlnode)
        tagid = (tag, id)
        if id != ():
            if tagid in self.ids_deleted:
                self.ids_deleted.remove(tagid)
                self.id_attrs[tagid] = self.compareAttrs(
                    self.id_attrs[tagid], attrs, tag)
            else:
                self.ids_created.add(tagid)
                self.id_attrs[tagid] = attrs

            children = self.id_attrs[tagid][2]
            if children:
                for child in xmlnode.childNodes:
                    if child.nodeType == Node.ELEMENT_NODE:
                        children.compare(child)
                if tag == TAG_TLL or tag in self.copy_tags:  # see CAVEAT2
                    child_strings = StringIO()
                    children.writeDeleted(child_strings)
                    children.writeCreated(child_strings)
                    children.writeChanged(child_strings)

                    if len(child_strings.getvalue()) > 0 or tag in self.copy_tags:
                        # there are some changes. Go back and store everything
                        children = AttributeStore(
                            self.type, self.copy_tags, self.level + 1)
                        for child in xmlnode.childNodes:
                            if child.nodeType == Node.ELEMENT_NODE:
                                children.compare(child)
                        self.id_attrs[tagid] = self.id_attrs[
                            tagid][0:2] + (children,)

        else:
            self.no_children_supported(children, tag)
            if attrs in self.idless_deleted[tag]:
                self.idless_deleted[tag].remove(attrs)
                if tag in self.copy_tags:
                    self.idless_copied[tag].add(attrs)
            else:
                self.idless_created[tag].add(attrs)

    def no_children_supported(self, children, tag):
        if children:
            print(
                "WARNING: Handling of children only supported for elements with id. Ignored for element '%s'" % tag)

    def compareAttrs(self, sourceAttrs, destAttrs, tag):
        snames, svalues, schildren = sourceAttrs
        dnames, dvalues, dchildren = destAttrs
        # for traffic lights, always use dchildren
        if schildren and dchildren:
            dchildren = schildren
        if snames == dnames:
            values = tuple([self.diff(n, s, d)
                            for n, s, d in zip(snames, svalues, dvalues)])
            return snames, values, dchildren
        else:
            sdict = defaultdict(lambda: None, zip(snames, svalues))
            ddict = defaultdict(lambda: None, zip(dnames, dvalues))
            names = tuple(set(snames + dnames))
            values = tuple([self.diff(n, sdict[n], ddict[n]) for n in names])
            return names, values, dchildren

    def diff(self, name, sourceValue, destValue):
        if sourceValue == destValue:
            return None
        elif destValue == None:
            return DEFAULT_VALUES[name]
        else:
            return destValue

    def writeDeleted(self, file):
        # data loss if two elements with different tags
        # have the same id
        for tag, id in self.ids_deleted:
            comment_start, comment_end = ("", "")
            additional = ""
            delete_element = DELETE_ELEMENT

            if self.type == TYPE_TLLOGICS and tag == TAG_CONNECTION:
                # see CAVEAT4
                names, values, children = self.id_attrs[(tag, id)]
                additional = " " + self.attr_string(names, values)

            if tag == TAG_TLL:  # see CAVEAT3
                comment_start, comment_end = (
                    "<!-- implicit via changed node type: ", " -->")

            if tag == TAG_CROSSING:
                delete_element = tag
                additional = ' discard="true"'

            if tag == TAG_ROUNDABOUT:
                delete_element = tag
                additional = ' discard="true"'
                comment_start, comment_end = (
                    "<!-- deletion of roundabouts not yet supported. see #2225 ", " -->")

            self.write(file, '%s<%s %s%s/>%s\n' % (
                comment_start,
                delete_element, self.id_string(tag, id), additional,
                comment_end))
        # data loss if two elements with different tags
        # have the same list of attributes and values
        for value_set in self.idless_deleted.values():
            self.write_idless(file, value_set, DELETE_ELEMENT)

    def writeCreated(self, file):
        self.write_tagids(file, self.ids_created, True)
        for tag, value_set in self.idless_created.items():
            self.write_idless(file, value_set, tag)

    def writeChanged(self, file):
        tagids_changed = self.ids_copied - \
            (self.ids_deleted | self.ids_created)
        self.write_tagids(file, tagids_changed, False)

    def writeCopies(self, file, copy_tags):
        tagids_unchanged = self.ids_copied - \
            (self.ids_deleted | self.ids_created)
        self.write_tagids(file, tagids_unchanged, False)
        for tag, value_set in self.idless_copied.items():
            self.write_idless(file, value_set, tag)

    def write_idless(self, file, attr_set, tag):
        for names, values, children in attr_set:
            self.write(file, '<%s %s/>\n' %
                       (tag, self.attr_string(names, values)))

    def write_tagids(self, file, tagids, create):
        for tagid in tagids:
            tag, id = tagid
            names, values, children = self.id_attrs[tagid]
            attrs = self.attr_string(names, values)
            child_strings = StringIO()
            if children:
                # writeDeleted is not supported
                children.writeCreated(child_strings)
                children.writeChanged(child_strings)

            if len(attrs) > 0 or len(child_strings.getvalue()) > 0 or create or tag in self.copy_tags:
                close_tag = "/>\n"
                if len(child_strings.getvalue()) > 0:
                    close_tag = ">\n%s" % child_strings.getvalue()
                self.write(file, '<%s %s %s%s' % (
                    tag,
                    self.id_string(tag, id),
                    attrs,
                    close_tag))
                if len(child_strings.getvalue()) > 0:
                    self.write(file, "</%s>\n" % tag)

    def write(self, file, item):
        file.write(" " * INDENT * self.level)
        file.write(item)

    def attr_string(self, names, values):
        return ' '.join(['%s="%s"' % (n, v) for n, v in sorted(zip(names, values)) if v != None])

    def id_string(self, tag, id):
        idattrs = IDATTRS[tag]
        return ' '.join(['%s="%s"' % (n, v) for n, v in sorted(zip(idattrs, id))])


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <source> <dest> <output-prefix>"
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("-p", "--use-prefix", action="store_true",
                         default=False, help="interpret source and dest as plain-xml prefix instead of network names")
    optParser.add_option("-d", "--direct", action="store_true",
                         default=False, help="compare source and dest files directly")
    optParser.add_option(
        "-c", "--copy", help="comma-separated list of element names to copy (if they are unchanged)")
    optParser.add_option("--path", dest="path", help="Path to binaries")
    options, args = optParser.parse_args()
    if len(args) != 3:
        sys.exit(USAGE)
    if options.use_prefix and options.direct:
        optParser.error(
            "Options --use-prefix and --direct are mutually exclusive")
    options.source, options.dest, options.outprefix = args
    return options


def create_plain(netfile, netconvert):
    prefix = netfile[:-8]
    call([netconvert,
          "--sumo-net-file", netfile,
          "--plain-output-prefix", prefix])
    return prefix


# creates diff of a flat xml structure
# (only children of the root element and their attrs are compared)
def xmldiff(source, dest, diff, type, copy_tags):
    attributeStore = AttributeStore(type, copy_tags)
    root_open = None
    have_source = os.path.isfile(source)
    have_dest = os.path.isfile(dest)
    if have_source:
        root_open, root_close = handle_children(source, attributeStore.store)
    if have_dest:
        root_open, root_close = handle_children(dest, attributeStore.compare)

    if not have_source and not have_dest:
        print("Skipping %s due to lack of input files" % diff)
    else:
        if not have_source:
            print(
                "Source file %s is missing. Assuming all elements are created" % source)
        elif not have_dest:
            print(
                "Dest file %s is missing. Assuming all elements are deleted" % dest)

        with codecs.open(diff, 'w', 'utf-8') as diff_file:
            diff_file.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            diff_file.write(root_open)
            if copy_tags:
                attributeStore.write(diff_file, "<!-- Copied Elements -->\n")
                attributeStore.writeCopies(diff_file, copy_tags)
            attributeStore.write(diff_file, "<!-- Deleted Elements -->\n")
            attributeStore.writeDeleted(diff_file)
            attributeStore.write(diff_file, "<!-- Created Elements -->\n")
            attributeStore.writeCreated(diff_file)
            attributeStore.write(diff_file, "<!-- Changed Elements -->\n")
            attributeStore.writeChanged(diff_file)
            diff_file.write(root_close)


# calls function handle_parsenode for all children of the root element
# returns opening and closing tag of the root element
def handle_children(xmlfile, handle_parsenode):
    root_open = None
    root_close = None
    level = 0
    xml_doc = pulldom.parse(xmlfile)
    for event, parsenode in xml_doc:
        if event == pulldom.START_ELEMENT:
            # print level, parsenode.getAttribute(ID_ATTR)
            if level == 0:
                root_open = parsenode.toprettyxml(indent="")
                # since we did not expand root_open contains the closing slash
                root_open = root_open[:-3] + ">\n"
                # change the schema for edge diffs
                root_open = root_open.replace(
                    "edges_file.xsd", "edgediff_file.xsd")
                root_close = "</%s>\n" % parsenode.localName
            if level == 1:
                # consumes END_ELEMENT, no level increase
                xml_doc.expandNode(parsenode)
                handle_parsenode(parsenode)
            else:
                level += 1
        elif event == pulldom.END_ELEMENT:
            level -= 1
    return root_open, root_close


# run
def main():
    options = parse_args()
    copy_tags = options.copy.split(',') if options.copy else []
    if options.direct:
        type = '.xml'
        xmldiff(options.source,
                options.dest,
                options.outprefix + type,
                type,
                copy_tags)
    else:
        if not options.use_prefix:
            netconvert = sumolib.checkBinary("netconvert", options.path)
            options.source = create_plain(options.source, netconvert)
            options.dest = create_plain(options.dest, netconvert)
        for type in PLAIN_TYPES:
            xmldiff(options.source + type,
                    options.dest + type,
                    options.outprefix + type,
                    type,
                    copy_tags)

if __name__ == "__main__":
    main()
