"""
@author  Daniel.Krajzewicz@dlr.de
@date    2014-09-01
@version $Id: __init__.py 3774 2014-08-06 10:23:19Z erdm_ja $

Copyright (C) 2014 DLR/TS, Germany
All rights reserved
"""

from . import *
import os
import sumolib.net.generator.cross as netGenerator
import sumolib.net.generator.demand as demandGenerator
from sumolib.net.generator.network import *



class Scenario_BasicCross(Scenario):
  NAME = "BasicCross"
  THIS_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), NAME)
  TLS_FILE = os.path.join(THIS_DIR, "tls.add.xml")
  NET_FILE = os.path.join(THIS_DIR, "net.net.xml") 

  def __init__(self, withDefaultDemand=True):
    Scenario.__init__(self, self.NAME)
    self.netName = self.fullPath(self.NET_FILE)
    self.demandName = self.fullPath("routes.rou.xml")
    # network
    if fileNeedsRebuild(self.netName, "netconvert"):
      print "Network in '%s' needs to be rebuild" % self.netName
      defaultEdge = Edge(numLanes=1, maxSpeed=13.89)
      defaultEdge.addSplit(100, 1)
      defaultEdge.lanes = [Lane(dirs="rs"), Lane(dirs="l")]
      netGen = netGenerator.cross(None, defaultEdge)
      netGen.build(self.netName) # not nice, the network name should be given/returned
    # demand
    if withDefaultDemand:
      print "Demand in '%s' needs to be rebuild" % self.demandName
      self.demand = demandGenerator.Demand()
      self.demand.addStream(demandGenerator.Stream(None, 0, 3600, 1000, "2/1_to_1/1", "1/1_to_0/1", { "hdv":.2, "passenger":.8})) # why isn't it possible to get a network and return all possible routes or whatever - to ease the process
      if fileNeedsRebuild(self.demandName, "duarouter"):
        self.demand.build(0, 3600, self.netName, self.demandName)
