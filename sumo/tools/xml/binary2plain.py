#!/usr/bin/env python
"""
@file    binary2plain.py
@author  Michael Behrisch
@date    2012-03-11
@version $Id$

Converter between SUMO's binary XML and plain XML

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2012-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sys, struct

BYTE = 0
INTEGER = 1
FLOAT = 2
STRING = 3
LIST = 4
XML_TAG_START = 5
XML_TAG_END = 6
XML_ATTRIBUTE = 7
EDGE = 8
LANE = 9
POSITION_2D = 10
POSITION_3D = 11
COLOR = 12
NODE_TYPE = 13
EDGE_FUNCTION = 14

def read(content, format):
    return struct.unpack(format, content.read(struct.calcsize(format)))

def readInt(content):
    return read(content, "i")[0]

def readDouble(content):
    return read(content, "d")[0]

def readString(content):
    length = readInt(content)
    return read(content, "%ss" % length)[0]

def readStringList(content):
    n = read(content, "i")[0]
    list = []
    for i in range(n):
        read(content, "B") #type
        list.append(readString(content))
    return list


out = sys.stdout
content = open(sys.argv[1], 'rb')
print read(content, "BBB") #sbx version
print readString(content) #sumo version
read(content, "B") #type
elements = readStringList(content)
read(content, "B") #type
attributes = readStringList(content)
read(content, "B") #type
nodeTypes = readStringList(content)
read(content, "B") #type
edgeTypes = readStringList(content)
stack = []
startOpen = False
while True:
    typ = read(content, "B")
    if typ == XML_TAG_START:
        if startOpen:
            out.write(">\n")
        out.write("    " * len(stack))
        stack.append(readInt(content))
        out.write("<" + elements[stack[-1]])
        startOpen = True
    elif typ == XML_TAG_END:
        if startOpen:
            out.write(">")
        out.write("</%s>" % elements[stack.pop()])
        readInt(content)
    elif typ == XML_ATTRIBUTE:
        out.write(" %s=" % attributes[readInt(content)])
        valType = read(content, "B")
        if typ == BYTE:
            out.write('"%s"' % read(content, "B"))
        elif typ == INTEGER:
            out.write('"%s"' % readInt(content))
        elif typ == FLOAT:
            out.write('"%s"' % readDouble(content))
        elif typ == STRING:
            out.write('"%s"' % readInt(content))
        elif typ == LIST:
            out.write('"%s"' % readInt(content))
        elif typ == EDGE:
            out.write('"%s"' % readInt(content))
        elif typ == LANE:
            out.write('"%s"' % readInt(content))
        elif typ == POSITION_2D:
            out.write('"%s,%s"' % (readDouble(content),readDouble(content)))
        elif typ == POSITION_3D:
            out.write('"%s,%s,%s"' % (readInt(content),readDouble(content),readDouble(content)))
        elif typ == COLOR:
            out.write('"%s,%s,%s"' % read(content, "BBB"))
        elif typ == NODE_TYPE:
            out.write('"%s"' % nodeTypes[readInt(content)])
        elif typ == EDGE_FUNCTION:
            out.write('"%s"' % edgeTypes[readInt(content)])
