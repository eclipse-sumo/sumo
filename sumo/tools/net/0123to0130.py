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
import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

# attributes sorting lists
a = {}
a['edge'] = ( 'id', 'from', 'to', 'priority', 'type', 'function', 'inner' )
a['lane'] = ( 'id', 'depart', 'vclasses', 'allow', 'disallow', 'maxspeed', 'length', 'shape' )
a['junction'] = ( 'id', 'type', 'x', 'y', 'incLanes', 'intLanes', 'shape' )
a['logicitem'] = ( 'request', 'response', 'foes', 'cont' )
a['succ'] = ( 'edge', 'lane', 'junction' )
a['succlane'] = ( 'lane', 'via', 'tl', 'linkno', 'yield', 'dir', 'state', 'int_end' )
a['row-logic'] = ( 'id', 'requestSize' )
a['tl-logic'] = ( 'id', 'type', 'programID', 'offset' )
a['location'] = ( 'netOffset', 'convBoundary', 'origBoundary', 'projParameter' )
a['phase'] = ( 'duration', 'state', 'minDur', 'maxDur' )
a['district'] = ( 'id', 'shape', 'edges' )
a['dsink'] = ( 'id', 'weight' )
a['dsource'] = ( 'id', 'weight' )
a['roundabout'] = ( 'nodes', 'dummy' )

# elements which are single (not using opening/closing tag)
c = ( 'roundabout', 'logicitem', 'phase', 'succlane', 'dsource', 'dsink', 'junction', 'location', 'lane', 'timed_event' )

# remove these
r = ( 'lanes', 'logic' )


def getBegin(file):
    fd = open(file)
    content = "";
    for line in fd:
        if line.find("<net>")>=0 or line.find("<districts>")>=0:
            fd.close()
            return content
        content = content + line
    fd.close()
    return ""

class NetConverter(handler.ContentHandler):
    def __init__(self, outFileName, begin):
        self._out = open(outFileName, "w")
        self._out.write(begin)
        self._collect = False
        self._tree = []
        self._attributes = {}
        self._shapePatch = False
        self._skipping = False
        self._hadShape = False
        self._content = ""

    def checkWrite(self, what, isCharacters=False):
        self._out.write(what)

    def intend(self):
        self._out.write(" " * (3*len(self._tree)))


    def endDocument(self):
        self.checkWrite("\n")
        self._out.close()

    def startElement(self, name, attrs):
        if self._content.find("\n\n")>=0:
            self.checkWrite("\n")
        self._content = ""

        if name in r:
            return
        self.intend()
        self.checkWrite("<" + name)
        if name in a:
            for key in a[name]:
                if attrs.has_key(key):
                    self.checkWrite(' ' + key + '="' + attrs[key] + '"')
        if name not in c:
            self.checkWrite(">\n")
        else:
            self.checkWrite("/>\n")
        self._tree.append(name)


    def endElement(self, name):
        if name in r:
            return
        self._tree.pop()
        if name=="net":
            self.checkWrite("\n")
        if name not in c:
            self.intend()
            self.checkWrite("</" + name + ">")
            if name!="net":
                self.checkWrite("\n")
            

    def characters(self, content):
        self._content += content

                    
    def ignorableWhitespace(self, content):
        pass
        
    def skippedEntity(self, content):
        pass
        
    def processingInstruction(self, target, data):
        self.checkWrite('<?%s %s?>' % (target, data))




if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
beg = getBegin(sys.argv[1])
parser = make_parser()
net = NetConverter(sys.argv[1]+".chg", beg)
parser.setContentHandler(net)
parser.parse(sys.argv[1])

