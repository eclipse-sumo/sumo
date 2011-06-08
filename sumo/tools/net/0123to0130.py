#!/usr/bin/python
"""
@file    0103to0110.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007
@version $Id$

Changes xml network files from version 0.10.3 to version 0.11.0.

Copyright (C) 2009-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, string, sys, glob
from xml.sax import parse, handler
from optparse import OptionParser

# attributes sorting lists
a = {}
a['edge'] = ( 'id', 'from', 'to', 'name', 'priority', 'type', 'function', 'spread_type', 'shape' )
a['lane'] = ( 'id', 'depart', 'vclasses', 'allow', 'disallow', 'maxspeed', 'length', 'endOffset', 'width', 'shape' )
a['junction'] = ( 'id', 'type', 'x', 'y', 'incLanes', 'intLanes', 'shape' )
a['logicitem'] = ( 'request', 'response', 'foes', 'cont' )
a['succ'] = ( 'edge', 'lane', 'junction' )
a['succlane'] = ( 'lane', 'via', 'tl', 'linkno', 'dir', 'state' )
a['row-logic'] = a['ROWLogic'] = ( 'id', 'requestSize' )
a['tl-logic'] = a['tlLogic'] = ( 'id', 'type', 'programID', 'offset' )
a['location'] = ( 'netOffset', 'convBoundary', 'origBoundary', 'projParameter' )
a['phase'] = ( 'duration', 'state', 'minDur', 'maxDur', 'min_dur', 'max_dur' )
a['district'] = ( 'id', 'shape', 'edges' )
a['dsink'] = ( 'id', 'weight' )
a['dsource'] = ( 'id', 'weight' )
a['roundabout'] = ( 'nodes', 'dummy' )

# attributes which are optional
b = {}
b['edge'] = {}
b['edge']['type'] = ''
b['edge']['function'] = 'normal'

# elements which are single (not using opening/closing tag)
c = ( 'roundabout', 'logicitem', 'phase', 'succlane', 'dsource', 'dsink', 'junction', 'location', 'lane', 'timed_event' )

# remove these
removed = ( 'lanes', 'logic' )

# renamed elements
renamed = {'tl-logic': 'tlLogic', 'row-logic': 'ROWLogic'}

renamedAttrs = {'min_dur': 'minDur', 'max_dur': 'maxDur'}

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
    def __init__(self, outFileName, begin):
        self._out = open(outFileName, "w")
        self._out.write(begin)
        self._tree = []
        self._content = ""

    def checkWrite(self, what):
        self._out.write(what.encode('iso-8859-1'))

    def indent(self):
        self._out.write(" " * (4*len(self._tree)))

    def endDocument(self):
        self.checkWrite("\n")
        self._out.close()

    def startElement(self, name, attrs):
        if self._content.find("\n\n")>=0:
            self.checkWrite("\n")
        self._content = ""

        if name in removed:
            return
        self.indent()
        if name in renamed:
            self.checkWrite("<" + renamed[name])
        else:
            self.checkWrite("<" + name)
        if name in a:
            for key in a[name]:
                if attrs.has_key(key):
                    val = attrs[key]
                    if key in renamedValues:
                        val = renamedValues[key].get(val, val)
                    if name not in b or key not in b[name] or attrs[key]!=b[name][key]:
                        self.checkWrite(' ' + renamedAttrs.get(key, key) + '="%s"' % val)
                    
        if name not in c:
            self.checkWrite(">\n")
        else:
            self.checkWrite("/>\n")
        self._tree.append(name)

    def endElement(self, name):
        if name in removed:
            return
        self._tree.pop()
        if name=="net":
            self.checkWrite("\n")
        if name not in c:
            self.indent()
            if name in renamed:
                self.checkWrite("</" + renamed[name] + ">")
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
    net = NetConverter(fname+".chg", getBegin(fname))
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
