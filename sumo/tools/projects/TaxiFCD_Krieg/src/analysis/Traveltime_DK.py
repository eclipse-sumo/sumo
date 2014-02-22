#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    Traveltime_DK.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-21
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from pylab import *


#global vars
mainPath="../../../../Projekte/Diplom/Daten/auswertung/reiseZeitD/"
usedPath=mainPath+"used2.txt"
analysisPath=mainPath+"taxiAnalysisInformation.xml"


used = {}
data= {}
def main():
    global data
        
    print "start program"
    
    getDrivenEdges()
    data=processData()
    #createStats()
    #plotScatterAbs()
    #plotScatterRel()
    plotBarChart("relDiff")
    #plotBarChart("absDiff")
    
    print "end"


def getDrivenEdges():
	# parse used edges
	fd = open(usedPath)
	for line in fd:
		line = line.strip()
		edge = line[5:-2]
		used[edge] = 1
	fd.close()
	
	
def processData():
	# read data ->a List of Taxis with a dict of Infos for each source: {type:[time,edge,speed]} 
	def getAttr(source, what):
		mbeg = source.find(what + "=")
		mbeg = source.find('"', mbeg) + 1
		mend = source.find('"', mbeg)
		return source[mbeg:mend]

	data = {}
	fd = open(analysisPath)
	for line in fd:
		if line.find("<vehicle ")>=0:
			vehicle = getAttr(line, "id")
			cdata = {}
			cdata["vtypeProbe"] = []
			cdata["FCD"] = []
			cdata["simFCD"] = []
		elif line.find("<step ")>=0:
			edge =  getAttr(line, "edge")
			if edge[0]!=':':
				speed =  getAttr(line, "speed")
				t =  getAttr(line, "time")
				type = getAttr(line, "source")
				cdata[type].append( [float(t), edge, float(speed)] )
		elif line.find("</vehicle")>=0:
			#if vehicle!="351_11":
				#print cdata
	#			continue
			data[vehicle] = cdata
	fd.close()
	
	# join same edge information
	data2 = {}
	for veh in data:
		lastEdge = None
		seen = set()
		skip = False
		#check vtype
		for elm in data[veh]["vtypeProbe"]:
			if lastEdge!=None and elm[1]==lastEdge:
				continue
			lastEdge = elm[1]
			if lastEdge in seen:
				skip = True
				#print veh
			seen.add(lastEdge)
		if skip:
			continue
		lastEdge = None #reset vars
		seen = set()
		skip = False
		#check Fcd
		for elm in data[veh]["FCD"]:
			if lastEdge!=None and elm[1]==lastEdge:
				continue
			lastEdge = elm[1]
			if lastEdge in seen:
				skip = True
			seen.add(lastEdge)
		if skip:
			continue
		lastEdge = None
		seen = set()
		skip = False
		#check simFCD
		for elm in data[veh]["simFCD"]:
			if lastEdge!=None and elm[1]==lastEdge:
				continue
			lastEdge = elm[1]
			if lastEdge in seen:
				skip = True
			seen.add(lastEdge)
		if skip:
			continue
	
		#remove double edges
		data2[veh] = {}
		data2[veh]["vtypeProbe"] = []
		for elm in data[veh]["vtypeProbe"]:                        #last edge uneven akt edge
			if len(data2[veh]["vtypeProbe"])==0 or data2[veh]["vtypeProbe"][-1][1]!=elm[1]:
				data2[veh]["vtypeProbe"].append(elm)
				data2[veh]["vtypeProbe"][-1].append(1)
			else:
				data2[veh]["vtypeProbe"][-1][-1] = data2[veh]["vtypeProbe"][-1][-1] + 1 #count edges
				data2[veh]["vtypeProbe"][-1][-2] = data2[veh]["vtypeProbe"][-1][-2] + elm[2] #add speed
		data2[veh]["FCD"] = []
		for elm in data[veh]["FCD"]:
			if len(data2[veh]["FCD"])==0 or data2[veh]["FCD"][-1][1]!=elm[1]:
				data2[veh]["FCD"].append(elm)
				data2[veh]["FCD"][-1].append(1)
			else:
				data2[veh]["FCD"][-1][-1] = data2[veh]["FCD"][-1][-1] + 1
				data2[veh]["FCD"][-1][-2] = data2[veh]["FCD"][-1][-2] + elm[2]
		data2[veh]["simFCD"] = []
		for elm in data[veh]["simFCD"]:
			if len(data2[veh]["simFCD"])==0 or data2[veh]["simFCD"][-1][1]!=elm[1]:
				data2[veh]["simFCD"].append(elm)
				data2[veh]["simFCD"][-1].append(1)
			else:
				data2[veh]["simFCD"][-1][-1] = data2[veh]["simFCD"][-1][-1] + 1
				data2[veh]["simFCD"][-1][-2] = data2[veh]["simFCD"][-1][-2] + elm[2]
	data = data2
	print "after remove double edges (simFCD)351_11 ",data["351_11"]["simFCD"]
	
	# make comparison table
	#cut data if edge in between belongs to the "bad driven" edges
	data2 = {} #-> contains only the FCD values 
	for veh in data:
		index = 0
		haveBegin = False
		for elm in data[veh]["FCD"]:
			if elm[1] in used: #if edge contains to the "good" edges
				if not haveBegin:
					id = veh + "_" + str(index) #new ID
					index = index + 1
					data2[id] = []
					haveBegin = True
			else:
				haveBegin = False
			if haveBegin:
				data2[id].append(elm)
	print "after cutting data 351_11_0",data2["351_11_0"]
	
	# ok, we should now have a list of valid FCD-trip parts within the container
	#for t in data["440_18"]:
	#	print t
	#	for e in data["440_18"][t]:
	#		print e
	#print data["440_18"]
	
	for veh in data2:
		# add information about simulated speeds
		for elm in data2[veh]:
			vid = veh[:veh.rfind('_')]
			for j in data[vid]["simFCD"]:
				if elm[1]==j[1]: #if edge FCD same as edge simFCD
					if len(elm)>5:
						a = 1
						#hier passiert nichts!
	#					e[-2] = j[2]
	#					e[-1] = j[0]
					else: #add corresponding simFCD values to the data
						elm.append(j[2]) #speed
						elm.append(j[0]) #time
	
	
	#remove those entries without simFCD information
	data3 = {}
	for veh in data2:
		data2[veh].pop() #del last elm
		ok = True
		for elm in data2[veh]:
			if len(elm)<6:
				ok = False
				#print "len not ok ",veh
		if ok:
			data3[veh] = data2[veh]
	print "afer remove entrys without simFCD 351_11_0 ",data3["351_11_0"] 
	
	return data3


def createStats():	
	#create stats
	fd = open(mainPath+"stats.csv", "w")
	fd.write("veh;real;sim;absDiff;relDiff;edges\n")
	for veh in data:
		if len(data[veh])<2: #by less then 2 entries continue
			continue
		travelTimeDict=calcTraveltime(data[veh])
		if travelTimeDict["relDiff"]==None:
			continue
		
		edges = len(data[veh])		
		#if abs(norm)>3.9:
			#print veh
		"""
		if veh=="351_11_0" :
			for elm in data3[veh]:
				print elm[5],elm[0]
			print real
			print sim
		"""
		fd.write(veh + ";" + str(travelTimeDict["real"]) + ";" + str(travelTimeDict["sim"]) + ";" + str(travelTimeDict["absDiff"]) + ";" + str(travelTimeDict["relDiff"]) + ";" + str(edges) + "\n")
	fd.close()


def calcTraveltime(veh):
	travelTimeDict={"sim":None,"real":None, "absDiff":None, "relDiff":None}
	#data3[veh][-1]: [0:timeFCD, 1:edgeFCD, 2:speedFCD,3:edgeCount,4:speedSimFCD, 5:timeSimFCD]
	
		
	travelTimeDict["sim"] = veh[-1][5]-veh[0][5] #sim = data[veh][-1][0]-data[veh][0][0] 
	travelTimeDict["real"] = veh[-1][0]-veh[0][0] #real = data[veh][-1][5]-data[veh][0][5] 
	if travelTimeDict["real"]!=0: #to avoid error
		travelTimeDict["absDiff"] = (travelTimeDict["sim"]-travelTimeDict["real"]) #diff = (real-sim)
		travelTimeDict["relDiff"] = travelTimeDict["absDiff"] / travelTimeDict["real"] #norm = (real-sim) / real
	
	return travelTimeDict


def plotScatterAbs():
	# plot scatter abs
	travelSim = []
	travelFCD = []
	for veh in data:
		if len(data[veh])<2:
			continue
		travelTimeDict=calcTraveltime(data[veh])	
		travelSim.append(travelTimeDict["real"])
		travelFCD.append(travelTimeDict["sim"])
		if travelTimeDict["real"]<0:
			print veh
			print data[veh]
	plot(travelFCD, travelSim, '.')
	xlim(0,2000)
	ylim(0,2000)
	#clf()
	#savefig(mainPath+"scatter_abs.png")
	show()


def plotScatterRel():
	travelSim = []
	travelFCD = []
	for veh in data:
		if len(data[veh])<2:
			continue
		travelTimeDict=calcTraveltime(data[veh])
		
		if travelTimeDict["sim"]!=0:
			travelSim.append(travelTimeDict["real"]/travelTimeDict["sim"])
			travelFCD.append(travelTimeDict["sim"]/travelTimeDict["sim"])
		if travelTimeDict["sim"]<0:
			print veh
			print data[veh]
	plot(travelSim, travelFCD, '.')
	#savefig(mainPath+"scatter_rel.png")
	show()


def plotBarChart(mode="relDiff"):
    l = []
    barsDict={}
    i=0
    print "total", len(data)
    for veh in data:
		if len(data[veh])<2:
			continue
		travelTimeDict=calcTraveltime(data[veh])
		
		#xticks(range(-200,210,10))
		#if -100<=travelTimeDict[mode]*100<=100:  
			#l.append(travelTimeDict[mode]*100)
		diff=int(travelTimeDict[mode]*100) 
		barsDict[(diff/10)*10]=barsDict.setdefault((diff/10)*10,0)+1
			
		#else:
		  #print travelTimeDict[mode]*100
		  #i+=1
    b=bar(barsDict.keys(),barsDict.values(), width=10, alpha=0.5)		   
	#hist(l, bins=41)
	#savefig(mainPath+"norm_abw_hist.png")
    print "over100",i
    show()


#start the program
#cProfile.run('main()')
main()
