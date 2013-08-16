#!/usr/bin/env python
"""
@file    binary2plain.py
@author  Michael Behrisch
@date    2012-03-11
@version $Id$

Converter between SUMO's binary XML and plain XML

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2012-2013 DLR (http://www.dlr.de/) and contributors
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
BOUNDARY = 12
COLOR = 13
NODE_TYPE = 14
EDGE_FUNCTION = 15
ROUTE = 16
SCALED2INT = 17
SCALED2INT_POSITION_2D = 18
SCALED2INT_POSITION_3D = 19

def read(content, format):
    return struct.unpack(format, content.read(struct.calcsize(format)))

def readByte(content):
    return read(content, "B")[0]

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

def readRoute(content):
    n = readInt(content)
    list = []
    first = readInt(content)
    if first < 0:
        bits = -first
        numFields = 8 * 4 / bits
        mask = (1 << bits) - 1
        edge = readInt(content)
        list.append(edges[edge])
        n -= 1
        field = numFields
        while n > 0:
            if field == numFields:
                data = readInt(content)
                field = 0
            followIndex = (data >> ((numFields - field - 1) * bits)) & mask;
            edge = followers[edge][followIndex]
            list.append(edges[edge])
            field += 1
            n -= 1
    else:
        list.append(edges[first])
        n -= 1
        while n > 0:
            list.append(edges[readInt(content)])
            n -= 1
    return list

def typedValueStr(content):
    valType = readByte(content)
    if valType == BYTE:
        return str(readByte(content))
    elif valType == INTEGER:
        return str(readInt(content))
    elif valType == FLOAT:
        return '%.2f' % readDouble(content)
    elif valType == STRING:
        return readString(content)
    elif valType == LIST:
        l = []
        for i in range(readInt(content)):
            l.append(typedValueStr(content))
        return " ".join(l)
    elif valType == EDGE:
        return edges[readInt(content)]
    elif valType == LANE:
        return '%s_%s' % (edges[readInt(content)], readByte(content))
    elif valType == POSITION_2D:
        return '%.2f,%.2f' % (readDouble(content),readDouble(content))
    elif valType == POSITION_3D:
        return '%.2f,%.2f,%.2f' % (readDouble(content),readDouble(content),readDouble(content))
    elif valType == BOUNDARY:
        return '%.2f,%.2f,%.2f,%.2f' % (readDouble(content),readDouble(content),
                                        readDouble(content),readDouble(content))
    elif valType == COLOR:
        val = read(content, "BBBB")
        return '%.2f,%.2f,%.2f' % (val[0]/255.,val[1]/255.,val[2]/255.)
    elif valType == NODE_TYPE:
        return nodeTypes[readByte(content)]
    elif valType == EDGE_FUNCTION:
        return edgeTypes[readByte(content)]
    elif valType == ROUTE:
        return " ".join(readRoute(content))
    elif valType == SCALED2INT:
        return '%.2f' % (readInt(content)/100.)
    elif valType == SCALED2INT_POSITION_2D:
        return '%.2f,%.2f' % (readInt(content)/100.,readInt(content)/100.)
    elif valType == SCALED2INT_POSITION_3D:
        return '%.2f,%.2f,%.2f' % (readInt(content)/100.,readInt(content)/100.,readInt(content)/100.)

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
    typ = readByte(content)
    if typ == XML_TAG_START:
        if startOpen:
            out.write(">\n")
        out.write("    " * len(stack))
        stack.append(readByte(content))
        out.write("<" + elements[stack[-1]])
        startOpen = True
    elif typ == XML_TAG_END:
        if startOpen:
            out.write("/>\n")
            stack.pop()
            startOpen = False
        else:
            out.write("    " * (len(stack)-1))
            out.write("</%s>\n" % elements[stack.pop()])
        readByte(content)
        if len(stack) == 0:
            break
    elif typ == XML_ATTRIBUTE:
        out.write(' %s="%s"' % (attributes[readByte(content)], typedValueStr(content)))
    else:
        print >> sys.stderr, "Unknown type %s" % typ