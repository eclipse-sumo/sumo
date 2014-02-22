"""
@file    omnet.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

This module includes functions for converting SUMO's fcd-output into
data files read by OMNET.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
import datetime
import sumolib.output
import sumolib.net

def fcd2omnet(inpFCD, outSTRM, further):
  print('<?xml version="1.0" encoding="utf-8"?>', file=outSTRM)
  print('<xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="mobility_trace.xsd">', file=outSTRM)
  print('<!-- generated on %s by %s -->\n' % (datetime.datetime.now(), further["app"]), file=outSTRM)
  print('<mobility_trace>', file=outSTRM)
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
        print("""  <create><nodeid>%s</nodeid><time>%s</time>\
<type>SimpleNode</type><location><xpos>%s</xpos><ypos>%s</ypos></location></create>""" % (nid, timestep.time, v.x, v.y), file=outSTRM)
      else:
        nid = vIDm.g(v.id)        
        print("""  <waypoint><nodeid>%s</nodeid><time>%s</time>\
<destination><xpos>%s</xpos><ypos>%s</ypos></destination><speed>%s</speed></waypoint>""" % (nid, timestep.time, v.x, v.y, v.speed), file=outSTRM) 
    _writeMissing(outSTRM, timestep.time, vIDm, seen)
  print('</mobility_trace>', file=outSTRM)


def _writeMissing(outSTRM, t, vIDm, seen):
  toDel = []
  for v in vIDm._m:
    if v in seen:
      continue
    nid = vIDm.g(v)        
    print("""  <destroy><time>%s</time><nodeid>%s</nodeid></destroy>""" % (t, nid), file=outSTRM)
    toDel.append(v)
  for v in toDel:
    vIDm.d(v) 

    