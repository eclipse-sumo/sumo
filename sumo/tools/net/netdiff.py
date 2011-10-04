#!/usr/bin/env python
"""
@file    netdiff.py
@author  Jakob.Erdmann@dlr.de
@date    2011-10-04
@version $Id$

Reads two networks (source, dest) and tries to produce the minimal plain-xml input
which can be loaded with netconvert alongside source to create dest

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import sys
import os
import StringIO
from xml.dom import pulldom
from optparse import OptionParser
from subprocess import call
from collections import namedtuple, defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', 'lib'))
from testUtil import checkBinary

INDENT = 4
PLAIN_TYPES = [
        '.nod.xml', 
        '.edg.xml', 
        '.con.xml' # different logic for deletes, 
        #  cannot start to print creates until the whole of dest is read
        
        ] # file types to compare
ID_ATTR = 'id' # attribute for unique identifier
DELETE_ELEMENT = 'reset' # the xml element for signifying deletes

# provide an order for the attribute names
ATTRIBUTE_NAMES = {
        #'.nod.xml' : ()
        #'.edg.xml' : () 
        #'.con.xml' : ()
        }

# default values for the given attribute (needed when attributes appear in source but do not appear in dest)
DEFAULT_VALUES = defaultdict(lambda: "")
DEFAULT_VALUES['width'] = "-1"
RESET = 0


# stores attributes for later comparison
class AttributeStore:

    def __init__(self, attrnames):
        self.attrnames = attrnames
        self.id_values = {}
        self.idless_deleted = set()
        self.idless_created = set()
        self.element_name = "notInitialized"


    # getAttribute returns "" if not present
    def getAttr(self, node, name):
        if node.hasAttribute(name):
            return node.getAttribute(name)
        else:
            return None


    def diff(self, sourceValue, destValue):
        if sourceValue == destValue:
            return None
        elif destValue == None:
            return RESET
        else:
            return destValue


    def store(self, xmlnode):
        self.element_name = xmlnode.localName
        values = tuple([self.getAttr(xmlnode, a) for a in self.attrnames])
        if xmlnode.hasAttribute(ID_ATTR):
            id = xmlnode.getAttribute(ID_ATTR)
            self.id_values[id] = values
        else:
            #print "add to deleted", values
            self.idless_deleted.add(values)


    def compare(self, xmlnode):
        values = tuple([self.getAttr(xmlnode, a) for a in self.attrnames])
        if xmlnode.hasAttribute(ID_ATTR):
            id = xmlnode.getAttribute(ID_ATTR)
            self.id_values[id] = tuple([self.diff(s, d) for s, d in zip(self.id_values[id], values)])
        else:
            if values in self.idless_deleted:
                #print "remove from deleted", values
                self.idless_deleted.remove(values)
            else:
                #print "add to created", values
                self.idless_created.add(values)


    def replace_RESET(self, name, value):
        if value == RESET:
            return DEFAULT_VALUES[name]
        else:
            return value


    def writeDeleted(self, file):
        self.write_idless(file, self.idless_deleted, DELETE_ELEMENT)


    def writeCreated(self, file):
        self.write_idless(file, self.idless_created, self.element_name)


    def write_idless(self, file, value_set, tag):
        for values in value_set:
            valueStrings = ['%s="%s"' % (n, self.replace_RESET(n,v)) for n,v in
                    zip(self.attrnames, values) if v != None]
            file.write('%s<%s %s/>\n' % (
                " " * INDENT,
                tag,
                ' '.join(valueStrings)))


    def writeChanged(self, file):
        for id, values in self.id_values.iteritems():
            valueStrings = ['%s="%s"' % (n, self.replace_RESET(n,v)) for n,v in
                    zip(self.attrnames, values) if v != None]
            if len(valueStrings) > 0:
                file.write('%s<%s %s="%s" %s/>\n' % (
                    " " * INDENT,
                    self.element_name,
                    ID_ATTR, id,
                    ' '.join(valueStrings)))



def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <source> <dest> <output-prefix>"
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
            default=False, help="Give more output")
    optParser.add_option("-p", "--use-prefix", action="store_true",
            default=False, help="interpret source and dest as plain-xml prefix instead of network names")
    optParser.add_option("--path", dest="path",
            default=os.environ.get("SUMO_BINDIR", ""), help="Path to binaries")
    options, args = optParser.parse_args()
    if len(args) != 3:
        sys.exit(USAGE)
    options.source, options.dest, options.outprefix = args
    return options 


def create_plain(netfile):
    netconvert = checkBinary("netconvert", options.path)        
    prefix = netfile[:-8]
    call([netconvert, 
        "--sumo-net-file", netfile, 
        "--plain-output-prefix", prefix])
    return prefix


# creates diff of a flat xml structure 
# (only children of the root element and their attrs are compared)
def xmldiff(source, dest, diff, type):
    # result streams
    deleted = StringIO.StringIO()
    created = StringIO.StringIO()
    changed = StringIO.StringIO()

    attrnames = set()
    source_ids, root_open, root_close = get_ids_and_attrs_and_root(source, attrnames)
    dest_ids, root_open, root_close = get_ids_and_attrs_and_root(dest, attrnames)
    # prepare attrnames
    if type in ATTRIBUTE_NAMES:
        attrnames = ATTRIBUTE_NAMES[type]
    else:
        attrnames.discard(ID_ATTR)
    attributeStore = AttributeStore(tuple(attrnames))
    # compare ids
    deleted_ids = source_ids.difference(dest_ids)
    created_ids = dest_ids.difference(source_ids)

    #print "source_ids", source_ids
    #print "dest_ids", dest_ids
    #print "deleted %s, created %s, maybechanged %s, have_idless %s" % (
    #        len(deleted_ids), len(created_ids), len(maybechanged_ids), have_idless)

    # process source, store maybechanged and print deletes
    def handle_source(parsenode):
        id = parsenode.getAttribute(ID_ATTR)
        if id in deleted_ids:
            deleted.write('%s<%s %s="%s"/>\n' % (
                " " * INDENT, DELETE_ELEMENT, ID_ATTR, id))
        else:
            attributeStore.store(parsenode)
    handle_children(source, handle_source)

    # process dest, compare maybechanged and print creates
    def handle_dest(parsenode):
        id = parsenode.getAttribute(ID_ATTR)
        if id in created_ids:
            created.write(" " * INDENT)
            created.write(parsenode.toprettyxml(indent=""))
        else:
            attributeStore.compare(parsenode)
    handle_children(dest, handle_dest)

    attributeStore.writeDeleted(deleted)
    attributeStore.writeCreated(created)
    attributeStore.writeChanged(changed)

    with open(diff, 'w') as diff_file:
        diff_file.write(root_open)
        write_section(diff_file, "Deleted Elements", deleted)
        write_section(diff_file, "Created Elements", created)
        write_section(diff_file, "Changed Elements", changed)
        diff_file.write(root_close)


def write_section(output_file, section_name, stringio):
    if stringio.len > 0:
        output_file.write(" " * INDENT + "<!-- %s -->\n" % section_name)
        output_file.write(stringio.getvalue())
    

# parses ids and compiles the set of attrnames of a flat xml structure 
# (only children of the root element and their attrs are compared)
# idea: get for attribute-values to avoid later comparisons
def get_ids_and_attrs_and_root(xmlfile, attrnames):
    ids = set()
    def callback(parsenode):
        for i in range(parsenode.attributes.length):
            attrnames.add(parsenode.attributes.item(i).localName)
        ids.add(parsenode.getAttribute(ID_ATTR))
    root_open, root_close = handle_children(xmlfile, callback)
    ids.discard("")
    return ids, root_open, root_close


# calls function handle_parsenode for all children of the root element
# returns opening and closing tag of the root element
def handle_children(xmlfile, handle_parsenode):
    root_open = None
    root_close = None
    xml_doc = pulldom.parse(xmlfile)
    level = 0
    for event, parsenode in xml_doc:
        if event == pulldom.START_ELEMENT: 
            # print level, parsenode.getAttribute(ID_ATTR)
            if level == 0:
                root_open = parsenode.toprettyxml(indent="")
                # since we did not expand root_open contains the closing slash
                root_open = root_open[:-3] + ">\n"
                root_close = "</%s>\n" % parsenode.localName
            if level == 1:
                xml_doc.expandNode(parsenode) # consumes END_ELEMENT, no level increase
                handle_parsenode(parsenode)
            else:
                level += 1
        elif event == pulldom.END_ELEMENT: 
            level -= 1
    return root_open, root_close


# run
options = parse_args()
if not options.use_prefix:
    options.source = create_plain(options.source)
    options.dest = create_plain(options.dest)
for type in PLAIN_TYPES:
    xmldiff(options.source + type, 
            options.dest + type, 
            options.outprefix + type, 
            type)

