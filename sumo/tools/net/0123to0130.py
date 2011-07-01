#!/usr/bin/python
"""
@file    0123to0130.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007
@version $Id$

Changes xml network files from version 0.12.3 to version 0.13.0.

Copyright (C) 2009-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, string, sys, glob
from xml.sax import parse, handler
from optparse import OptionParser
from collections import defaultdict

# attributes sorting lists
a = {}
a['edge'] = ( 'id', 'from', 'to', 'name', 'priority', 'type', 'function', 'spread_type', 'shape' )
a['lane'] = ( 'id', 'index', 'vclasses', 'allow', 'disallow', 'maxspeed', 'length', 'endOffset', 'width', 'shape' )
a['junction'] = ( 'id', 'type', 'x', 'y', 'incLanes', 'intLanes', 'shape' )
a['logicitem'] = ('response', 'foes', 'cont' )
a['succlane'] = ('via', 'tl', 'linkno', 'dir', 'state' )
a['connection'] = ('from', 'to', 'lane', 'via', 'tl', 'linkIdx', 'dir', 'state' )
a['row-logic'] = a['ROWLogic'] = ( 'id', 'requestSize' )
a['tl-logic'] = a['tlLogic'] = ( 'id', 'type', 'programID', 'offset' )
a['location'] = ( 'netOffset', 'convBoundary', 'origBoundary', 'projParameter' )
a['phase'] = ( 'duration', 'state', 'minDur', 'maxDur', 'min_dur', 'max_dur' )
a['district'] = ( 'id', 'shape', 'edges' )
a['dsink'] = ( 'id', 'weight' )
a['dsource'] = ( 'id', 'weight' )
a['taz'] = ( 'id', 'shape', 'edges' )
a['tazSink'] = ( 'id', 'weight' )
a['tazSource'] = ( 'id', 'weight' )
a['roundabout'] = ( 'nodes', 'dummy' )
a['request'] = ( 'index', 'response', 'foes', 'cont' )
a['succ'] = ( 'edge', 'lane', 'junction')

# attributes which are optional
b = {}
b['edge'] = {}
b['edge']['type'] = ''
b['edge']['function'] = 'normal'
b['succlane'] = {}
b['succlane']['tl'] = ''
b['succlane']['linkno'] = ''
b['succlane']['linkIndex'] = ''

# elements which are single (not using opening/closing tag)
SINGLE = ( 'roundabout', 'logicitem', 'phase', 'succlane', 'dsource', 'dsink', 'location', 
        'lane', 'timed_event', 'connection', 'request' )

# remove these
REMOVED = ( 'lanes', 'logic', 'succ', 'row-logic', 'ROWLogic', 'logicitem')

# renamed elements
RENAMED_TAGS = {'tl-logic': 'tlLogic', 
        'row-logic': 'ROWLogic', 
        'district':'taz', 
        'dsource':'tazSource', 
        'dsink':'tazSink', 
        'succlane':'connection'}

renamedAttrs = {'min_dur': 'minDur', 
        'max_dur': 'maxDur', 
        'spread_type': 
        'spreadType', 
        'maxspeed':'maxSpeed', 
        'linkIdx':'linkIndex', 
        'linkno':'linkIndex'}

renamedValues = {'state': {'t': 'o'} }

def getBegin(file):
    fd = open(file)
    content = "";
    for line in fd:
        if line.find("<net>")>=0 or line.find("<districts>")>=0 or line.find("<add>")>=0:
            fd.close()
            return content
        content = content + line
    fd.close()
    return ""

class NetConverter(handler.ContentHandler):
    def __init__(self, outFileName, begin, has_no_destination):
        self._out = open(outFileName, "w")
        self._out.write(begin)
        self._tree = []
        self._content = ""
        self._am_parsing_rowlogic = None # parsing old element
        self._logicitems = defaultdict(list) # maps junction ids to items
        self._laneCount = 0
        self._has_no_destination = has_no_destination

    def isSingle(self, name):
        if name in SINGLE:
            return True
        if name == "junction" and self._inner_junction:
            return True
        return False

    def remove(self, name):
        if name == "succ" and self._has_no_destination:
            return False
        if name in REMOVED:
            return True
        else:
            return False

    def rename(self, name):
        if name == "succlane" and self._has_no_destination:
            return False
        if name in RENAMED_TAGS:
            return True
        else:
            return False

    def checkWrite(self, what):
        self._out.write(what.encode('iso-8859-1'))

    def indent(self):
        self._out.write(" " * (4*len(self._tree)))

    def endDocument(self):
        self.checkWrite("\n")
        self._out.close()

    def startElement(self, name, attrs):
        # special preparations
        if name == "succ":
            self._succ_from = attrs["edge"]
            self._succ_fromIdx = attrs["lane"].split('_')[-1]
        if name == "row-logic" or name == "ROWLogic":
            self._am_parsing_rowlogic = attrs["id"]
        if name == "logicitem": 
            self._logicitems[self._am_parsing_rowlogic].append(attrs)
        if name == "junction":
            self._inner_junction = attrs["id"][0] == ":"
        if name == "edge":
            self._laneCount = 0
        # skip removed
        if self.remove(name):
            return
        # compress empty lines
        if self._content.find("\n\n")>=0:
            self.checkWrite("\n")
        self._content = ""
        # open tag
        self.indent()
        if self.rename(name):
            self.checkWrite("<" + RENAMED_TAGS[name])
        else:
            self.checkWrite("<" + name)
        # special attribute handling
        if name == "succlane":
            if self._has_no_destination:
                # keep original
                self.checkWrite(' lane="%s"' % attrs["lane"])
            else:
                # upgrade
                self.checkWrite(' from="%s"' % self._succ_from)
                sepIndex = attrs['lane'].rindex('_')
                toEdge = attrs['lane'][:sepIndex]
                toIdx = attrs['lane'][sepIndex+1:]
                self.checkWrite(' to="%s"' % toEdge)
                self.checkWrite(' lane="%s:%s"' % (self._succ_fromIdx, toIdx))
        # write attributes
        if name in a:
            for key in a[name]:
                val = None
                if key in renamedAttrs and attrs.has_key(renamedAttrs[key]):
                    key = renamedAttrs[key]
                if attrs.has_key(key):
                    val = attrs[key]
                    if key in renamedValues:
                        val = renamedValues[key].get(val, val)
                    if name == "succlane" and (key == "linkno" or key == "linkIndex") and attrs["tl"] == '':
                        val = ''
                    if name not in b or key not in b[name] or val!=b[name][key]:
                        self.checkWrite(' ' + renamedAttrs.get(key, key) + '="%s"' % val)
                elif name == "lane" and key == "index":
                    self.checkWrite(' %s="%s"' % (key,self._laneCount))
                    self._laneCount += 1
        # close tag
        if self.isSingle(name):
            self.checkWrite("/>\n")
        else:
            self.checkWrite(">\n")
        self._tree.append(name)
        # transfer items from removed element ROWLogic
        if name == "junction":
            for logicitem_attrs in self._logicitems[attrs["id"]]:
                self.indent()
                self.checkWrite("<request")
                self.checkWrite(' index="%s"' % logicitem_attrs["request"])
                for key in a["logicitem"]:
                    if logicitem_attrs.has_key(key): # cont is optional
                        self.checkWrite(' ' + key + '="%s"' % logicitem_attrs[key])
                self.checkWrite("/>\n")

    def endElement(self, name):
        # special preparations
        if name == "row-logic" or name == "ROWLogic":
            self._am_parsing_rowlogic = None
        # skip removed
        if self.remove(name):
            return
        # optionaly write closing tag
        self._tree.pop()
        if name=="net":
            self.checkWrite("\n")
        if not self.isSingle(name):
            self.indent()
            if self.rename(name):
                self.checkWrite("</" + RENAMED_TAGS[name] + ">")
            else:
                self.checkWrite("</" + name + ">")
            if name!="net":
                self.checkWrite("\n")

    def characters(self, content):
        self._content += content

    def processingInstruction(self, target, data):
        self.checkWrite('<?%s %s?>' % (target, data))

def changeFile(fname):
    if options.verbose:
        print "Patching " + fname + " ..."
    if (("_deprecated_" in fname and not "net.netconvert" in fname) or 
            (os.path.join('tools','net','0') in fname)):
        print "Skipping file (looks like intentionally deprecated input): " + fname
        return
    has_no_destination = False
    if "SUMO_NO_DESTINATION" in open(fname).read():
        print "Partial conversion (cannot convert SUMO_NO_DESTINATION): " + fname
        has_no_destination = True
    net = NetConverter(fname+".chg", getBegin(fname), has_no_destination)
    parse(fname, net)
    if options.inplace:
        os.remove(fname)
        os.rename(fname+".chg", fname)

def walkDir(srcRoot):
    for root, dirs, files in os.walk(srcRoot):
        for name in files:
            if name.endswith(".net.xml") or name in ["net.netconvert", "net.netgen",
                                                     "tls.scenario", "net.scenario"]:
                changeFile(os.path.join(root, name))
        for ignoreDir in ['.svn', 'foreign']:
            if ignoreDir in dirs:
                dirs.remove(ignoreDir)


optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true",
                     default=False, help="tell me what you are doing")
optParser.add_option("-i", "--inplace", action="store_true",
                     default=False, help="replace original files")
(options, args) = optParser.parse_args()

if len(args) == 0:
    print "Usage: " + sys.argv[0] + " <net>+"
    sys.exit()
for arg in args:
    for fname in glob.glob(arg):
        if os.path.isdir(fname):
            walkDir(fname)
        else:
            changeFile(fname)
