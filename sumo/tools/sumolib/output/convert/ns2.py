"""
@file    ns2.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

This module includes functions for converting SUMO's fcd-output into
data files read by ns2.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sumolib.output
import sumolib.net
import math, datetime

def fcd2ns2mobility(inpFCD, outSTRM, ignored):
  vIDm = sumolib._Running()
  begin = -1
  end = None
  area = [None, None, None, None]
  vehInfo = {}
  for timestep in inpFCD:
    if begin<0: begin = timestep.time
    end = timestep.time
    seen = set()
    if not timestep.vehicle:
      _writeMissing(timestep.time, vIDm, seen, vehInfo)
      continue
    for v in timestep.vehicle:
      seen.add(v.id)
      if not vIDm.k(v.id):
        nid = vIDm.g(v.id)
        if outSTRM:        
          print >> outSTRM, "$node_(%s) set X_ %s" % (nid, v.x) 
          print >> outSTRM, "$node_(%s) set Y_ %s" % (nid, v.y) 
          print >> outSTRM, "$node_(%s) set Z_ %s" % (nid, 0) 
        vehInfo[v.id] = [ nid, timestep.time, 0 ]
      nid = vIDm.g(v.id)        
      if outSTRM:
        print >> outSTRM, '$ns_ at %s "$node_(%s) setdest %s %s %s"' % (timestep.time, nid, v.x, v.y, 0)
      if not area[0]:
        area[0] = v.x
        area[1] = v.y
        area[2] = v.x
        area[3] = v.y
      area[0] = min(area[0], v.x)
      area[1] = min(area[1], v.y)
      area[2] = max(area[2], v.x)
      area[3] = max(area[3], v.y)
    _writeMissing(timestep.time, vIDm, seen, vehInfo)
  return vIDm, vehInfo, begin, end, area

def writeNS2activity(outSTRM, vehInfo):
  for v in vehInfo:
    i = vehInfo[v]
    print >> outSTRM, '$ns_ at %s "$g(%s) start"; # SUMO-ID: %s' % (i[1], i[0], v)
    print >> outSTRM, '$ns_ at %s "$g(%s) stop"; # SUMO-ID: %s' % (i[2], i[0], v)

def writeNS2config(outSTRM, vehInfo, ns2activityfile, ns2mobilityfile, begin, end, area):
  print >> outSTRM, "# set number of nodes\nset opt(nn) %s\n" % len(vehInfo)
  if ns2activityfile:
    print >> outSTRM, "# set activity file\nset opt(af) $opt(config-path)\nappend opt(af) /%s\n" % ns2activityfile
  if ns2mobilityfile:
    print >> outSTRM, "# set mobility file\nset opt(mf) $opt(config-path)\nappend opt(mf) /%s\n" % ns2mobilityfile
  xmin = area[0]
  ymin = area[1]
  xmax = area[2]
  ymax = area[3]
  print >> outSTRM, "# set start/stop time\nset opt(start) %s\nset opt(stop) %s\n" % (begin, end)
  print >> outSTRM, "# set floor size\nset opt(x) %s\nset opt(y) %s\nset opt(min-x) %s\nset opt(min-y) %s\n" % (xmax, ymax, xmin, ymin)


def _writeMissing(t, vIDm, seen, vehInfo):
  toDel = []
  for v in vIDm._m:
    if v in seen:
      continue
    nid = vIDm.g(v)        
    vehInfo[v][2] = t
    toDel.append(v)
  for v in toDel:
    vIDm.d(v) 

