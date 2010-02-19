"""
@file    vissim_parseRoutes.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-05-27
@version $Id$


Parses routes given in the Vissim file (first parameter) as (in-)flows and 
 route decisions and saves them into "routes.xml".
(Starting?) edges of the route may be renamed by setting them within "edgemap"
 variable (see below).

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

edgemap = {}
edgemap["203"] = "203[0]"


import sys
from random import *

def getName(vals, beg):
	name = vals[beg]
	while name.count('"')!=2:
		beg = beg + 1
		name = name + " " + vals[beg]
	return name.replace('"', '')
		

def parseInFlow(inflow):
	vals = inflow.split()
	i = vals.index("NAME")
	name = getName(vals, i+1)
	i = vals.index("STRECKE", i)
	strecke = vals[i+1]
	i = vals.index("Q", i+1)
	if vals[i+1]=="EXAKT":
		q = float(vals[i+2])
	else:
		q = float(vals[i+1])
	i = vals.index("ZUSAMMENSETZUNG", i+1)
	zusammensetzung = vals[i+1]
	i = vals.index("ZEIT", i)
	i = vals.index("VON", i)
	von = float(vals[i+1])
	i = vals.index("BIS")
	bis = float(vals[i+1])
	return (name, strecke, q, von, bis)

def parseRouteDecision(rd):
	vals = rd.split()
	i = vals.index("STRECKE")
	strecke = vals[i+1]
	i = vals.index("ZEIT", i) # probably, we would normally already here to iterate over time spans...
	i = vals.index("VON", i+1)
	von = float(vals[i+1])
	i = vals.index("BIS", i+1)
	bis = float(vals[i+1])
	if "ROUTE" in vals:
		i = vals.index("ROUTE", i+1)
	else:
		i = 0
	sumAnteil = 0
	routes = []
	while i>0:
		r_id = vals[i+1]
		i = vals.index("STRECKE", i+1)
	 	r_ziel = vals[i+1]
		i = vals.index("ANTEIL", i+1)
		r_anteil = float(vals[i+1])
		r_edges = []
		if vals[i+2]=="UEBER":
			i = i + 3
			while i<len(vals) and vals[i]!="ROUTE":
				if vals[i] in edgemap:
					r_edges.append(edgemap[vals[i]])
				else:
					r_edges.append(vals[i])
				i = i + 1
		else:
			try:
				i = vals.index("ROUTE", i)
			except:
				i = -1
		if r_ziel in edgemap:
			r_edges.append(edgemap[r_ziel])
		else:
			r_edges.append(r_ziel)
		routes.append( (r_id, r_anteil, r_edges) )
		sumAnteil = sumAnteil + r_anteil
		if i>=len(vals):
			i = -1
	print len(routes)
	return ( strecke, sumAnteil, von, bis, routes )

		

def sorter(idx):
	def t(i, j):
		if i[idx] < j[idx]:
			return -1
		elif i[idx] > j[idx]:
			return 1
		else:
			return 0


print "Parsing Vissim input..."
fd = open(sys.argv[1])
routeDecisions = []
haveRouteDecision = False
currentRouteDecision = ""
inflows = []
haveInFlow = False
currentInFlow = ""
for line in fd:
	# put all route decision ("ROUTENENTSCHEIDUNG") to a list (routeDecisions)
	if line.find("ROUTENENTSCHEIDUNG")==0:
		if haveRouteDecision:
			routeDecisions.append(" ".join(currentRouteDecision.split()))
		haveRouteDecision = True
		currentRouteDecision = ""
	elif line[0]!=' ':
		if haveRouteDecision:
			routeDecisions.append(" ".join(currentRouteDecision.split()))
		haveRouteDecision = False
	if haveRouteDecision:
		currentRouteDecision = currentRouteDecision + line
	# put all inflows ("ZUFLUSS") to a list (inflows)
	if line.find("ZUFLUSS")==0:
		if haveInFlow:
			inflows.append(" ".join(currentInFlow.split()))
		haveInFlow = True
		currentInFlow = ""
	elif line[0]!=' ':
		if haveInFlow:
			inflows.append(" ".join(currentInFlow.split()))
		haveInFlow = False
	if haveInFlow:
		currentInFlow = currentInFlow + line

# process inflows
print "Writing flows..."
fdo = open("flows.xml", "w")
fdo.write("<flowdefs>\n")
for inflow in inflows:
	(name, strecke, q, von, bis) = parseInFlow(inflow)
	fdo.write('    <flow id="' + name + '" from="' + strecke + '" begin="' + str(von) + '" end="' + str(bis) + '" no="' + str(int(q)) + '"/>\n')
fdo.write("</flowdefs>\n")	
fdo.close()

# process combinations
#  parse route decision
print "Building routes..."
edges2check = {}
edgesSumFlows = {}
for rd in routeDecisions:
	( strecke, sumAnteil, von, bis, routes ) = parseRouteDecision(rd)
	if strecke not in edges2check:
		edgesSumFlows[strecke] = sumAnteil
		edges2check[strecke] = ( von, bis, routes )
# compute emissions with routes
emissions = []
for inflow in inflows:
	(name, strecke, q, von, bis) = parseInFlow(inflow)
	if strecke in edges2check:
		routes = edges2check[strecke]
		for vi in range(0, int(q)):
			t = von + float(bis-von) / float(q) * float(vi)
			fi = random() * edgesSumFlows[strecke] 
			edges = []
			ri = 0
			rid = ""
			while len(edges)==0 and ri<len(routes[2]) and fi>=0:
				fi = fi - routes[2][ri][1]
				if fi<0:
					edges = routes[2][ri][2]
					rid = routes[2][ri][0]
				ri = ri + 1
			id = str(name.replace(" ", "_")) + "_" + str(rid) + "_" + str(vi)
			if strecke in edgemap:
				if edgemap[strecke] not in edges:
					edges.insert(0, edgemap[strecke])
			else:
				if strecke not in edges:
					edges.insert(0, strecke)
			emissions.append( ( int(t), id, edges ) )
# sort emissions
print "Sorting routes..."
emissions.sort(sorter(0))

# save emissions
print "Writing routes..."
fdo = open("routes.xml", "w")
fdo.write("<routes>\n")
for emission in emissions:
	if len(emission[2])<2:
		continue;
	fdo.write('    <vehicle id="' + emission[1] + '" depart="' + str(emission[0]) + '"><route>' + " ".join(emission[2]) + '</route></vehicle>\n');
fdo.write("</routes>\n")	
fdo.close()



