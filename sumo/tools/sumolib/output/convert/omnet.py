"""
@file    omnet.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

This module includes functions for converting SUMO's fcd-output into
data files read by OMNET.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sumolib.output
import sumolib.net
import datetime

def fcd2omnet(inpFCD, outSTRM, further):
  print >> outSTRM, '<?xml version="1.0" encoding="utf-8"?>'
  print >> outSTRM, '<xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="mobility_trace.xsd">'
  print >> outSTRM, '<!-- generated on %s by %s -->\n' % (datetime.datetime.now(), further["app"])
  print >> outSTRM, '<mobility_trace>'
  vIDm = sumolib._Running()
  for timestep in inpFCD:
    seen = set()
    if not timestep.vehicle:
      _writeMissing(outSTRM, timestep.time, vIDm, seen)
      continue
    for v in timestep.vehicle:
      seen.add(v.id)
      # create if not given
      if not vIDm.k(v.id):
        nid = vIDm.g(v.id)        
        print >> outSTRM, """  <create><nodeid>%s</nodeid><time>%s</time>\
<type>SimpleNode</type><location><xpos>%s</xpos><ypos>%s</ypos></location></create>""" % (nid, timestep.time, v.x, v.y)
      else:
        nid = vIDm.g(v.id)        
        print >> outSTRM, """  <waypoint><nodeid>%s</nodeid><time>%s</time>\
<destination><xpos>%s</xpos><ypos>%s</ypos></destination><speed>%s</speed></waypoint>""" % (nid, timestep.time, v.x, v.y, v.speed) 
    _writeMissing(outSTRM, timestep.time, vIDm, seen)
  print >> outSTRM, '</mobility_trace>'


def _writeMissing(outSTRM, t, vIDm, seen):
  toDel = []
  for v in vIDm._m:
    if v in seen:
      continue
    nid = vIDm.g(v)        
    print >> outSTRM, """  <destroy><time>%s</time><nodeid>%s</nodeid></destroy>""" % (t, nid)
    toDel.append(v)
  for v in toDel:
    vIDm.d(v) 

    