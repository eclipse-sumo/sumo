"""
@file    shawn.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

This module includes functions for converting SUMO's fcd-output into
data files read by Shawn.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import sumolib.output
import sumolib.net
import datetime

def fcd2shawn(inpFCD, outSTRM, further):
  print >> outSTRM, '<?xml version="1.0" encoding="utf-8"?>'
  print >> outSTRM, '<!-- generated on %s by %s -->\n' % (datetime.datetime.now(), further["app"])
  print >> outSTRM, '<scenario>'
  vIDm = sumolib._Running() # is it necessary to convert the ids?
  for timestep in inpFCD:
    print >> outSTRM, '   <snapshot id="%s">' % timestep.time
    if timestep.vehicle:
      for v in timestep.vehicle:
        nid = vIDm.g(v.id)        
        print >> outSTRM, '     <node id="%s"> <location x="%s" y="%s" z="%s"/> </node>' % (nid, v.x, v.y, v.z)
    print >> outSTRM, '   </snapshot>'
  print >> outSTRM, '</scenario>'

