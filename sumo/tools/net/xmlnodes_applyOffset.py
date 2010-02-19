#!/usr/bin/env python
"""
@file    xmlnodes_applyOffset.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id$

Applies a given offset to edges given in an xml-node-file.
The results are written into <XMLNODES>.mod.xml.
Call: xmlnodes_applyOffset.py <XMLNODES> <X-OFFSET> <Y-OFFSET>

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

class XMLNodesReader(handler.ContentHandler):
    def __init__(self, outFileName, xoff, yoff):
        self._out = open(outFileName, 'w')
        self._xoff = xoff
        self._yoff = yoff
        
    def endDocument(self):
        self._out.close()

    def startElement(self, name, attrs):
        self._out.write('<' + name)
        for (key, value) in attrs.items():
            if key == "x":
                self._out.write(' %s="%s"' % (key, saxutils.escape(str(float(value)+self._xoff))))
            elif key == "y":
                self._out.write(' %s="%s"' % (key, saxutils.escape(str(float(value)+self._yoff))))
            else:
                self._out.write(' %s="%s"' % (key, saxutils.escape(value)))
        self._out.write('>')

    def endElement(self, name):
        self._out.write('</' + name + '>')

    def characters(self, content):
        self._out.write(saxutils.escape(content))
                    
    def ignorableWhitespace(self, content):
        self._out.write(content)
        
    def processingInstruction(self, target, data):
        self._out.write('<?%s %s?>' % (target, data))


            
if len(sys.argv) < 4:
    print "Usage: " + sys.argv[0] + " <XMLNODES> <X-OFFSET> <Y-OFFSET>"
    sys.exit()
parser = make_parser()
reader = XMLNodesReader(sys.argv[1]+".mod.xml", float(sys.argv[2]), float(sys.argv[3]))
parser.setContentHandler(reader)
parser.parse(sys.argv[1])
