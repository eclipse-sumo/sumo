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
ROUTE = 15

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
    n = readInt(content)
    list = []
    for i in range(n):
        read(content, "B") #type
        list.append(readString(content))
    return list

def readIntListList(content):
    n = readInt(content)
    list = []
    for i in range(n):
        read(content, "B") #type
        n1 = readInt(content)
        list.append([])
        for j in range(n1):
            read(content, "B") #type
            list[-1].append(readInt(content))
    return list


out = sys.stdout
content = open(sys.argv[1], 'rb')
read(content, "BBB") #type, sbx version, type
readString(content) #sumo version
read(content, "B") #type
elements = readStringList(content)
read(content, "B") #type
attributes = readStringList(content)
read(content, "B") #type
nodeTypes = readStringList(content)
read(content, "B") #type
edgeTypes = readStringList(content)
read(content, "B") #type
edges = readStringList(content)
read(content, "B") #type
followers = readIntListList(content)
stack = []
startOpen = False
while True:
    typ = read(content, "B")[0]
    if typ == XML_TAG_START:
        if startOpen:
            out.write(">\n")
        out.write("    " * len(stack))
        stack.append(readInt(content))
        out.write("<" + elements[stack[-1]])
        startOpen = True
    elif typ == XML_TAG_END:
        if startOpen:
            out.write(">\n")
            startOpen = False
        out.write("    " * (len(stack)-1))
        out.write("</%s>\n" % elements[stack.pop()])
        readInt(content)
        if len(stack) == 0:
            break
    elif typ == XML_ATTRIBUTE:
        out.write(" %s=" % attributes[readInt(content)])
        valType = read(content, "B")[0]
        if valType == BYTE:
            out.write('"%s"' % read(content, "B"))
        elif valType == INTEGER:
            out.write('"%s"' % readInt(content))
        elif valType == FLOAT:
            out.write('"%s"' % readDouble(content))
        elif valType == STRING:
            out.write('"%s"' % readString(content))
        elif valType == LIST:
            out.write('"%s"' % readInt(content))
        elif valType == EDGE:
            out.write('"%s"' % readInt(content))
        elif valType == LANE:
            out.write('"%s"' % readInt(content))
        elif valType == POSITION_2D:
            out.write('"%s,%s"' % (readDouble(content),readDouble(content)))
        elif valType == POSITION_3D:
            out.write('"%s,%s,%s"' % (readInt(content),readDouble(content),readDouble(content)))
        elif valType == COLOR:
            out.write('"%s,%s,%s"' % read(content, "BBB"))
        elif valType == NODE_TYPE:
            out.write('"%s"' % nodeTypes[readInt(content)])
        elif valType == EDGE_FUNCTION:
            out.write('"%s"' % edgeTypes[readInt(content)])
