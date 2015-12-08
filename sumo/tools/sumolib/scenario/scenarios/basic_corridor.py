"""
@author  Daniel.Krajzewicz@dlr.de
@date    2014-09-01
@version $Id: __init__.py 3774 2014-08-06 10:23:19Z erdm_ja $

Copyright (C) 2014 DLR/TS, Germany
All rights reserved
"""


from . import *
import os
import sumolib.net.generator.corridor as netGenerator
import sumolib.net.generator.demand as demandGenerator
from sumolib.net.generator.network import *



class Scenario_BasicCorridor(Scenario):
  NAME = "BasicCorridor"
  THIS_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), NAME)
  TLS_FILE = os.path.join(THIS_DIR, "tls.add.xml")
  NET_FILE = os.path.join(THIS_DIR, "corridor_%s.net.xml") 

  def __init__(self, xoff, withDefaultDemand=True):
    Scenario.__init__(self, self.NAME)
    self.NET_FILE = self.NET_FILE % xoff
    self.netName = self.fullPath(self.NET_FILE)
    self.demandName = self.fullPath("routes.rou.xml")
    # network
    if fileNeedsRebuild(self.netName, "netconvert"):
      print "Network in '%s' needs to be rebuild" % self.netName
      defaultEdge = Edge(numLanes=1, maxSpeed=13.89)
      defaultEdge.addSplit(100, 1)
      defaultEdge.lanes = [Lane(dirs="rs"), Lane(dirs="l")]
      netGen = netGenerator.corridor(5, None, defaultEdge)
      xpos = xoff
      for i in range(1, 6):
        netGen._nodes["%s/0"%i].x = xpos
        netGen._nodes["%s/1"%i].x = xpos
        netGen._nodes["%s/2"%i].x = xpos
        xpos = xpos + xoff
      netGen._nodes["6/1"].x = xpos
      netGen.build(self.netName) # not nice, the network name should be given/returned
    # demand
    if withDefaultDemand:
      print "Demand in '%s' needs to be rebuild" % self.demandName
      self.demand = demandGenerator.Demand()
      self.demand.addStream(demandGenerator.Stream(None, 0, 3600, 1000, "6/1_to_5/1", "1/1_to_0/1", { .2:"hdv", .8:"passenger"})) # why isn't it possible to get a network and return all possible routes or whatever - to ease the process
      if fileNeedsRebuild(self.demandName, "duarouter"):
        self.demand.build(0, 3600, self.netName, self.demandName)
