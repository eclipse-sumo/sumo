#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
@file    network_POI.py
@author  Ronald.Nippold@dlr.de
@date    2009-20-10
@version
@usage   python network_POI.py <network-file-to-be-analyzed.net.xml> <edge-with-POIs>,<edge-with-POIs,...> <distance-between-POIs>

* Spatial distribute of POIs along given edges on a given network
* Edges are separated with comma and without spaces in between
* The distance between POIs may be any positive real number
* So far POIs are situated in the middle on all edges without regard to the type of the edge (street, junction)
* Edges may be given in arbitrary order, connected edges are found automatically
* Therefore: crossing chains of connected edges are not allowed -> this needs two different runs of this script
* Output is written in file 'poi.xml'

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet

if len(sys.argv) < 4:
	print("Usage: " + sys.argv[0] + " <net>." + "<edge1,edge2,...>" + "<distance between POIs")
	sys.exit()
edgeList = sys.argv[2].split(',')
POIdist = float(sys.argv[3])

def findPred(connEdgesTmp,edgeList, success = 0):
	for edge in edgeList:
		if connEdgesTmp[0][1] == edge[2]:	# check first element of connEdgesTmp for predecessor
			connEdgesTmp.insert(0,edge)
			edgeList.remove(edge)
			success = 1
			return success
	return success

def findSucc(connEdgesTmp,edgeList, success = 0):
	for edge in edgeList:
		if connEdgesTmp[len(connEdgesTmp)-1][2] == edge[1]:	# check last element of connEdgesTmp for successor
			connEdgesTmp.append(edge)
			edgeList.remove(edge)
			success = 1
			return success
	return success

def poiAppend(poi_edge, poi_nr, poi_x, poi_y, poi_buf):
	poi_buf.append("\t<poi id=\"" + str(poi_edge) + "-" + str(poi_nr) + "\" type=\"default\" color=\"1,0,0\" layer=\"0\" x=\"" + str(poi_x) + "\" y=\"" + str(poi_y) + "\"/>")
	return

print("Reading net ...\n")
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
net = net.getNet()

# find all predecessor/successor nodes for the given edges
nodeList = []
for edge in net._edges:
	for elm in sys.argv[2].split(','):
		if str(edge._id) == elm:
			data = []
			data.append(edge._id)
			data.append(edge._from._id)
			data.append(edge._to._id)
			data.append(edge._shape)
			nodeList.append(data)

# find connected edges
connEdges = []
connEdgesTmp = []
pred = 1
succ = 1
tmpList = nodeList[:]								# real copy, not just reference because of loop in nodeList
for elm in nodeList:
	if elm in tmpList:								# Has this edge already been processed?
		while pred == 1:							# search for predecessor
			if elm in tmpList:
				tmpList.remove(elm)					# don't look up the current edge in search list
			if elm not in connEdgesTmp:
				connEdgesTmp.append(elm)			# for the first element when searching for connected edges
			pred = findPred(connEdgesTmp,tmpList)
		while succ == 1:							# search for successor
			succ = findSucc(connEdgesTmp,tmpList)	# all other things already done in predecessor
		connEdges.append(connEdgesTmp)
		connEdgesTmp = []							# clean up
		pred = 1									# reset
		succ = 1

# prepare output
POIbuf = []
POIbuf.append("<?xml version=\"1.0\"?>\n")
POIbuf.append("<!--\n\nPOIs for edges:\n\t" + str(sys.argv[2]) + "\n\non network:\n\t" + str(sys.argv[1]) + "\n-->\n")
POIbuf.append("<POIs>")
# space out POIs on the found (connected) edges
for elm in connEdges:
	shapes = []										# tmp array for storing all shape coordinate of connected edges
	offset = 0										# Anything left from the edge before?
	for edge in elm:
		for p in edge[3]:
#			shapes.append(p)
			shapes.append([p[0], p[1], edge[0]])	# collect all shapes of connected edges
	for i in range(0, len(shapes)-1):
		x1 = shapes[i][0]							# point at the beginning of the current edge
		y1 = shapes[i][1]
		x2 = shapes[i+1][0]							# point at the end of the current edge
		y2 = shapes[i+1][1]
		dX = x2 - x1
		dY = y2 - y1
		eucDist = pow(pow(dX, 2) + pow(dY, 2), 0.5)	# Euclidean distance
		normAscX = dX / eucDist						# normalized Euclidean distance in x
		normAscY = dY / eucDist						# normalized Euclidean distance in y
		xCur = x1									# the current POI position
		yCur = y1
		if i == 0:
			POIid = 1
			poiAppend(shapes[i][2], POIid, xCur, yCur, POIbuf)
			POIid = POIid + 1
			POIpos = POIdist
		else:
			if shapes[i][2] != shapes[i-1][2]:		# begin of new edge
				POIid = 1
				POIpos = offset						# reset POI-position at the beginning of an edge
		# check if first (new) point is on current edge, consider possible offset from the edge before
		if eucDist + offset > POIdist:
			xCur = xCur + normAscX * (POIdist - offset)
			yCur = yCur + normAscY * (POIdist - offset)
			poiAppend(shapes[i][2], POIid, xCur, yCur, POIbuf)
			POIid = POIid + 1
			POIpos = POIpos + POIdist				# always add POIdist, reset to current offset when changing the edge
			offset = 0								# reset offset
			# other points - if possible - with normal spacing in between
			while pow(pow((x2 - xCur), 2) + pow((y2 - yCur), 2), 0.5) > POIdist:
				xCur = xCur + normAscX * POIdist
				yCur = yCur + normAscY * POIdist
				poiAppend(shapes[i][2], POIid, xCur, yCur, POIbuf)
				POIid = POIid + 1
				POIpos = POIpos + POIdist			# always add POIdist, reset to current offset when changing the edge
		# calculate offset (residual) for the next connected edge
		offset = offset + pow(pow((x2 - xCur), 2) + pow((y2 - yCur), 2), 0.5)
POIbuf.append("</POIs>")

print("writing output\n")
# finally write output in file
outfile = open('poi.xml', 'w')
for POIs in POIbuf:
	outfile.write(POIs + "\n")
outfile.close()
print("done")
