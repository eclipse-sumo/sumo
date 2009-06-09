#!/usr/bin/python
"""
@file    0103to0110.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007
@version $Id: 0103to0110.py 6858 2009-02-25 13:27:57Z behrisch $

Changes xml network files from version 0.10.3 to version 0.11.0.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

a = {}
a['edge'] = ( 'id', 'from', 'to', 'priority', 'type', 'function', 'inner' )
a['lane'] = ( 'id', 'depart', 'vclasses', 'maxspeed', 'length' )
a['junction'] = ( 'id', 'type', 'x', 'y' )
a['logicitem'] = ( 'request', 'response', 'foes', 'cont' )
a['succ'] = ( 'edge', 'lane', 'junction' )
a['succlane'] = ( 'lane', 'via', 'tl', 'linkno', 'yield', 'dir', 'state', 'int_end' )

c = ( 'logicitem', 'phase', 'succlane', 'dsource', 'dsink' )

def getBegin(file):
    fd = open(file)
    content = "";
    for line in fd:
        if line.find("<net>")>=0:
            fd.close()
            return content
        content = content + line
    fd.close()
    return ""

def patchPhase(attrs):
    state = ""
    for i in range(0, len(attrs['phase'])):
        c = 'g'
        if attrs['phase'][i]=='0':
            if attrs['yellow'][i]=='1':
                c = 'y'
            else:
                c = 'r'
        if attrs['brake'][i]=='0':
            if c=='y':
                c = 'Y'
            if c=='g':
                c = 'G'
        state = c + state
    return state


class NetConverter(handler.ContentHandler):
    def __init__(self, outFileName, begin):
        self._out = open(outFileName, "w")
        self._out.write(begin)

    def endDocument(self):
        self._out.write("\n")
        self._out.close()

    def startElement(self, name, attrs):
        self._out.write("<" + name)
        if name=="phase" and attrs.has_key('phase'):
            # patch phase definition
            state = patchPhase(attrs)
            for key in attrs.getNames():
                if key=='phase':
                    self._out.write(' state="' + state + '"')
                elif key!='yellow' and key!='brake':
                    self._out.write(' ' + key + '="' + attrs[key] + '"')
        elif name=='lane' and attrs.has_key('vclasses'):
            for key in a['lane']:
                if key == 'vclasses':
                    allowed = []
                    disallowed = []
                    for clazz in attrs['vclasses'].split(";"):
                    	if clazz:
	                        if clazz[0] == '-':
	                            disallowed.append(clazz[1:])
	                        else:
	                            allowed.append(clazz)
                    if allowed:
                     	self._out.write(' allow="%s"' % (" ".join(allowed)))
                    if disallowed:
                     	self._out.write(' disallow="%s"' % (" ".join(disallowed)))
                elif attrs.has_key(key):
                    self._out.write(' ' + key + '="' + attrs[key] + '"')
        else:
            if name not in a:
                for key in attrs.getNames():
                    self._out.write(' ' + key + '="' + attrs[key] + '"')
            else:
                for key in a[name]:
                    if attrs.has_key(key):
                        self._out.write(' ' + key + '="' + attrs[key] + '"')
        if name in c:
            self._out.write("/")
        self._out.write(">")

    def endElement(self, name):
        if name not in c:
            self._out.write("</" + name)
            self._out.write(">")

    def characters(self, content):
        self._out.write(content)
                    
    def ignorableWhitespace(self, content):
        self._out.write(content)
        
    def skippedEntity(self, content):
        self._out.write(content)
        
    def processingInstruction(self, target, data):
        self._out.write('<?%s %s?>' % (target, data))




if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
beg = getBegin(sys.argv[1])
parser = make_parser()
net = NetConverter(sys.argv[1]+".chg", beg)
parser.setContentHandler(net)
parser.parse(sys.argv[1])

